#include "LogFile.h"
#include "TimeStamp.h"

#include <string>
#include <iostream>
#include <cerrno>
#include <cstring>     // for strerror()
#include <sys/stat.h>   // for mkdir()
#include <unistd.h>     // for access()

// 构造函数：尝试打开指定日志文件，若失败则按当前日期创建目录，按小时创建日志文件
LogFile::LogFile(const char* filepath)
        : fp_(nullptr),
          filepath_(filepath),
          writtenBytes_(0),
          lastWrite_(0),
          lastFlush_(0),
          currentHour_(-1)  // 初始非法值，触发首次创建
{
    time_t now = ::time(nullptr);       
    struct tm tmNow;
    localtime_r(&now, &tmNow);          
    currentHour_ = tmNow.tm_hour;       // 保存当前小时
    
    if (filepath_) 
    {
        currentFilePath_ = filepath_;
        fp_ = ::fopen(filepath_, "a+");
    }

    // 打开失败，自动回退创建日志文件（含日期子目录）
    if (!fp_) 
    {
        currentFilePath_ = GenerateLogFilePath();
        fp_ = ::fopen(currentFilePath_.c_str(), "a+");

        if (!fp_) 
        {
            std::cerr << "Failed to open log file: "
                      << (filepath ? filepath : currentFilePath_)
                      << " | Reason: " << strerror(errno) << std::endl;
        } 
        else 
        {
            std::cerr << "Fallback: log file created at " << currentFilePath_ << std::endl;
        }
    }
}

// 析构函数：关闭文件并刷新缓冲区
LogFile::~LogFile()  
{
    if (fp_) 
    {
        Flush();
        ::fclose(fp_);
        fp_ = nullptr;
    }
}

// 写入日志内容（调用者需确保线程安全）
void LogFile::Write(const char* data, std::size_t len)
{
    if (!fp_ || len == 0) return;

    // 跨小时自动切换新文件
    if (IsNewHour()) 
    {
        RollFile();
    }

    std::size_t pos = 0;
    while (pos < len) 
    {
        std::size_t written = fwrite_unlocked(data + pos, sizeof(char), len - pos, fp_);
        if (written == 0) 
        {
            std::cerr << "fwrite_unlocked failed in LogFile::Write" << std::endl;
            break;
        }
        pos += written;
    }

    time_t now = ::time(nullptr);

    if (pos > 0) 
    {
        lastWrite_ = now;
        writtenBytes_ += pos;
    }

    if (now - lastFlush_ >= kFlushInterval) 
    {
        Flush();
        lastFlush_ = now;
    }
}

// 获取累计写入字节数
int64_t LogFile::WrittenBytes() const  
{ 
    return writtenBytes_; 
}

// 刷新缓冲区
void LogFile::Flush()  
{ 
    if (fp_) 
        ::fflush(fp_);
}

// 判断当前是否跨小时
bool LogFile::IsNewHour()  
{
    time_t now = ::time(nullptr);
    struct tm tmNow;
    localtime_r(&now, &tmNow);
    return tmNow.tm_hour != currentHour_;
}

// 滚动日志文件（跨小时调用）
void LogFile::RollFile()
{
    if (fp_) 
    {
        Flush();
        ::fclose(fp_);
        fp_ = nullptr;
    }

    currentFilePath_ = GenerateLogFilePath();
    fp_ = ::fopen(currentFilePath_.c_str(), "a+");

    if (!fp_) 
    {
        std::cerr << "RollFile failed, fallback failed too: "
                  << currentFilePath_ << " | Reason: " << strerror(errno) << std::endl;
    }

    time_t now = ::time(nullptr);
    struct tm tmNow;
    localtime_r(&now, &tmNow);
    currentHour_ = tmNow.tm_hour;

    writtenBytes_ = 0;
}

// 生成带有日期子目录的日志路径（如 ../LogFiles/20250611/LogFile_20250611_15.log）
std::string LogFile::GenerateLogFilePath()  
{
    std::string date = TimeStamp::NowTime().ToStringDaily(); // 20250611
    std::string dir = filepath_ + date;

    // 如果目录不存在，则创建
    if (::access(dir.c_str(), F_OK) != 0) 
    {
        ::mkdir(dir.c_str(), 0755);  // 递归创建也可以用 mkdir -p 替代
    }

    // 构造文件名
    std::string filename = "LogFile_" + TimeStamp::NowTime().ToStringHourly() + ".log";
    return dir + "/" + filename;
}
