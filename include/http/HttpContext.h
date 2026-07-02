#pragma once
#include <muduo/net/Buffer.h>

#include "HttpRequest.h"
//解析报文，存储到HttpContext中
namespace http {
    class HttpContext {
    public:
        enum HttpRequestParseState {
            kExpectRequestLine,  //解析请求行
            kExpectHeaders,     //解析请求头
            kExpectBody,       //解析请求体
            kGotAll,          //解析完成
        };

        HttpContext() : state_(kExpectRequestLine) {}

        bool parseRequest(muduo::net::Buffer* buf, muduo::Timestamp receiveTime);
        bool gotAll() const { return state_ == kGotAll; }

        void reset() {
            state_ = kExpectRequestLine;
            HttpRequest dummyData;
            request_.swap(dummyData);
        }

        const HttpRequest& request() const { return request_; }
        HttpRequest& request() { return request_; }
    private:
        bool processRequestLine(const char* start, const char* end);
        HttpRequestParseState state_;
        HttpRequest           request_;
    };
}
