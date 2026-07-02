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
    // 绑定回调
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
        // 为每个新连接创建 HttpContext 并绑定到 connection 上下文
        conn->setContext(HttpContext());
        LOG_INFO << "New connection from " << conn->peerAddress().toIpPort();
    } else {
        // 连接关闭，无需清理（context 自动析构）
        LOG_INFO << "Connection closed";
    }
}

void HttpServer::onMessage(const muduo::net::TcpConnectionPtr& conn,
                           muduo::net::Buffer* buf,
                           muduo::Timestamp receiveTime) {
    // 获取当前连接的 HttpContext
    HttpContext* context = boost::any_cast<HttpContext>(conn->getMutableContext());

    // 不断尝试解析，直到数据不足或解析出错
    while (true) {
        bool ok = context->parseRequest(buf, receiveTime);
        if (!ok) { // 解析失败（格式错误）
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

        if (context->gotAll()) { // 完整请求已收到
            // 准备响应
            HttpResponse response;
            // 默认长连接（根据 HTTP 版本和 Connection 头决定，简化起见先保持长连接）
            response.setCloseConnection(false);

            // 先尝试路由匹配
            bool routed = router_.route(context->request(), &response);
            if (!routed && httpCallback_) {
                // 若未匹配且设置了全局回调，则调用
                httpCallback_(context->request(), &response);
            } else if (!routed) {
                // 完全未处理，返回 404
                response.setStatusCode(HttpResponse::k404NotFound);
                response.setStatusMessage("Not Found");
                response.setBody("404 Not Found\n");
                // 添加 Content-Length 头部（由 appendToBuffer 负责）
            }

            // 发送响应
            muduo::net::Buffer output;
            response.appendToBuffer(&output);
            conn->send(&output);

            // 根据响应决定是否关闭连接
            if (response.closeConnection()) {
                conn->shutdown();
                return;
            }

            // 重置 HttpContext 以便接收下一个请求（相同连接）
            context->reset();
        } else {
            // 数据不足，等待下次消息
            break;
        }
    }
}

} // namespace http