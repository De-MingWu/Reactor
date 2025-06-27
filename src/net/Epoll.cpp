#include <unistd.h>

#include "Epoll.h"

Epoll::Epoll()
{
    if((epollFd_ = epoll_create(1)) == -1)
    {
        printf("epoll_create() failed(%d).\n", errno);
        exit(-1);
    }
}

Epoll::~Epoll(){::close(epollFd_);}

/*
void Epoll::AddFd(int fd, uint32_t op)
{
    epoll_event ev;// 声明事件的数据结构
    ev.data.fd=fd; // 指定事件的自定义数据，会随着epoll_wait()返回的事件一并返回
    ev.events=op;  // 让epoll监视fd的

    // 把需要监视的fd和它的事件加入epollfd中
    if (epoll_ctl(epollFd_,EPOLL_CTL_ADD,fd,&ev) == -1)
    {
        printf("epoll_ctl() failed(%d).\n", errno);
        exit(-1);
    }
}*/

//将channel添加、更新到红黑树上，channel中有fd，也有需要监视的事件
void Epoll::UpdateChannel(Channel *ch)
{
    epoll_event ev;  //声明事件的数据结构
    ev.data.ptr = ch;//指定channel
    ev.events = ch->GetEvents();//指定的事件

    //判断channel是否在红黑树上，不在就添加
    if(ch->GetInpoll())
    {
        if(epoll_ctl(epollFd_, EPOLL_CTL_MOD, ch->GetFd(), &ev) == -1)
        {
            printf("epoll_ctl failed(%d).\n", errno);
            exit(-1);
        }
    }
    else //不在树上
    {
        if(epoll_ctl(epollFd_, EPOLL_CTL_ADD, ch->GetFd(), &ev) == -1)
        {
            printf("epoll_ctl failed(%d).\n", errno);
            exit(-1);
        }
        ch->SetInEpoll();
    }
}

// 从红黑树上删除channel
void Epoll::RemoveChannel(Channel *ch)
{
    //判断channel是否在红黑树上，在就删除
    if(ch->GetInpoll())
    {
        if(epoll_ctl(epollFd_, EPOLL_CTL_DEL, ch->GetFd(), 0) == -1)
        {
            printf("epoll_ctl failed(%d).\n", errno);
            exit(-1);
        }
    }
}

// 运行epoll_wait()，等待事件的发生，已发生的事件用vector容器返回。
std::vector<Channel *> Epoll::Loop(int timeOut)
{
    std::vector<Channel *> channels;        // 存放epoll_wait()返回的事件。

    bzero(events_,sizeof(events_));
    int infds = epoll_wait(epollFd_,events_, MAXEVENTS, timeOut);       // 等待监视的fd有事件发生。

    // 返回失败。
    if (infds < 0)
    {
        // EBADF  ：epfd不是一个有效的描述符。
        // EFAULT ：参数events指向的内存区域不可写。
        // EINVAL ：epfd不是一个epoll文件描述符，或者参数maxevents小于等于0。
        // EINTR  ：阻塞过程中被信号中断，epoll_pwait()可以避免，或者错误处理中，解析error后重新调用epoll_wait()。
        // 在Reactor模型中，不建议使用信号，因为信号处理起来很麻烦，没有必要。------ 陈硕
        perror("epoll_wait() failed");
        exit(-1);
    }

    // 超时。
    if (infds == 0)
    {
        // 如果epoll_wait()超时，表示系统很空闲，返回的channels将为空。
        //printf("epoll_wait() timeout.\n");
        return channels ;
    }

    // 如果infds>0，表示有事件发生的fd的数量。
    // 遍历epoll返回的数组events_
    for (int i = 0; i < infds; ++i)
    {
        Channel *ch = (Channel *)events_[i].data.ptr; //取出已发生事件的channel
        ch->SetRevents(events_[i].events);        //设置channel中revents_成员值
        channels.push_back(ch);
    }

    return channels;
}

