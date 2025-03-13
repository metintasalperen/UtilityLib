#include "TcpSessionCls.h"

namespace UtilityLib
{
    namespace Socket
    {
        TcpSessionCls::TcpSessionCls(SOCKET clientHandlerSock) :
            TcpCommonCls()
        {
            Sock = clientHandlerSock;
        }
    }; // namespace Socket
}