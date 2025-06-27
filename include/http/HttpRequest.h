#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <string>
#include <map>

// HTTP 请求方法的枚举类型
enum class Method
{
    kInvalid = 0,  // 无效方法，默认值
    kGet,          // GET 请求
    kPost,         // POST 请求
    kHead,         // HEAD 请求
    kPut,          // PUT 请求
    kDelete        // DELETE 请求
};

// HTTP 版本的枚举类型
enum class Version
{
    kUnknown = 0,  // 未知版本，默认值
    kHttp10,       // HTTP/1.0
    kHttp11        // HTTP/1.1
};

// 定义 HttpRequest 类，用于表示一个 HTTP 请求
class HttpRequest 
{
private:
    Method method_;   // 当前请求的方法（枚举）
    Version version_; // 当前 HTTP 版本（枚举）

    std::map<std::string, std::string> requestParams_;  // 请求参数，key-value形式

    std::string url_;  // 请求路径

    std::string protocol_;  // 协议字符串，通常是 "HTTP"

    std::map<std::string, std::string> headers_; // 请求头字段集合，key-value形式

    std::string body_; // 请求体内容

public:
    HttpRequest();   // 构造函数
    ~HttpRequest();  // 析构函数

    // 设置 HTTP 版本（传入字符串形式，如 "HTTP/1.1"）
    void SetVersion(const std::string &ver);
    Version GetVersion() const;// 获取当前 HTTP 版本（枚举类型）
    std::string GetVersionString() const;// 获取 HTTP 版本对应的字符串形式

    // 设置 HTTP 请求方法（传入字符串，如 "GET"）
    // 返回设置是否成功（字符串是否匹配有效方法）
    bool SetMethod(const std::string &method);
    Method GetMethod() const;// 获取请求方法（枚举类型）
    std::string GetMethodString() const;// 获取请求方法对应的字符串形式（如 "GET"）

    void SetUrl(const std::string &url);// 设置请求路径（URL 中路径部分）
    const std::string &GetUrl() const;// 获取请求路径的常量引用

    // 设置请求参数（key-value形式，通常用于 URL 查询参数）
    void SetRequestParams(const std::string &key, const std::string &value);
    
    std::string GetRequestParamsByKey(const std::string &key) const;// 根据 key 获取请求参数对应的值
    const std::map<std::string, std::string> & GetRequestParams() const;// 获取所有请求参数的常量引用（map）

    void SetProtocol(const std::string &str);// 设置协议字符串（通常是 "HTTP"）
    const std::string & GetProtocol() const;// 获取协议字符串

    // 添加 HTTP 请求头（字段名和值）
    void AddHeader(const std::string &field, const std::string &value);
    std::string GetHeader(const std::string &field) const;// 根据字段名获取请求头的值
    const std::map<std::string, std::string> & GetHeaders() const;// 获取所有请求头的常量引用

    // 设置请求体（POST 或 PUT 请求的内容）
    void SetBody(const std::string &str);
    const std::string & GetBody() const;// 获取请求体内容

    bool IsKeepAlive() const;

};

#endif
