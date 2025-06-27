#include <cstdio>
#include <cstring>
#include "LogStream.h"

// 向 LogStream 内部 buffer_ 追加数据
void LogStream::Append(const char* data, std::size_t len)  
{
    buffer_.Append(data, len);
}

// 获取内部 buffer
const LogBuffer<LogBufferSize>& LogStream::GetBuffer() const  
{
    return buffer_;
}

// 重置内部 buffer
void LogStream::ResetBuffer()  
{
    buffer_.Clear();  // 内存清零
    buffer_.Reset();  // 重置 cur_
}

// bool 类型重载
LogStream& LogStream::operator<<(bool v)  
{
    buffer_.Append(v ? "1" : "0", 1);
    return *this;
}

LogStream& LogStream::operator<<(short num)  
{
    return (*this) << static_cast<int>(num);
}

LogStream& LogStream::operator<<(unsigned short num)  
{
    return (*this) << static_cast<unsigned int>(num);
}

LogStream& LogStream::operator<<(int num)  
{
    FormatInteger(num);
    return *this;
}

LogStream& LogStream::operator<<(unsigned int num)  
{
    FormatInteger(num);
    return *this;
}

LogStream& LogStream::operator<<(long num)
{
    FormatInteger(num);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long num)
{
    FormatInteger(num);
    return *this;
}

LogStream& LogStream::operator<<(long long num)
{
    FormatInteger(num);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long long num)
{
    FormatInteger(num);
    return *this;
}

// float 重载，转 double 调用
LogStream& LogStream::operator<<(const float& num)
{
    return (*this) << static_cast<const double>(num);
}

// double 重载，snprintf 转字符串写入 buffer
LogStream& LogStream::operator<<(const double& num)
{
    char buf[32];
    int len = std::snprintf(buf, sizeof(buf), "%g", num);  // %g：紧凑格式
    if (len >= 0 && len < sizeof(buf)) { // 确保没有溢出
        buffer_.Append(buf, len);
    } 
    else 
    {
        // 处理 snprintf 出错的情况
        const char* error_msg = "(error)";
        buffer_.Append(error_msg, std::strlen(error_msg));
    }
    return *this;
}

// 单字符
LogStream& LogStream::operator<<(char v)
{
    buffer_.Append(&v, 1);
    return *this;
}

// C 风格字符串
LogStream& LogStream::operator<<(const char* str)
{
    if (str) 
    {
        buffer_.Append(str, std::strlen(str));
    } 
    else 
    {
        const char* null_msg = "(null)";
        buffer_.Append(null_msg, std::strlen(null_msg));
    }
    return *this;
}

// std::string
LogStream& LogStream::operator<<(const std::string& v)
{
    buffer_.Append(v.c_str(), v.size());
    return *this;
}