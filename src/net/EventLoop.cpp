#include <sys/eventfd.h>
#include <sys/syscall.h>
#include <unistd.h> 

#include "EventLoop.h"

// 创建定时器fd
int CreatTimeFd(int sec = 30)
{
    int tFd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC|TFD_NONBLOCK);   //创建timerfd
    struct itimerspec timeOut;  //定时时间的数据结构。
    memset(&timeOut, 0, sizeof(struct itimerspec));
    timeOut.it_value.tv_sec = sec; //定时时间为5秒，调试方便，实际应该由类外传入。
    timeOut.it_value.tv_nsec = 0;
    timerfd_settime(tFd, 0, &timeOut, 0);//开始计时alarm(5)

    return tFd;
}

// 在构造函数中创建Epoll对象ep_
EventLoop::EventLoop(bool mainLoop, int timeTval, int timeOut)
          :ep_(new Epoll),
           mainLoop_(mainLoop),
           timeTvl_(timeTval),
           timeOut_(timeOut),
           stop_(false),
           wakeEventFd_(eventfd(0, EFD_NONBLOCK)),
           wakeChannel_(new Channel(this, wakeEventFd_)),
           timeFd_(CreatTimeFd(timeOut_)),
           timeChannel_(new Channel(this, timeFd_))
{
    wakeChannel_->SetReadCallBack(std::bind(&EventLoop::HandleWakeUp, this));
    wakeChannel_->EnableReading();

    timeChannel_->SetReadCallBack([this]() {HandleTime();});
    timeChannel_->EnableReading();
}

// 在析构函数中销毁ep_
EventLoop::~EventLoop(){}

// 运行事件循环。
void EventLoop::RunLoop()
{
    threadID_ = syscall(SYS_gettid);//获取事件循环所在id
    while (!stop_)        // 事件循环。
    {
        std::vector<Channel *> channels = ep_->Loop(10*1000);// 等待监视的fd有事件发生

        // 如果channels为空，表示超时，回调TcpServer::epolltimeout()。
        if (channels.size() == 0) epollTimeoutCallback_(this);
        else
        {
            for (auto &ch:channels)
            {
                ch->HandleEvent(); // 处理epoll_wait()返回的事件。
            }
        }
    }
}

/**
 * 停止事件运行
 */
void EventLoop::StopEvent()
{
    stop_ = true;
    //唤醒事件循环，否则事件循环将持续到下次闹钟时间或epoll_wait()超时才会停止
    WakeUp();
}


// 把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件
void EventLoop::UpdateChannel(Channel *ch) {ep_->UpdateChannel(ch);}

// 从红黑树上删除channel
void EventLoop::RemoveChannel(Channel *ch) { ep_->RemoveChannel(ch); }

// 设置epoll_wait()超时的回调函数。
void EventLoop::SetEpollTimeoutCallback(std::function<void(EventLoop *)> fn)
{
    epollTimeoutCallback_ = fn;
}

//判断当前线程是否为事件循环线程
bool EventLoop::IsInLoopThread()
{
    return threadID_ == syscall(SYS_gettid);
}

// 将任务添加到队列中
void EventLoop::QueueInLoop(std::function<void()> fn)
{
    //声明锁的作用域
    {
        std::lock_guard<std::mutex> gd(mutex_);//给任务加锁
        taskQueue_.push(fn);  //任务队列
    }
    
    // 如果不是 事件循环 线程，或者当前 事件循环 正在调用 HandleWakeUp，才需要 WakeUp
    // 防止本 事件循环 内反复 WakeUp
    if (!IsInLoopThread() || callingFunctors_)WakeUp();
}

/**
 * 用eventFd唤醒事件循环线程
 */
void EventLoop::WakeUp()
{
    uint64_t val = 1;
    ::write(wakeEventFd_, &val, sizeof(val));
}

/**
 * 事件循环唤醒后执行函数
 */
void EventLoop::HandleWakeUp()
{
    uint64_t val;
    ::read(wakeEventFd_, &val, sizeof(val));

    std::function<void()> fn;
    callingFunctors_ = true;  // 标记正在处理任务队列
    {
        std::lock_guard<std::mutex> gd(mutex_); //给任务队列加锁
        while(taskQueue_.size() > 0)
        {
            fn = std::move(taskQueue_.front()); //出队一个元素
            taskQueue_.pop();
            fn();//执行任务
        }
    }
    callingFunctors_ = false;
}

// 闹钟响时执行的函数
void EventLoop::HandleTime()
{
    struct itimerspec timeOut;  //定时时间的数据结构。
    memset(&timeOut, 0, sizeof(struct itimerspec));
    //timeOut.it_value.tv_sec = 5;
    timeOut.it_value.tv_sec = timeTvl_; //定时时间为5秒，调试方便，实际应该由类外传入。
    timeOut.it_value.tv_nsec = 0;
    timerfd_settime(timeFd_, 0, &timeOut, 0);//开始计时alarm(5)

    if (mainLoop_)
    {
        //printf("主事件循环的闹钟时间到了。\n");
    }
    else
    {
        //printf("从事件循环的闹钟时间到了。\n");
        //printf("EventLoop::handletime() thread is %ld. fd",syscall(SYS_gettid));
        time_t nowTime = time(0);
        std::lock_guard<std::mutex> lockGuard(cMutex_);
        for (auto it = connects_.begin(); it != connects_.end(); )
        {
            if (it->second->IsTimeOut(nowTime, timeOut_))
            {
                int connId = it->first;
                it = connects_.erase(it);  // 删除并前移迭代器
                timeCallBack_(connId);     // 回调放在锁外更安全（可选优化）
            }
            else
            {
                ++it;
            }
        }
    }
}

// 将Connectiond对象保存在conns_中
void EventLoop::NewConnection(spConnection connect)
{
    {
        std::lock_guard<std::mutex> lockGuard(cMutex_);
        connects_[connect->GetFd()] = connect;
    }
}

// 设置TcpServer::RemoveConnection()函数
void EventLoop::SetTimeCallBack(std::function<void(int)> fn)
{
    timeCallBack_ = fn;
}