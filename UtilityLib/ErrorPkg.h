#ifndef ERRORPKG_H
#define ERRORPKG_H

#include <mutex>

namespace UtilityLib
{
    namespace Error
    {
        enum class ErrorEnum
        {
            Success,

            InvalidArgument,
            OutOfRange,
            InsufficientMemory,

            WindowsError,
            
            WinsockError,
            WinsockConnClosed,
            WinsockCallGetAddressInfo,
            WinsockCallCreateSocket
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