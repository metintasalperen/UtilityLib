#ifndef ERRORPKG_H
#define ERRORPKG_H

#include <mutex>

namespace UtilityLib
{
    namespace Error
    {
        enum class ErrorEnum
        {
            Success = 0,
            InvalidArgument = -1,
            OutOfRange = -2,
            WinsockError = -3,
            WindowsError = -4,
            InsufficientMemory = -5
        };

        class WSAErrorCls
        {
        private:
            int LastWsaError;
            std::mutex WsaErrorMutex;

        protected:
            void SetLastWsaError(int errorCode);
            void SetLastWsaErrorAsync(int errorCode);

        public:
            WSAErrorCls();
            int GetLastWsaError();
            int GetLastWsaErrorAsync();
        };
        class WindowsErrorCls
        {
        private:
            int LastWindowsError;
            std::mutex WindowsErrorMutex;

        protected:
            void SetLastWindowsError(int errorCode);
            void SetLastWindowsErrorAsync(int errorCode);

        public:
            WindowsErrorCls();
            int GetLastWindowsError();
            int GetLastWindowsErrorAsync();
        };
    };
};

#endif