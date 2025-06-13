#ifndef LATCH_H
#define LATCH_H


#include <thread>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <cassert>
#include <atomic>

#include "Common.h"

// Latch: 线程同步原语
// 1. 多线程调用 notify() 递减 count_
// 2. 主线程调用 wait() 或 WaitFor() 等待 count_ == 0

class Latch
{
private:
    std::mutex mtx_;
    std::condition_variable cv_;
    std::atomic<int> count_;

public:
    DISALLOW_COPY_AND_MOVE(Latch);

    explicit Latch(int count): count_(count)
    {
        assert(count >= 0);  // count 不能小于 0
    }

    // 等待直到 count_ == 0
    void Wait()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        while (count_ > 0)
        {
            cv_.wait(lock);
        }
    }

    // 等待直到 count_ == 0，或者超时，返回 true 表示成功归零，false 表示超时
    template<typename Rep, typename Period>
    bool WaitFor(const std::chrono::duration<Rep, Period>& timeout)
    {
        std::unique_lock<std::mutex> lock(mtx_);
        return cv_.wait_for(lock, timeout, [this]() { return count_ == 0; });
    }

    // 通知：递减 count_，归零后唤醒所有 wait() 线程
    void Notify()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        if (count_ > 0)
        {
            --count_;
            if (count_ == 0)
            {
                cv_.notify_all();
            }
        }
    }

    // 返回当前 count_，用于监控 / 调试
    int Count()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return count_;
    }
};

#endif
