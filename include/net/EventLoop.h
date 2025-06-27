#ifndef LEARN_EVENTLOOP_H
#define LEARN_EVENTLOOP_H

#include <sys/timerfd.h>
#include <memory>
#include <queue>
#include <map>
#include <mutex>
#include <atomic>
#include <functional>

#include "Epoll.h"
#include "Connection.h"
#include "Common.h"

class Channel;
class Epoll;
class Connection;

using spConnection=std::shared_ptr<Connection>;

class EventLoop
{
private:
    int timeTvl_; //闹钟事件间隔，秒
    int timeOut_; //Connection对象超时时间， 秒
    std::unique_ptr<Epoll> ep_; // 每个事件循环只有一个Epoll
    std::function<void(EventLoop*)> epollTimeoutCallback_; // epoll_wait()超时的回调函数。
    pid_t threadID_;  //事件循环所在ID
    std::queue<std::function<void()>> taskQueue_; //事件循环线程被eventFd唤醒后执行的任务队列
    std::mutex mutex_; //任务队列同步互斥锁
    std::atomic<bool> callingFunctors_{false}; // 是否正在执行任务队列中的任务，防止无意义 wakeup
    int wakeEventFd_; //用于唤醒事件循环线程的eventfd
    std::unique_ptr<Channel> wakeChannel_; //eventFd的channel

    int timeFd_; //定时器的fd
    std::unique_ptr<Channel> timeChannel_;//定时器的Channel
    bool mainLoop_;  //true-是主事件循环，false-是从事件循环

    std::mutex cMutex_;                    //保护conns_的互斥锁
    std::map<int,spConnection> connects_;  //存放运行在该事件循环上全部的Connection对象
    std::function<void(int)> timeCallBack_;//删除TcpServer中超时的Connection对象，将被设置为TcpServer::RemoveConnection()
    std::atomic_bool stop_;                //初始值为false， 设置为true，表示停止事件循环

    // 1、在事件循环中增加map<int,spConnect> conns_容器，存放运行在该事件循环上全部的Connection对象
    // 2、如果闹钟时间到了，遍历conns_，判断每个Connection对象是否超时。
    // 3、如果超时了，从conns_中删除Connection对象；
    // 4、还需要从TcpServer.conns_中删除Connection对象。
    // 5、TcpServer和EventLoop的map容器需要加锁。
    // 6、闹钟时间间隔和超时时间参数化。

public:
    DISALLOW_COPY_AND_MOVE(EventLoop); // 禁止拷贝和复制
    EventLoop(bool mainLoop, int timeTval = 30, int timeOut = 80); // 在构造函数中创建Epoll对象ep_
    ~EventLoop();// 在析构函数中销毁ep_

    void RunLoop(); // 运行事件循环
    void StopEvent(); //停止事件运行

    void UpdateChannel(Channel *ch); // 把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件
    void RemoveChannel(Channel *ch);//从红黑树上删除channel
    void SetEpollTimeoutCallback(std::function<void(EventLoop*)> fn);  // 设置epoll_wait()超时的回调函数。

    bool IsInLoopThread(); //判断当前线程是否为事件循环线程

    //将任务添加到队列中
    void QueueInLoop(std::function<void()> fn);
    //用eventFd唤醒事件循环线程
    void WakeUp();
    //事件循环唤醒后执行函数
    void HandleWakeUp();

    //闹钟响时执行的函数
    void HandleTime();
    //将Connectiond对象保存在conns_中
    void NewConnection(spConnection conn);

    //设置TcpServer::RemoveConnection()函数
    void SetTimeCallBack(std::function<void(int)> fn);
};


#endif //LEARN_EVENTLOOP_H
