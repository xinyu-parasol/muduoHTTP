#pragma once

#include <vector>
#include <memory>
#include "Middleware.h"

//中间件链管理
namespace http {
    namespace middleware {
        class MiddlewareChain {
        public:
            void addMiddleware(std::shared_ptr<Middleware> middleware);
            void processBefore(HttpRequest& request);
            void processAfter(HttpResponse& response);
        private:
            std::vector<std::shared_ptr<Middleware>> middlewares_;
        };
    }
}
