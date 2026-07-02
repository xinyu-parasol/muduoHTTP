#pragma once
#include <functional>
#include <memory>
#include <muduo/base/noncopyable.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpServer.h>

#include "../router/Router.h"
#include "../http/HttpRequest.h"
#include "../http/HttpResponse.h"
#include "../http/HttpContext.h"
#include "../middleware/MiddlewareChain.h"
#include "../session/SessionManager.h"

namespace http {
    using HttpCallback = std::function<void(const HttpRequest&, HttpResponse*)>;

    class HttpServer : muduo::noncopyable {
    public:
        HttpServer(muduo::net::EventLoop* loop,
                    const muduo::net::InetAddress& listenAddr,
                    const std::string& name = "HttpServer");
        ~HttpServer() = default;

        void setHttpCallback(const HttpCallback& cb) { httpCallback_ = cb; }
        void setRouter(const router::Router& router) { router_ = router; }
        router::Router& router() { return router_; }

        middleware::MiddlewareChain& middlewareChain() { return middlewareChain_; }
        session::SessionManager& sessionManager() { return *sessionManager_; }
        void setSessionManager(std::unique_ptr<session::SessionManager> sm) {
            sessionManager_ = std::move(sm);
        }

        void start();
    private:
        void onConnection(const muduo::net::TcpConnectionPtr& conn);
        void onMessage(const muduo::net::TcpConnectionPtr& conn,
                       muduo::net::Buffer* buf,
                       muduo::Timestamp receiveTime);

        muduo::net::TcpServer server_;
        router::Router router_;
        HttpCallback httpCallback_;
        middleware::MiddlewareChain middlewareChain_;
        std::unique_ptr<session::SessionManager> sessionManager_;
    };
}
