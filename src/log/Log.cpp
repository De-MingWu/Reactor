#include <utility>
#include <cstdio>
#include <cstring>
#include "Log.h"
#include "CurrentThread.h"

// 为了提高多线程日志时间格式化效率，使用 __thread 缓存时间字符串和上次秒数
__thread char t_time[64];        // 当前线程缓存的时间字符串
__thread time_t t_lastsecond;    // 当前线程上次记录的秒数

// 辅助类 Template，方便固定长度字符串写入 LogStream
class Template 
{
public:
    Template(const char *str, std::size_t len) : str_(str), len_(len) {}
    const char *str_;
    const std::size_t len_;
};

// 重载 << 运算符支持 Template 类型
inline LogStream &operator<<(LogStream &s, Template v) 
{
    s.Append(v.str_, v.len_);
    return s;
}

// 重载 << 运算符支持 SourceFile 类型
inline LogStream &operator<<(LogStream &s, const Log::SourceFile &v) 
{
    s.Append(v.Data(), v.Size());
    return s;
}

// SourceFile 构造函数，提取文件名
Log::SourceFile::SourceFile(const char *data)
       :data_(data), size_(static_cast<std::size_t>(strlen(data))) 
{
    const char *slash = strrchr(data, '/');
    if (slash) 
    {
        data_ = slash + 1;
        size_ = static_cast<std::size_t>(strlen(data_));
    }
}

// Impl 构造函数，增加 func 参数
Log::Impl::Impl(const SourceFile &source, int line, const char *func, LogLevel level)
       :sourceFile_(source), 
        line_(line), 
        func_(func),
        level_(level) 
 {
    FormatTime();
    CurrentThread::Tid();

    stream_ << Template(CurrentThread::TidString(), CurrentThread::TidStringLength());
    stream_ << Template(GetLogLevelString(), 6);
}

void Log::Impl::FormatTime()  
{
    TimeStamp now = TimeStamp::NowTime();
    time_t seconds = now.ToInt();
    int microseconds = static_cast<int>(now.MicrosecondsSinceEpoch() % 1000000);

    if (t_lastsecond != seconds) 
    {
        struct tm tm_time;
        localtime_r(&seconds, &tm_time);

        snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d.",
                 tm_time.tm_year + 1900,
                 tm_time.tm_mon + 1,
                 tm_time.tm_mday,
                 tm_time.tm_hour,
                 tm_time.tm_min,
                 tm_time.tm_sec);

        t_lastsecond = seconds;
    }

    // 打印微秒 + 'Z'
    Fmt us(".%06dZ  ", microseconds);

    stream_ << Template(t_time, 17) << Template(us.GetDataAddr(), 9);
}

void Log::Impl::Finish()  
{
    stream_ << " - " << sourceFile_.Data() << ":" << line_ << "\n";
}

LogStream &Log::Impl::Stream()  
{
    return stream_;
}

const char *Log::Impl::GetLogLevelString() const  
{
    switch (level_) 
    {
        case LogLevel::DEBUG: return "DEBUG ";
        case LogLevel::INFO:  return "INFO  ";
        case LogLevel::WARN:  return "WARN  ";
        case LogLevel::ERROR: return "ERROR ";
        case LogLevel::FATAL: return "FATAL ";
    }
    return "UNKNOWN";
}

// 默认输出函数
void DefaultOutput(const char *msg, std::size_t len)  
{
    fwrite(msg, 1, len, stdout);
}

// 默认刷新函数
void DefaultFlush()  
{
    fflush(stdout);
}

// 全局默认值
Log::OutputFunc g_output = DefaultOutput;
Log::FlushFunc g_flush = DefaultFlush;

std::atomic<LogLevel> gLogLevel { LogLevel::INFO };

// Log 构造
Log::Log(const char *file, int line, LogLevel level)  
    : Impl_(SourceFile(file), line, nullptr, level) {}

Log::Log(const char *file, int line, LogLevel level, const char *func)  
    : Impl_(SourceFile(file), line, func, level) {}

Log::~Log()  
{
    Impl_.Finish();
    const LogBuffer<LogBufferSize> &buf(Stream().GetBuffer());
    g_output(buf.GetDataAddr(), buf.GetLen());

    if (Impl_.GetLevel() == LogLevel::FATAL) 
    {
        g_flush();
        std::abort();  // Call abort when log level is fatal
    }
}

LogStream &Log::Stream()  
{
    return Impl_.Stream();
}

void Log::SetOutput(Log::OutputFunc func)  
{
    g_output = func;
}

void Log::SetFlush(Log::FlushFunc func)  
{
    g_flush = func;
}

void Log::SetLogLevel(LogLevel level)  
{
    gLogLevel.store(level);
}
