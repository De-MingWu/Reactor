#ifndef LOGBUFFER_H
#define LOGBUFFER_H

#include <string>
#include <algorithm>
#include <cstring>
#include <assert.h>
#include <iostream>

#include "Common.h" 


// 模板类：固定大小的内存缓冲区，避免动态内存分配，提高日志性能
template <int SIZE>
class LogBuffer 
{
private:
    char data_[SIZE];   // 缓冲区本体
    char* cur_;         // 当前写入位置指针

public:
    LogBuffer();
    ~LogBuffer() = default;

    // 向缓冲区追加数据
    void Append(const char* buf, int len);

    // 返回缓冲区已有数据的起始地址
    const char* GetDataAddr() const;

    // 返回缓冲区当前已写入的数据长度
    int GetLen() const;

    // 返回当前可写入的位置
    char* GetCurrent();

    // 返回剩余可写入空间
    int GetAvailSpace() const;

    // 移动写入指针
    void Move(int len);

    // 重置缓冲区（重新开始写入）
    void Reset();

    // 清空缓冲区（将内存清零）
    void Clear();

    // 返回缓冲区末尾地址（用于边界判断）
    const char* GetEndAddr() const;
};

// 构造函数：初始化写指针
template <int SIZE>
LogBuffer<SIZE>::LogBuffer() : cur_(data_) {}

// 向缓冲区追加数据（仅在空间足够时）
template <int SIZE>
void LogBuffer<SIZE>::Append(const char* buf, int len) 
{
    if (GetAvailSpace() >= len) {
        std::memcpy(cur_, buf, len);
        cur_ += len;
    }
    // 可根据需要添加 else 分支记录 drop 警告
}

// 获取缓冲区数据起始地址
template <int SIZE>
const char* LogBuffer<SIZE>::GetDataAddr() const 
{
    return data_;
}

// 获取已写入数据长度
template <int SIZE>
int LogBuffer<SIZE>::GetLen() const 
{
    return static_cast<int>(cur_ - data_);
}

// 获取当前写入位置
template <int SIZE>
char* LogBuffer<SIZE>::GetCurrent() 
{
    return cur_;
}

// 获取剩余空间
template <int SIZE>
int LogBuffer<SIZE>::GetAvailSpace() const 
{
    return static_cast<int>(GetEndAddr() - cur_);
}

// 移动写入指针
template <int SIZE>
void LogBuffer<SIZE>::Move(int len) 
{
    cur_ += len;
}

// 重置缓冲区
template <int SIZE>
void LogBuffer<SIZE>::Reset() 
{
    cur_ = data_;
}

// 清空缓冲区内容
template <int SIZE>
void LogBuffer<SIZE>::Clear() 
{
    memset(data_, 0, SIZE);
    cur_ = data_;
}

// 获取末尾地址
template <int SIZE>
const char* LogBuffer<SIZE>::GetEndAddr() const 
{
    return data_ + sizeof(data_);
}

#endif