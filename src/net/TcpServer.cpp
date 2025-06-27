#include "TcpServer.h"

TcpServer::TcpServer(const std::string &ip,const uint16_t port, int threadNum)
          :threadNum_(threadNum)
{
<<<<<<< HEAD
    mainLoop_ = std::unique_ptr<EventLoop>(new EventLoop(true));
    mainLoop_->SetEpollTimeoutCallback(bind(&TcpServer::EpollTimeout, this, std::placeholders::_1));

    acceptor_ = std::unique_ptr<Acceptor>(new Acceptor(mainLoop_.get(), ip, port));
=======
    mainLoop_ = std::make_unique<EventLoop>(true);
    mainLoop_->SetEpollTimeoutCallback(bind(&TcpServer::EpollTimeout, this, std::placeholders::_1));

    acceptor_ = std::make_unique<Acceptor>(mainLoop_.get(), ip, port);
>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14
    acceptor_->SetNewConnectionCB([this](std::unique_ptr<Socket> sock) {
        this->NewConnection(std::move(sock));
    });

    threadPool_ = new ThreadPool(threadNum_, "IO");

    // 创建从事件循环。
    for (int i = 0; i < threadNum_; ++i)
    {
        subLoops_.emplace_back(new EventLoop(false));              // 创建从事件循环，存入subloops_容器中。
        subLoops_[i]->SetEpollTimeoutCallback(std::bind(&TcpServer::EpollTimeout, this, std::placeholders::_1));   // 设置timeout超时的回调函数
        subLoops_[i]->SetTimeCallBack(std::bind(&TcpServer::RemoveConnection, this, std::placeholders::_1));
        threadPool_->AddTasks(std::bind(&EventLoop::RunLoop, subLoops_[i].get()));    // 在线程池中运行从事件循环。
    }
}

TcpServer::~TcpServer()
{
    delete threadPool_;
}

void TcpServer::Start()
{
    mainLoop_->RunLoop();
}

// 停止服务
void TcpServer::StopService()
{
    //停止主事件循环
    mainLoop_->StopEvent();
    //printf("主事件已停止\n");

    //停止从事件循环
    for(int i = 0; i < threadNum_; i++)
    {
        subLoops_[i]->StopEvent();
    }
    //printf("从事件已停止\n");

    //停止IO线程
    threadPool_->StopThread();
    //printf("IO线程已停止\n");
}

// 处理新客户端连接请求
void TcpServer::NewConnection(std::unique_ptr<Socket> clientSock)
{
    //printf("TcpServer: clientSock address = %p client Fd = %d\n", clientSock.get(), clientSock->GetFd());
    int fd = clientSock->GetFd() % threadNum_;
    //为新客户端准备读事件，并添加到epoll中
    spConnection conn(new Connection(subLoops_[fd].get(), std::move(clientSock)));

    conn->SetCloseCallBack(std::bind(&TcpServer::CloseConnect, this, std::placeholders::_1));
    conn->SetErrorCallBack(std::bind(&TcpServer::ErrorConnect, this, std::placeholders::_1));
    conn->SetHandleMessageCallback(std::bind(&TcpServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
    conn->SetSendCompleteCallback(std::bind(&TcpServer::SendComplete, this, std::placeholders::_1));

    {
        std::lock_guard<std::mutex>lock(cMutex_);
        conns_[conn->GetFd()] = conn; // 将新建连接存入map中
    }
    
    subLoops_[fd]->NewConnection(conn);

    // 回调EchoServer::HandleNewConnection()
    if(newConnectionCb_)
        newConnectionCb_(conn);
}

// 关闭客户端的连接，在Connection类中回调此函数。
void TcpServer::CloseConnect(spConnection connect)
{
    // 回调EchoServer::HandleClose()
    if (closeConnectionCb_) closeConnectionCb_(connect);

    // close(conn->fd());            // 关闭客户端的fd。

    {
        std::lock_guard<std::mutex>lock(cMutex_);
        conns_.erase(connect->GetFd());   // 从map中删除conn
    }
}

// 客户端的连接错误，在Connection类中回调此函数。
void TcpServer::ErrorConnect(spConnection connect)
{
    // 回调EchoServer::HandleError()
    if (errorConnectionCb_) errorConnectionCb_(connect);
    // printf("client(eventfd = %d) error.\n",connect->GetFd());
    // close(conn->fd());            // 关闭客户端的fd。
    {
        std::lock_guard<std::mutex>lock(cMutex_);
        conns_.erase(connect->GetFd());   // 从map中删除conn
    }
}

// 处理客户端的请求报文，在Connection类中回调此函数
void TcpServer::HandleMessage(spConnection conn, std::string &message)
{
    if (handleMessageCb_) handleMessageCb_(conn,message);     // 回调EchoServer::HandleMessage()
}

// 数据发送完成后，在Connection类中回调此函数。
void TcpServer:: SendComplete(spConnection conn)
{
    //printf("send complete.\n");
    if (sendCompleteCb_) sendCompleteCb_(conn);  // 回调EchoServer::HandleSendComplete()

}

// epoll_wait()超时，在EventLoop类中回调此函数。
void TcpServer::EpollTimeout(EventLoop *loop)
{
    // 回调EchoServer::HandleTimeOut()
    if (timeOutCb_)  timeOutCb_(loop);
}


void TcpServer::SetNewConnectionCB(std::function<void(spConnection)> fn)
{
    newConnectionCb_ = fn;
}

void TcpServer::SetCloseConnectionCB(std::function<void(spConnection)> fn)
{
    closeConnectionCb_ = fn;
}

void TcpServer::SetErrorConnectionCB(std::function<void(spConnection)> fn)
{
    errorConnectionCb_ = fn;
}

void TcpServer::SetHandleMessageCB(std::function<void(spConnection, std::string &)> fn)
{
    handleMessageCb_ = fn;
}

void TcpServer::SetSendCompleteCB(std::function<void(spConnection)> fn)
{
    sendCompleteCb_ = fn;
}

void TcpServer::SetTimeOutCB(std::function<void(EventLoop *)> fn)
{
    timeOutCb_ = fn;
}

// 删除connects中的Connection对象, 在EventLoop::HandleTime()中回调
void TcpServer::RemoveConnection(int fd)
{
    {
        std::lock_guard<std::mutex>lock(cMutex_);
        conns_.erase(fd);   // 从map中删除conn
    }
}