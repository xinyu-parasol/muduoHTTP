#pragma once
#include <functional>
#include <muduo/base/noncopyable.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpServer.h>

#include "../router/Router.h"
#include "../http/HttpRequest.h"
#include "../http/HttpResponse.h"
#include "../http/HttpContext.h"

class HttpRequest;
class HttpResponse;

namespace http {
    using HttpCallback = std::function<void(const HttpRequest&, HttpResponse*)>;

    class HttpServer : muduo::noncopyable {
    public:
        HttpServer(muduo::net::EventLoop* loop,
                    const muduo::net::InetAddress& listenAddr,
                    const std::string& name = "HttpServer");
        ~HttpServer() = default;

        // 设置业务回调（优先级低于路由器，当路由器未匹配时调用）
        void setHttpCallback(const HttpCallback& cb) { httpCallback_ = cb; }

        // 设置路由器（也可以直接在构造函数中传入）
        void setRouter(const router::Router& router) { router_ = router; }
        router::Router& router() { return router_; }

        void start();
    private:
        // 网络回调
        void onConnection(const muduo::net::TcpConnectionPtr& conn);
        void onMessage(const muduo::net::TcpConnectionPtr& conn,
                       muduo::net::Buffer* buf,
                       muduo::Timestamp receiveTime);

        // 处理完整请求（内部函数）
        void onRequest(const muduo::net::TcpConnectionPtr& conn,
                       const HttpRequest& req,
                       HttpResponse* resp);

        muduo::net::TcpServer server_;
        router::Router router_;
        HttpCallback httpCallback_;
    };
}