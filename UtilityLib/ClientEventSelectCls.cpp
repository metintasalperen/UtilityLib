#include "ClientEventSelectCls.h"

namespace UtilityLib
{
    namespace Network
    {
        ClientEventSelectCls::ClientEventSelectCls() :
            SocketClientCls(),
            Event(WSACreateEvent()),
            IsRunning(true)
        {
            if (Event == WSA_INVALID_EVENT)
            {
                SetLastWsaError(WSAGetLastError());
            }
        }
        ClientEventSelectCls::ClientEventSelectCls(const addrinfo& hints, const std::string& nodeName, const std::string& serviceName) :
            SocketClientCls(hints, nodeName, serviceName),
            Event(WSACreateEvent()),
            IsRunning(true)
        {
            if (Event == WSA_INVALID_EVENT)
            {
                SetLastWsaError(WSAGetLastError());
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
                SetLastWsaError(WSAGetLastError());
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
                    SetLastWsaError(WSAGetLastError());
                    IsRunning = false;
                    break;
                }

                WSANETWORKEVENTS netEvents{};
                if (WSAEnumNetworkEvents(Sock, Event, &netEvents) == SOCKET_ERROR)
                {
                    SetLastWsaError(WSAGetLastError());
                    return;
                }

                if (netEvents.lNetworkEvents & FD_READ)
                {
                    std::string buffer;
                    auto errResult = Recv(buffer, 4096);

                    if (errResult == ErrorEnum::Success)
                    {
                        WriteToBuffer(buffer);
                    }
                    else
                    {
                        IsRunning = false;
                        return;
                    }
                }
                if (netEvents.lNetworkEvents & FD_CLOSE)
                {
                    SetLastWsaError(0);
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
                    size_t sentBytes;
                    auto result = Send(msg, sentBytes);
                    if (result != ErrorEnum::Success)
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
