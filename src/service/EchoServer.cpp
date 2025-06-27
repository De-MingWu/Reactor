#include "EchoServer.h"

EchoServer::EchoServer(const std::string &ip,const uint16_t port, int subThreadNum, int workThreadNum)
           :tcpServer_(ip, port, subThreadNum), threadPool_(workThreadNum, "Works")
{
    // 以下代码不是必须的，业务关心什么事件，就指定相应的回调函数。
    tcpServer_.SetNewConnectionCB(std::bind(&EchoServer::HandleNewConnection, this, std::placeholders::_1));
    tcpServer_.SetCloseConnectionCB(std::bind(&EchoServer::HandleClose, this, std::placeholders::_1));
    tcpServer_.SetErrorConnectionCB(std::bind(&EchoServer::HandleError, this, std::placeholders::_1));
    tcpServer_.SetHandleMessageCB(std::bind(&EchoServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
    tcpServer_.SetSendCompleteCB(std::bind(&EchoServer::HandleSendComplete, this, std::placeholders::_1));
    tcpServer_.SetTimeOutCB(std::bind(&EchoServer::HandleTimeOut, this, std::placeholders::_1));
}

EchoServer::~EchoServer()
{

}

// 启动服务。
void EchoServer::Start()
{
    tcpServer_.Start();
}

//停止服务
void EchoServer::StopService()
{
    //停止工作线程
    threadPool_.StopThread();
    printf("工作线程已停止。\n");

    //停止IO线程
    tcpServer_.StopService();
}

// 处理新客户端连接请求，在TcpServer类中回调此函数。
void EchoServer::HandleNewConnection(spConnection conn)
{
    printf("EchoServer: new connection(eventfd = %d, ip = %s, port = %d).\n",
           conn->GetFd(), conn->GetIP().c_str(), conn->GetPort());

    // 根据业务的需求，在这里可以增加其它的代码。
}

// 关闭客户端的连接，在TcpServer类中回调此函数。
void EchoServer::HandleClose(spConnection conn)
{
    printf("EchoServer: client(eventfd = %d) disconnected.\n",conn->GetFd());

    // 根据业务的需求，在这里可以增加其它的代码。
}

// 客户端的连接错误，在TcpServer类中回调此函数。
void EchoServer::HandleError(spConnection conn)
{
    printf("EchoServer conn error.\n");

    // 根据业务的需求，在这里可以增加其它的代码。
}

// 处理客户端的请求报文，在TcpServer类中回调此函数。
void EchoServer::HandleMessage(spConnection conn, std::string &message)
{
    //printf("EchoServer::HandleMessage() thread is %ld.\n",syscall(SYS_gettid));
    if(threadPool_.GetSize() == 0)
    {
        //没有工作线程， 直接在I/O线程中计算
        OnMessage(conn, message);
    }
    else// 把业务添加到线程池的任务队列中。
        threadPool_.AddTasks(std::bind(&EchoServer::OnMessage, this, conn, message));
}

void EchoServer::OnMessage(spConnection conn,std::string &message)
{
    // 在这里，将经过若干步骤的运算。
    message = "reply:" + message;
    //printf("%s\n", message.c_str());

    conn->SendData(message.data(), message.size());
}

// 数据发送完成后，在TcpServer类中回调此函数。
void EchoServer::HandleSendComplete(spConnection conn)
{
    //printf("Message send complete.\n");

    // 根据业务的需求，在这里可以增加其它的代码。
}


// epoll_wait()超时，在TcpServer类中回调此函数。
void EchoServer::HandleTimeOut(EventLoop *loop)
{
    //printf("EchoServer timeout.\n");

    // 根据业务的需求，在这里可以增加其它的代码。
}