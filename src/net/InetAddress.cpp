#include "InetAddress.h"
// 监听fd构造函数
InetAddress::InetAddress(const std::string &ip, uint16_t port)
{
    addr_.sin_family = AF_INET; // IPv4网络协议的套接字类型。
    addr_.sin_addr.s_addr = inet_addr(ip.c_str()); // 服务端用于监听的ip地址。
    addr_.sin_port = htons(port);// 服务端用于监听的端口。
}
InetAddress::~InetAddress(){}

//返回字符串表示的地址
const char *InetAddress::GetIP() const
{
    return inet_ntoa(addr_.sin_addr);
}

// 返回整数表示的端口
uint16_t InetAddress::GetPort() const
{
    return ntohs(addr_.sin_port);
}

// 返回addr_成员的地址，转换成sockaddr
const sockaddr *InetAddress::GetAddr() const
{
    return (sockaddr*)&addr_;
}

//设置addr_的值
void InetAddress::SetAddr(sockaddr_in clientAddr){ addr_ = clientAddr; }
