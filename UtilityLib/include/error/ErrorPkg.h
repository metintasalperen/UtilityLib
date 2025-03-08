#ifndef GENERICTYPEPKG_H
#define GENERICTYPEPKG_H

#include <mutex>

namespace UtilityLib
{
	namespace Error
	{
		enum class ErrorEnum
		{
			Success,

            // At least one of the parameter provided is invalid
			InvalidArgument,
            // At least one of the parameter provided is out of range
			OutOfRange,
            // Cannot dynamically allocate memory
            InsufficientMemory,

            // Windows API related error, use GetLastWindowsError() or GetLastWindowsErrorAsnyc() to retrieve spesific error code
            WindowsError,

            // Winsock2 API related error, use GetLastWsaError() or GetLastWsaErrorAsync() to retrieve spesific error code
			WinsockError,
            // Remote host closed connection gracefully
			WinsockConnClosed,
            // A successful call to GetAddressInfo() must be performed first
            WinsockCallGetAddressInfo,
            // A successful call to CreateSocket() must be performed first
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