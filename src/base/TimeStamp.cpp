#include <sys/time.h>
#include <stdio.h>

#include "TimeStamp.h"

TimeStamp::TimeStamp()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);   // 秒 + 微秒
    microsecondsSinceEpoch_ = static_cast<int64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
}

TimeStamp::TimeStamp(int64_t microsecondsSinceEpoch)
    : microsecondsSinceEpoch_(microsecondsSinceEpoch){}


//返回当前时间的Timestamp对象
TimeStamp TimeStamp::NowTime()
{
    return TimeStamp();   //返回当前时间。
}

int64_t TimeStamp::MicrosecondsSinceEpoch() const { return microsecondsSinceEpoch_; }

time_t TimeStamp::ToInt()const
{
    return static_cast<time_t>(microsecondsSinceEpoch_ / 1000000);
}

std::string TimeStamp::ToString() const
{
    time_t seconds = ToInt();
    struct tm tm_time;
    localtime_r(&seconds, &tm_time);

    char buf[64] = {0};
    snprintf(buf, sizeof(buf), "%4d-%02d-%02d %02d:%02d:%02d",
             tm_time.tm_year + 1900,
             tm_time.tm_mon + 1,
             tm_time.tm_mday,
             tm_time.tm_hour,
             tm_time.tm_min,
             tm_time.tm_sec);

    return buf;
}

// 生成格式为 "YYYYMMDD_HH" 的字符串，用于按小时切分日志
std::string TimeStamp::ToStringHourly() const
{
    time_t seconds = ToInt();          // 转为秒级时间戳
    struct tm tm_time;
    localtime_r(&seconds, &tm_time);   // 转为本地时间

    char buf[32] = {0};
    snprintf(buf, sizeof(buf), "%4d%02d%02d_%02d",
             tm_time.tm_year + 1900,
             tm_time.tm_mon + 1,
             tm_time.tm_mday,
             tm_time.tm_hour);

    return buf;
}

std::string TimeStamp::ToStringDaily() const
{
    time_t seconds = ToInt();          // 转为秒级时间戳
    struct tm tm_time;
    localtime_r(&seconds, &tm_time);   // 转为本地时间

    char buf[32] = {0};
    snprintf(buf, sizeof(buf), "%4d%02d%02d",
             tm_time.tm_year + 1900,
             tm_time.tm_mon + 1,
             tm_time.tm_mday);

    return buf;
}
