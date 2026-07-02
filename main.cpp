#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>

#include "include/router/Router.h"
#include "include/http/HttpServer.h"
#include "include/http/HttpResponse.h"
#include "include/middleware/MiddlewareChain.h"
#include "include/middleware/Middleware.h"
#include "include/session/SessionManager.h"
#include "include/session/SessionStorage.h"

using namespace http;
using namespace muduo::net;

// 测试中间件：为每个响应添加 X-Middleware 头
class DemoMiddleware : public middleware::Middleware {
public:
    void before(HttpRequest& request) override {}
    void after(HttpResponse& response) override {
        response.addHeader("X-Middleware", "processed");
    }
};

int main() {
    EventLoop loop;
    InetAddress listenAddr(8080);
    HttpServer server(&loop, listenAddr, "MyHttpServer");

    // 注册中间件
    server.middlewareChain().addMiddleware(std::make_shared<DemoMiddleware>());

    // 设置 Session 管理器（内存存储）
    server.setSessionManager(std::make_unique<session::SessionManager>(
        std::make_unique<session::MemorySessionStorage>()));
    auto& sm = server.sessionManager();

    // 路由：GET /hello — 基础测试
    server.router().registerCallback(http::HttpRequest::kGet, "/hello",
        [](const http::HttpRequest& req, http::HttpResponse* resp) {
            resp->setStatusCode(http::HttpResponse::k200Ok);
            resp->setBody("Hello, World!\n");
        });

    // 路由：GET /session — 测试 Session（记录访问次数）
    server.router().registerCallback(http::HttpRequest::kGet, "/session",
        [&sm](const http::HttpRequest& req, http::HttpResponse* resp) {
            auto session = sm.getSession(req, resp);
            int count = 0;
            std::string countStr = session->getValue("count");
            if (!countStr.empty()) {
                count = std::stoi(countStr);
            }
            count++;
            session->setValue("count", std::to_string(count));
            resp->setStatusCode(http::HttpResponse::k200Ok);
            resp->setBody("Visit count: " + std::to_string(count) + "\n");
        });

    // 路由：GET /test — 验证中间件是否生效
    server.router().registerCallback(http::HttpRequest::kGet, "/test",
        [](const http::HttpRequest& req, http::HttpResponse* resp) {
            resp->setStatusCode(http::HttpResponse::k200Ok);
            resp->setBody("Middleware is working!\n");
        });

    server.start();
    loop.loop();
    return 0;
}
