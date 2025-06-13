#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "HttpServer.h"

HttpServer *httpServer;

/**
 * 信号2和15处理函数，功能是停止服务程序
 * @param sig
 */
void StopSignal(int sig)
{
    printf("sig = %d\n", sig);
    httpServer->StopService();
    //调用HttpServer::StopService()停止服务
    //printf("httpserver已停止。\n");
    delete httpServer;
    //printf("delete httpServer\n");
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage: ./httpservice <ip> <port>\n");
        return -1;
    }

    // 设置信号处理函数
    signal(SIGTERM, StopSignal);
    signal(SIGINT, StopSignal);

    // 创建并启动异步日志系统
    auto asyncLogger = std::make_shared<AsyncLogging>("http_server_log.txt");
    asyncLogger->Start();

    // 创建 HttpServer 并绑定日志系统
    httpServer = new HttpServer(argv[1], atoi(argv[2]), 3, 2); // subReactor=3，workThread=2
    httpServer->SetAsyncLogger(asyncLogger);  // ✨ 绑定异步日志

    // 事件循环
    httpServer->Start();

    return 0;
}

