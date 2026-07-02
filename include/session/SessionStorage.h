#pragma once
#include <memory>

#include "Session.h"

//定义存储接口的抽象类
//会话数据既可以存在数据库中，也可以存在内存中
namespace http {
    namespace session {
        //会话存储
        class SessionStorage {
        public:
            virtual ~SessionStorage() = default;
            virtual void save(std::shared_ptr<Session> session) = 0;
            virtual std::shared_ptr<Session> load(const std::string& sessionId) = 0;
            virtual void remove(const std::string& sessionId) = 0;
        };

        //内存会话存储
        class MemorySessionStorage : public SessionStorage {
        public:
            void save(std::shared_ptr<Session> session) override;
            std::shared_ptr<Session> load(const std::string& sessionId) override;
            void remove(const std::string& sessionId) override;
        private:
            std::unordered_map<std::string, std::shared_ptr<Session>> sessions_;
        };
    }
}
