#include "Buffer.h"
#include <iostream>
Buffer::Buffer(size_t initialSize)
       :buffer_(kCheapPrepend + initialSize),
        readerIndex_(kCheapPrepend),
        writerIndex_(kCheapPrepend) {}

// 可读区域大小 = 写指针 - 读指针
size_t Buffer::ReadableBytes() const 
{
    return writerIndex_ - readerIndex_;
}

// 可写区域大小 = 缓冲区总大小 - 写指针
size_t Buffer::WritableBytes() const 
{
    return buffer_.size() - writerIndex_;
}

// 可预写区域大小 = 当前读指针位置（前面的空区）
size_t Buffer::PrependableBytes() const 
{
    return readerIndex_;
}

// 返回可读区域首地址
const char* Buffer::Peek() const 
{
    return Begin() + readerIndex_;
}

// 取出 len 字节（移动读指针）
void Buffer::Retrieve(size_t len) {
    assert(len <= ReadableBytes());
    if (len < ReadableBytes()) 
    {
        readerIndex_ += len;
    } 
    else 
    {
        RetrieveAll();
    }
}

// 取出到指定 end 指针为止
void Buffer::RetrieveUntil(const char* end) {
    assert(Peek() <= end && end <= Begin() + writerIndex_);
    Retrieve(end - Peek());
}

// 清空可读区域，重置读写指针
void Buffer::RetrieveAll() {
    readerIndex_ = kCheapPrepend;
    writerIndex_ = kCheapPrepend;
}

// 读取 len 字节为字符串，并从缓冲区移除
std::string Buffer::RetrieveAsString(size_t len) 
{
    assert(len <= ReadableBytes());
    std::string result(Peek(), len);
    Retrieve(len);
    return result;
}

// 读取全部为字符串
std::string Buffer::RetrieveAllAsString() 
{
    return RetrieveAsString(ReadableBytes());
}

// 向缓冲区尾部写入数据（内存拷贝）
void Buffer::Append(const char* data, size_t len) 
{
    EnsureWritableBytes(len);
    std::memcpy(BeginWrite(), data, len);
    HasWritten(len);
}

void Buffer::Append(const std::string& str) 
{
    Append(str.data(), str.size());
}

void Buffer::Append(const void* data, size_t len) 
{
    Append(static_cast<const char*>(data), len);
}

// 确保写空间充足（不足则整理或扩容）
void Buffer::EnsureWritableBytes(size_t len) 
{
    if (WritableBytes() < len) 
    {
        MakeSpace(len);
    }
    assert(WritableBytes() >= len);
}

// 写指针
char* Buffer::BeginWrite() 
{
    return Begin() + writerIndex_;
}

const char* Buffer::BeginWrite() const 
{
    return Begin() + writerIndex_;
}

// 写完数据后更新写指针
void Buffer::HasWritten(size_t len) 
{
    writerIndex_ += len;
}

// 向前写入数据（仅限于可预写区域）
void Buffer::Prepend(const void* data, size_t len) 
{
    assert(len <= PrependableBytes());
    readerIndex_ -= len;
    std::memcpy(Begin() + readerIndex_, data, len);
}

// 返回 buffer 首地址
const char* Buffer::Begin() const 
{
    return &*buffer_.begin();
}

char* Buffer::Begin() 
{
    return &*buffer_.begin();
}

// 整理或扩容：优先回收空间，其次 resize 扩容
void Buffer::MakeSpace(size_t len) 
{
    if (WritableBytes() + PrependableBytes() < len + kCheapPrepend) {
        // 空间仍不足，扩容
        buffer_.resize(writerIndex_ + len);
    } 
    else 
    {
        // 整理数据到前方
        size_t readable = ReadableBytes();
        std::memmove(Begin() + kCheapPrepend, Peek(), readable);
        readerIndex_ = kCheapPrepend;
        writerIndex_ = readerIndex_ + readable;
    }
}

// 缩小 buffer 大小，仅保留需要的数据和预留空间
void Buffer::Shrink(size_t reserve) 
{
    std::vector<char> buf(kCheapPrepend + ReadableBytes() + reserve);
    std::memcpy(buf.data() + kCheapPrepend, Peek(), ReadableBytes());
    buffer_.swap(buf);
    writerIndex_ = kCheapPrepend + ReadableBytes();
    readerIndex_ = kCheapPrepend;
}
