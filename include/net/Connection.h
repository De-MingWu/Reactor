#ifndef LEARN_CONNECTION_H
#define LEARN_CONNECTION_H

#include <memory>

#include "EventLoop.h"
#include "Buffer.h"
#include "Socket.h"
#include "TimeStamp.h"
#include "Common.h"

class Channel;
class EventLoop;
class Connection;

using spConnection = std::shared_ptr<Connection>;

class Connection:public std::enable_shared_from_this<Connection>
{
private:
    EventLoop *loop_;       // Connection对应的事件循环，在构造函数中传入
    std::unique_ptr<Socket> clientSock_;    // 与客户端通讯的Socket
    std::unique_ptr<Channel> clientChannel_;// Connection对应的channel，在构造函数中创建
    std::unique_ptr<Buffer> inputBuffer_;  // 接收缓冲区
    std::unique_ptr<Buffer> outputBuffer_; // 发送缓冲区
    std::atomic_bool disConnect_; // 客户端连接是否已断开， 如果已断开，则设为true

    std::function<void(spConnection)> closeCallBack_; // 关闭fd_的回调函数，将回调TcpServer::CloseConnection()
    std::function<void(spConnection)> errorCallBack_; // fd_发生了错误的回调函数，将回调TcpServer::ErrorConnection()
    std::function<void(spConnection,std::string&)> handleMessageCallback_;   // 处理报文的回调函数，将回调TcpServer::onmessage()
    std::function<void(spConnection)> sendCompleteCallback_; // 发送完成后，回调TcpServer类 SendComplete()函数

    TimeStamp lastTime_; // 时间戳，创建Connection对象时为当前时间，每接收到一个报文，把时间戳更新为当前时间

<<<<<<< HEAD
    // 保存http请求context上下文
    std::shared_ptr<void> context_;

=======
>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14
public:
    DISALLOW_COPY_AND_MOVE(Connection);
    Connection(EventLoop* loop, std::unique_ptr<Socket> clientSock);
    ~Connection();

    void Tie();
    int GetFd() const;
    std::string GetIP() const;
    uint16_t GetPort() const;

    void HandleMessage(); // 处理对端发送过来的消息
    void HttpClose(); // http服务端主动断开连接

    void CloseCallBack(); // TCP连接关闭（断开）的回调函数，供Channel回调
    void ErrorCallBack(); // TCP连接错误的回调函数，供Channel回调
    void WriteCallback(); // 处理写事件的回调函数，供Channel回调

    void SetCloseCallBack(const std::function<void(spConnection)>& fn);// 设置回调connection类 CloseCallBack()函数 回调值
    void SetErrorCallBack(const std::function<void(spConnection)>& fn);// 回调connection类 ErrorCallBack()函数 回调值
    void SetHandleMessageCallback(std::function<void(spConnection, std::string&)> fn);// 设置处理报文的回调函数
    void SetSendCompleteCallback(std::function<void(spConnection)> fn);// 发送数据完成后的回调函数

    // 发送数据，不论再那种线程中都调用此函数发送数据
    void SendData(const char *data, size_t size);
    // 发送数据, 如果为IO线程直接调用，工作线程则将此函数传递给IO线程
<<<<<<< HEAD
    void SendDataByThread(const std::string &data);
=======
    void SendDataByThread(const char *data, size_t size);
>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14

    // 连接是否已断开
    bool IsCloseConnection();
    // 判断TCP连接是否超时（空闲太久）
    bool IsTimeOut(time_t nowTime, int val);
<<<<<<< HEAD

    // 修改context相关方法
    void SetContext(const std::shared_ptr<void>& context);
    const std::shared_ptr<void>& GetContext() const;
=======
>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14
};


#endif //LEARN_CONNECTION_H
