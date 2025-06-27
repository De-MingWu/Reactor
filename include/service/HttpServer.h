#ifndef HTTPSERVER_H
#define HTTPSERVER_H

<<<<<<< HEAD
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>
#include <functional>
#include <fstream>
#include <sstream>
#include <iostream>

=======
>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14
#include "TcpServer.h"       // 基于 epoll 的 TCP 服务器封装
#include "ThreadPool.h"      // 简易线程池，用于处理业务请求
#include "HttpResponse.h"    // HTTP 响应生成模块
#include "HttpContext.h"     // HTTP 请求上下文管理
#include "AsyncLog.h"        // 异步日志系统，用于高性能日志输出
#include "LogStream.h"       // 包含 Fmt 和 LogStream 定义
#include "HttpRequest.h"

<<<<<<< HEAD
=======
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>
#include <functional>

>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14
/**
 * @brief 一个支持异步日志记录和线程池处理的简易 HttpServer 类
 */
class HttpServer
{
private:
    TcpServer tcpServer_;    // 基于 Reactor 的 TCP 服务器
    ThreadPool threadPool_;  // 工作线程池，用于并发处理请求
<<<<<<< HEAD
    std::shared_ptr<AsyncLog> asyncLogger_; // 异步日志对象，用于非阻塞记录日志
=======
    std::shared_ptr<AsyncLogging> asyncLogger_; // 异步日志对象，用于非阻塞记录日志
>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14

    // 记录每个连接对应的 HttpContext，便于多次接收数据时解析
    std::unordered_map<int, std::shared_ptr<HttpContext>> contextMap_;
    std::mutex ctxMutex_;  // 用于保护 contextMap_

public:
    // 构造函数，初始化服务器监听地址、线程数及线程池大小
    HttpServer(const std::string& ip, uint16_t port, int subThreadNum, int workThreadNum);

    ~HttpServer();

    // 启动服务器开始监听与事件循环
    void Start();

    // 停止服务器服务，包括线程池与 TcpServer 以及异步日志
    void StopService();
<<<<<<< HEAD
    
=======

    // 设置异步日志对象
    void SetAsyncLogger(std::shared_ptr<AsyncLogging> logger) { asyncLogger_ = std::move(logger); }
>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14

private:
    // 新连接建立时的回调函数
    void HandleNewConnection(spConnection conn);
<<<<<<< HEAD
    // 连接关闭时的回调函数
    void HandleClose(spConnection conn);
    // 连接发生错误时的回调函数
    void HandleError(spConnection conn);
    // 发送数据完成时的回调函数
    void HandleSendComplete(spConnection conn);
    // epoll_wait 超时的回调函数，通常不做日志记录以免膨胀日志文件
    void HandleTimeOut(EventLoop* loop);

    // 生成响应
    std::string GenerateHttpResponse(const std::string& message, 
                                   const HttpStatusCode code);
    // 发送错误报文
    void SendBadRequestResponse(spConnection conn, const HttpStatusCode code, const std::string& message);

    // 有消息到达时的回调函数，如果线程池启用则放入任务队列
    void HandleMessage(spConnection conn, std::string &message);
    // 实际处理 HTTP 请求的函数
    void OnMessage(spConnection conn, std::string &message);
};

=======

    // 连接关闭时的回调函数
    void HandleClose(spConnection conn);

    // 连接发生错误时的回调函数
    void HandleError(spConnection conn);

    // 有消息到达时的回调函数，如果线程池启用则放入任务队列
    void HandleMessage(spConnection conn, std::string& message);

    // 实际处理 HTTP 请求的函数
    void OnMessage(spConnection conn, std::string& message);

    // 发送数据完成时的回调函数
    void HandleSendComplete(spConnection conn);

    // epoll_wait 超时的回调函数，通常不做日志记录以免膨胀日志文件
    void HandleTimeOut(EventLoop* loop);

    // 简易 HTTP 请求解析函数，仅支持 GET 方法（声明，定义在源码中未使用）
    bool ParseHttpRequest(const std::string& request, std::string& method, std::string& path);

    // 生成响应
    std::string GenerateHttpResponse(const HttpRequest& request);

    void SendBadRequestResponse(spConnection conn);
};

// 支持传入 Fmt、std::string、const char* 三种类型的日志输出辅助函数
inline void HttpLog(std::shared_ptr<AsyncLogging> logger, const std::string& msg) 
{
    if (logger) 
    {
        logger->Append(msg.c_str(), msg.length());
    }
}

inline void HttpLog(std::shared_ptr<AsyncLogging> logger, const Fmt& fmt) 
{
    if (logger) 
    {
        logger->Append(fmt.GetDataAddr(), fmt.Length());
    }
}

inline void HttpLog(std::shared_ptr<AsyncLogging> logger, const char* msg) {
    if (logger) {
        logger->Append(msg, strlen(msg));
    }
}

>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14
#endif // HTTPSERVER_H
