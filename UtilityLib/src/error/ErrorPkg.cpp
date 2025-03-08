#include "ErrorPkg.h"

namespace UtilityLib
{
    namespace Error
    {
        WSAErrorCls::WSAErrorCls() :
            LastWsaError(0)
        {
        }
        int WSAErrorCls::GetLastWsaError()
        {
            return LastWsaError;
        }
        void WSAErrorCls::SetLastWsaError(int errorCode)
        {
            LastWsaError = errorCode;
        }
        int WSAErrorCls::GetLastWsaErrorAsync()
        {
            std::lock_guard<std::mutex> lock(WsaErrorMutex);
            return LastWsaError;
        }
        void WSAErrorCls::SetLastWsaErrorAsync(int errorCode)
        {
            std::lock_guard<std::mutex> lock(WsaErrorMutex);
            LastWsaError = errorCode;
        }

        WindowsErrorCls::WindowsErrorCls() :
            LastWindowsError(0)
        {
        }
        int WindowsErrorCls::GetLastWindowsError()
        {
            return LastWindowsError;
        }
        void WindowsErrorCls::SetLastWindowsError(int errorCode)
        {
            LastWindowsError = errorCode;
        }
        int WindowsErrorCls::GetLastWindowsErrorAsync()
        {
            std::lock_guard<std::mutex> lock(WindowsErrorMutex);
            return LastWindowsError;
        }
        void WindowsErrorCls::SetLastWindowsErrorAsync(int errorCode)
        {
            std::lock_guard<std::mutex> lock(WindowsErrorMutex);
            LastWindowsError = errorCode;
        }
    }
}