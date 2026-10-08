#pragma once
#include <Mswsock.h>
#include <WinSock2.h>

namespace ms {

    class CAcceptExHelper {
    public:
        BOOL Init(SOCKET sockListen)
        {
            if (m_bInit)
            {
                return m_listen == sockListen;
            }
            m_listen = sockListen;

            GUID GuidAcceptEx = WSAID_ACCEPTEX;
            DWORD dwBytes = 0;
            int nResult = WSAIoctl(sockListen,
                SIO_GET_EXTENSION_FUNCTION_POINTER,
                &GuidAcceptEx, sizeof(GuidAcceptEx),
                &m_lpfnAcceptEx, sizeof(m_lpfnAcceptEx),
                &dwBytes, NULL, NULL
            );
            if (SOCKET_ERROR == nResult)
            {
                return FALSE;
            }
            dwBytes = 0;
            GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
            nResult = WSAIoctl(sockListen,
                SIO_GET_EXTENSION_FUNCTION_POINTER,
                &GuidGetAcceptExSockAddrs,
                sizeof(GuidGetAcceptExSockAddrs),
                &m_lpfnGetAcceptExSockAddrs,
                sizeof(m_lpfnGetAcceptExSockAddrs),
                &dwBytes,
                NULL,
                NULL);
            if (SOCKET_ERROR == nResult)
            {
                return FALSE;
            }
            m_bInit = TRUE;
            return m_bInit;
        }
        // site: docs.microsoft.com AcceptEx
        BOOL AcceptEx(_In_ SOCKET sAcceptSocket,
            _Out_writes_bytes_(dwReceiveDataLength + dwLocalAddressLength + dwRemoteAddressLength) PVOID lpOutputBuffer,
            _In_ DWORD dwReceiveDataLength,
            _In_ DWORD dwLocalAddressLength,
            _In_ DWORD dwRemoteAddressLength,
            _Out_ LPDWORD lpdwBytesReceived,
            _Inout_ LPOVERLAPPED lpOverlapped
        )
        {
            BOOL bResult = m_lpfnAcceptEx(m_listen, sAcceptSocket, lpOutputBuffer, dwReceiveDataLength,
                dwLocalAddressLength, dwRemoteAddressLength
                , lpdwBytesReceived,
                lpOverlapped);
            return bResult;
        }
        // site: docs.microsoft.com GetAcceptExSockAddrs
        void GetAcceptExSockAddrs(PVOID    lpOutputBuffer,
            DWORD    dwReceiveDataLength,
            DWORD    dwLocalAddressLength,
            DWORD    dwRemoteAddressLength,
            sockaddr** LocalSockaddr,
            LPINT    LocalSockaddrLength,
            sockaddr** RemoteSockaddr,
            LPINT    RemoteSockaddrLength)
        {
            m_lpfnGetAcceptExSockAddrs(lpOutputBuffer, dwReceiveDataLength,
                dwLocalAddressLength, dwRemoteAddressLength
                , LocalSockaddr, LocalSockaddrLength, RemoteSockaddr, RemoteSockaddrLength);
            return;
        }
    private:
        BOOL m_bInit{ FALSE };
        SOCKET m_listen{ INVALID_SOCKET };
        LPFN_ACCEPTEX m_lpfnAcceptEx{ NULL };
        LPFN_GETACCEPTEXSOCKADDRS m_lpfnGetAcceptExSockAddrs{ NULL };
    };

};// ms