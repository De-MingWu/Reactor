#include <cassert>
#include "Acceptor.h"

Acceptor::Acceptor(EventLoop *loop,const std::string &ip,const uint16_t port)
         :loop_(loop), servSock_(CreateNonBlocking())
{
    //servSock_ = new Socket(CreateNonBlocking());
    // 服务端地址以及协议
    InetAddress servAddr(ip, port);

    // 设置listenfd的属性，
    servSock_.SetReuseAddr(true);
    servSock_.SetReusePort(true);
    servSock_.SetTCPNoDelay(true);
    servSock_.SetKeepAlive(true);

    // 绑定
    servSock_.Bind(servAddr);
    // 监听
    servSock_.Listen();

<<<<<<< HEAD
    acceptChannel_ = std::unique_ptr<Channel>(new Channel(loop_, servSock_.GetFd()));
=======
    acceptChannel_ = std::make_unique<Channel>(loop_, servSock_.GetFd());
>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14
    acceptChannel_->SetReadCallBack(std::bind(&Acceptor::NewConnection, this));
    // 监听listenFd 读事件，采用水平触发
    acceptChannel_->EnableReading();
}

Acceptor::~Acceptor(){ }

// 处理新客户端连接请求
void Acceptor::NewConnection()
{
    // 客户端地址信息
    InetAddress clientAddr;
    // 客户端的fd只能new，不能放在栈上，否则析构函数会关闭fd
    std::unique_ptr<Socket> clientSock(new Socket(servSock_.Accept(clientAddr)));

    clientSock->SetIPAndPort(clientAddr.GetIP(), clientAddr.GetPort());
    //printf("Acceptor: clientSock address move before = %p\n", clientSock.get());
    //为新客户端准备读事件，并添加到epoll中
    //Connection *conn = new Connection(loop_, clientSock);
    newConnectioncb_(std::move(clientSock));      // 回调TcpServer::newconnection()
}

// 设置处理新客户端连接请求的回调函数。
void Acceptor::SetNewConnectionCB(std::function<void(std::unique_ptr<Socket>)> fn)
{
    newConnectioncb_ = fn;
}
