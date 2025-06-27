#ifndef LEARN_CHANNEL_H
#define LEARN_CHANNEL_H

#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Common.h"

class EventLoop;

class Channel
{
private:
    int fd_ = -1;          // Channel拥有的fd，Channel和fd是一对一的关系。
    EventLoop *loop_ = nullptr;  // Channel对应的事件循环，Channel与EventLoop是多对一的关系，一个Channel只对应一个EventLoop。
    bool inEpoll_ = false; // Channel是否已添加到epoll树上，如果未添加，调用epoll_ctl()的时候用EPOLL_CTL_ADD，否则用EPOLL_CTL_MOD。
    uint32_t events_ = 0;  // fd_需要监视的事件。listenfd和clientfd需要监视EPOLLIN，clientfd还可能需要监视EPOLLOUT。
    uint32_t revents_ = 0; // fd_已发生的事件

    std::weak_ptr<void> tie_;  // weak_ptr 防止循环引用，保护 Connection 生命周期
    bool tied_ = false;        // 标记是否已 Tie 过

    std::function<void()> readCallBack_;// fd_读事件的回调函数，如果是acceptchannel，将回调Acceptor::newconnection()，如果是clientchannel，将回调Channel::onmessage()
    std::function<void()> closeCallBack_; //关闭fd_的回调函数，将回调Connection::closecallback()
    std::function<void()> errorCallBack_; //fd_发生了错误的回调函数，将回调Connection::errorcallback()
    std::function<void()> writeCallback_; //回调connection类 WriteCallback()函数

public:
    DISALLOW_COPY_AND_MOVE(Channel); // 禁止拷贝和复制
    Channel(EventLoop* loop, int fd); // 构造函数。
    ~Channel();                // 析构函数
    int GetFd() const;       //返回 fd_ 成员
    void EnableET();         // 设置边缘触发
    void EnableReading(); // 令epoll_wait()监听fd_读事件
    void DisableReading();// 取消监听读事件
    void EnableWriting(); // 令epoll_wait()监听fd_写事件
    void DisableWriting();// 取消监听写事件
    void DisableAll();    // 取消全部事件
    void RemoveChannel(); // 从事件循环中删除channel
    void SetInEpoll();    // 设置inEpoll_为 true
    void SetRevents(uint32_t ev); // 设置revents_成员参数为ev
    bool GetInpoll() const;     // 获取inepoll_的值
    uint32_t GetEvents(); // 获取events_的值
    uint32_t GetRevents();// 获取revents_的值

    void HandleEvent(); // 事件处理函数，epoll_wait()返回的时候，执行它
    void HandleEventWithGuard();// 真正处理事件逻辑，保证调用期间对象有效
    
    // 绑定shared_ptr对象，保证HandleEvent期间对象不被释放
    void Tie(const std::shared_ptr<void>& obj);

    void SetReadCallBack(std::function<void()> fn); // 设置fd_读事件的回调函数
    void SetCloseCallBack(std::function<void()> fn);//设置回调connection类 CloseCallBack()函数 回调值
    void SetErrorCallBack(std::function<void()> fn);//回调connection类 ErrorCallBack()函数 回调值
    void SetWriteCallback(std::function<void()> fn); // 设置写事件的回调函数。

};


#endif //LEARN_CHANNEL_H
