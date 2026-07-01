#pragma once

#include <regex>

#include "RouterHandler.h"
#include "../http/HttpRequest.h"
#include "../http/HttpResponse.h"

namespace http {
    namespace router {
        //选择注册对象式的路由处理器还是注册回调函数式的处理器取决于处理器执行的复杂程度
        //如果是简单的处理可以注册回调函数
class Router {
public:
    using HandlerPtr = std::shared_ptr<RouterHandler>;
    using HandlerCallback = std::function<void(const HttpRequest &, HttpResponse *)>;

    //路由键: 请求方法 + URL
    struct RouteKey {
        HttpRequest::Method method;
        std::string path;
        bool operator==(const RouteKey &other) const {
            return method == other.method && path == other.path;
        }
    };

    //为RouteKey定义哈希函数
    struct RouteKeyHash {
        size_t operator()(const RouteKey &key) const {
            size_t methodHash = std::hash<int>{}(static_cast<int>(key.method));
            size_t pathHash = std::hash<std::string>{}(key.path);
            return methodHash * 31 + pathHash;
        }
    };

    //注册路由处理器
    void registerHandler(HttpRequest::Method method, const std::string &path, HandlerPtr handler);

    //注册回调函数形式的处理器
    void registerCallback(HttpRequest::Method method, const std::string &path, const HandlerCallback &callback);

    //注册动态路由处理器
    void addRegexHandler(HttpRequest::Method method, const std::string &path, HandlerPtr handler) {
        std::regex pathRegex = convertToRegex(path);
        regexHandlers_.emplace_back(method, pathRegex, handler);
    }
    //注册动态路由处理函数
    void addRegexCallback(HttpRequest::Method method, const std::string &path, const HandlerCallback &callback) {
        std::regex pathRegex = convertToRegex(path);
        regexCallbacks_.emplace_back(method, pathRegex, callback);
    }
    //处理请求
    bool route(const HttpRequest &req, HttpResponse *resp);

private:
    std::regex convertToRegex(const std::string &pathPattern) {
        //将路径模式转化为正则表达式
        std::string regexPattern = "^" + std::regex_replace(pathPattern, std::regex(R"(/:([^/]+))"), R"(/([^/]+))") + "$";
        return std::regex(regexPattern);
    }

    void extractPathParameters(const std::smatch &matches, HttpRequest &request) {
        //提取路径参数
        for (size_t i = 1; i < matches.size(); ++i) {
            request.setPathParameters("param" + std::to_string(i), matches[i].str());
        }
    }
private:
    struct RouterCallbackObj {
        HttpRequest::Method method_;
        std::regex pathRegex_;
        HandlerCallback callback_;
        RouterCallbackObj(HttpRequest::Method method, std::regex pathRegex, const HandlerCallback &callback)
            :method_(method), pathRegex_(pathRegex), callback_(callback) {}
    };

    struct RouterHandlerObj {
        HttpRequest::Method method_;
        std::regex pathRegex_;
        HandlerPtr handler_;
        RouterHandlerObj(HttpRequest::Method method, std::regex pathRegex, HandlerPtr handler)
            :method_(method), pathRegex_(pathRegex), handler_(handler) {}
    };
    //精确匹配
    std::unordered_map<RouteKey, HandlerPtr, RouteKeyHash>         handlers_;
    std::unordered_map<RouteKey, HandlerCallback, RouteKeyHash>    callbacks_;
    //正则匹配
    std::vector<RouterHandlerObj>   regexHandlers_;
    std::vector<RouterCallbackObj>  regexCallbacks_;
};
    }
}