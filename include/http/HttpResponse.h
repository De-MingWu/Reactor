#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <string>
#include <map>

#include <string>
#include <unordered_map>
#include <iostream>

// HTTP 状态码枚举
enum class HttpStatusCode 
{
    kUnknown = 0,
    k100Continue = 100,
    k200OK = 200,
    k201Created = 201,
    k204NoContent = 204,
    k302Found = 302,
    k400BadRequest = 400,
    k401Unauthorized = 401,
    k403Forbidden = 403,
    k404NotFound = 404,
    k405MethodNotAllowed = 405,
    k500InternalServerError = 500
};

class HttpResponse 
{
public:
    explicit HttpResponse(bool close_connection);
    ~HttpResponse();

    void SetStatusCode(HttpStatusCode statusCode);
    void SetStatusMessage(const std::string& statusMessage);
    void SetVersion(const std::string& version);
    void SetCloseConnection(bool closeConnection);
    void SetContentType(const std::string& contentType);
    void SetBody(const std::string& body);
    void AddHeader(const std::string& key, const std::string& value);
    void AddSetCookie(const std::string& cookie);
    void AddDateHeader();
    bool IsCloseConnection() const;
    std::string ResponseMessage();         // 构造完整 HTTP 响应字符串
    void DebugPrint() const;               // 打印响应内容（调试用）

private:
    std::string GetStatusLine() const;     // 响应行
    static std::string GetDefaultStatusMessage(HttpStatusCode code);

private:
    HttpStatusCode statusCode_;
    std::string statusMessage_;
    std::string version_;
    bool closeConnection_;
    std::unordered_map<std::string, std::string> headers_;
    std::string body_;
};

#endif