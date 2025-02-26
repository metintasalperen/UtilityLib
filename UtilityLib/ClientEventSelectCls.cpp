#include "ClientEventSelectCls.h"

namespace UtilityLib
{
    namespace Network
    {
        ClientEventSelectCls::ClientEventSelectCls() :
            SocketCls(),
            Event(WSACreateEvent()),
            IsRunning(true)
        {
            if (Event == WSA_INVALID_EVENT)
            {
                LastWsaError = WSAGetLastError();
            }
        }
        ClientEventSelectCls::ClientEventSelectCls(const addrinfo& hints) :
            SocketCls(hints),
            Event(WSACreateEvent()),
            IsRunning(true)
        {
            if (Event == WSA_INVALID_EVENT)
            {
                LastWsaError = WSAGetLastError();
            }
        }
        ClientEventSelectCls::ClientEventSelectCls(const addrinfo& hints, 
                                                   const std::string& nodeName, 
                                                   const std::string& serviceName) :
            SocketCls(hints, nodeName, serviceName),
            Event(WSACreateEvent()),
            IsRunning(true)
        {
            if (Event == WSA_INVALID_EVENT)
            {
                LastWsaError = WSAGetLastError();
            }
        }

        std::string ClientEventSelectCls::GetBuffer()
        {
            std::lock_guard<std::mutex> lock(BufferMutex);
            return Buffer;
        }

        void ClientEventSelectCls::PushMessage(const std::string& msg)
        {
            std::lock_guard<std::mutex> lock(MsgMutex);
            MsgQueue.push(msg);
        }
        void ClientEventSelectCls::StopThread()
        {
            IsRunning = false;
        }
        void ClientEventSelectCls::ThreadEntryPoint()
        {
            if (WSAEventSelect(Sock, Event, FD_READ | FD_CLOSE) == SOCKET_ERROR)
            {
                LastWsaError = WSAGetLastError();
                return;
            }

            while (IsRunning == true)
            {
                DWORD result = WSAWaitForMultipleEvents(1, &Event, FALSE, 100, FALSE);
                if (result == WSA_WAIT_TIMEOUT)
                {
                    CheckMessageQueue();
                }
                else if (result == WSA_WAIT_FAILED)
                {
                    LastWsaError = WSAGetLastError();
                    IsRunning = false;
                    break;
                }

                WSANETWORKEVENTS netEvents{};
                if (WSAEnumNetworkEvents(Sock, Event, &netEvents) == SOCKET_ERROR)
                {
                    LastWsaError = WSAGetLastError();
                    return;
                }

                if (netEvents.lNetworkEvents & FD_READ)
                {
                    std::string buffer;
                    int recvBytes = RecvAll(buffer);

                    if (recvBytes > 0)
                    {
                        WriteToBuffer(buffer);
                    }
                    else if (WSAGetLastError() != WSAEWOULDBLOCK)
                    {
                        LastWsaError = WSAGetLastError();
                        IsRunning = false;
                        break;
                    }
                }
                if (netEvents.lNetworkEvents & FD_CLOSE)
                {
                    LastWsaError = 0;
                    IsRunning = false;
                    break;
                }

                CheckMessageQueue();
            }
        }

        void ClientEventSelectCls::CheckMessageQueue()
        {
            std::lock_guard<std::mutex> lock(MsgMutex);
            while (MsgQueue.empty() == false)
            {
                std::string msg = MsgQueue.front();
                MsgQueue.pop();

                if (Sock != INVALID_SOCKET)
                {
                    int bytesSent = SendAll(msg);
                    if (bytesSent < 0)
                    {
                        IsRunning = false;
                        return;
                    }
                }
            }
        }
        void ClientEventSelectCls::WriteToBuffer(const std::string& buffer)
        {
            std::lock_guard<std::mutex> lock(BufferMutex);
            Buffer = buffer;
        }

    };
};
