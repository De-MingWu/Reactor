#ifndef LEARN_ACCEPTOR_H
#define LEARN_ACCEPTOR_H

#include <memory>
#include <functional>

#include "EventLoop.h"
#include "Connection.h"
#include "Common.h"

class Acceptor
{
private:
    EventLoop *loop_;       // Acceptor对应的事件循环，在构造函数中传入
    Socket servSock_;      // 服务端用于监听的socket，在构造函数中创建
    std::unique_ptr<Channel> acceptChannel_;// Acceptor对应的channel，在构造函数中创建
    std::function<void(std::unique_ptr<Socket>)> newConnectioncb_;   // 处理新客户端连接请求的回调函数，将指向TcpServer::newconnection()

public:
    DISALLOW_COPY_AND_MOVE(Acceptor);
    Acceptor(EventLoop *loop,const std::string &ip,const uint16_t port);
    ~Acceptor();

    void NewConnection();   // 处理新客户端连接请求

    // 设置处理新客户端连接请求的回调函数，将在创建Acceptor对象的时候（TcpServer类的构造函数中）设置。
    void SetNewConnectionCB(std::function<void(std::unique_ptr<Socket>)> fn);
};


#endif //LEARN_ACCEPTOR_H
