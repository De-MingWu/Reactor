#include <sys/syscall.h> 
#include <netinet/tcp.h>   // TCP_NODELAY
#include <arpa/inet.h>// inet_ntoa, htons
#include <unistd.h>

#include "Socket.h"

// 创建非阻塞的 socket套接字
int CreateNonBlocking()
{
    int listenFd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, IPPROTO_TCP);
    if(listenFd < 0)
    {
        printf("%s: %s: %d listen socket create error: %d\n", __FILE__, __FUNCTION__, __LINE__, errno);
        exit(-1);
    }
    return listenFd;
}

Socket::Socket(int fd):fd_(fd){};
Socket::~Socket(){ ::close(fd_);}

int Socket::GetFd() const{ return fd_; }

std::string Socket::GetIP() const{ return IP_; }

uint16_t Socket::GetPort() const { return port_; }

//设置SO_REUSEADDR选项
void Socket::SetReuseAddr(bool flage)
{
    int optval = flage ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

//设置SO_REUSEPORT选项
void Socket::SetReusePort(bool flage)
{
    int optval = flage ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
}

// 设置TCP_NODELAY选项
void Socket::SetTCPNoDelay(bool flage)
{
    int optval = flage ? 1 : 0;
    ::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

//设置SO_KEEPALIVE选项
void Socket::SetKeepAlive(bool flage)
{
    int optval = flage ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}
//服务端的socket将调用此函数 绑定socket
void Socket::Bind(const InetAddress& servAddr)
{
    if(::bind(fd_, servAddr.GetAddr(), sizeof(servAddr)) < 0)
    {
        perror("bind() failed");
        ::close(fd_);
        exit(-1);
    }
    SetIPAndPort(servAddr.GetIP(), servAddr.GetPort());
}
// 设置ip_和port_成员。
void Socket::SetIPAndPort(const std::string &ip, uint16_t port)
{
    IP_ = ip;
    port_ = port;
}

//服务端的socket将调用此函数 监听事件
void Socket::Listen(int n)
{
    if(listen(fd_, n) != 0) //在高并发的网络服务器中，第二个参数要大一些。
    {
        perror("listen() failed");
        ::close(fd_);
        exit(-1);
    }
}

//服务端的socket将调用此 接受连接
int Socket::Accept(InetAddress& clientAddr)
{
    sockaddr_in peerAddr;
    socklen_t len = sizeof(peerAddr);
    //accept4() 中SOCK_NONBLOCK可将fd设为非阻塞
    int clientFd = accept4(fd_, (struct sockaddr*)&peerAddr, &len, SOCK_NONBLOCK);
    clientAddr.SetAddr(peerAddr);

    return clientFd;
}
