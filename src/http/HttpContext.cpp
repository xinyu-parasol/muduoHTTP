#include "../../include/http/HttpContext.h"
//用于管理HTTP请求在处理过程中的状态，存储httpRequest对象
using namespace muduo::net;
using namespace muduo;
//从buffer中解读数据
namespace http {
    //将报文解析出来的关键信息存储到httpRequest对象中
    bool HttpContext::parseRequest(muduo::net::Buffer *buf, muduo::Timestamp receiveTime) {
        bool ok = true; //解析每行请求格式信息是否正确
        bool hasMore = true;
        while (hasMore) {
            if (state_ == kExpectRequestLine)
            {
                const char* crlf = buf->findCRLF();
                if (crlf) {
                    ok = processRequestLine(buf->peek(), crlf);
                    if (ok) {
                        request_.setReceiveTime(receiveTime);
                        buf->retrieveUntil(crlf + 2);
                        state_ = kExpectHeaders;
                    }else {
                        hasMore = false;
                    }
                }else {
                    hasMore = false;
                }
            }
            else if (state_ == kExpectHeaders)
            {
                const char* crlf = buf->findCRLF();
                if (crlf) {
                    const char* colon = std::find(buf->peek(), crlf, ':');
                    if (colon < crlf) {
                        request_.addHeader(buf->peek(), colon, crlf);
                    }else if (buf->peek() == crlf) {
                        //空行，结束Header
                        //根据请求方法和报文长度判断是否需要继续读取Body
                        if (request_.method() == HttpRequest::kPost ||
                            request_.method() == HttpRequest::kPut) {
                            std::string contentLength = request_.getHeader("Content-Length");
                            if (contentLength.empty()) {
                                request_.setContentLength(std::stoi(contentLength));
                                if (request_.contentLength() > 0) {
                                    state_ =  kExpectBody;
                                }else {
                                    state_ = kGotAll;
                                    hasMore = false;
                                }
                            }else { //Post/put请求没有contentlength，是HTTP语法错误
                                ok = false;
                                hasMore = false;
                            }
                        }else {  //get/head/delete没有请求体的方法直接完成
                            state_ = kGotAll;
                            hasMore = false;
                        }
                    }else {  //Header行格式错误
                        ok = false;
                        hasMore = false;
                    }
                    buf->retrieveUntil(crlf + 2);  //开始读指针指向下一行数据
                }else {
                    hasMore = false;
                }
            }
            else if (state_ == kExpectBody)
            {
                //检查缓冲区中是否有足够元素
                if (buf->readableBytes() < request_.contentLength()) {
                    hasMore = false;  //数据不完整、等待
                    return true;
                }
                //只读contentlength指定的长度
                std::string body(buf->peek(), buf->peek() + request_.contentLength());
                request_.setBody(body);
                //准确读移动指针
                buf->retrieve(request_.contentLength());

                state_ = kGotAll;
                hasMore = false;
            }
        }
        return ok;
    }

    //解析请求行
    bool HttpContext::processRequestLine(const char *begin, const char *end) {
        bool succeed = false;
        const char* start = begin;
        const char* space = std::find(start, end, ' ');
        if (space != end && request_.setMethod(start,space)) {
            start = space + 1;
            space = std::find(start, end, ' ');
            if (space != end) {
                const char* argumentStart =std::find(start, space, '?');
                if (argumentStart != space) { //请求带参数
                    request_.setPath(start, argumentStart);
                    request_.setQueryParameters(argumentStart + 1, space);
                }else { //请求不带参数
                    request_.setPath(start, space);
                }

                start = space + 1;
                succeed = ((end - start == 8) &&
                            std::equal(start, end - 1, "HTTP/1."));
                if (succeed) {
                    if (*(end - 1) == '1') {
                        request_.setVersion("HTTP/1.1");
                    }else if (*(end - 1) == '0') {
                        request_.setVersion("HTTP/1.0");
                    }else {
                        succeed = false;
                    }
                }
            }
        }
        return succeed;
    }
}
