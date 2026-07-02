#include "../../include/session/SessionStorage.h"
#include <iostream>

namespace http {
    namespace session {
        void MemorySessionStorage::save(std::shared_ptr<Session> session) {
            //创建会话副本并存储
            sessions_[session->getId()]  = session;
        }

        //通过会话id从存储中加载会话
        std::shared_ptr<Session> MemorySessionStorage::load(const std::string& sessionId) {
            auto it = sessions_.find(sessionId);
            if (it != sessions_.end()) {
                if (!it->second->isExpired()) {
                    return it->second;
                }else {
                    //如果会话已经过期，需要从存储中移除
                    sessions_.erase(it);
                }
            }
            return nullptr;
        }

        //通过会话id从存储中移除会话
        void MemorySessionStorage::remove(const std::string& sessionId) {
            sessions_.erase(sessionId);
        }
    }
}