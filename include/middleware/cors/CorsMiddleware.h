#pragma once

#include "CorsConfig.h"
#include "../Middleware.h"
#include "../../http/HttpRequest.h"
#include "../../http/HttpResponse.h"

//CORS中间件实现
namespace http {
    namespace middleware {
        class CorsMiddleware : public Middleware {
        public:
            explicit CorsMiddleware(const CorsConfig &config = CorsConfig::defaultCorsConfig());

            void before(HttpRequest &request) override;
            void after(HttpResponse &response) override;

            std::string join(const std::vector<std::string>& strings, const std::string& delimiter);
        private:
            bool isOriginAllowed(const std::string& origin) const;
            void hanlePreflightRequest(const HttpRequest& request, HttpResponse& response);
            void addCorsHeaders(HttpResponse& response, const std::string& origin);

            CorsConfig config_;
        };
    }
}