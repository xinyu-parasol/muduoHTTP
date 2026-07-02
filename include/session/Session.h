#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include <chrono>

//session会话类：表示一个用户会话，用于保存会话数据并管理会话过期
namespace http {
    namespace session {
        class SessionManager;

        class Session : public std::enable_shared_from_this<Session> {
        public:
            //构造函数默认1小时过期
            Session(const std::string& sessionId, SessionManager* sessionManager, int maxAge = 3600);

            const std::string& getId() const {
                return sessionId_;
            }

            bool isExpired() const; //检查是否过期
            void refresh(); //刷新过期时间

            void setManager(SessionManager* sessionManager) {
                sessionManager_ = sessionManager;
            }
            SessionManager* getSessionManager() const {
                return sessionManager_;
            }

            //数据存取
            void setValue(const std::string& key, const std::string& value);  //存储键值对
            std::string getValue(const std::string& key) const;
            void remove(const std::string& key);   //移除键值对
            void clear();
        private:
            std::string                                 sessionId_;
            std::unordered_map<std::string, std::string> data_;
            std::chrono::system_clock::time_point       expiryTime_;
            int                                         maxAge_;   //过期时间
            SessionManager*                             sessionManager_;
        };
    }
}