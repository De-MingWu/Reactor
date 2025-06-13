#ifndef LEARN_BUFFER_H
#define LEARN_BUFFER_H

#include <vector>
#include <string>
#include <cassert>
#include <cstring>

// 高性能环形缓冲区，适用于 muduo 网络库数据收发
class Buffer 
{
public:
    static const size_t kCheapPrepend = 8;      // 预留头部空间，方便 prepend 写入协议头等数据
    static const size_t kInitialSize = 4096;    // 初始缓冲区大小

    explicit Buffer(size_t initialSize = kInitialSize);
    ~Buffer() = default;

    // 可读字节数（writer - reader）
    size_t ReadableBytes() const;
    // 可写字节数（缓冲区剩余空间）
    size_t WritableBytes() const;
    // 可预写字节数（reader 之前的区域）
    size_t PrependableBytes() const;

    // 返回读指针（Peek 表示读取但不取出）
    const char* Peek() const;

    // 取出 len 字节
    void Retrieve(size_t len);
    void RetrieveUntil(const char* end);         // 取出直到指定位置（[Peek, end)）
    void RetrieveAll();                          // 取出全部数据，重置索引
    std::string RetrieveAsString(size_t len);    // 读取并返回为字符串
    std::string RetrieveAllAsString();           // 全部读取为字符串

    // 向缓冲区追加数据
    void Append(const char* data, size_t len);
    void Append(const std::string& str);
    void Append(const void* data, size_t len);

    // 确保写空间充足，必要时自动扩容或整理
    void EnsureWritableBytes(size_t len);
    char* BeginWrite();                     // 返回写指针
    const char* BeginWrite() const;
    void HasWritten(size_t len);           // 写入数据后更新写索引

    // 向前插入数据（如协议头），前提是有足够的 prepend 空间
    void Prepend(const void* data, size_t len);

    // 返回缓冲区底层起始地址（通常用于调试或底层系统调用）
    const char* Begin() const;
    char* Begin();

    // 收缩 buffer 容量，只保留可读区 + 预留空间
    void Shrink(size_t reserve);

private:
    // 为写入腾出空间：若空间不足则整理或扩容
    void MakeSpace(size_t len);

    std::vector<char> buffer_;       // 实际缓冲区存储
    size_t readerIndex_;            // 当前读指针
    size_t writerIndex_;            // 当前写指针
};


#endif //LEARN_BUFFER_H
