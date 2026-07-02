#pragma once
#include <string>
#include <vector>

//CORS配置类
namespace http {
    namespace middleware {
        struct CorsConfig {
            std::vector<std::string> allowOrigins;
            std::vector<std::string> allowMethods;
            std::vector<std::string> allowHeaders;
            bool allowCredentials = false;
            int maxAge = 3600;

            static CorsConfig defaultCorsConfig() {
                CorsConfig config;
                config.allowOrigins = {"*"};
                config.allowMethods = {"GET", "POST", "PUT", "DELETE", "OPTIONS"};
                config.allowHeaders = {"Content-Type", "Authorization"};
                return config;
            }
        };
    }
}