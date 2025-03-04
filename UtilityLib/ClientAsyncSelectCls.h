#ifndef CLIENTASYNCSELECTCLS_H
#define CLIENTASYNCSELECTCLS_H

#include "SocketPkg.h"
#include <Windows.h>
#include <mutex>

#define WM_SOCKET (WM_APP + 1)
#define WM_SEND_STRING (WM_APP + 2)
#define WM_QUIT_THREAD (WM_APP + 3)

namespace UtilityLib
{
    namespace Network
    {
        class AsyncSelectBaseCls
        {
        protected:
            HWND Hwnd;

        public:
            AsyncSelectBaseCls();

            virtual void MessageLoop() = 0;
            virtual bool RegisterMessageWindowClass(const std::string& name) = 0;
            virtual bool CreateMessageWindow(const std::string& name) = 0;
            virtual LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) = 0;
            virtual void ThreadEntryPoint() = 0;

            static LRESULT CALLBACK ThreadWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        };

        class ClientAsyncSelectCls :
            public SocketClientCls,
            public AsyncSelectBaseCls
        {
        private:
            int LastWindowsError;

        public:
            std::string Buffer;
            std::mutex BufferMutex;

            ClientAsyncSelectCls();
            ClientAsyncSelectCls(const addrinfo& hints,
                const std::string& nodeName,
                const std::string& serviceName);

            int GetLastWindowsError() const;
            void SetLastWindowsError(int errorCode);
            HWND GetHwnd() const;
            std::string GetBuffer();

            void MessageLoop() override;
            bool RegisterMessageWindowClass(const std::string& name) override;
            bool CreateMessageWindow(const std::string& name) override;
            LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) override;
            void ThreadEntryPoint() override;

        private:
            void HandleRecvMessage();
            void HandleSendMessage(const char* buffer);
        };
    };
};

#endif