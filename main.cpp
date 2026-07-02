#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>

#include "include/router/Router.h"
#include "include/http/HttpServer.h"
#include "include/http/HttpResponse.h"

using namespace http;
using namespace muduo::net;

int main() {
    EventLoop loop;
    InetAddress listenAddr(8080);
    HttpServer server(&loop, listenAddr, "MyHttpServer");

    // 注册路由（示例）
    server.router().registerCallback(http::HttpRequest::kGet, "/hello",
        [](const http::HttpRequest& req, http::HttpResponse* resp) {
            resp->setStatusCode(http::HttpResponse::k200Ok);
            resp->setBody("Hello, World!\n");
        });

    server.start();
    loop.loop();
    return 0;
}