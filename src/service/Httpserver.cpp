<<<<<<< HEAD
#include <sstream> 
#include <functional>
#include <mutex>
#include <nlohmann/json.hpp>

#include "HttpServer.h"
#include "Log.h"  // 引入 Log 头文件

using json = nlohmann::json;

// 构造函数，初始化 TcpServer、线程池及日志
=======
#include "HttpServer.h"
#include <sstream>
#include <functional>
#include <mutex>

// 构造函数，初始化 TcpServer、线程池及异步日志
>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14
HttpServer::HttpServer(const std::string& ip, uint16_t port, int subThreadNum, int workThreadNum)
    : tcpServer_(ip, port, subThreadNum),
      threadPool_(workThreadNum, "HttpWorks")
{
<<<<<<< HEAD
=======
    // 异步日志初始化
    asyncLogger_ = std::make_shared<AsyncLogging>("http_log.txt");
    asyncLogger_->Start();

>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14
    // 设置 TcpServer 各种事件回调绑定，使用 std::bind 绑定成员函数及 this 指针
    tcpServer_.SetNewConnectionCB(std::bind(&HttpServer::HandleNewConnection, this, std::placeholders::_1));
    tcpServer_.SetCloseConnectionCB(std::bind(&HttpServer::HandleClose, this, std::placeholders::_1));
    tcpServer_.SetErrorConnectionCB(std::bind(&HttpServer::HandleError, this, std::placeholders::_1));
    tcpServer_.SetHandleMessageCB(std::bind(&HttpServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
    tcpServer_.SetSendCompleteCB(std::bind(&HttpServer::HandleSendComplete, this, std::placeholders::_1));
    tcpServer_.SetTimeOutCB(std::bind(&HttpServer::HandleTimeOut, this, std::placeholders::_1));
}

HttpServer::~HttpServer() 
{
<<<<<<< HEAD
    // 析构时停止服务
=======
    // 析构时停止日志和线程池，确保资源释放
>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14
    StopService();
}

// 启动 TCP 服务器
void HttpServer::Start() {
    tcpServer_.Start();
}

// 停止服务，停止线程池及日志，关闭 TCP 服务
<<<<<<< HEAD
void HttpServer::StopService() 
{
    threadPool_.StopThread();

    // 使用 Log 输出日志
    LOG_INFO << "HttpServer: 工作线程已停止。";

    tcpServer_.StopService();
=======
void HttpServer::StopService() {
    threadPool_.StopThread();

    LogStream log;
    log << "HttpServer: 工作线程已停止。\n";
    asyncLogger_->Append(log.GetBuffer().GetDataAddr(), log.GetBuffer().GetLen());

    tcpServer_.StopService();

    if (asyncLogger_) 
        asyncLogger_->Stop();
>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14
}

// 新连接回调，创建 HttpContext 上下文对象用于请求解析等
void HttpServer::HandleNewConnection(spConnection conn) 
{
<<<<<<< HEAD
    // 为每个新连接创建一个 HttpContext
    conn->SetContext(std::make_shared<HttpContext>());

    // 使用 Log 输出日志
    LOG_INFO << "HttpServer: 新连接建立(fd=" << conn->GetFd()
             << ", ip=" << conn->GetIP() << ", port=" << conn->GetPort() << ")";
}

// 连接关闭回调，清理上下文信息
void HttpServer::HandleClose(spConnection conn) 
{
    // 清理上下文
    conn->SetContext(std::shared_ptr<void>());

    // 使用 Log 输出日志
    LOG_INFO << "HttpServer: 连接关闭 (IP: " << conn->GetIP() << ")";
}

// 连接错误回调，记录日志
void HttpServer::HandleError(spConnection conn) 
{

}

// 数据发送完成回调
void HttpServer::HandleSendComplete(spConnection conn) 
{
     // 使用 Log 输出日志
     LOG_INFO << "HttpServer: 数据发送完毕 fd=" << conn->GetFd();
}

// 超时处理回调
void HttpServer::HandleTimeOut(EventLoop* loop) 
{
    // 可扩展逻辑
}

// 根据 HttpRequest 生成 HttpResponse 并返回完整响应字符串
std::string HttpServer::GenerateHttpResponse(const std::string& message, 
                                             const HttpStatusCode code)
{
    json responses = {
            {"code", static_cast<int>(code)},
            {"message", message}
        };
    HttpResponse response(true);  
    response.SetStatusCode(code);
    response.SetContentType("application/json");
    response.AddHeader("Connection", "close");
    response.SetBody(responses.dump());

    return response.ResponseMessage();
}

// 发送 400 Bad Request 响应
void HttpServer::SendBadRequestResponse(spConnection conn, const HttpStatusCode code, const std::string& message) 
{
    HttpResponse response(true);

    json body = {
            {"code", static_cast<int>(code)},
            {"message", message}
        };
    response.SetStatusCode(code);
    response.SetContentType("application/json");
    response.AddHeader("Connection", "close");
    response.SetBody(body.dump());

    const std::string& resp = response.ResponseMessage();
    conn->SendData(resp.data(), resp.size());

    // 使用 Log 输出日志
    LOG_ERROR << "HttpServer: 请求解析失败，返回 400";
}

// 处理客户端的请求报文
void HttpServer::HandleMessage(spConnection conn, std::string &message)
{
=======
    {
        std::lock_guard<std::mutex> lock(ctxMutex_);
        // 为新连接分配新的 HttpContext，上下文存储在 map 中，key 为连接 fd
        contextMap_[conn->GetFd()] = std::make_shared<HttpContext>();
    }

    LogStream log;
    log << "HttpServer: 新连接建立(fd=" << conn->GetFd()
        << ", ip=" << conn->GetIP() << ", port=" << conn->GetPort() << ")\n";
    asyncLogger_->Append(log.GetBuffer().GetDataAddr(), log.GetBuffer().GetLen());
}

// 连接关闭回调，清理上下文信息
void HttpServer::HandleClose(spConnection conn) {
    {
        std::lock_guard<std::mutex> lock(ctxMutex_);
        contextMap_.erase(conn->GetFd());
    }

    LogStream log;
    log << "HttpServer: 连接关闭 (fd=" << conn->GetFd() << ")\n";
    asyncLogger_->Append(log.GetBuffer().GetDataAddr(), log.GetBuffer().GetLen());
}

// 连接错误回调，记录日志
void HttpServer::HandleError(spConnection conn) {
    LogStream log;
    log << "HttpServer: 连接错误 (fd=" << conn->GetFd() << ")\n";
    asyncLogger_->Append(log.GetBuffer().GetDataAddr(), log.GetBuffer().GetLen());
}

// 处理客户端的请求报文，在TcpServer类中回调此函数。
void HttpServer::HandleMessage(spConnection conn, std::string &message)
{
    //printf("EchoServer::HandleMessage() thread is %ld.\n",syscall(SYS_gettid));
>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14
    if(threadPool_.GetSize() == 0)
    {
        //没有工作线程， 直接在I/O线程中计算
        OnMessage(conn, message);
    }
<<<<<<< HEAD
    else
        threadPool_.AddTasks(std::bind(&HttpServer::OnMessage, this, conn, message));
}

// 消息处理回调
void HttpServer::OnMessage(spConnection conn, std::string &message)
{
    std::shared_ptr<HttpContext> ctx = std::static_pointer_cast<HttpContext>(conn->GetContext());

    if (!ctx->ParseRequest(message.data(), static_cast<int>(message.size()))) 
    {
        SendBadRequestResponse(conn, HttpStatusCode::k404NotFound, "Not Found");
        return;
    }

    const HttpRequest* request = ctx->GetRequest();
    std::string body;
    std::string responseStr;
    if (request->GetMethod() == Method::kGet) 
    {
        body = "Get 请求成功";
        responseStr = GenerateHttpResponse(body, HttpStatusCode::k200OK);
    } 
    else if (request->GetMethod() == Method::kPost) 
    {
        std::string username = request->GetRequestParamsByKey("username");
        std::string password = request->GetRequestParamsByKey("password");

        if (username == "admin" && password == "123456") 
        {
            body = "登录成功欢迎，管理员！";
            responseStr = GenerateHttpResponse(body, HttpStatusCode::k200OK);
        } 
        else 
        {
            body = "登录失败，用户名或密码错误！";
            responseStr = GenerateHttpResponse(body, HttpStatusCode::kUnknown);
        }
    } 
    else 
    {
        body = "仅支持 GET 和 POST 方法";
        responseStr = GenerateHttpResponse(body, HttpStatusCode::k405MethodNotAllowed);
    }
    
    conn->SendData(responseStr.data(), responseStr.size());

    // 使用 Log 输出日志
    LOG_INFO << "HttpServer: 已处理请求方法=" << request->GetMethodString()
             << ", 路径=" << request->GetUrl();
=======
    else// 把业务添加到线程池的任务队列中。
        threadPool_.AddTasks(std::bind(&HttpServer::OnMessage, this, conn, message));
}

// 消息处理回调，主线程或线程池分发调用
void HttpServer::OnMessage(spConnection conn, std::string& message)
{
    std::shared_ptr<HttpContext> ctx;
    {
        std::lock_guard<std::mutex> lock(ctxMutex_);
        auto it = contextMap_.find(conn->GetFd());
        if (it == contextMap_.end()) {
            // 若找不到对应上下文，则创建一个新的 HttpContext
            ctx = std::make_shared<HttpContext>();
            contextMap_[conn->GetFd()] = ctx;
        } else {
            ctx = it->second;
        }
    }

    // 使用 HttpContext 解析请求
    if (!ctx->ParseRequest(message.data(), static_cast<int>(message.size()))) {
        // 解析失败返回 400
        SendBadRequestResponse(conn);
        return;
    }

    // 获取 HttpRequest 对象，生成响应字符串
    const HttpRequest* request = ctx->GetRequest();
    std::string responseStr = GenerateHttpResponse(*request);
    // 发送响应数据
    conn->SendData(responseStr.data(), responseStr.size());

    LogStream log;
    // 调用 HttpRequest 类成员的获取方法字符串接口（注意接口设计）
    log << "HttpServer: 已处理请求方法="
        << request->GetMethodString()
        << ", 路径=" << request->GetUrl() << "\n";
    asyncLogger_->Append(log.GetBuffer().GetDataAddr(), log.GetBuffer().GetLen());

    // 根据请求是否为长连接决定是否关闭连接
    if (!request->IsKeepAlive()) 
    {
        conn->HttpClose();
        // 清理上下文
        std::lock_guard<std::mutex> lock(ctxMutex_);
        contextMap_.erase(conn->GetFd());
    } 
    else 
    {
        // 重置 HttpContext 状态，准备下一次请求
        ctx->ResetContextStatus();
    }
}

// 发送 400 Bad Request 响应
void HttpServer::SendBadRequestResponse(spConnection conn) {
    HttpResponse response(true);  // 请求后关闭连接
    response.SetStatusCode(HttpStatusCode::k400BadRequest);
    response.SetContentType("text/plain");
    response.SetBody("Bad Request");

    const std::string& resp = response.ResponseMessage();
    conn->SendData(resp.data(), resp.size());

    LogStream log;
    log << "HttpServer: 请求解析失败，返回 400\n";
    asyncLogger_->Append(log.GetBuffer().GetDataAddr(), log.GetBuffer().GetLen());
}

// 数据发送完成回调
void HttpServer::HandleSendComplete(spConnection conn) 
{
    if (conn->IsCloseConnection()) {
        conn->HttpClose();

        LogStream log;
        log << "HttpServer: 已关闭连接 fd=" << conn->GetFd() << "\n";
        asyncLogger_->Append(log.GetBuffer().GetDataAddr(), log.GetBuffer().GetLen());
    }
}

// 超时处理回调，暂不做任何处理（可扩展）
void HttpServer::HandleTimeOut(EventLoop* loop) {
    // 这里可以添加定时清理空闲连接逻辑
}

// 根据 HttpRequest 生成 HttpResponse 并返回完整响应字符串，支持 GET 和 POST
std::string HttpServer::GenerateHttpResponse(const HttpRequest& request)
{
    // 判断是否保持连接（HttpRequest 里有 GetKeepAlive() 函数）
    HttpResponse response(!request.IsKeepAlive());  
    response.SetStatusCode(HttpStatusCode::k200OK);
    response.SetContentType("text/html; charset=UTF-8");

    std::string body;

    if (request.GetMethod() == Method::kGet) {
        // GET 请求返回请求路径信息
        body = "<html><head><title>HttpServer GET</title></head><body>"
               "<h1>你请求的路径是: " + request.GetUrl() + "</h1>"
               "</body></html>";
    } 
    else if (request.GetMethod() == Method::kPost) {
        // POST 请求读取请求参数，验证用户名密码
        std::string username = request.GetRequestParamsByKey("username");
        std::string password = request.GetRequestParamsByKey("password");

        if (username == "admin" && password == "123456") {
            body = "<html><head><title>登录成功</title></head><body>"
                   "<h1>欢迎，管理员！</h1>"
                   "</body></html>";
        } else {
            body = "<html><head><title>登录失败</title></head><body>"
                   "<h1>用户名或密码错误！</h1>"
                   "</body></html>";
        }
    }
    else {
        // 不支持的 HTTP 方法返回 405 Method Not Allowed
        response.SetStatusCode(HttpStatusCode::k405MethodNotAllowed);
        body = "<html><head><title>方法不支持</title></head><body>"
               "<h1>仅支持 GET 和 POST 方法</h1>"
               "</body></html>";
    }

    // 设置响应体内容
    response.SetBody(body);

    // 生成完整响应字符串（响应行 + 响应头 + 响应体）
    return response.ResponseMessage();
>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14
}
