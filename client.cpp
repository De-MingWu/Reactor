<<<<<<< HEAD

// #include <iostream>
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <errno.h>
// #include <string.h>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <time.h>

// using namespace std;

// int main(int argc, char *argv[])
// {
//     int cycle = 3;
//     if(argc != 3)
//     {
//         printf("usage:./client 172.21.153.82 5005\n");
//         return -1;
//     }

//     //1、创建客户端socket
//     struct sockaddr_in servaddr;
//     char buffer[1024];
//     int sockfd = socket(AF_INET, SOCK_STREAM, 0);
//     if(sockfd == -1)
//     {
//         perror("socker");
//         return -1;
//     }

//     memset(&servaddr, 0, sizeof(servaddr));
//     servaddr.sin_family = AF_INET;
//     servaddr.sin_addr.s_addr = inet_addr(argv[1]); //指定服务端 IP 地址
//     servaddr.sin_port = htons(atoi(argv[2])); //指定服务端通信端口

//     //向服务器发起连接请求
//     if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
//     {
//         perror("connect");
//         close(sockfd);
//         return -1;
//     }
//     //与服务端通信，客户发送一个请求后等待服务端回复，收到回复后，发送下一个请求报文
//     for(int i = 0; i < 10; i++)
//     {
// //        memset(buffer, 0, sizeof(buffer));
// //        printf("please input:");
// //        scanf("%s", buffer);
//         memset(buffer, 0, sizeof(buffer));
//         sprintf(buffer, "这是第%d个超级女生。", i);

//         char tmpbuf[1024]; // 临时的buffer，报文头部+报文内容。
//         memset(tmpbuf, 0, sizeof(tmpbuf));
//         int len = strlen(buffer);  // 计算报文的大小。
//         memcpy(tmpbuf, &len, 4);// 拼接报文头部。
//         memcpy(tmpbuf + 4, buffer, len);  // 拼接报文内容。

//         //想服务端发送请求报文
//         if((send(sockfd, tmpbuf, len + 4, 0)) <= 0)
//         {
//             perror("send");
//             break;
//         }
//         cout << "发送：" << buffer << endl;


//         recv(sockfd,&len, 4, 0);            // 先读取4字节的报文头部。

//         memset(buffer, 0, sizeof(buffer));
//         recv(sockfd, buffer, len, 0);           // 读取报文内容。

//         printf("recv: %s\n", buffer);sleep(1);
//     }

// }
=======
/**
  ******************************************************************************
  * @file           : client.cpp
  * @author         : wdm
  * @brief          : 客户端
  * @email          : wdm0311@zjnu.edu.cn
  * @date           : 24-8-3
  ******************************************************************************
  */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

using namespace std;

int main(int argc, char *argv[])
{
    int cycle = 3;
    if(argc != 3)
    {
        printf("usage:./client 172.21.153.82 5005\n");
        return -1;
    }

    //1、创建客户端socket
    struct sockaddr_in servaddr;
    char buffer[1024];
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        perror("socker");
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]); //指定服务端 IP 地址
    servaddr.sin_port = htons(atoi(argv[2])); //指定服务端通信端口

    //向服务器发起连接请求
    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
    {
        perror("connect");
        close(sockfd);
        return -1;
    }
    //与服务端通信，客户发送一个请求后等待服务端回复，收到回复后，发送下一个请求报文
    for(int i = 0; i < 10; i++)
    {
//        memset(buffer, 0, sizeof(buffer));
//        printf("please input:");
//        scanf("%s", buffer);
        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "这是第%d个超级女生。", i);

        char tmpbuf[1024]; // 临时的buffer，报文头部+报文内容。
        memset(tmpbuf, 0, sizeof(tmpbuf));
        int len = strlen(buffer);  // 计算报文的大小。
        memcpy(tmpbuf, &len, 4);// 拼接报文头部。
        memcpy(tmpbuf + 4, buffer, len);  // 拼接报文内容。

        //想服务端发送请求报文
        if((send(sockfd, tmpbuf, len + 4, 0)) <= 0)
        {
            perror("send");
            break;
        }
        cout << "发送：" << buffer << endl;


        recv(sockfd,&len, 4, 0);            // 先读取4字节的报文头部。

        memset(buffer, 0, sizeof(buffer));
        recv(sockfd, buffer, len, 0);           // 读取报文内容。

        printf("recv: %s\n", buffer);sleep(1);
    }

}
>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14

// post 测试
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        cout << "usage: ./client <IP> <port>\n";
        return -1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        return -1;
    }

    sockaddr_in servaddr{};
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0)
    {
        perror("inet_pton");
        close(sockfd);
        return -1;
    }

    if (connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) != 0)
    {
        perror("connect");
        close(sockfd);
        return -1;
    }

    string body = "username=admin&password=123456";
    string request = "POST /login HTTP/1.1\r\n"
                     "Host: " + string(ip) + "\r\n"
                     "Content-Type: application/x-www-form-urlencoded\r\n"
                     "Content-Length: " + to_string(body.size()) + "\r\n"
                     "Connection: close\r\n"   // 用close，服务器收到后一般会关闭连接
                     "\r\n" + body;

    ssize_t sent = send(sockfd, request.c_str(), request.size(), 0);
    if (sent <= 0)
    {
        perror("send");
        close(sockfd);
        return -1;
    }

    // 接收HTTP响应，先读头，解析Content-Length
    const int BUF_SIZE = 4096;
    char buffer[BUF_SIZE];
    string response;
    int content_length = -1;
    bool header_received = false;
    size_t pos = 0;

    while (true)
    {
        ssize_t n = recv(sockfd, buffer, BUF_SIZE - 1, 0);
        if (n == 0)
        {
            // 连接关闭，退出循环
            break;
        }
        else if (n < 0)
        {
            perror("recv");
            break;
        }

        buffer[n] = '\0';
        response.append(buffer);

        if (!header_received)
        {
            // 查找头部结束位置：\r\n\r\n
            pos = response.find("\r\n\r\n");
            if (pos != string::npos)
            {
                header_received = true;
                // 解析Content-Length
                size_t cl_pos = response.find("Content-Length:");
                if (cl_pos != string::npos)
                {
                    size_t start = cl_pos + strlen("Content-Length:");
                    size_t end = response.find("\r\n", start);
                    string cl_str = response.substr(start, end - start);
                    content_length = stoi(cl_str);
                }
                else
                {
                    // 没有Content-Length，简单处理为不确定长度，直接打印所有数据
                    content_length = -1;
                }
            }
        }

        if (header_received && content_length != -1)
        {
            // 计算响应体起始位置
            size_t body_start = pos + 4;
            size_t body_len = response.size() - body_start;

            if (body_len >= (size_t)content_length)
            {
                // 收到完整body，结束接收
                break;
            }
        }
    }

    cout << "====== HTTP RESPONSE ======\n";
    cout << response << endl;

    close(sockfd);
    return 0;
}
