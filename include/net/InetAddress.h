#ifndef LEARN_INETADDRESS_H
#define LEARN_INETADDRESS_H

#include <netinet/in.h>  // sockaddr_in 的定义
#include <arpa/inet.h>   // inet_pton 等
#include <string>
#include <string.h>

class InetAddress
{
private:
    sockaddr_in addr_; //表示地址协议结构体
public:
    InetAddress(){};
    InetAddress(const std::string &ip, uint16_t port); // 监听fd构造函数
    InetAddress(const sockaddr_in addr): addr_(addr){}; // 客户端连接过来的fd 构造函数
    ~InetAddress();

    const char *GetIP() const; //返回字符串表示的地址
    uint16_t GetPort() const;  // 返回整数表示的端口
    const sockaddr *GetAddr() const; // 返回addr_成员的地址，转换成sockaddr
    void SetAddr(sockaddr_in clientAddr); //设置addr_的值
};


#endif //LEARN_INETADDRESS_H
