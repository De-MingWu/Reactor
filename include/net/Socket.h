#ifndef LEARN_SOCKET_H
#define LEARN_SOCKET_H

#include <string>

#include "InetAddress.h"

// 创建非阻塞的 socket套接字
int CreateNonBlocking();

class Socket
{
private:
    const int fd_;
    std::string IP_;    // 如果是listenfd，存放服务端监听的ip，如果是客户端连接的fd，存放对端的ip
    uint16_t port_;// 如果是listenfd，存放服务端监听的port，如果是客户端连接的fd，存放外部端口
public:
    Socket(int fd);
    ~Socket();

    int GetFd() const;
    std::string GetIP() const;
    uint16_t GetPort() const;
    void SetIPAndPort(const std::string &ip, uint16_t port); //设置IP和端口

    void SetReuseAddr(bool flage);  // 设置SO_REUSEADDR选项，true-打开，false-关闭
    void SetReusePort(bool flage);  // 设置SO_REUSEPORT选项
    void SetTCPNoDelay(bool flage); // 设置TCP_NODELAY选项
    void SetKeepAlive(bool flage);  // 设置SO_KEEPALIVE选项
    void Bind(const InetAddress& servAddr);//服务端的socket将调用此函数 绑定socket
    void Listen(int n = 128);              //服务端的socket将调用此函数 监听事件
    int Accept(InetAddress& clientAddr);   //服务端的socket将调用此 接受连接

};


#endif //LEARN_SOCKET_H
