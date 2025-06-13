#include "Channel.h"

Channel::Channel(EventLoop *loop,int fd):loop_(loop),fd_(fd){}

/**
 *  在析构函数中，不要销毁ep_，也不能关闭fd_，
 *  因为这两个东西不属于Channel类，Channel类只是需要它们，使用它们而已
 */
Channel::~Channel(){}

//返回 fd_ 成员
int Channel::GetFd()const{return fd_;}

// 设置边缘触发
void Channel::EnableET(){events_ = events_|EPOLLET;}

/**
 * 令epoll_wait()监听fd_读事件
 */
void Channel::EnableReading()
{
    events_ |= EPOLLIN;
    loop_->UpdateChannel(this);
}

// 取消监听读事件
void Channel::DisableReading()
{
    events_ &= ~EPOLLIN;
    loop_->UpdateChannel(this);
}

// 令epoll_wait()监听fd_写事件
void Channel::EnableWriting()
{
    events_ |= EPOLLOUT;
    loop_->UpdateChannel(this);
}

// 取消监听写事件
void Channel::DisableWriting()
{
    events_ &= ~EPOLLOUT;
    loop_->UpdateChannel(this);
}

// 取消全部事件
void Channel::DisableAll()
{
    events_ = 0;
    loop_->UpdateChannel(this);
}

// 从事件循环中删除channel
void Channel::RemoveChannel()
{
    //先取消所有事件
    DisableAll();
    loop_->RemoveChannel(this); //从红黑树上删除fd
}

// 设置inEpoll_为 true
void Channel::SetInEpoll(){ inEpoll_ = true;}

// 设置revents_成员参数为ev
void Channel::SetRevents(uint32_t ev){ revents_ = ev; }

//获取inepoll_的值
bool Channel::GetInpoll() const{ return inEpoll_; }

// 获取events_的值
uint32_t Channel::GetEvents(){return events_;}

//获取revents_的值
uint32_t Channel::GetRevents(){ return revents_; }

// 事件处理，epoll_wait返回时调用
void Channel::HandleEvent()
{
    if (tied_) // 如果已绑定weak_ptr
    {
        std::shared_ptr<void> guard = tie_.lock();  // 尝试提升为shared_ptr
        if (guard)// 对象还存在，安全处理事件
            HandleEventWithGuard();
        // 对象已销毁，不处理事件，直接返回
    }
    else// 未绑定Tie，直接处理事件（兼容以前代码逻辑）
        HandleEventWithGuard();
}


// 真正处理事件逻辑，保证调用期间对象有效
void Channel::HandleEventWithGuard()
{
    //对方已关闭，有些系统检测不到，可以使用EPOLLIN，recv()返回0
    if (revents_ & EPOLLRDHUP)
    {
        closeCallBack_();
    }                        //普通数据  带外数据
    else if (revents_ & (EPOLLIN|EPOLLPRI))// 接收缓冲区中有数据可以读
    {
        readCallBack_();
    }
    else if (revents_ & EPOLLOUT) //有数据需要写入，暂时没有代码，以后再说。
    {
        writeCallback_();
    }
    else // 其它事件，都视为错误。
    {
        errorCallBack_();
    }
}

// 绑定shared_ptr对象，保证HandleEvent期间对象不被释放
void Channel::Tie(const std::shared_ptr<void>& obj)
{
    tie_ = obj;     // 保存weak_ptr
    tied_ = true;   // 标记已绑定
}


// 设置fd_读事件的回调函数
void Channel::SetReadCallBack(std::function<void()> fn)
{
    readCallBack_ = fn;
}

//设置回调connection类 CloseCallBack()函数 回调值
void Channel::SetCloseCallBack(std::function<void()> fn)
{
    closeCallBack_ = fn;
}

// 回调connection类 ErrorCallBack()函数 回调值
void Channel:: SetErrorCallBack(std::function<void()> fn)
{
    errorCallBack_ = fn;
}

// 设置写事件的回调函数。
void Channel::SetWriteCallback(std::function<void()> fn)
{
    writeCallback_ = fn;
}