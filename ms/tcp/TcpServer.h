#pragma once
#include <string>
#include <list>
#include <map>
#include <thread>
#include <memory>
#include <queue>
#include <functional>
#include <WS2tcpip.h>
#include <Mswsock.h>
#include <WinSock2.h>

#include "core/ObjectPool.h"
#include "core/LockMap.h"
#include "core/CBuffer.h"
#include "string/string.h"
#include "code.h"
#include "CompletionPort.h"
#include "AcceptExHelper.h"


#define BUFFER_SIZE 8192

namespace ms{

enum EmOperationType
{
    POST_UNKNOW = 0,
    POST_RECV,
    POST_SEND,
    POST_ACCEPT,
    POST_CLOSE,
};

class CPeerIoData
{
public:
    WSAOVERLAPPED m_overlapped;
    EmOperationType m_operationType{ EmOperationType::POST_UNKNOW };
    WSABUF m_wsabuf;
    ms::CBuffer m_buffer;
    DWORD m_dwTransfered{ 0 };
    DWORD m_dwNeedTrans{ 0 };
    DWORD m_dwLength{ 0 };
    DWORD m_flags{ 0 };
    SOCKET m_client{ INVALID_SOCKET };
};


typedef std::shared_ptr<CPeerIoData> PeerPtr;

class CClient {
public:
    enum {IP_BUF=50};
    TCHAR ip[IP_BUF];
    unsigned short nPort;
    SOCKET m_socket;
    SOCKADDR_IN client_addr;
    std::list<PeerPtr> m_listRecv;
    std::list<PeerPtr> m_listSend;
};

typedef std::shared_ptr<CClient> ClientPtr;

typedef  std::function<int(SOCKET, CPeerIoData*)> AcceptCallbackFunc;
typedef  std::function<int(SOCKET, CPeerIoData*)> RecvCallbackFunc;
typedef  std::function<int(SOCKET, CPeerIoData*)> RecvCompleteCallbackFunc;
typedef  std::function<int(SOCKET, CPeerIoData*)> SendCompleteCallbackFunc;
typedef  std::function<int(SOCKET, CPeerIoData*)> ShutdownCompleteCallbackFunc;

class CTcpHelper;

class CTcpServer
{
private:
public:
    CTcpServer();
    virtual ~CTcpServer();
    int Init(const std::string& szIp, unsigned short m_port);

    int Start();
    void Stop();

    void SetRecvCallback(RecvCallbackFunc func)
    {
        m_funRecv = func;
    }
    void SetCallback(AcceptCallbackFunc afunc
        , RecvCallbackFunc rfunc, RecvCompleteCallbackFunc rcfunc
        , SendCompleteCallbackFunc scfunc
        , ShutdownCompleteCallbackFunc sdcfunc
    )
    {
        if (afunc)
            m_funAccept = afunc;
        if (rfunc)
            m_funRecv = rfunc;
        if (rcfunc)
            m_funRecvComplete = rcfunc;
        if (scfunc)
            m_funSendComplete = scfunc;
        if (sdcfunc)
            m_funShutdownComplete = sdcfunc;
    }
    // send
    int PostSend(SOCKET socket, const char* buf, long nSize);
    
    int RegRecv(SOCKET nSocket);
    int RegAcceptEx();
    int RegSend(SOCKET nSocket, CPeerIoData* peer);
    // 
    ms::mstring GetRemote(SOCKET nSocket);

private:
    int Working();
    void CloseComplete();

private:
    AcceptCallbackFunc m_funAccept;
    RecvCallbackFunc m_funRecv;
    RecvCompleteCallbackFunc m_funRecvComplete;
    SendCompleteCallbackFunc m_funSendComplete;
    ShutdownCompleteCallbackFunc m_funShutdownComplete;

    int DoRecv(ClientPtr clientPtr, CPeerIoData* peer, DWORD dwTransfered);
    int DoSend(ClientPtr clientPtr, CPeerIoData* peer, DWORD dwTransfered);
    int DoAcceptEx(ClientPtr clientPtr, CPeerIoData* peer);
    int DoShutdown(ClientPtr clientPtr, CPeerIoData* peer);

private:
    // AcceptEx Helper
    CAcceptExHelper m_acceptExHelper;
    //LPFN_ACCEPTEX m_lpfnAcceptEx{ NULL };
    //LPFN_GETACCEPTEXSOCKADDRS m_lpfnGetAcceptExSockAddrs{ NULL };

    CCompletionPort* m_pCompletionPort{ NULL };
    //HANDLE m_hIOCompletionPort;
    SOCKET m_sockListen;
    //struct sockaddr_in m_serverAddress;
    bool m_run{ false };
    std::string m_szAddress;
    unsigned short m_nPort;
    // lock
    CLockMap<SOCKET> m_lockMap;
    // accept list
    std::list<PeerPtr> m_listAccept;
    // thread
    int m_nProcessors{ 0 };
    std::list< std::shared_ptr<std::thread>> m_pWorkingThread;
    // client
    std::map<SOCKET, std::shared_ptr<CClient>> m_clientMap;
    // pool
    ObjectPool<CPeerIoData> m_pool;


protected:
    int AcceptCallback(SOCKET s, CPeerIoData*)
    {
        RegRecv(s);
        return 0;
    }
    int RecvCallback(SOCKET s, CPeerIoData* io)
    {
        return 0;
    }
    int RecvCompleteCallback(SOCKET s, CPeerIoData*)
    {
        RegRecv(s);
        return 0;
    }
    int SendCompleteCallback(SOCKET s, CPeerIoData*)
    {
        // 
        return 0;
    }
    int ShutdownCompleteCallback(SOCKET s, CPeerIoData*)
    {
        // 
        return 0;
    }
};

};// ms
