#include "ClientAsyncSelectCls.h"

namespace UtilityLib
{
    namespace Network
    {
        AsyncSelectBaseCls::AsyncSelectBaseCls() :
            Hwnd(nullptr)
        {
        }
        LRESULT AsyncSelectBaseCls::ThreadWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            AsyncSelectBaseCls* thisPtr = NULL;
            if (msg == WM_NCCREATE)
            {
                CREATESTRUCT* pcs = reinterpret_cast<CREATESTRUCT*>(lParam);
                thisPtr = reinterpret_cast<AsyncSelectBaseCls*>(pcs->lpCreateParams);
                SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(thisPtr));
                thisPtr->Hwnd = hwnd;
            }
            else
            {
                thisPtr = reinterpret_cast<AsyncSelectBaseCls*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
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
        
        ClientAsyncSelectCls::ClientAsyncSelectCls() :
            SocketCls(),
            AsyncSelectBaseCls(),
            LastWindowsError(0)
        {
        }
        ClientAsyncSelectCls::ClientAsyncSelectCls(const addrinfo& hints) :
            SocketCls(hints),
            AsyncSelectBaseCls(),
            LastWindowsError(0)
        {
        }
        ClientAsyncSelectCls::ClientAsyncSelectCls(const addrinfo& hints, 
                                             const std::string& nodeName, 
                                             const std::string& serviceName) :
            SocketCls(hints, nodeName, serviceName),
            AsyncSelectBaseCls(),
            LastWindowsError(0)
        {
        }

        int ClientAsyncSelectCls::GetLastWindowsError() const
        {
            return LastWindowsError;
        }
        void ClientAsyncSelectCls::SetLastWindowsError(int errorCode)
        {
            LastWindowsError = errorCode;
        }

        HWND ClientAsyncSelectCls::GetHwnd() const
        {
            return Hwnd;
        }

        std::string ClientAsyncSelectCls::GetBuffer()
        {
            std::lock_guard<std::mutex> lock(BufferMutex);
            return Buffer;
        }

        void ClientAsyncSelectCls::MessageLoop()
        {
            MSG msg;
            while (GetMessage(&msg, nullptr, 0, 0))
            {
                if (msg.message == WM_QUIT_THREAD) break;
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        bool ClientAsyncSelectCls::RegisterMessageWindowClass(const std::string& name)
        {
            WNDCLASS wc = { 0 };
            wc.lpfnWndProc = UtilityLib::Network::AsyncSelectBaseCls::ThreadWndProc;
            wc.hInstance = GetModuleHandle(NULL);
            wc.lpszClassName = TEXT(name.c_str());

            if (!RegisterClass(&wc))
            {
                LastWindowsError = GetLastError();
                return false;
            }
            return true;
        }
        bool ClientAsyncSelectCls::CreateMessageWindow(const std::string& name)
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
                LastWindowsError = GetLastError();
                return false;
            }

            return true;
        }
        LRESULT ClientAsyncSelectCls::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
        {
            if (msg == WM_SOCKET)
            {
                int32_t event = WSAGETSELECTEVENT(lParam);
                int32_t error = WSAGETSELECTERROR(lParam);

                if (error != 0)
                {
                    return 0;
                }

                switch (event)
                {
                    case FD_READ:
                    {
                        HandleRecvMessage();
                        break;
                    }
                    case FD_CLOSE:
                    {
                        CloseSocket();
                        break;
                    }
                }

                return 0;
            }
            else if (msg == WM_SEND_STRING)
            {
                const char* buffer = reinterpret_cast<const char*>(lParam);
                HandleSendMessage(buffer);

                return 0;
            }
            else if (msg == WM_QUIT_THREAD)
            {

            }
            else
            {
                return DefWindowProc(Hwnd, msg, wParam, lParam);
            }
        }
        void ClientAsyncSelectCls::ThreadEntryPoint()
        {
            bool result = RegisterMessageWindowClass("ClientThreadWindowClass");
            if (!result)
            {
                return;
            }

            result = CreateMessageWindow("ClientThreadWindowClass");
            if (!result)
            {
                return;
            }

            int32_t iResult = WSAAsyncSelect(Sock, Hwnd, WM_SOCKET, FD_READ | FD_CLOSE);
            if (iResult == SOCKET_ERROR)
            {
                SetLastSocketError(WSAGetLastError());
                DestroyWindow(Hwnd);
                return;
            }

            MessageLoop();
            DestroyWindow(Hwnd);
        }

        void ClientAsyncSelectCls::HandleRecvMessage()
        {
            std::string buffer;
            int32_t recvBytes = RecvAll(buffer);

            if (recvBytes > 0)
            {
                std::lock_guard<std::mutex> lock(BufferMutex);
                Buffer = buffer;
            }
            else
            {
                LastWindowsError = 0;
                // LastWsaError is already set by RecvAll
            }
        }
        void ClientAsyncSelectCls::HandleSendMessage(const char* buffer)
        {
            int32_t bytesSent = SendAll(buffer);
            if (bytesSent < 0)
            {
                LastWindowsError = 0;
                // LastWsaError is already set by Send()
            }
        }
    };
};