//http框架核心，负责接受连接、读取请求和发送响应
#include "../../include/http/HttpServer.h"
#include <muduo/net/Buffer.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/base/Logging.h>

namespace http {

HttpServer::HttpServer(muduo::net::EventLoop* loop,
                       const muduo::net::InetAddress& listenAddr,
                       const std::string& name)
    : server_(loop, listenAddr, name)
{
    server_.setConnectionCallback(
        std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
    server_.setMessageCallback(
        std::bind(&HttpServer::onMessage, this,
                  std::placeholders::_1,
                  std::placeholders::_2,
                  std::placeholders::_3));
}

void HttpServer::start() {
    server_.start();
}

void HttpServer::onConnection(const muduo::net::TcpConnectionPtr& conn) {
    if (conn->connected()) {
        conn->setContext(HttpContext());
        LOG_INFO << "New connection from " << conn->peerAddress().toIpPort();
    } else {
        LOG_INFO << "Connection closed";
    }
}

void HttpServer::onMessage(const muduo::net::TcpConnectionPtr& conn,
                           muduo::net::Buffer* buf,
                           muduo::Timestamp receiveTime) {
    HttpContext* context = boost::any_cast<HttpContext>(conn->getMutableContext());

    while (true) {
        bool ok = context->parseRequest(buf, receiveTime);
        if (!ok) {
            LOG_ERROR << "HTTP parse error, sending 400";
            HttpResponse resp;
            resp.setStatusCode(HttpResponse::k400BadRequest);
            resp.setStatusMessage("Bad Request");
            resp.setCloseConnection(true);
            muduo::net::Buffer output;
            resp.appendToBuffer(&output);
            conn->send(&output);
            conn->shutdown();
            return;
        }

        if (context->gotAll()) {
            HttpRequest& req = context->request();
            HttpResponse response;
            response.setCloseConnection(false);

            // 执行中间件前置处理
            middlewareChain_.processBefore(req);

            // 路由匹配
            bool routed = router_.route(req, &response);
            if (!routed && httpCallback_) {
                httpCallback_(req, &response);
            } else if (!routed) {
                response.setStatusCode(HttpResponse::k404NotFound);
                response.setStatusMessage("Not Found");
                response.setBody("404 Not Found\n");
            }

            // 执行中间件后置处理
            middlewareChain_.processAfter(response);

            muduo::net::Buffer output;
            response.appendToBuffer(&output);
            conn->send(&output);

            if (response.closeConnection()) {
                conn->shutdown();
                return;
            }

            context->reset();
        } else {
            break;
        }
    }
}

} // namespace http
