#ifndef Log_H
#define Log_H

#include <atomic>
#include <cstring>

#include "Common.h"
#include "TimeStamp.h"
#include "LogStream.h"

// 日志级别定义
enum class LogLevel 
{
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
};

class Log {
public:
    // 编译器计算源文件名
    class SourceFile 
    {
    private:
        const char *data_;
        std::size_t size_;
    public:
        explicit SourceFile(const char *data);
        const char *Data() const   { return data_; }
        std::size_t Size() const   { return size_; }
    };
    
private:
    // 内部类，组装日志信息
    class Impl 
    {
    private:
        SourceFile sourceFile_; // 源代码文件名
        int line_;              // 行号
        const char *func_;      // 函数名
        LogStream stream_;      // 日志缓存流
        LogLevel level_;        // 日志级别
    public:
        DISALLOW_COPY_AND_MOVE(Impl);

        Impl(const SourceFile &source, int line, const char *func, LogLevel level);
        void FormatTime()  ;   // 格式化时间信息
        void Finish()  ;       // 完成格式化，补充源文件、位置等信息

        LogStream &Stream()  ;
        const char *GetLogLevelString() const  ; // 获取日志级别对应字符串
        LogLevel GetLevel() const   { return level_; }
    };

    Impl Impl_;

public:
    DISALLOW_COPY_AND_MOVE(Log);

    // 两个构造函数，支持宏 __func__ 传入
    Log(const char *file, int line, LogLevel level)  ;
    Log(const char *file, int line, LogLevel level, const char *func)  ;
    ~Log()  ;

    // 用于日志宏，返回 Impl 的输出流
    LogStream &Stream()  ;

    // 全局方法，设置/获取全局日志级别，设置输出/刷新函数
    static LogLevel GetLogLevel()  ;
    static void SetLogLevel(LogLevel level)  ;

    typedef void (*OutputFunc)(const char *data, std::size_t len); // 输出函数
    typedef void (*FlushFunc)();                                   // 刷新函数

    static void SetOutput(OutputFunc func);
    static void SetFlush(FlushFunc func);
};

// 全局日志级别，线程安全
extern std::atomic<LogLevel> gLogLevel;

// 静态成员函数内联定义
inline LogLevel Log::GetLogLevel()  
{
    return gLogLevel.load();
}

// 日志宏定义（调用 Log::GetLogLevel()，调用 .Stream()）
#define LOG_DEBUG \
    if (Log::GetLogLevel() <= LogLevel::DEBUG) \
        Log(__FILE__, __LINE__, LogLevel::DEBUG).Stream()
#define LOG_INFO \
    if (Log::GetLogLevel() <= LogLevel::INFO) \
        Log(__FILE__, __LINE__, LogLevel::INFO).Stream()

#define LOG_WARN \
    Log(__FILE__, __LINE__, LogLevel::WARN).Stream()

#define LOG_ERROR \
    Log(__FILE__, __LINE__, LogLevel::ERROR).Stream()

#define LOG_FATAL \
    Log(__FILE__, __LINE__, LogLevel::FATAL).Stream()

#endif // Log_H
