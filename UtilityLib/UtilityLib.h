#ifndef UTILITYLIB_H
#define UTILITYLIB_H

// @Author: Alperen Metintaþ
// @Mail  : metintasalperen@gmail.com
// 
// UtilityLib
// 
// Contains some helper functions about following fields:
// 
// 1. String:
// Uses std::string, provides functions to manipulate strings
// To enable, define STRING_ON
// 
// 2. File IO
// Provides functions for making file IO
// To enable, define FILE_IO_ON
// 
// 3. Windows Network IO
// Provides functions for networking using Winsock2 API
// To enable, define WINDOWS_NETWORK_IO_ON
// 
// 4. Json:
// Directly includes nlohmann/json single header Json library
// To enable, define JSON_ON

#include "StringUtility.h"

#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <cctype>

#ifdef FILE_IO_ON
#include <fstream>
#endif

#ifdef WINDOWS_NETWORK_IO_ON
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <mutex>
#include <queue>

#pragma comment(lib, "Ws2_32.lib")

#define WM_SOCKET (WM_APP + 1)
#define WM_SEND_STRING (WM_APP + 2)

#undef WIN32_LEAN_AND_MEAN
#else
// WIN32_LEAN_AND_MEAN already defined somewhere else, do not undefine it here to not mess with other files that defined it
#include <Windows.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <mutex>

#pragma comment(lib, "Ws2_32.lib")

#define WM_SOCKET (WM_APP + 1)
#define WM_SEND_STRING (WM_APP + 2)
#endif // WIN32_LEAN_AND_MEAN
#endif // WINDOWS_NETWORK_IO_ON

#ifdef JSON_ON
#include "nlohmann\json.hpp"
#endif

namespace UtilityLib
{

    #ifdef WINDOWS_NETWORK_IO_ON
    namespace NetworkIO
    {


        class EventSelectBase
        {
        public:
            virtual void ThreadEntryPoint() = 0;
            virtual void PushMessage(const std::string& msg) = 0;
            virtual void Stop() = 0;
        };

        class ClientEventSelectCls : public Socket, public EventSelectBase
        {
        private:
            WSAEVENT Event;
            std::queue<std::string> MessageQueue;
            std::mutex MessageMutex;
            bool IsRunning;
            std::string RecvbufString;
            std::mutex RecvbufMutex;

        public:
            ClientEventSelectCls() : 
                Socket(), 
                Event(WSACreateEvent()), 
                IsRunning(true) 
            {
            }
            ClientEventSelectCls(const addrinfo& hints) : 
                Socket(hints), 
                Event(WSACreateEvent()), 
                IsRunning(true)
            {
            }
            ClientEventSelectCls(const addrinfo& hints, const std::string& nodeName, const std::string& serviceName) : 
                Socket(hints, nodeName, serviceName), 
                Event(WSACreateEvent()), 
                IsRunning(true)
            {
            }

            std::string GetRecvBuffer()
            {
                std::lock_guard<std::mutex> lock(RecvbufMutex);
                return RecvbufString;
            }

            void Stop() override
            {
                IsRunning = false;
            }
            void PushMessage(const std::string& msg) override
            {
                std::lock_guard<std::mutex> lock(MessageMutex);
                MessageQueue.push(msg);
            }
            void ThreadEntryPoint() override
            {
                if (!IsWSAStartupSuccessful || !IsConnected)
                {
                    std::cerr << "Cannot start thread: Socket not initialized or connected" << std::endl;
                    return;
                }

                if (WSAEventSelect(GetSocket(), Event, FD_READ | FD_CLOSE) == SOCKET_ERROR)
                {
                    std::cerr << "WSAEventSelect failed: " << WSAGetLastError() << std::endl;
                    return;
                }

                while (IsRunning)
                {
                    DWORD result = WSAWaitForMultipleEvents(1, &Event, FALSE, 100, FALSE);
                    if (result == WSA_WAIT_TIMEOUT)
                    {
                        CheckSendQueue();
                    }
                    else if (result == WSA_WAIT_FAILED)
                    {
                        std::cerr << "WSAWaitForMultipleEvents failed: " << WSAGetLastError() << std::endl;
                        break;
                    }

                    WSANETWORKEVENTS netEvents;
                    if (WSAEnumNetworkEvents(GetSocket(), Event, &netEvents) == SOCKET_ERROR)
                    {
                        std::cerr << "WSAEnumNetworkEvents failed: " << WSAGetLastError() << std::endl;
                        break;
                    }

                    if (netEvents.lNetworkEvents & FD_READ)
                    {
                        std::string recvbuf;
                        int32_t recvBytes = Receive(recvbuf);
                        if (recvBytes > 0)
                        {
                            std::lock_guard<std::mutex> lock(RecvbufMutex);
                            RecvbufString = recvbuf;
                            std::cout << "Received: " << RecvbufString << "\n";
                        }
                    }
                    if (netEvents.lNetworkEvents & FD_CLOSE)
                    {
                        std::cout << "FD_CLOSE received" << std::endl;
                        Disconnect();
                        IsRunning = false;
                    }

                    CheckSendQueue();
                }
            }

        private:
            void CheckSendQueue()
            {
                std::lock_guard<std::mutex> lock(MessageMutex);
                while (!MessageQueue.empty())
                {
                    std::string msg = MessageQueue.front();
                    MessageQueue.pop();

                    if (GetSocket() != INVALID_SOCKET)
                    {
                        int32_t bytesSent = Send(msg);
                        if (bytesSent < 0)
                        {
                            std::cerr << "Error while sending: " << WSAGetLastError() << std::endl;
                            Stop();
                            return;
                        }
                    }
                }
            }
        };

        
    }; // End of namespace "NetworkIO"
    #endif
}; // End of namespace "UtilityLib"

#endif