#ifndef LOGFILE_H
#define LOGFILE_H

#include <cstdio>
#include <unistd.h>
#include <sys/time.h>
#include <string>

#include "TimeStamp.h"

// 日志文件刷新间隔，单位：秒
static constexpr time_t kFlushInterval = 3;

class LogFile 
{
private:
    FILE *fp_;               // 文件指针，日志文件句柄
    const char* filepath_;                // 日志输出路径
    int64_t writtenBytes_;   // 累计写入的字节数
    time_t lastWrite_;       // 上一次写入的时间，用于判断是否写入超时
    time_t lastFlush_;       // 上一次刷新时间，用于定时刷新文件缓冲

    int currentHour_;            // 当前日志文件对应小时，跨小时切换用
    std::string currentFilePath_; // 当前日志文件完整路径
    
    // 判断当前是否已跨小时，若是则需要切换日志文件
    bool IsNewHour()  ;

    // 切换日志文件，创建新的日志文件（含自动创建当天目录）
    void RollFile();

    // 生成带日期子目录和按小时命名的日志文件完整路径
    std::string GenerateLogFilePath()  ;

public:
    // 构造函数，打开日志文件，参数为日志文件路径，默认空指针表示不打开文件
    explicit LogFile(const char *filepath = nullptr);

    // 析构函数，确保关闭文件并刷新缓存
    ~LogFile();

    // 写入数据到日志文件，线程不安全，调用者需保证同步
    void Write(const char *data, std::size_t len);

    // 刷新日志缓冲区，强制写入磁盘
    void Flush()  ;

    // 返回已经写入的字节数
    int64_t WrittenBytes() const  ;
};

#endif
