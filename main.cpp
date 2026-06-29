#include <muduo/net/EventLoop.h>
#include <muduo/net/http/HttpServer.h>
#include <muduo/net/http/HttpRequest.h>
#include <muduo/net/http/HttpResponse.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/Buffer.h>
#include <string>

using namespace muduo;
using namespace muduo::net;

void onHttpRequest(const HttpRequest& req, HttpResponse* resp)
{
    resp->setStatusCode(HttpResponse::k200Ok);
    resp->setStatusMessage("OK");
    resp->setContentType("text/html; charset=utf-8");
    resp->addHeader("Server", "Muduo Verify Demo");

    std::string body = R"(
<html>
<head>
    <meta charset="utf-8">
    <title>Muduo 环境验证成功</title>
</head>
<body>
    <h1>✅ Muduo 编译、头文件、链接全部正常！</h1>
    <p>请求路径：)" + req.path() + R"(</p>
</body>
</html>
)";
    resp->setBody(body);
}

int main()
{
    EventLoop loop;
    InetAddress addr("0.0.0.0", 8080);
    HttpServer server(&loop, addr, "MuduoHttpTest");

    // 签名完全匹配，不再报类型转换错误
    server.setHttpCallback(onHttpRequest);

    printf("Muduo HTTP 服务启动，监听 0.0.0.0:8080\n");
    printf("浏览器访问 http://127.0.0.1:8080 验证\n");

    server.start();
    loop.loop();

    return 0;
}