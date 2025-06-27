#ifndef HTTPCONTEXT_H
#define HTTPCONTEXT_H

#include <string>
#include <memory>
#include <mutex>

class HttpRequest;

#define CR '\r'
#define LF '\n'

/**
 * @brief HTTP 请求解析状态机状态
 */
<<<<<<< HEAD
enum class HttpRequestParseState 
{
=======
enum HttpRequestParseState {
>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14
    kINVALID,              // 无效
    kINVALID_METHOD,       // 无效请求方法
    kINVALID_URL,          // 无效请求路径
    kINVALID_VERSION,      // 无效协议版本号
    kINVALID_HEADER,       // 无效请求头

    START,                 // 解析开始
    METHOD,                // 请求方法

    BEFORE_URL,            // 请求 URL 前状态，需 '/' 开头
    IN_URL,                // URL 中

    BEFORE_URL_PARAM_KEY,  // URL 参数 Key 之前
    URL_PARAM_KEY,         // URL 参数 Key
    BEFORE_URL_PARAM_VALUE,// URL 参数 Value 之前
    URL_PARAM_VALUE,       // URL 参数 Value

    BEFORE_PROTOCOL,       // 协议解析之前
    PROTOCOL,              // 协议部分

    BEFORE_VERSION,        // 版本开始前
    VERSION,               // 版本

    HEADER_KEY,            // Header Key
    HEADER_VALUE,          // Header Value

    WHEN_CR,               // 遇到 CR
    CR_LF,                 // 遇到 CR LF
    CR_LF_CR,              // 遇到 CR LF CR

    BODY,                  // 请求体

    COMPLETE,              // 完成
};

/**
 * @brief HTTP 请求上下文，保存解析状态
 */
<<<<<<< HEAD
class HttpContext 
{
=======
class HttpContext {
>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14
private:
    std::unique_ptr<HttpRequest> request_;  // 指向HttpRequest对象，保存请求数据
    HttpRequestParseState state_;  // 当前解析状态

public:
    HttpContext();
    ~HttpContext();

    // 解析输入的HTTP请求数据
    // true表示请求完整解析完成，false表示需要更多数据或解析失败
    bool ParseRequest(const char* begin, int size);
    // 是否完成整个HTTP请求解析
    bool GetCompleteRequest() const;
    
    HttpRequest* GetRequest(); // 获取当前解析出的HttpRequest对象
    // 重置解析上下文，清空所有状态和请求数据
    void ResetContextStatus();
};

#endif