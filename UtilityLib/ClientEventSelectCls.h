#ifndef CLIENTEVENTSELECTCLS_H
#define CLIENTEVENTSELECTCLS_H

#include "SocketCls.h"
#include <Windows.h>
#include <mutex>
#include <queue>

namespace UtilityLib
{
    namespace Network
    {
        class EventSelectBaseCls
        {
        public:
            virtual void PushMessage(const std::string& msg) = 0;
            virtual void StopThread() = 0;
            virtual void ThreadEntryPoint() = 0;
        };

        class ClientEventSelectCls :
            public SocketClientCls,
            public EventSelectBaseCls
        {
        private:
            WSAEVENT Event;

            std::queue<std::string> MsgQueue;
            std::mutex MsgMutex;

            std::string Buffer;
            std::mutex BufferMutex;

            bool IsRunning;
            
        public:
            ClientEventSelectCls();
            ClientEventSelectCls(const addrinfo& hints,
                const std::string& nodeName,
                const std::string& serviceName);

            std::string GetBuffer();

            void PushMessage(const std::string& msg) override;
            void StopThread() override;
            void ThreadEntryPoint() override;
            
        private:
            void CheckMessageQueue();
            void WriteToBuffer(const std::string& buffer);
        };
    };
};

#endif