#pragma once

#include "../http/HttpRequest.h"
#include "../http/HttpResponse.h"

//中间件基类接口
namespace http {
    namespace middleware {
        class Middleware {
        public:
            virtual ~Middleware() = default;

            //请求前处理
            virtual void before(::http::HttpRequest& request) = 0;

            //响应后处理
            virtual void after(::http::HttpResponse& response) = 0;

            //设置下一个中间件
            void setNext(std::shared_ptr<Middleware> next) {
                nextMiddleware_ = next;
            }
        protected:
            std::shared_ptr<Middleware> nextMiddleware_;
        };
    }
}