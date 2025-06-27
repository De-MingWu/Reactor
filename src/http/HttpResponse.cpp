#include "HttpResponse.h"
#include <ctime>
#include <sstream>

HttpResponse::HttpResponse(bool closeConnection)
            : statusCode_(HttpStatusCode::kUnknown),
              statusMessage_(""),
              version_("HTTP/1.1"),
              closeConnection_(closeConnection)
{}

HttpResponse::~HttpResponse() {}

// 设置 HTTP 状态码，并自动匹配默认状态消息
void HttpResponse::SetStatusCode(HttpStatusCode statusCode) 
{
    statusCode_ = statusCode;
    statusMessage_ = GetDefaultStatusMessage(statusCode_);
}

// 设置自定义状态消息（覆盖默认）
void HttpResponse::SetStatusMessage(const std::string& statusMessage) 
{
    statusMessage_ = statusMessage;
}

// 设置 HTTP 协议版本
void HttpResponse::SetVersion(const std::string& version) 
{
    version_ = version;
}

// 设置是否关闭连接（Connection: close / Keep-Alive）
void HttpResponse::SetCloseConnection(bool closeConnection) 
{
    closeConnection_ = closeConnection;
}

// 设置 Content-Type 头部
void HttpResponse::SetContentType(const std::string& contentType) 
{
    AddHeader("Content-Type", contentType);
}

// 添加自定义头部
void HttpResponse::AddHeader(const std::string& key, const std::string& value) 
{
    headers_[key] = value;
}

// 设置响应正文内容
void HttpResponse::SetBody(const std::string& body) { body_ = body; }

// 添加 Set-Cookie 响应头（用于登录状态维持等）
void HttpResponse::AddSetCookie(const std::string& cookie) 
{
    headers_.insert({"Set-Cookie", cookie});
}

// 添加 Date 响应头（格式为 GMT 时间）
void HttpResponse::AddDateHeader() 
{
    char buf[100];
    std::time_t now = std::time(nullptr);
    std::tm gmt;
    gmtime_r(&now, &gmt);
    std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", &gmt);
    AddHeader("Date", buf);
}

// 返回是否关闭连接
bool HttpResponse::IsCloseConnection() const 
{
    return closeConnection_;
}

// 构造完整响应字符串（包含响应行、头部和正文）
std::string HttpResponse::ResponseMessage() 
{
    std::string message;

    AddDateHeader(); // 加入标准时间字段

    // 1、响应行
    message += GetStatusLine();

    // 2、Content-Length 必须有
    message += "Content-Length: " + std::to_string(body_.size()) + "\r\n";

    // 3、Connection 控制头
    if (closeConnection_) {
        message += "Connection: close\r\n";
    } else {
        message += "Connection: Keep-Alive\r\n";
    }

    // 4、附加头部字段
    for (const auto& header : headers_) {
        message += header.first + ": " + header.second + "\r\n";
    }

    // 5、空行 + 正文
    message += "\r\n";
    message += body_;

    return message;
}

// 构造响应状态行，例如：HTTP/1.1 200 OK
std::string HttpResponse::GetStatusLine() const 
{
    return version_ + " " +
           std::to_string(static_cast<int>(statusCode_)) + " " +
           statusMessage_ + "\r\n";
}

// 根据状态码返回默认状态消息
std::string HttpResponse::GetDefaultStatusMessage(HttpStatusCode code) 
{
    switch (code) 
    {
        case HttpStatusCode::k100Continue: return "Continue";
        case HttpStatusCode::k200OK: return "OK";
        case HttpStatusCode::k201Created: return "Created";
        case HttpStatusCode::k204NoContent: return "No Content";
        case HttpStatusCode::k302Found: return "Found";
        case HttpStatusCode::k400BadRequest: return "Bad Request";
        case HttpStatusCode::k401Unauthorized: return "Unauthorized";
        case HttpStatusCode::k403Forbidden: return "Forbidden";
        case HttpStatusCode::k404NotFound: return "Not Found";
        case HttpStatusCode::k405MethodNotAllowed: return "Method Not Allowed";
        case HttpStatusCode::k500InternalServerError: return "Internal Server Error";
        default: return "Unknown";
    }
}

// 打印响应内容（调试用）
void HttpResponse::DebugPrint() const 
{
    std::cout << "----- HTTP Response Begin -----\n";
    std::cout << const_cast<HttpResponse*>(this)->ResponseMessage();
    std::cout << "\n----- HTTP Response End -----\n";
}
