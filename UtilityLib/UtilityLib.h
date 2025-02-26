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


        class AsyncSelectBase
        {
        public:
            HWND Hwnd;

            AsyncSelectBase() : Hwnd(nullptr) {}

            virtual void MessageLoop() = 0;
            virtual bool RegisterMessageWindowClass(const std::string& name) = 0;
            virtual bool CreateMessageWindow(const std::string& name) = 0;
            virtual void ThreadEntryPoint() = 0;
            virtual LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) = 0;

            static LRESULT CALLBACK ThreadWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
            {
                AsyncSelectBase* thisPtr = NULL;
                if (msg == WM_NCCREATE)
                {
                    CREATESTRUCT* pcs = reinterpret_cast<CREATESTRUCT*>(lParam);
                    thisPtr = reinterpret_cast<AsyncSelectBase*>(pcs->lpCreateParams);
                    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(thisPtr));
                    thisPtr->Hwnd = hwnd;
                }
                else
                {
                    thisPtr = reinterpret_cast<AsyncSelectBase*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
                }

                if (thisPtr)
                {
                    return thisPtr->HandleMessage(msg, wParam, lParam);
                }
                else
                {
                    return DefWindowProc(hwnd, msg, wParam, lParam);
                }
            }
        };

        class ClientAsnycSelectCls : public Socket, public AsyncSelectBase
        {
        public:
            std::string RecvbufString;
            std::mutex RecvbufMutex;

            ClientAsnycSelectCls() : 
                Socket(), 
                AsyncSelectBase()
            {
            }
            ClientAsnycSelectCls(const addrinfo& hints) : 
                Socket(hints), 
                AsyncSelectBase() 
            {
            }
            ClientAsnycSelectCls(const addrinfo& hints, const std::string& nodeName, const std::string& serviceName) : 
                Socket(hints, nodeName, serviceName), 
                AsyncSelectBase() 
            {
            }

            // WSAAsyncSelect implementation
            void MessageLoop() override
            {
                MSG msg;
                while (GetMessage(&msg, nullptr, 0, 0))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
            bool RegisterMessageWindowClass(const std::string& name) override
            {
                WNDCLASS wc = { 0 };
                wc.lpfnWndProc = UtilityLib::NetworkIO::AsyncSelectBase::ThreadWndProc;
                wc.hInstance = GetModuleHandle(NULL);
                wc.lpszClassName = TEXT(name.c_str());

                return RegisterClass(&wc) != 0;
            }
            bool CreateMessageWindow(const std::string& name) override
            {
                Hwnd = CreateWindow(name.c_str(),
                    "",
                    0,
                    0, 0, 0, 0,
                    HWND_MESSAGE,
                    NULL,
                    GetModuleHandle(NULL),
                    this);

                if (!Hwnd)
                {
                    return false;
                }

                return true;
            }
            LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) override
            {
                if (msg == WM_SOCKET)
                {
                    int32_t event = WSAGETSELECTEVENT(lParam);
                    int32_t error = WSAGETSELECTERROR(lParam);

                    if (error != 0)
                    {
                        std::cerr << "Some error occurred: " << error << std::endl;
                        return 0;
                    }

                    switch (event)
                    {
                    case FD_READ:
                    {
                        std::string recvbuf;
                        int32_t recvBytes = Receive(recvbuf);

                        std::lock_guard<std::mutex> lock(RecvbufMutex);
                        RecvbufString = recvbuf;

                        break;
                    }
                    case FD_CLOSE:
                    {
                        Disconnect();
                        break;
                    }
                    }

                    return 0;
                }
                else if (msg == WM_SEND_STRING)
                {
                    const char* sendbuf = reinterpret_cast<const char*>(lParam);

                    int32_t bytesSent = Send(sendbuf);
                    if (bytesSent < 0)
                    {
                        std::cerr << "Error while sending: " << WSAGetLastError() << std::endl;
                    }

                    return 0;
                }
                else
                {
                    return DefWindowProc(Hwnd, msg, wParam, lParam);
                }
            }
            void ThreadEntryPoint() override
            {
                bool result = RegisterMessageWindowClass("ClientThreadWindowClass");
                if (!result)
                {
                    std::cerr << "Register Class call failed, last error: " << GetLastError() << std::endl;
                    return;
                }

                result = CreateMessageWindow("ClientThreadWindowClass");
                if (!result)
                {
                    std::cerr << "Window Creation failed, last error: " << GetLastError() << std::endl;
                    return;
                }

                int32_t iResult = WSAAsyncSelect(GetSocket(), Hwnd, WM_SOCKET, FD_READ | FD_CLOSE);
                if (iResult == SOCKET_ERROR)
                {
                    std::cerr << "Something is wrong with the socket, last error: " << WSAGetLastError() << std::endl;
                    DestroyWindow(Hwnd);
                    return;
                }

                MessageLoop();
                DestroyWindow(Hwnd);
            }

            // WSAEventSelect implementation
        };

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