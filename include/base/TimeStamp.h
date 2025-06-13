#ifndef REACTOR_TIMESTAMP_H
#define REACTOR_TIMESTAMP_H

#include <iostream>
#include <string>

class TimeStamp
{
private:
    int64_t microsecondsSinceEpoch_;   // 微秒精度时间戳（从1970到现在已逝去的秒数）
    
public:
    TimeStamp();                     //用当前时间初始化对象
    explicit TimeStamp(int64_t microsecondsSinceEpoch);//用一个整数表示的时间初始化对象

    static TimeStamp NowTime();  //返回当前时间的Timestamp对象

    int64_t MicrosecondsSinceEpoch() const;
    time_t ToInt() const;   //返回整数表示的时间
    std::string ToString() const;//返回字符串表示的时间，格式：yyyy-mm-dd hh24:mi:ss
    
    std::string ToStringHourly() const; // 返回格式：20250611_15
    std::string ToStringDaily() const;  // 返回格式：20250611
};


#endif //REACTOR_TIMESTAMP_H
