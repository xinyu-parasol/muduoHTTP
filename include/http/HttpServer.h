#pragma once
#include <functional>
#include <muduo/base/noncopyable.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpServer.h>

#include "../router/Router.h"
class HttpRequest;
class HttpResponse;

namespace http {
    class HttpServer : muduo::noncopyable {
    public:
        using HttpCallback = std::function<void(const HttpRequest&, HttpResponse*)>;

        //构造函数
        HttpServer(int port,
                    const std::string& name,
                    bool useSSL = false,
                    muduo::net::TcpServer::Option option = muduo::net::TcpServer::kNoReusePort);
    private:
        muduo::net::InetAddress   listenAddr_;
        muduo::net::TcpServer     server_;
        muduo::net::EventLoop     mainloop_;
        HttpCallback              httpCallback_;
        Router                    router_;
    };
}