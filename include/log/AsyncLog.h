#ifndef ASYNCLOG_H
#define ASYNCLOG_H

#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <fstream>

#include "Latch.h"
#include "LogBuffer.h"

// 日志缓冲区尺寸配置（8MB 大缓冲区）
static constexpr std::size_t FixedLargeBuffferSize = 8 * 1024 * 1024;

// 日志系统相关参数
static constexpr double BufferWriteTimeout = 3.0;                 // 超时刷盘时间（秒）
static constexpr std::size_t FileMaximumSize = 1024LL * 1024 * 1024;  // 单文件最大日志体积，1GB

// 异步日志类：用于将前端日志缓存异步写入磁盘文件
class AsyncLog 
{
public:
    using Buffer = LogBuffer<FixedLargeBuffferSize>;  // 定义大缓冲区类型

    // 构造函数：日志文件路径可传入，默认空路径（stdout）
    explicit AsyncLog(const char* filepath = nullptr);

    // 析构函数：确保停止线程并清理资源
    ~AsyncLog();

    // 启动后台写入线程
    void Start()  ;

    // 停止线程并强制写入未刷缓冲
    void Stop()  ;

    // 提供线程安全的 Append 接口
    void Append(const char* data, std::size_t len)  ;

    // 立即刷新日志缓冲区
    void Flush()  ;

private:
    void ThreadFunc()  ;  // 线程主逻辑函数

private:
    std::atomic<bool> running_;           // 控制写线程是否运行
    const char* filepath_;                // 日志输出路径

    std::mutex mutex_;                    // 用于保护 current_/next_/buffers_
    std::condition_variable cv_;          // 通知写线程刷新日志

    Latch latch_;                         // 保证线程就绪的同步机制
    std::thread thread_;                  // 后台日志线程

    std::unique_ptr<Buffer> current_;  // 当前缓冲区
    std::unique_ptr<Buffer> next_;     // 预留的备用缓冲区
    std::vector<std::unique_ptr<Buffer>> buffers_;  // 已满等待刷盘的缓冲集合
};

#endif  // ASYNCLOG_H
