#ifndef LOGSTREAM_H
#define LOGSTREAM_H

#include <string>
#include <algorithm>
#include <cstring>
#include <cassert>
#include <iostream>
#include <type_traits> // For std::is_arithmetic
#include <cstdio> // For snprintf

#include "Common.h"
#include "LogBuffer.h" 

// 固定缓冲区大小 4KB
constexpr std::size_t LogBufferSize = 4096;

// 整数转换成字符串最大空间（足够表示64位整数 + 符号）
constexpr std::size_t kMaxNumericSize = 48;

// 日志流类
class LogStream 
{
private:
    LogBuffer<LogBufferSize> buffer_;  // 使用模板化缓冲区

    // 整数类型格式化为字符串存入 buffer
    template <typename T>
    void FormatInteger(T value)  ;

public:
    DISALLOW_COPY_AND_MOVE(LogStream);

    LogStream()   = default;
    ~LogStream() = default;

    // 追加原始数据到 buffer
    void Append(const char* data, std::size_t len)  ;

    // 获取内部 buffer
    const LogBuffer<LogBufferSize>& GetBuffer() const  ;

    // 重置缓冲区
    void ResetBuffer()  ;

    // 支持各类型流式写入
    LogStream& operator<<(bool v)  ;
    LogStream& operator<<(short num)  ;
    LogStream& operator<<(unsigned short num)  ;
    LogStream& operator<<(int num)  ;
    LogStream& operator<<(unsigned int num)  ;
    LogStream& operator<<(long num)  ;
    LogStream& operator<<(unsigned long num)  ;
    LogStream& operator<<(long long num)  ;
    LogStream& operator<<(unsigned long long num)  ;
    LogStream& operator<<(const float& num)  ;
    LogStream& operator<<(const double& num)  ;
    LogStream& operator<<(char v)  ;
    LogStream& operator<<(const char* str)  ;
    LogStream& operator<<(const std::string& v)  ;
};

// 整数格式化实现
template <typename T>
void LogStream::FormatInteger(T value)  
{
    if (buffer_.GetAvailSpace() >= kMaxNumericSize) 
    {
        char* buf = buffer_.GetCurrent();
        char* cur = buf;

        T absValue = value;
        if (value < 0) 
        {
            absValue = -value;
        }

        // 逆序写入数字字符
        do 
        {
            int digit = static_cast<int>(absValue % 10);
            *cur++ = '0' + digit;
            absValue /= 10;
        } while (absValue != 0);

        // 如果是负数加负号
        if (value < 0) 
        {
            *cur++ = '-';
        }

        // 反转数字
        std::reverse(buf, cur);

        buffer_.Move(static_cast<int>(cur - buf));
    }
}

// 辅助格式化数值，存入内部 buf_，减少临时字符串创建。
class Fmt 
{
private:
    char buf_[64];   // 扩展为64字节以适应更多的数值范围
    std::size_t length_;

public:
    template <typename T>
    Fmt(const char* fmt, T val);

    const char* GetDataAddr() const   { return buf_; }
    std::size_t Length() const   { return length_; }
};

// Fmt 模板构造函数，编译期保证 T 为算术类型
template <typename T>
Fmt::Fmt(const char* fmt, T val) 
{
    static_assert(std::is_arithmetic<T>::value, "Must be arithmetic type");

    // 格式化写入 buf_
    length_ = std::snprintf(buf_, sizeof(buf_), fmt, val);
    if (length_ < 0 || static_cast<std::size_t>(length_) >= sizeof(buf_)) {
        std::cerr << "Warning: Fmt formatting overflow, using truncated value!" << std::endl;
        length_ = sizeof(buf_) - 1;
    }
}

// 全局 operator<< 重载，允许 LogStream << Fmt 使用
inline LogStream& operator<<(LogStream& s, const Fmt& fmt)  
{
    s.Append(fmt.GetDataAddr(), fmt.Length());
    return s;
}

// 显式实例化 Fmt 模板，减少编译期代码膨胀
template Fmt::Fmt(const char* fmt, char);
template Fmt::Fmt(const char* fmt, short);
template Fmt::Fmt(const char* fmt, unsigned short);
template Fmt::Fmt(const char* fmt, int);
template Fmt::Fmt(const char* fmt, unsigned int);
template Fmt::Fmt(const char* fmt, long);
template Fmt::Fmt(const char* fmt, unsigned long);
template Fmt::Fmt(const char* fmt, long long);
template Fmt::Fmt(const char* fmt, unsigned long long);
template Fmt::Fmt(const char* fmt, float);

#endif
