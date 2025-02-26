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

    #ifdef FILE_IO_ON
    namespace FileIO
    {
        // IsFileExist()
        // 
        // Summary
        // Check if specified file exist or not
        // 
        // Arguments:
        // std::string "filePath"  --- In
        // 
        // Returns
        // bool
        bool IsFileExist(const std::string& filePath)
        {
            std::ifstream file(filePath);
            return file.good();
        }

        // ReadFile()
        // 
        // Summary
        // Reads the file and returns the content as a single string
        // 
        // Arguments:
        // std::string "filePath"  --- In
        // 
        // Returns:
        // std::string
        std::string ReadFile(const std::string& filePath)
        {
            // Open at the end to get file size
            std::ifstream file(filePath, std::ios::ate);

            // File not opened, return empty string
            if (!file)
            {
                return "";
            }

            // Get file size
            std::streamsize size = file.tellg();
            // Move back to the beginning of file
            file.seekg(0);

            // Allocate space to string according to size
            std::string fileContent(static_cast<size_t>(size), '\0');
            // Read file into the string
            file.read(&fileContent[0], size);
            file.close();

            return fileContent;
        }

        // WriteFile()
        // 
        // Summary
        // Writes string to the specified file
        // 
        // Arguments:
        // std::string "filePath"  --- In
        // std::string "content"   --- In
        // 
        // Returns:
        // bool
        // 
        // Assumptions:
        // 1. If file does not exist, a new file will be created
        // 2. If file contains data, old data is deleted
        bool WriteFile(const std::string& filePath, const std::string& content)
        {
            bool result = false;

            std::ofstream file(filePath);
            if (file.is_open())
            {
                file << content;
                file.close();
                result = true;
            }

            return result;
        }
    }; // End of namespace "FileIO"
    #endif

    #ifdef WINDOWS_NETWORK_IO_ON
    namespace NetworkIO
    {

        enum BlockingModeEnum
        {
            BlockingMode_Blocking = 0,
            BlockingMode_NonBlocking = 1
        };

        class Socket
        {
        private:
            SOCKET Sock;
            addrinfo* AddressInfoResults;
            addrinfo Hints;

        public:
            bool IsConnected;
            bool IsWSAStartupSuccessful;


            Socket() : Sock(INVALID_SOCKET),
                AddressInfoResults(nullptr),
                Hints(),
                IsConnected(false),
                IsWSAStartupSuccessful(InitializeWinsock())
            {
            }
            Socket(const addrinfo& hints) : Sock(INVALID_SOCKET),
                AddressInfoResults(nullptr),
                Hints(hints),
                IsConnected(false),
                IsWSAStartupSuccessful(InitializeWinsock())
            {
            }
            Socket(const addrinfo& hints, const std::string& nodeName, const std::string& serviceName) : Sock(INVALID_SOCKET),
                AddressInfoResults(nullptr),
                Hints(hints),
                IsConnected(false),
                IsWSAStartupSuccessful(InitializeWinsock())
            {
                if (IsWSAStartupSuccessful)
                {
                    bool winsockResult = GetAddressInfo(nodeName, serviceName);
                    if (winsockResult)
                    {
                        winsockResult = CreateSocket();

                        if (winsockResult)
                        {
                            Connect();

                            if (!IsConnected)
                            {
                                std::cerr << "Error while connecting to server: " << WSAGetLastError() << std::endl;
                            }
                        }
                        else
                        {
                            std::cerr << "Error during socket creation: " << WSAGetLastError() << std::endl;
                        }
                    }
                    else
                    {
                        std::cerr << "Error during getaddrinfo: " << WSAGetLastError() << std::endl;
                    }
                }
                else
                {
                    std::cerr << "Error during initialization: " << WSAGetLastError() << std::endl;
                }
            }

            ~Socket()
            {
                if (AddressInfoResults != nullptr)
                {
                    freeaddrinfo(AddressInfoResults);
                }

                Disconnect();
                CleanupWinsock();
            }

            bool InitializeWinsock()
            {
                bool result = true;

                WSADATA wsaData;
                int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

                if (iResult != 0)
                {
                    result = false;
                }

                return result;
            }
            void CleanupWinsock()
            {
                WSACleanup();
            }

            // Getters, setters...
            void SetHints(const addrinfo& hints)
            {
                Hints = hints;
            }
            addrinfo* GetAddressInfoResults()
            {
                return AddressInfoResults;
            }
            SOCKET GetSocket() const
            {
                return Sock;
            }

            bool GetAddressInfo(const std::string& nodeName, const std::string& serviceName)
            {
                int iResult = getaddrinfo(nodeName.c_str(), serviceName.c_str(), &Hints, &AddressInfoResults);
                if (iResult < 0)
                {
                    return false;
                }
                return true;
            }
            bool CreateSocket()
            {
                Sock = INVALID_SOCKET;

                for (const addrinfo* addrinfoPtr = AddressInfoResults;
                    addrinfoPtr != nullptr && Sock == INVALID_SOCKET;
                    addrinfoPtr = addrinfoPtr->ai_next)
                {
                    Sock = socket(addrinfoPtr->ai_family, addrinfoPtr->ai_socktype, addrinfoPtr->ai_protocol);
                }

                if (Sock == INVALID_SOCKET)
                {
                    return false;
                }
                return true;
            }
            bool Connect()
            {
                for (const addrinfo* addrinfoPtr = AddressInfoResults; addrinfoPtr != nullptr; addrinfoPtr = addrinfoPtr->ai_next)
                {
                    // Try to connect to a remote server
                    int iResult = connect(Sock, addrinfoPtr->ai_addr, static_cast<int>(addrinfoPtr->ai_addrlen));

                    if (iResult != SOCKET_ERROR)
                    {
                        IsConnected = true;
                        return true;
                    }
                }

                return false;
            }
            void Disconnect()
            {
                closesocket(Sock);
                Sock = INVALID_SOCKET;
                IsConnected = false;
            }
            void SetBlockingMode(const BlockingModeEnum& mode) const
            {
                u_long blockingMode = static_cast<u_long>(mode);
                ioctlsocket(Sock, FIONBIO, &blockingMode);
            }

            int32_t Send(const std::string& buffer) const
            {
                return send(Sock, buffer.c_str(), buffer.size(), 0);
            }
            int32_t Send(const uint8_t* buffer) const
            {
                const char* buf = reinterpret_cast<const char*>(buffer);
                return send(Sock, buf, static_cast<int32_t>(strlen(buf)), 0);
            }
            int32_t Send(const char* buffer) const
            {
                return send(Sock, buffer, static_cast<int32_t>(strlen(buffer)), 0);
            }

            int32_t Receive(std::string& recvbuf) const
            {
                const size_t bufSize = 512;
                int iResult = 0;
                char buf[bufSize + 1];
                int bytesRecv = 0;

                do
                {
                    iResult = recv(Sock, buf, bufSize, 0);

                    if (iResult >= static_cast<int32_t>(bufSize))
                    {
                        buf[bufSize] = '\0';
                        recvbuf += buf;
                        bytesRecv += iResult;
                    }
                    else if (iResult > 0)
                    {
                        buf[iResult] = '\0';
                        recvbuf += buf;
                        bytesRecv += iResult;
                    }
                    else if (iResult < 0)
                    {
                        auto x = WSAGetLastError();
                        if (x == WSAEWOULDBLOCK)
                        {
                            return bytesRecv;
                        }
                        return 0;
                    }
                } while (iResult > 0);

                return bytesRecv;
            }
            int32_t Receive(char* recvbuf, size_t bufSize) const
            {
                int iResult = 0;
                int bytesRecv = 0;

                char* tempBuf = recvbuf;

                do
                {
                    iResult = recv(Sock, tempBuf, bufSize, 0);

                    if (iResult > 0)
                    {
                        bytesRecv += iResult;

                        if (bytesRecv >= bufSize)
                        {
                            bytesRecv = bufSize;
                            return bytesRecv;
                        }

                        tempBuf += iResult;
                    }
                } while (iResult > 0);

                return bytesRecv;
            }
            int32_t Receive(uint8_t* recvbuf, size_t bufSize) const
            {
                int iResult = 0;
                int bytesRecv = 0;

                char* tempBuf = reinterpret_cast<char*>(recvbuf);

                do
                {
                    iResult = recv(Sock, tempBuf, bufSize, 0);

                    if (iResult > 0)
                    {
                        bytesRecv += iResult;

                        if (bytesRecv >= bufSize)
                        {
                            bytesRecv = bufSize;
                            return bytesRecv;
                        }

                        tempBuf += iResult;
                    }
                } while (iResult > 0);

                return bytesRecv;
            }

            int32_t Recv(char* recvbuf, size_t bufSize) const
            {
                return recv(Sock, recvbuf, bufSize, 0);
            }
            int32_t Recv(uint8_t* recvbuf, size_t bufSize) const
            {
                return recv(Sock, reinterpret_cast<char*>(recvbuf), bufSize, 0);
            }
        };

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

        std::string RecvBufString;
        std::mutex RecvMutex;

        // EncodeBase64()
        // 
        // Summary
        // Encodes provided string to base 64
        // 
        // Arguments:
        // std::string "in"  --- In
        // 
        // Returns:
        // 
        // std::string
        std::string EncodeBase64(const std::string& in)
        {
            static const std::string BASE64_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
            std::string out;
            int val = 0;
            int valb = -6;

            for (unsigned char c : in)
            {
                val = (val << 8) + c;
                valb += 8;
                while (valb >= 0)
                {
                    out.push_back(BASE64_CHARS[(val >> valb) & 0x3F]);
                    valb -= 6;
                }
            }

            if (valb > -6)
            {
                out.push_back(BASE64_CHARS[((val << 8) >> (valb + 8)) & 0x3F]);
            }

            while (out.size() % 4)
            {
                out.push_back('=');
            }

            return out;
        }

        // InitializeWinsock()
        // 
        // Summary
        // Initializes Winsock
        // 
        // Arguments:
        // 
        // Returns:
        // bool
        // 
        // Assumptions:
        // Call this function before using any other Winsock function
        bool InitializeWinsock()
        {
            bool result = true;

            WSADATA wsaData;
            int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

            if (iResult != 0)
            {
                result = false;
            }

            return result;
        }

        // CleanupWinsock()
        // 
        // Summary
        // Closes Winsock
        // 
        // Arguments:
        // 
        // Returns:
        // void
        void CleanupWinsock()
        {
            WSACleanup();
        }

        // ResolveServerAddrAndPort()
        // 
        // Summary
        // Maps a host name and a service name to a set of socket addresses
        // 
        // Arguments:
        // const addrinfo& "hints"         --- In
        // addrinfo*& "result"             --- Out
        // const std::string& nodeName     --- In
        // const std::string& serviceName  --- In
        // 
        // Retuns:
        // bool
        // 
        // Assumptions:
        // "hints" should be filled by caller before calling this.
        // If "hints" contains invalid data, result will be undefined
        bool GetAddressInfo(const addrinfo& hints,
            addrinfo*& result,
            const std::string& nodeName,
            const std::string& serviceName)
        {
            int iResult = getaddrinfo(nodeName.c_str(), serviceName.c_str(), &hints, &result);
            if (iResult != 0)
            {
                return false;
            }

            return true;
        }

        // CreateSocket()
        // 
        // Summary
        // Creates a new socket
        // 
        // Arguments:
        // const addrinfo* "result"  --- In
        // 
        // Returns:
        // SOCKET
        // 
        // Assumptions:
        // "result" should be set by calling ResolveServerAddrAndPort() first before calling this function
        // If "result" contains invalid data, result will be undefined
        SOCKET CreateSocket(const addrinfo* result)
        {
            SOCKET sock = INVALID_SOCKET;

            for (const addrinfo* addrinfoPtr = result;
                addrinfoPtr != nullptr && sock == INVALID_SOCKET;
                addrinfoPtr = addrinfoPtr->ai_next)
            {
                sock = socket(addrinfoPtr->ai_family, addrinfoPtr->ai_socktype, addrinfoPtr->ai_protocol);
            }

            return sock;
        }

        // Connect()
        // 
        // Summary
        // Tries to connect to a remote server
        // 
        // Arguments:
        // const SOCKET "sock"            --- In
        // const addrinfo* "result"  --- In
        // 
        // Returns:
        // bool
        // 
        // Assumptions:
        // "result" should be set by calling ResolveServerAddrAndPort() first before calling this function
        // If "result" contains invalid data, result will be undefined
        bool Connect(const SOCKET sock, const addrinfo* result)
        {
            for (const addrinfo* addrinfoPtr = result; addrinfoPtr != nullptr; addrinfoPtr = addrinfoPtr->ai_next)
            {
                // Try to connect to a remote server
                int iResult = connect(sock, addrinfoPtr->ai_addr, static_cast<int>(addrinfoPtr->ai_addrlen));

                if (iResult != SOCKET_ERROR)
                {
                    return true;
                }
            }

            return false;
        }

        // Disconnect()
        // 
        // Summary
        // Closes socket and sets socket to INVALID_SOCKET
        //
        // Arguments:
        // SOCKET& "sock"  --- InOut
        // 
        // Returns:
        // void
        void Disconnect(SOCKET& sock)
        {
            closesocket(sock);
            sock = INVALID_SOCKET;
        }

        // Send()
        // 
        // Summary
        // Sends buffer
        // 
        // Arguments:
        // SOCKET "sock"                --- InOut
        // const std::string& "buffer"  --- In
        // 
        // Returns:
        // int32_t
        // 
        // Assumptions:
        // Result will be set to the successfully sent byte count
        int32_t Send(const SOCKET sock, const std::string& buffer)
        {
            return send(sock, buffer.c_str(), buffer.size(), 0);
        }

        // Send()
        // 
        // Summary
        // Sends buffer
        // 
        // Arguments:
        // SOCKET "sock"              --- InOut
        // const uint8_t* "buffer"    --- In
        // 
        // Returns:
        // int32_t
        // 
        // Assumptions:
        // Result will be set to the successfully sent byte count
        int32_t Send(const SOCKET sock, const uint8_t* buffer)
        {
            const char* buf = reinterpret_cast<const char*>(buffer);
            return send(sock, buf, strlen(buf), 0);
        }

        // Send()
        // 
        // Summary
        // Sends buffer
        // 
        // Arguments:
        // SOCKET "sock"              --- InOut
        // const char* "buffer"       --- In
        // 
        // Returns:
        // int32_t (number of sent bytes)
        // 
        // Assumptions:
        // Result will be set to the successfully sent byte count
        int32_t Send(const SOCKET sock, const char* buffer)
        {
            return send(sock, buffer, strlen(buffer), 0);
        }

        // Receive()
        // 
        // Summary
        // Receives data from socket in a blocking manner until nothing is read from recv() anymore
        // 
        // Arguments:
        // SOCKET "sock"           --- InOut
        // std::string& "recvbuf"  --- Out
        // 
        // Returns:
        // int32_t (number of received bytes)
        int32_t Receive(const SOCKET sock, std::string& recvbuf)
        {
            const size_t bufSize = 512;
            int iResult = 0;
            char buf[bufSize + 1];
            int bytesRecv = 0;

            do
            {
                iResult = recv(sock, buf, bufSize, 0);

                if (iResult >= bufSize)
                {
                    buf[bufSize] = '\0';
                    recvbuf += buf;
                    bytesRecv += iResult;
                }
                else if (iResult > 0)
                {
                    buf[iResult] = '\0';
                    recvbuf += buf;
                    bytesRecv += iResult;
                }
                else if (iResult < 0)
                {
                    return 0;
                }
            } while (iResult > 0);

            return bytesRecv;
        }

        // Receive()
        // 
        // Summary
        // Receives data from socket in a blocking manner until nothing is read from recv() anymore or until buffer size is full
        // 
        // Arguments:
        // SOCKET "sock"           --- InOut
        // char* "recvbuf"         --- Out
        // size_t "bufSize"        --- In
        // 
        // Returns:
        // int32_t (number of received bytes)
        // 
        // Assumptions:
        // * Memory allocation for recvbuf must be handled on caller side
        // * Do not supply a buffer with size 0, result will be undefined
        // * Do not supply nullptr as recvbuf, nullptr check is not performed, therefore result will be undefined
        int32_t Receive(const SOCKET sock, char* recvbuf, size_t bufSize)
        {
            int iResult = 0;
            int bytesRecv = 0;

            char* tempBuf = recvbuf;

            do
            {
                iResult = recv(sock, tempBuf, bufSize, 0);

                if (iResult > 0)
                {
                    bytesRecv += iResult;

                    if (bytesRecv >= bufSize)
                    {
                        bytesRecv = bufSize;
                        return bytesRecv;
                    }

                    tempBuf += iResult;
                }
            } while (iResult > 0);

            return bytesRecv;
        }

        // Receive()
        // 
        // Summary
        // Receives data from socket in a blocking manner until nothing is read from recv() anymore or until buffer size is full
        // 
        // Arguments:
        // SOCKET "sock"           --- InOut
        // uint8_t* "recvbuf"      --- Out
        // size_t "bufSize"        --- In
        // 
        // Returns:
        // int32_t (number of received bytes)
        // 
        // Assumptions:
        // * Memory allocation for recvbuf must be handled on caller side
        // * Do not supply a buffer with size 0, result will be undefined
        // * Do not supply nullptr as recvbuf, nullptr check is not performed, therefore result will be undefined
        int32_t Receive(const SOCKET sock, uint8_t* recvbuf, size_t bufSize)
        {
            int iResult = 0;
            int bytesRecv = 0;

            char* tempBuf = reinterpret_cast<char*>(recvbuf);

            do
            {
                iResult = recv(sock, tempBuf, bufSize, 0);

                if (iResult > 0)
                {
                    bytesRecv += iResult;

                    if (bytesRecv >= bufSize)
                    {
                        bytesRecv = bufSize;
                        return bytesRecv;
                    }

                    tempBuf += iResult;
                }
            } while (iResult > 0);

            return bytesRecv;
        }

        // Recv()
        // 
        // Summary
        // Receives data from socket
        // 
        // Arguments:
        // SOCKET "sock"           --- InOut
        // char* "recvbuf"         --- Out
        // size_t "bufSize"        --- In
        // 
        // Returns:
        // int32_t (number of received bytes)
        // 
        // Assumptions:
        // 1. Only tries to receive data once. If no data is received or not all data is received
        // It is caller's responsibility to call this function again by relocating "recvbuf" to receive all data
        int32_t Recv(const SOCKET sock, char* recvbuf, size_t bufSize)
        {
            return recv(sock, recvbuf, bufSize, 0);
        }

        // Recv()
        // 
        // Summary
        // Receives data from socket
        // 
        // Arguments:
        // SOCKET "sock"           --- InOut
        // uint8_t* "recvbuf"      --- Out
        // size_t "bufSize"        --- In
        // 
        // Returns:
        // int32_t (number of received bytes)
        // 
        // Assumptions:
        // 1. Only tries to receive data once. If no data is received or not all data is received
        // It is caller's responsibility to call this function again by relocating "recvbuf" to receive all data
        int32_t Recv(const SOCKET sock, uint8_t* recvbuf, size_t bufSize)
        {
            return recv(sock, reinterpret_cast<char*>(recvbuf), bufSize, 0);
        }

        // SetBlockingMode()
        // 
        // Summary
        // Changes socket mode to non-blocking
        // 
        // Arguments:
        // SOCKET "sock"                   --- InOut
        // const BlockingModeEnum& "mode"  --- In
        // 
        // Returns:
        // void
        void SetBlockingMode(const SOCKET sock, const BlockingModeEnum& mode)
        {
            u_long blockingMode = static_cast<u_long>(mode);
            ioctlsocket(sock, FIONBIO, &blockingMode);
        }


        void MessageLoop()
        {
            MSG msg;
            while (GetMessage(&msg, nullptr, 0, 0))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        int32_t AsyncSocket_WindowHandle(SOCKET sock, HWND hWnd)
        {
            return WSAAsyncSelect(sock, hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
        }

        LRESULT CALLBACK ReceiverThreadWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            SOCKET sock = wParam;

            if (msg == WM_SOCKET)
            {
                int32_t event = WSAGETSELECTEVENT(lParam);
                int32_t error = WSAGETSELECTERROR(lParam);

                if (error == 0)
                {
                    switch (event)
                    {
                    case FD_READ:
                    {
                        std::string recvbuf;
                        int32_t recvBytes = Receive(sock, recvbuf);

                        std::lock_guard<std::mutex> lock(RecvMutex);
                        RecvBufString = recvbuf;

                        break;
                    }
                    case FD_CLOSE:
                    {
                        Disconnect(sock);
                        break;
                    }
                    }
                }

                return 0;
            }
            else if (msg == WM_SEND_STRING)
            {
                const char* sendbuf = reinterpret_cast<const char*>(lParam);

                int32_t bytesSent = Send(sock, sendbuf);
                #ifdef DEBUG_ENABLED
                if (bytesSent >= 0)
                {
                    std::cout << "Number of bytes sent: " << bytesSent << "\n";
                }
                else
                {
                    std::cerr << "Error while sending: " << WSAGetLastError() << std::endl;
                }

                return 0;
                #endif
            }

            return DefWindowProc(hwnd, msg, wParam, lParam);
        }

        bool RegisterMessageWindowClass(WNDPROC callbackFunction, const std::string& name)
        {
            WNDCLASS wc = { 0 };
            wc.lpfnWndProc = callbackFunction;
            wc.hInstance = GetModuleHandle(NULL);
            wc.lpszClassName = TEXT(name.c_str());

            return RegisterClass(&wc) != 0;
        }

        HWND CreateMessageWindow(const std::string& name)
        {
            HWND hwnd = CreateWindow(name.c_str(),
                "",
                0,
                0, 0, 0, 0,
                HWND_MESSAGE,
                NULL,
                GetModuleHandle(NULL),
                NULL);

            return hwnd;
        }

        void ClientThreadFunction(HWND* outHwnd, const SOCKET sock)
        {
            bool result = RegisterMessageWindowClass(ReceiverThreadWndProc, "ClientThreadWindowClass");
            if (result == false)
            {
                std::cerr << "Register Class call failed, last error: " << GetLastError() << std::endl;
                return;
            }

            HWND hwnd = CreateMessageWindow("ClientThreadWindowClass");
            if (!hwnd)
            {
                std::cerr << "Window Creation failed, last error: " << GetLastError() << std::endl;
                return;
            }

            *outHwnd = hwnd;

            int32_t asyncResult = AsyncSocket_WindowHandle(sock, hwnd);
            if (asyncResult == SOCKET_ERROR)
            {
                std::cerr << "Something is wrong with the socket, last error: " << WSAGetLastError() << std::endl;
                *outHwnd = NULL;
                DestroyWindow(hwnd);
                return;
            }

            MessageLoop();

            *outHwnd = NULL;
            DestroyWindow(hwnd);
        }
    }; // End of namespace "NetworkIO"
    #endif
}; // End of namespace "UtilityLib"

#endif