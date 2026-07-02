#include "../../include/session/SessionManager.h"
#include <iomanip>
#include <sstream>
#include <iostream>

namespace http {
    namespace session {
        //初始化会话管理器，设置会话存储对象和随机数生成器
        SessionManager::SessionManager(std::unique_ptr<SessionStorage> storage)
            : storage_(std::move(storage))
            , rng_(std::random_device{}())
        {}

        //从请求中获取或创建对话
        std::shared_ptr<Session> SessionManager::getSession(const HttpRequest &req, HttpResponse *resp) {
            std::string sessionId = getSessionIdFromCookie(req);
            std::shared_ptr<Session> session;

            if (!sessionId.empty()) {
                session = storage_->load(sessionId);
            }

            if (!session || session->isExpired()) {
                sessionId = generateSessionId();
                session = std::make_shared<Session>(sessionId,this);
                setSessionCookie(sessionId, resp);
            }else {  //为现有会话设置管理器
                session->setManager(this);
            }

            session->refresh();
            storage_->save(session);
            return session;
        }

        //生成唯一会话标识符，确保会话安全性和唯一性
        std::string SessionManager::generateSessionId() {
            std::stringstream ss;
            std::uniform_int_distribution<> dist(0, 15);

            //生成32个字符的会话id,每个字符是十六进制数字
            for (int i = 0 ; i < 32 ; ++i) {
                ss << std::hex << dist(rng_);
            }
            return ss.str();
        }

        void SessionManager::destroySession(const std::string &sessionId) {
            storage_->remove(sessionId);
        }

        void SessionManager::cleanExpiredSessions() {

        }

        std::string SessionManager::getSessionIdFromCookie(const HttpRequest &req) {
            std::string sessionId;
            std::string cookie = req.getHeader("Cookie");
            if (!cookie.empty()) {
                size_t pos = cookie.find("sessionId=");
                if (pos != std::string::npos) {
                    pos += 10; //跳过sessionId=
                    size_t end = cookie.find(";", pos);
                    if (end != std::string::npos) {
                        sessionId = cookie.substr(pos, end - pos);
                    }else {
                        sessionId = cookie.substr(pos);
                    }
                }
            }

            return sessionId;
        }

        void SessionManager::setSessionCookie(const std::string &sessionId, HttpResponse *resp) {
            //设置会话id到响应头，作为Cookie
            std::string cookie = "sessionId=" + sessionId + "; Path=/; HttpOnly";
            resp->addHeader("Set-Cookie", cookie);
        }
    }
}