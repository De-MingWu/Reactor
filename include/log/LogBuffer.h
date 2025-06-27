#ifndef LOGBUFFER_H
#define LOGBUFFER_H

#include <string>
#include <algorithm>
#include <cstring>
#include <cassert>
#include <array>
#include <iostream>

#include "Common.h"

// 模板类：固定大小的内存缓冲区，避免动态内存分配，提高日志性能
template <std::size_t SIZE>
class LogBuffer
{
private:
    std::array<char, SIZE> data_;   // 使用std::array，增强类型安全性
    char* cur_;                     // 当前写入位置指针

public:
    LogBuffer() noexcept;           // 构造函数，标记为noexcept
    ~LogBuffer() = default;

    // 向缓冲区追加数据
    void Append(const char* buf, std::size_t len);

    // 返回缓冲区已有数据的起始地址
    const char* GetDataAddr() const noexcept;

    // 返回缓冲区当前已写入的数据长度
    std::size_t GetLen() const noexcept;

    // 返回当前可写入的位置
    char* GetCurrent() noexcept;

    // 返回剩余可写入空间
    std::size_t GetAvailSpace() const noexcept;

    // 移动写入指针
    void Move(std::size_t len) noexcept;

    // 重置缓冲区（重新开始写入）
    void Reset() noexcept;

    // 清空缓冲区（将内存清零）
    void Clear() noexcept;

    // 返回缓冲区末尾地址（用于边界判断）
    const char* GetEndAddr() const noexcept;
};

// 构造函数：初始化写指针
template <std::size_t SIZE>
LogBuffer<SIZE>::LogBuffer() noexcept : cur_(data_.data()) {}

// 向缓冲区追加数据（仅在空间足够时）
template <std::size_t SIZE>
void LogBuffer<SIZE>::Append(const char* buf, std::size_t len)
{
    if (GetAvailSpace() >= len) {
        std::memcpy(cur_, buf, len);
        cur_ += len;
    } else {
        // 可根据需要添加日志记录或抛出异常
        std::cerr << "Buffer overflow, log data was dropped!" << std::endl;
    }
}

// 获取缓冲区数据起始地址
template <std::size_t SIZE>
const char* LogBuffer<SIZE>::GetDataAddr() const noexcept
{
    return data_.data();
}

// 获取已写入数据长度
template <std::size_t SIZE>
std::size_t LogBuffer<SIZE>::GetLen() const noexcept
{
    return static_cast<std::size_t>(cur_ - data_.data());
}

// 获取当前写入位置
template <std::size_t SIZE>
char* LogBuffer<SIZE>::GetCurrent() noexcept
{
    return cur_;
}

// 获取剩余空间
template <std::size_t SIZE>
std::size_t LogBuffer<SIZE>::GetAvailSpace() const noexcept
{
    return static_cast<std::size_t>(GetEndAddr() - cur_);
}

// 移动写入指针
template <std::size_t SIZE>
void LogBuffer<SIZE>::Move(std::size_t len) noexcept
{
    cur_ += len;
}

// 重置缓冲区
template <std::size_t SIZE>
void LogBuffer<SIZE>::Reset() noexcept
{
    cur_ = data_.data();
}

// 清空缓冲区内容
template <std::size_t SIZE>
void LogBuffer<SIZE>::Clear() noexcept
{
    std::memset(data_.data(), 0, SIZE);
    cur_ = data_.data();
}

// 获取末尾地址
template <std::size_t SIZE>
const char* LogBuffer<SIZE>::GetEndAddr() const noexcept
{
    return data_.data() + SIZE;
}

#endif
