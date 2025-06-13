#include "AsyncLog.h"
#include "LogFile.h"

#include <vector>
#include <memory>
#include <chrono>
#include <functional>

// 构造函数，初始化成员变量与缓冲区
AsyncLogging::AsyncLogging(const char *filepath)
             : running_(false),
               filepath_(filepath),
               latch_(1) // 用于等待后台线程启动
{
    current_ = std::make_unique<Buffer>(); // 当前活动写缓冲区
    next_ = std::make_unique<Buffer>();    // 预备写缓冲区
}

// 析构函数，确保日志线程被正常停止
AsyncLogging::~AsyncLogging()
{
    if (running_) 
    {
        Stop(); // 安全关闭日志线程
    }
}

// 启动后台日志线程
void AsyncLogging::Start()
{
    running_ = true;
    // 使用 std::bind 绑定类成员函数为线程入口
    thread_ = std::thread(std::bind(&AsyncLogging::ThreadFunc, this));
    latch_.Wait(); // 等待日志线程初始化完成
}

// 停止后台日志线程，并等待其安全退出
void AsyncLogging::Stop()
{
    running_ = false;
    cv_.notify_one(); // 唤醒线程以便退出
    if (thread_.joinable())
        thread_.join(); // 等待线程退出完成
}

// 手动刷新stdout缓冲区（若启用stdout输出可用）
void AsyncLogging::Flush()
{
    fflush(stdout);
}

// 线程安全的日志追加接口
void AsyncLogging::Append(const char *data, int len)
{
    bool needNotify = false; // 标志是否需要唤醒写线程

    {
        std::unique_lock<std::mutex> lock(mutex_);

        if (current_->GetAvailSpace() >= len) 
        {
            // 当前缓冲区有足够空间，直接写入
            current_->Append(data, len);
        } 
        else 
        {
            // 当前缓冲区满，转移至缓冲池等待写入
            buffers_.push_back(std::move(current_));

            // 启用备用缓冲区作为新的当前缓冲区
            if (next_) 
            {
                current_ = std::move(next_);
            } 
            else 
            {
                // 无备用缓冲区，则分配新的缓冲区
                current_ = std::make_unique<Buffer>();
            }

            // 将新日志写入新的 current_
            current_->Append(data, len);
            needNotify = true; // 唤醒写线程
        }
    }

    if (needNotify)
        cv_.notify_one(); // 通知写线程有新数据
}

// 异步后台日志线程主函数，负责写文件
void AsyncLogging::ThreadFunc()
{
    latch_.Notify(); // 通知主线程：“后台日志线程已启动”

    // 初始化备用缓冲区
    std::unique_ptr<Buffer> newCurrent = std::make_unique<Buffer>();
    std::unique_ptr<Buffer> newNext = std::make_unique<Buffer>();
    newCurrent->Clear();
    newNext->Clear();

    std::vector<std::unique_ptr<Buffer>> activeBuffers;

    // 初始化 LogFile（按当前小时自动生成路径）
    std::unique_ptr<LogFile> logfile = std::make_unique<LogFile>(nullptr);

    while (running_)
    {
        std::unique_lock<std::mutex> lock(mutex_);

        // 等待数据或超时
        if (buffers_.empty())
        {
            cv_.wait_until(lock, std::chrono::system_clock::now() +
                                    std::chrono::milliseconds(static_cast<int>(BufferWriteTimeout * 1000)));
        }

        // 当前缓冲区加入待写入队列
        buffers_.push_back(std::move(current_));

        // 将主缓冲队列与局部队列交换（减少锁持有时间）
        activeBuffers.swap(buffers_);

        // 复用备用缓冲区
        current_ = std::move(newCurrent);
        if (!next_)
            next_ = std::move(newNext);

        lock.unlock();

        // 写入所有日志数据（LogFile 内部会判断是否跨小时并自动切换日志文件）
        for (const auto& buffer : activeBuffers)
        {
            logfile->Write(buffer->GetDataAddr(), buffer->GetLen());
        }

        // 日志文件过大，手动轮换（不依赖小时）
        if (logfile->WrittenBytes() >= FileMaximumSize)
        {
            logfile.reset(new LogFile(nullptr)); // 新建日志文件（自动按当前小时命名）
        }

        // 缓冲区重用策略：保留最多2块备用缓冲区
        if (activeBuffers.size() > 2)
        {
            activeBuffers.resize(2);
        }

        // 复用旧缓冲区
        if (!newCurrent && !activeBuffers.empty())
        {
            newCurrent = std::move(activeBuffers.back());
            activeBuffers.pop_back();
            newCurrent->Clear();
        }

        if (!newNext && !activeBuffers.empty())
        {
            newNext = std::move(activeBuffers.back());
            activeBuffers.pop_back();
            newNext->Clear();
        }

        activeBuffers.clear(); // 清空本轮写入数据
    }

    // 写入剩余数据（退出时）
    if (current_ && current_->GetLen() > 0)
    {
        activeBuffers.push_back(std::move(current_));
    }

    activeBuffers.insert(activeBuffers.end(),
                         std::make_move_iterator(buffers_.begin()),
                         std::make_move_iterator(buffers_.end()));

    for (const auto& buffer : activeBuffers)
    {
        logfile->Write(buffer->GetDataAddr(), buffer->GetLen());
    }

    logfile->Flush(); // 最终刷写日志
}
