#include "HttpContext.h"
#include "HttpRequest.h"
#include "Common.h"

#include <memory>
#include <iostream>

#define DEBUG_HTTP_PARSE 0  // 设置为 1 可开启 Debug 日志输出

// 工具函数：解析 application/x-www-form-urlencoded 的键值对
static void ParseUrlEncodedForm(const std::string& body, HttpRequest* request) 
{
    size_t start = 0;
    while (start < body.size()) 
    {
        size_t equal = body.find('=', start);
        if (equal == std::string::npos) break;
        size_t amp = body.find('&', equal);
        std::string key = body.substr(start, equal - start);
        std::string value = (amp == std::string::npos) ? body.substr(equal + 1)
                                                       : body.substr(equal + 1, amp - equal - 1);
        request->SetRequestParams(key, value);
        if (amp == std::string::npos) break;
        start = amp + 1;
    }
}

// 构造函数，初始化状态为 START，分配一个新的 HttpRequest 对象
HttpContext::HttpContext() : state_(HttpRequestParseState::START)
{
<<<<<<< HEAD
    request_ = std::unique_ptr<HttpRequest>(new HttpRequest()); 
=======
    request_ = std::make_unique<HttpRequest>();
>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14
}

// 析构函数（空实现）
HttpContext::~HttpContext() {}

// 判断请求是否完整
bool HttpContext::GetCompleteRequest() const 
{
    return state_ == HttpRequestParseState::COMPLETE;
}

// 核心函数：解析请求内容
bool HttpContext::ParseRequest(const char* begin, int size) 
{
    char* start = const_cast<char*>(begin);  // 当前字段起始位置
    char* end = start; // 当前处理位置
    char* colon = end; // 用于保存 URL 参数和 Header key:value 分割位置

    // 状态机主循环
    while(state_ != HttpRequestParseState::kINVALID &&
          state_ != HttpRequestParseState::COMPLETE &&
          end - begin < size) 
    {

        char ch = *end;  // 当前字符

#if DEBUG_HTTP_PARSE
        std::cout << "[DEBUG] State=" << state_ << ", Char='" << ch << "'\n";
#endif

        switch(state_) 
        {
            case HttpRequestParseState::START: 
            {
                // 跳过空白字符，遇到大写字母切换到 METHOD 状态
                if (ch == CR || ch == LF || isblank(ch)) 
                {
                    // Skip whitespace
                } 
                else if (isupper(ch)) 
                {
                    state_ = HttpRequestParseState::METHOD;
                } 
                else 
                {
                    state_ = HttpRequestParseState::kINVALID;
                }
                break;
            }
            case HttpRequestParseState::METHOD: 
            {
                // 继续收集 METHOD，遇空格则结束 METHOD，切换到 BEFORE_URL
                if (isupper(ch)) 
                {
                    // Continue parsing METHOD
                } 
                else if (isblank(ch)) 
                {
                    request_->SetMethod(std::string(start, end));
                    state_ = HttpRequestParseState::BEFORE_URL;
                    start = end + 1;
                } 
                else 
                {
                    state_ = HttpRequestParseState::kINVALID;
                }
                break;
            }
            case HttpRequestParseState::BEFORE_URL: 
            {
                // 跳过空格，遇 '/' 开始 URL
                if (ch == '/') 
                {
                    state_ = HttpRequestParseState::IN_URL;
                    start = end;  // URL 起点
                } 
                else if (isblank(ch)) 
                {
                    // Skip
                } 
                else 
                {
                    state_ = HttpRequestParseState::kINVALID;
                }
                break;
            }
            case HttpRequestParseState::IN_URL: 
            {
                // 收集 URL，遇 '?' 开始解析 URL 参数，遇空格结束 URL
                if (ch == '?') 
                {
                    request_->SetUrl(std::string(start, end));
                    start = end + 1;
                    state_ = HttpRequestParseState::BEFORE_URL_PARAM_KEY;
                } 
                else if (isblank(ch)) 
                {
                    request_->SetUrl(std::string(start, end));
                    start = end + 1;
                    state_ = HttpRequestParseState::BEFORE_PROTOCOL;
                }
                break;
            }
            case HttpRequestParseState::BEFORE_URL_PARAM_KEY: 
            {
                // URL 参数 Key 前，遇非法字符立即失败
                if (isblank(ch) || ch == CR || ch == LF) 
                {
                    state_ = HttpRequestParseState::kINVALID;
                } 
                else 
                {
                    state_ = HttpRequestParseState::URL_PARAM_KEY;
                }
                break;
            }
            case HttpRequestParseState::URL_PARAM_KEY: 
            {
                // 收集 URL 参数 Key，遇 '=' 切换到 Value
                if (ch == '=') 
                {
                    colon = end;
                    state_ = HttpRequestParseState::BEFORE_URL_PARAM_VALUE;
                } 
                else if (isblank(ch)) 
                {
                    state_ = HttpRequestParseState::kINVALID;
                }
                break;
            }
            case HttpRequestParseState::BEFORE_URL_PARAM_VALUE: 
            {
                // URL 参数 Value 前，遇非法字符立即失败
                if (isblank(ch) || ch == LF || ch == CR) 
                {
                    state_ = HttpRequestParseState::kINVALID;
                } 
                else 
                {
                    state_ = HttpRequestParseState::URL_PARAM_VALUE;
                }
                break;
            }
            case HttpRequestParseState::URL_PARAM_VALUE: 
            {
                // 收集 URL 参数 Value，遇 '&' 切换到下一个参数，遇空格结束 URL 参数
                if (ch == '&') 
                {
                    request_->SetRequestParams(std::string(start, colon), std::string(colon + 1, end));
                    start = end + 1;
                    state_ = HttpRequestParseState::BEFORE_URL_PARAM_KEY;
                } 
                else if (isblank(ch)) 
                {
                    request_->SetRequestParams(std::string(start, colon), std::string(colon + 1, end));
                    start = end + 1;
                    state_ = HttpRequestParseState::BEFORE_PROTOCOL;
                }
                break;
            }
            case HttpRequestParseState::BEFORE_PROTOCOL: 
            {
                // 跳过空格，进入 PROTOCOL 状态
                if (isblank(ch)) 
                {
                    // Skip
                } 
                else 
                {
                    state_ = HttpRequestParseState::PROTOCOL;
                    start = end;
                }
                break;
            }
            case HttpRequestParseState::PROTOCOL: 
            {
                // 收集协议名，遇 '/' 切换到 BEFORE_VERSION
                if (ch == '/') 
                {
                    request_->SetProtocol(std::string(start, end));
                    start = end + 1;
                    state_ = HttpRequestParseState::BEFORE_VERSION;
                }
                break;
            }
            case HttpRequestParseState::BEFORE_VERSION: 
            {
                // 版本号必须从数字开始
<<<<<<< HEAD
                if (isdigit(ch)) 
                {
                    state_ = HttpRequestParseState::VERSION;
                    start = end;
                } 
                else 
                {
=======
                if (isdigit(ch)) {
                    state_ = HttpRequestParseState::VERSION;
                    start = end;
                } else {
>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14
                    state_ = HttpRequestParseState::kINVALID;
                }
                break;
            }
            case HttpRequestParseState::VERSION: 
            {
                // 收集协议版本号，遇 CR 结束
                if (ch == CR) 
                {
                    request_->SetVersion(std::string(start, end));
                    start = end + 1;
                    state_ = HttpRequestParseState::WHEN_CR;
                } else if (!(isdigit(ch) || ch == '.')) 
                {
                    state_ = HttpRequestParseState::kINVALID;
                }
                break;
            }
            case HttpRequestParseState::HEADER_KEY: 
            {
                // 收集 Header Key，遇 ':' 切换到 HEADER_VALUE
                if (ch == ':') 
                {
                    colon = end;
                    state_ = HttpRequestParseState::HEADER_VALUE;
                }
                break;
            }
            case HttpRequestParseState::HEADER_VALUE: 
            {
                // 收集 Header Value，遇 CR 结束当前 Header
                if (start == colon + 1 && isblank(ch)) 
                {
                    start++;  // 跳过冒号后的空格
                } 
                else if (ch == CR) 
                {
                    request_->AddHeader(std::string(start, colon), std::string(colon + 2, end));
                    start = end + 1;
                    state_ = HttpRequestParseState::WHEN_CR;
                }
                break;
            }
            case HttpRequestParseState::WHEN_CR: 
            {
                // CR 后必须跟 LF
                if (ch == LF) 
                {
                    start = end + 1;
                    state_ = HttpRequestParseState::CR_LF;
                } else {
                    state_ = HttpRequestParseState::kINVALID;
                }
                break;
            }

            case HttpRequestParseState::CR_LF: 
            {
                // 判断是否进入空行（CR_LF_CR）还是继续 Header 解析
                if (ch == CR) 
                {
                    state_ = HttpRequestParseState::CR_LF_CR;
                } 
                else if (isblank(ch)) 
                {
                    state_ = HttpRequestParseState::kINVALID;
                } 
                else 
                {
                    start = end;
                    state_ = HttpRequestParseState::HEADER_KEY;
                }
                break;
            }

            case HttpRequestParseState::CR_LF_CR: 
            {
                // CR_LF_CR 后如果是 LF 则判断是否需要解析 Body
                if (ch == LF) 
                {
                    int content_length = 0;
                    if (request_->GetHeaders().count("Content-Length")) 
                    {
                        content_length = atoi(request_->GetHeader("Content-Length").c_str());
                    }

                    if (content_length > 0) 
                    {
                        state_ = HttpRequestParseState::BODY;
                    } 
                    else 
                    {
                        state_ = HttpRequestParseState::COMPLETE;
                    }
                    start = end + 1;
                } 
                else 
                {
                    state_ = HttpRequestParseState::kINVALID;
                }
                break;
            }

            case HttpRequestParseState::BODY: 
            {
                // 解析请求体
                int content_length = 0;
                if (request_->GetHeaders().count("Content-Length")) 
                {
                    content_length = atoi(request_->GetHeader("Content-Length").c_str());
                }

                // 判断 Body 是否完整
                if (size - (start - begin) >= content_length) 
                {
                    request_->SetBody(std::string(start, start + content_length));
                    state_ = HttpRequestParseState::COMPLETE;
                    if (request_->GetMethodString() == "POST") 
                    {
                        auto it = request_->GetHeaders().find("Content-Type");
                        if (it != request_->GetHeaders().end() &&
                            it->second.find("application/x-www-form-urlencoded") != std::string::npos) 
                        {
                            ParseUrlEncodedForm(request_->GetBody(), request_.get());
                        }
                    }

                } 
                else 
                {
                    // Body 未完整，等待更多数据
                }
                break;
            }

            default:
                state_ = HttpRequestParseState::kINVALID;
                break;
        }

        end++;
    }

    return state_ == HttpRequestParseState::COMPLETE;
}

// 获取解析出的 HttpRequest 对象
HttpRequest* HttpContext::GetRequest() {return request_.get();}

// 重置解析状态机和请求对象
void HttpContext::ResetContextStatus() 
{
    state_ = HttpRequestParseState::START;
<<<<<<< HEAD
    request_ = std::unique_ptr<HttpRequest>(new HttpRequest()); ;  // 重新分配一个 HttpRequest
=======
    request_ = std::make_unique<HttpRequest>();  // 重新分配一个 HttpRequest
>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14
}
