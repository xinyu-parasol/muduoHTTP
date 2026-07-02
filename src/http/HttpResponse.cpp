#include "../../include/http/HttpResponse.h"
//http响应：构建带有适当状态码、头部和主体内容的响应
//将响应数据格式化成一个符合HTTP/1.1协议的字节流
namespace http {
    void HttpResponse::appendToBuffer(muduo::net::Buffer *outputBuf) const {
        //httpResponse封装的信息格式化输出
        char buf[32];
        //状态信息不方便定义一个固定大小的内存存储
        snprintf(buf, sizeof(buf), "%s %d", httpVersion_.c_str(), statusCode_);

        outputBuf->append(buf);
        outputBuf->append(" ");
        outputBuf->append(statusMessage_);
        outputBuf->append("\r\n");

        if (closeConnection_) {
            outputBuf->append("Connection: close\r\n");
        }else {
            outputBuf->append("Connection: Keep-Alive\r\n");
        }

        for (const auto& header : headers_) {
            outputBuf->append(header.first);
            outputBuf->append(": ");
            outputBuf->append(header.second);
            outputBuf->append("\r\n");
        }
        if (!body_.empty()) {
            char len[32];
            snprintf(len, sizeof(len), "%zu", body_.size());
            outputBuf->append("Content-Length: ");
            outputBuf->append(len);
            outputBuf->append("\r\n");
        }
        outputBuf->append("\r\n");
        outputBuf->append(body_);
    }

    void HttpResponse::setDefaultStatusMessage() {
        switch (statusCode_) {
            case k200Ok: statusMessage_ = "OK"; break;
            case k204NoContent: statusMessage_ = "No Content"; break;
            case k301MovedPermanently: statusMessage_ = "Moved Permanently"; break;
            case k400BadRequest: statusMessage_ = "Bad Request"; break;
            case k401Unauthorized: statusMessage_ = "Unauthorized"; break;
            case k403Forbidden: statusMessage_ = "Forbidden"; break;
            case k404NotFound: statusMessage_ = "Not Found"; break;
            case k409Conflict: statusMessage_ = "Conflict"; break;
            case k500InternalServerError: statusMessage_ = "Internal Server Error"; break;
            default: statusMessage_ = ""; break;
        }
    }

    void HttpResponse::setStatusLine(const std::string &version,
                                    HttpStatusCode statusCode,
                                    const std::string &statusMessage)
    {
        httpVersion_ = version;
        statusCode_ = statusCode;
        statusMessage_ = statusMessage;
    }
}
