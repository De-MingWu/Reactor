#ifndef CURRENTTHREAD_H
#define CURRENTTHREAD_H

#include <stdint.h>    // 定义标准整型，比如 int32_t
#include <pthread.h>   // pthread_self() 等 pthread API
#include <stdio.h>     // snprintf 用于格式化输出

namespace CurrentThread
{
    // 线程局部变量，__thread 修饰表示每个线程一份副本
    // 缓存本线程的 tid，减少频繁系统调用
    extern __thread int t_cachedTid;

    // 缓存 tid 格式化为字符串，供日志直接使用，避免每次 sprintf
    extern __thread char t_formattedTid[32];

    // 格式化字符串的长度，供日志使用
    extern __thread int t_formattedTidLength;

    // 函数声明，实际实现里会调用系统接口获取 tid 并缓存到 t_cachedTid
    void CacheTid();

    // 获取真正的 tid，通常是通过系统调用 gettid 实现
    pid_t GetTid();

    // 快速获取当前线程的 tid，带缓存逻辑
    inline int Tid() 
    {
        // __builtin_expect 是 GCC/Clang 内置，提示编译器：
        // t_cachedTid == 0 很少发生（即 if 很少成立），优化分支预测
        if (__builtin_expect(t_cachedTid == 0, 0)) 
        {
            CacheTid();  // 如果缓存没填，填一下
        }
        return t_cachedTid;
    }

    // 返回 tid 的字符串表示，供日志直接使用
    inline const char *TidString() { return t_formattedTid; }

    // 返回字符串长度
    inline int TidStringLength() { return t_formattedTidLength; }
}

#endif