#include "HttpRequest.h"

#include <iostream>

// 构造函数，初始化请求方法为无效，HTTP版本为未知
HttpRequest::HttpRequest() : method_(Method::kInvalid), version_(Version::kUnknown) {}

// 析构函数，当前无特殊清理操作
HttpRequest::~HttpRequest() {}

// 设置HTTP版本，支持 "1.0" 和 "1.1"，其他设置为未知
void HttpRequest::SetVersion(const std::string &ver) 
{
    if(ver == "1.0") 
      version_ = Version::kHttp10;
    else if(ver == "1.1")
      version_ = Version::kHttp11;
    else 
      version_ = Version::kUnknown;
}

// 返回HTTP版本枚举
Version HttpRequest::GetVersion() const { return version_;}

// 获取HTTP版本字符串，标准格式如 HTTP/1.0、HTTP/1.1
std::string HttpRequest::GetVersionString() const 
{
    switch(version_) 
    {
        case Version::kHttp10: return "HTTP/1.0";
        case Version::kHttp11: return "HTTP/1.1";
        default: return "UNKNOWN";
    }
}

// 设置请求方法，支持GET, POST, HEAD, PUT, DELETE（不区分大小写）
// 返回设置是否成功
bool HttpRequest::SetMethod(const std::string &method) 
{
    std::string methodUpper;
    // 转换为大写，简易实现
    methodUpper.reserve(method.size());
    for(auto ch : method) 
    {
      methodUpper.push_back(toupper(ch));
    }  
  
    if(methodUpper == "GET") 
        method_ = Method::kGet;
    else if (methodUpper == "POST") 
        method_ = Method::kPost;
    else if (methodUpper == "HEAD")
        method_ = Method::kHead;
    else if (methodUpper == "PUT")
        method_ = Method::kPut;
    else if (methodUpper == "DELETE") 
        method_ = Method::kDelete;
    else 
        method_ = Method::kInvalid;
    
    return method_ != Method::kInvalid;
}

// 获取请求方法枚举
Method HttpRequest::GetMethod() const {return method_;}

// 获取请求方法字符串，大写标准HTTP方法名
std::string HttpRequest::GetMethodString() const 
{
    switch (method_) 
    {
        case Method::kGet: return "GET";
        case Method::kPost: return "POST";
        case Method::kHead: return "HEAD";
        case Method::kPut: return "PUT";
        case Method::kDelete: return "DELETE";
        default: return "INVALID";
    }
}

// 设置请求路径，使用std::move避免拷贝
// 这里用=即可，std::move会让传入参数失效，通常传值时用move，传引用时不用move
void HttpRequest::SetUrl(const std::string &url) { url_ = url; }

// 获取请求路径的常量引用
const std::string & HttpRequest::GetUrl() const {return url_;}

// 设置请求参数（如GET请求的查询参数）
void HttpRequest::SetRequestParams(const std::string &key, const std::string &value) 
{
    requestParams_[key] = value;
}

// 获取请求参数值，传入key，找不到返回空字符串
std::string HttpRequest::GetRequestParamsByKey(const std::string &key) const 
{
    auto it = requestParams_.find(key); 
    return (it != requestParams_.end()) ? it->second : "";
}

// 获取所有请求参数的常量引用
const std::map<std::string, std::string> & HttpRequest::GetRequestParams() const {return requestParams_;}

// 设置协议字符串（通常是 "HTTP"）
void HttpRequest::SetProtocol(const std::string &str) {protocol_ = str;}

// 获取协议字符串的常量引用
const std::string & HttpRequest::GetProtocol() const {return protocol_;}

// 添加HTTP请求头
void HttpRequest::AddHeader(const std::string &field, const std::string &value) 
{
    headers_[field] = value;
}

// 获取请求头中指定字段的值，找不到返回空字符串
std::string HttpRequest::GetHeader(const std::string &field) const 
{
    auto it = headers_.find(field);
    return (it != headers_.end()) ? it->second : "";
}

// 获取所有请求头的常量引用
const std::map<std::string, std::string> & HttpRequest::GetHeaders() const 
{
    return headers_;
}

// 设置请求体内容
void HttpRequest::SetBody(const std::string &str) {body_ = str;}

// 获取请求体的常量引用
const std::string & HttpRequest::GetBody() const {return body_;}

bool HttpRequest::IsKeepAlive() const 
{
    auto it = headers_.find("Connection");
    if (it != headers_.end()) 
    {
        if (it->second == "keep-alive" || it->second == "Keep-Alive")
            return true;
    }
    if (version_ == Version::kHttp11)
        return true;
    return false;
}
