#include "CurrentThread.h"
#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>

namespace CurrentThread
{
    // 线程局部变量，线程安全
    __thread int t_cachedTid = 0;
    __thread char t_formattedTid[32];
    __thread int t_formattedTidLength = 0;

    // 获取真实线程 ID，供日志使用
    pid_t GetTid() 
    {
        return static_cast<pid_t>(syscall(SYS_gettid));
    }

    // 缓存线程 ID，避免频繁系统调用
    void CacheTid()
    {
        if (t_cachedTid == 0)
        {
            t_cachedTid = gettid();
            t_formattedTidLength = snprintf(t_formattedTid, sizeof(t_formattedTid), "%5d ", t_cachedTid);
        }
    }
}
