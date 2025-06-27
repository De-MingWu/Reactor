#ifndef LEARN_TCPSERVER_H
#define LEARN_TCPSERVER_H

#include <map>
#include <mutex>
#include <memory>
#include <functional>

#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"
#include "Common.h"

class TcpServer
{
private:
    std::unique_ptr<EventLoop> mainLoop_; // 主事件循环
    std::vector<std::unique_ptr<EventLoop>> subLoops_; // 从事件循环
    std::unique_ptr<Acceptor> acceptor_; // 一个TcpServer只有一个Acceptor对象
    int threadNum_; // 线程池的大小，即从事件循环的个数
    ThreadPool *threadPool_; // 线程池
    int sep_;// 报文格式
    std::map<int, spConnection> conns_;  // 一个TcpServer有多个Connection对象，存放在map容器中

    std::mutex cMutex_; //保护conns_的互斥锁

    std::function<void(spConnection)> newConnectionCb_;                      // 回调EchoServer::HandleNewConnection()
    std::function<void(spConnection)> closeConnectionCb_;                    // 回调EchoServer::HandleClose()
    std::function<void(spConnection)> errorConnectionCb_;                    // 回调EchoServer::HandleError()
    std::function<void(spConnection,std::string &message)> handleMessageCb_; // 回调EchoServer::HandleMessage()
    std::function<void(spConnection)> sendCompleteCb_;                       // 回调EchoServer::HandleSendComplete()
    std::function<void(EventLoop*)>  timeOutCb_;                            // 回调EchoServer::HandleTimeOut()

    public:
    TcpServer(const std::string &ip, const uint16_t port, int threadNum = 3);
    ~TcpServer();

    void Start();   // 运行事件循环
    void StopService(); // 停止IO线程和事件循环

    void NewConnection(std::unique_ptr<Socket> clientSock);   // 处理新客户端连接请求

    void CloseConnect(spConnection connect); //关闭客户端连接，在connection中回调此函数
    void ErrorConnect(spConnection connect); //客户端连接发生错误，在connection中回调此函数
    void HandleMessage(spConnection conn, std::string &message);// 处理客户端的请求报文，在Connection类中回调此函数
    void SendComplete(spConnection conn); // 数据发送完成后，在Connection类中回调此函数
    void EpollTimeout(EventLoop *loop);  // epoll_wait()超时，在EventLoop类中回调此函数

    //设置回调函数代码
    void SetNewConnectionCB(std::function<void(spConnection)> fn);
    void SetCloseConnectionCB(std::function<void(spConnection)> fn);
    void SetErrorConnectionCB(std::function<void(spConnection)> fn);
    void SetHandleMessageCB(std::function<void(spConnection, std::string &message)> fn);
    void SetSendCompleteCB(std::function<void(spConnection)> fn);
    void SetTimeOutCB(std::function<void(EventLoop *)> fn);

    //删除connects中的Connection对象, 在EventLoop::HandleTime()中回调
    void RemoveConnection(int fd);

};


#endif //LEARN_TCPSERVER_H
