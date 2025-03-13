#ifndef TCPSESSIONCLS_H
#define TCPSESSIONCLS_H

#include "TcpCommonCls.h"

namespace UtilityLib
{
    namespace Socket
    {
        class TcpSessionCls : public TcpCommonCls
        {
        public:
            TcpSessionCls(SOCKET clientHandlerSock);

            TcpSessionCls() = delete;
            TcpSessionCls(const TcpSessionCls&) = delete;
            TcpSessionCls& operator=(const TcpSessionCls&) = delete;
            TcpSessionCls(TcpSessionCls&&) = delete;
            TcpSessionCls& operator=(TcpSessionCls&&) = delete;
        };
    }
}

#endif