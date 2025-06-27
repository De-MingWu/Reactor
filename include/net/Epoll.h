#ifndef LEARN_EPOLL_H
#define LEARN_EPOLL_H

#include <sys/epoll.h>
#include <vector>

#include "Common.h"
#include "Channel.h"

class Channel;

class Epoll
{
private:
    static const int MAXEVENTS = 100; //epoll_wait()返回事件数组大小
    int epollFd_ = -1;                //epoll句柄，在构造函数中创建
    epoll_event events_[MAXEVENTS];   //存放epoll_wait()返回事件的数组，在构造函数中分配内存
public:
    DISALLOW_COPY_AND_MOVE(Epoll); // 禁止拷贝和复制
    Epoll();
    ~Epoll();

    //将fd添加、更新到红黑树上
    //void AddFd(int fd, uint32_t op);

    void UpdateChannel(Channel *ch);//将channel添加、更新到红黑树上，channel中有fd，也有需要监视的事件。
    void RemoveChannel(Channel *ch);//从红黑树上删除channel

    //运行epoll_wait()，等待事件发生，已发生的事件用vector容器返回
    std::vector<Channel *> Loop(int timeOut = -1);
};

#endif //LEARN_EPOLL_H
