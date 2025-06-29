#ifndef LEARN_ECHOSERVER_H
#define LEARN_ECHOSERVER_H

#include "TcpServer.h"
#include "EventLoop.h"
#include "Connection.h"
#include "ThreadPool.h"

class EchoServer
{
private:
    TcpServer tcpServer_;
    ThreadPool threadPool_; //工作线程池

public:
    EchoServer(const std::string &ip,const uint16_t port, int subThreadNum = 3, int workThreadNum = 5);
    ~EchoServer();

    void Start(); // 启动服务
    void StopService();//停止程序运行

    void HandleNewConnection(spConnection conn); // 处理新客户端连接请求，在TcpServer类中回调此函数
    void HandleClose(spConnection conn); // 关闭客户端的连接，在TcpServer类中回调此函数
    void HandleError(spConnection conn); // 客户端的连接错误，在TcpServer类中回调此函数
    void HandleMessage(spConnection conn,std::string &message); // 处理客户端的请求报文，在TcpServer类中回调此函数
    void HandleSendComplete(spConnection conn);// 数据发送完成后，在TcpServer类中回调此函数
    void HandleTimeOut(EventLoop *loop);// epoll_wait()超时，在TcpServer类中回调此函数

    void OnMessage(spConnection conn,std::string &message); // 处理客户端的请求报文，在TcpServer类中回调此函数
};


#endif //LEARN_ECHOSERVER_H
