#include "../../include/http/HttpRequest.h"

#include <bits/locale_facets_nonio.h>
//http请求：负责解析http方法、头部和主体内容

namespace http {
    void HttpRequest::setReceiveTime(muduo::Timestamp t) {
        receiveTime_ = t;
    }

    bool HttpRequest::setMethod(const char *start, const char *end) {
        assert(method_ == kInvalid);
        std::string m(start, end);
        if (m == "GET") {
            method_ = kGet;
        }else if (m == "POST") {
            method_ = kPost;
        }else if (m == "PUT") {
            method_ = kPut;
        }else if (m == "DELETE") {
            method_ = kDelete;
        }else if (m == "OPTIONS") {
            method_ = kOptions;
        }else {
            method_ = kInvalid;
        }
        return method_ != kInvalid;
    }

    void HttpRequest::setPath(const char *start, const char *end) {
        path_.assign(start, end);
    }

    void HttpRequest::setPathParameters(const std::string &key, const std::string &value) { //路径参数
        pathParameters_[key] = value;
    }
    std::string HttpRequest::getPathParameters(const std::string &key) const {
        auto it = pathParameters_.find(key);
        if (it != pathParameters_.end()) {
            return it->second;
        }
        return "";
    }

    std::string HttpRequest::getQueryParameters(const std::string& key) const {
        auto it = queryParameters_.find(key);
        if (it != queryParameters_.end()) {
            return it->second;
        }
        return "";
    }
    //将整个查询字符串（？后面）解析为键值对存入queryParameters_中，不负责解码，只做分割
    void HttpRequest::setQueryParameters(const char *start, const char *end) { //查询参数
        std::string argumentStr(start, end);
        std::string::size_type pos = 0;
        std::string::size_type prev = 0;
        //按照 & 分割多个参数
        while ((pos = argumentStr.find("&", prev)) != std::string::npos) {
            std::string pair = argumentStr.substr(prev, pos - prev);
            std::string::size_type equalPos = pair.find("=");

            if (equalPos != std::string::npos) {
                std::string key = pair.substr(0, equalPos);
                std::string value = pair.substr(equalPos + 1);
                queryParameters_[key] = value;
            }
            prev = pos + 1;
        }
        //处理最后一个参数
        std::string lastPair = argumentStr.substr(prev);
        std::string::size_type equalPos = lastPair.find("=");
        if (equalPos != std::string::npos) {
            std::string key = lastPair.substr(0, equalPos);
            std::string value = lastPair.substr(equalPos + 1);
            queryParameters_[key] = value;
        }
    }

    void HttpRequest::addHeader(const char *start, const char *colon, const char *end) {
        //解析请求头部并清理空白，colon指向的是第一个冒号的位置
        std::string key(start, colon);
        ++colon;
        while (colon < end && isspace(*colon)) { //消除冒号后面可能存在的空格
            ++colon;
        }
        std::string value(colon, end);
        while (!value.empty() && isspace(value[value.size() - 1])) { //消除尾部空格
            value.resize(value.size() - 1);
        }
        headers_[key] = value;
    }
    std::string HttpRequest::getHeader(const std::string &field) const {
        std::string result;
        auto it = headers_.find(field);
        if (it != headers_.end()) {
            result = it->second;
        }
        return result;
    }

    void HttpRequest::swap(HttpRequest &that) {
        std::swap(method_, that.method_);
        std::swap(path_, that.path_);
        std::swap(pathParameters_, that.pathParameters_);
        std::swap(queryParameters_, that.queryParameters_);
        std::swap(version_, that.version_);
        std::swap(headers_, that.headers_);
        std::swap(receiveTime_, that.receiveTime_);
    }
}
