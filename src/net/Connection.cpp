#include <unistd.h>

#include "Connection.h"

Connection::Connection(EventLoop* loop, std::unique_ptr<Socket> clientSock)
           :loop_(loop), 
            clientSock_(std::move(clientSock)), 
            disConnect_(false),
            clientChannel_(new Channel(loop_, clientSock_->GetFd()))
{
    clientChannel_->SetReadCallBack(bind(&Connection::HandleMessage, this));
    clientChannel_->SetCloseCallBack(bind(&Connection::CloseCallBack,this));
    clientChannel_->SetErrorCallBack(bind(&Connection::ErrorCallBack,this));
    clientChannel_->SetWriteCallback(bind(&Connection::WriteCallback, this));
    clientChannel_->EnableET();           // 客户端连上来的fd采用边缘触发
    clientChannel_->EnableReading();   // 让epoll_wait()监视clientchannel的读事件

    inputBuffer_ = std::make_unique<Buffer>();
    outputBuffer_ = std::make_unique<Buffer>();
}

Connection::~Connection(){ }

/**
 * 关键新增：Tie Connection对象
 * 保证在Channel::HandleEvent期间，TcpConnection不会被析构
 * 避免 TcpServer::CloseConnection() 提前释放导致悬空Channel访问
 */
void Connection::Tie()
{
    clientChannel_->Tie(shared_from_this());
}


int Connection::GetFd() const{ return clientSock_->GetFd(); }

std::string Connection::GetIP() const{ return clientSock_->GetIP(); }

uint16_t Connection::GetPort() const { return clientSock_->GetPort(); }

// 处理对端发送过来的消息
void Connection::HandleMessage()
{
    char buffer[1024];

    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        ssize_t nRead = read(GetFd(), buffer, sizeof(buffer));

        if (nRead > 0)
        {
            // 将接收到的数据追加到 inputBuffer_
            inputBuffer_->Append(buffer, static_cast<size_t>(nRead));
        }
        else if (nRead == -1 && errno == EINTR)
        {
            continue; // 被信号打断，继续读
        }
        else if (nRead == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            // 所有数据读取完毕，可以处理业务了
            // 实际场景中你需要根据协议，提取完整的一条一条的消息（如 TLV、HTTP、固定长度等）
            std::string message = inputBuffer_->RetrieveAllAsString();

            if (!message.empty())
            {
                lastTime_ = TimeStamp::NowTime(); // 更新连接活跃时间
                handleMessageCallback_(shared_from_this(), message); // 交给上层逻辑处理
            }

            break;
        }
        else if (nRead == 0)
        {
            // 客户端关闭连接
            CloseCallBack();
            break;
        }
        else
        {
            perror("read error");
            CloseCallBack();
            break;
        }
    }
}

// http服务端主动断开连接
void Connection::HttpClose()
{
    CloseCallBack();
}

// TCP连接关闭（断开）的回调函数，供Channel回调
void Connection::CloseCallBack()
{
    if (!disConnect_)
    {
        disConnect_ = true;
        clientChannel_->RemoveChannel();
        closeCallBack_(shared_from_this());
    }
}

//  TCP连接错误的回调函数，供Channel回调
void Connection::ErrorCallBack()
{
    disConnect_ = true; //关闭tcp连接
    clientChannel_->RemoveChannel();
    errorCallBack_(shared_from_this());
}

/**
 * 处理写事件的回调函数，供Channel回调
 */
void Connection::WriteCallback()
{
    while (outputBuffer_->ReadableBytes() > 0)
    {
        ssize_t n = ::send(GetFd(), outputBuffer_->Peek(), outputBuffer_->ReadableBytes(), 0);
        if (n > 0)
        {
            outputBuffer_->Retrieve(static_cast<size_t>(n)); // 移动读指针
        }
        else if (n == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                // 发送缓冲区满，等待下一次可写通知
                break;
            }
            else if (errno == EINTR)
            {
                // 被中断，重试
                continue;
            }
            else
            {
                std::cerr << "send error, fd: " << GetFd() << ", errno: " << errno << std::endl;
                CloseCallBack();
                return;
            }
        }
        else if (n == 0)
        {
            // 一般不会发生，但作为保护性代码保留
            break;
        }
    }

    if (outputBuffer_->ReadableBytes() == 0)
    {
        clientChannel_->DisableWriting(); // 发送完毕，停止关注写事件
        if (sendCompleteCallback_)
            sendCompleteCallback_(shared_from_this());
    }
}


// 设置关闭fd_的回调函数。
void Connection::SetCloseCallBack(const std::function<void(spConnection)>& fn)
{
    closeCallBack_ = fn;     // 回调TcpServer::closeconnection()。
}

// 设置fd_发生了错误的回调函数。
void Connection::SetErrorCallBack(const std::function<void(spConnection)>& fn)
{
    errorCallBack_ = fn;     // 回调TcpServer::errorconnection()。
}

// 设置处理报文的回调函数。
void Connection::SetHandleMessageCallback(std::function<void(spConnection, std::string&)> fn)
{
    handleMessageCallback_ = fn;       // 回调TcpServer::onmessage()。
}
// 发送数据完成后的回调函数。
void Connection::SetSendCompleteCallback(std::function<void(spConnection)> fn)
{
    sendCompleteCallback_ = fn;
}

// 发送数据。
void Connection::SendData(const char *data, size_t size)
{ 
    if(disConnect_)return;
    std::string data_copy(data, size); // 一定要拷贝一份，不然会报错，echoserver不知道为啥这里不报错
    //判断当前线程是否为事件循环线程(IO线程）
    if(loop_->IsInLoopThread())
    {
        //是IO线程，执行发送数据操作
        SendDataByThread(data_copy.data(), data_copy.size());
    }
    else
    {
        //不是IO线程，将发送数据的操作交给IO线程
        loop_->QueueInLoop([this, data_copy=std::move(data_copy)](){
            SendDataByThread(data_copy.data(), data_copy.size());
        });
    }
}

//发送数据, 如果为IO线程直接调用，工作线程则将此函数传递给IO线程
void Connection::SendDataByThread(const char *data, size_t size)
{
    outputBuffer_->Append(data, size);      // 把需要发送的数据保存到Connection的发送缓冲区中。
    clientChannel_->EnableWriting();        // 启用写事件监听
}

// 连接是否已断开
bool Connection::IsCloseConnection()
{
    return disConnect_;
}

// 判断TCP连接是否超时（空闲太久）
bool Connection::IsTimeOut(time_t nowTime, int val)
{
    return nowTime - lastTime_.ToInt() > val;
}

