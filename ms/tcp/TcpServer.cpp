#include "stdafx.h"
#include "tcp/TcpServer.h"

#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "Ws2_32.lib")


/*
start 

accept acceptcall 


*/
namespace ms {


    CTcpServer::CTcpServer()
    {
        m_funAccept = std::bind(&CTcpServer::AcceptCallback, this, std::placeholders::_1, std::placeholders::_2);
        m_funRecv = std::bind(&CTcpServer::RecvCallback, this, std::placeholders::_1, std::placeholders::_2);
        m_funRecvComplete = std::bind(&CTcpServer::RecvCompleteCallback, this, std::placeholders::_1, std::placeholders::_2);
        m_funSendComplete = std::bind(&CTcpServer::SendCompleteCallback, this, std::placeholders::_1, std::placeholders::_2);
        m_funShutdownComplete = std::bind(&CTcpServer::ShutdownCompleteCallback, this, std::placeholders::_1, std::placeholders::_2);
    }


    CTcpServer::~CTcpServer()
    {
        Stop();
    }

    int CTcpServer::Init(const std::string& szIp, unsigned short m_port)
    {
        int nResult = GENERAL_ERROR;
        do
        {

            nResult = RESULT_OK;
        } while (0);

        return nResult;
    }

    int CTcpServer::Start()
    {
        if (m_run)
            return RESULT_OK;

        int nResult = GENERAL_ERROR;

        m_run = true;
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        m_nProcessors = si.dwNumberOfProcessors;
        // 要创建不关联的I / O完成端口，请将FileHandle参数设置为INVALID_HANDLE_VALUE，
        // 将ExistingCompletionPort参数设置为NULL，
        // 并将CompletionKey参数设置为零（在这种情况下将被忽略）。
        // 对于新的I/O完成端口，将NumberOfConcurrentThreads参数设置为所需的并发值，
        // 对于默认值（系统中的处理器数量），请将其设置为零。
        //m_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE
        //    , NULL, 0, m_nProcessors);
        
        m_pCompletionPort = CCompletionPort::Create(m_nProcessors);

        if (NULL == m_pCompletionPort)
        {
            nResult = GENERAL_ERROR;
            return nResult;
        }

        auto working = std::bind(&CTcpServer::Working, this);

        for (int i = 0; i < m_nProcessors; ++i)
        {
            auto w = std::make_shared<std::thread>(working);
            m_pWorkingThread.push_back(w);
        }

        m_sockListen = WSASocket(AF_INET, SOCK_STREAM, 0
            , NULL, 0, WSA_FLAG_OVERLAPPED);
        if (m_sockListen == INVALID_SOCKET)
        {
            return nResult;
        }

        SOCKADDR_IN InternetAddr;
        InternetAddr.sin_family = AF_INET;
        InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        InternetAddr.sin_port = htons(6688);

        if (SOCKET_ERROR == bind(m_sockListen, (struct sockaddr*) & InternetAddr, sizeof(InternetAddr)))
        {
            return nResult;
        }
        int nl = listen(m_sockListen, SOMAXCONN);

        // 关联完成端口
        //auto h2 = CreateIoCompletionPort((HANDLE)m_sockListen
        //    , m_hIOCompletionPort, (ULONG_PTR)m_sockListen, 0);

        BOOL bResult = m_pCompletionPort->Bind((HANDLE)m_sockListen, (ULONG_PTR)m_sockListen);
        if (!bResult)
        {
            nResult = GENERAL_ERROR;
            return nResult;
        }
        //if (!m_lpfnAcceptEx)
        //{
        //    GUID GuidAcceptEx = WSAID_ACCEPTEX;
        //    DWORD dwBytes = 0;
        //    int nResult = WSAIoctl(m_sockListen,
        //        SIO_GET_EXTENSION_FUNCTION_POINTER,
        //        &GuidAcceptEx, sizeof(GuidAcceptEx),
        //        &m_lpfnAcceptEx, sizeof(m_lpfnAcceptEx),
        //        &dwBytes, NULL, NULL
        //    );
        //    if (!m_lpfnAcceptEx)
        //    {
        //        return nResult;
        //    }
        //}
        //if (!m_lpfnGetAcceptExSockAddrs)
        //{
        //    DWORD dwBytes = 0;
        //    GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
        //    int nResult = WSAIoctl(m_sockListen,
        //        SIO_GET_EXTENSION_FUNCTION_POINTER,
        //        &GuidGetAcceptExSockAddrs,
        //        sizeof(GuidGetAcceptExSockAddrs),
        //        &m_lpfnGetAcceptExSockAddrs,
        //        sizeof(m_lpfnGetAcceptExSockAddrs),
        //        &dwBytes,
        //        NULL,
        //        NULL);
        //    if (!m_lpfnGetAcceptExSockAddrs)
        //    {
        //        return false;
        //    }
        //}
        if (!m_acceptExHelper.Init(m_sockListen))
        {
            return nResult;
        }

        nResult = RegAcceptEx();
        if (RESULT_OK != nResult)
        {
            return nResult;
        }
        return nResult;
    }

    void CTcpServer::Stop()
    {
        if (!m_run)
        {
            return;
        }
        CloseComplete();
        for (auto& it : m_listAccept)
        {
            closesocket(it->m_client);
            it->m_client = INVALID_SOCKET;
        }
        for (auto& it : m_clientMap)
        {
            closesocket(it.second->m_socket);
            it.second->m_socket = INVALID_SOCKET;
            it.second->m_listRecv.clear();
            it.second->m_listSend.clear();
        }
        m_run = false;
        for (auto& it : m_pWorkingThread)
        {
            if (it->joinable())
            {
                it->join();
            }
        }
        if (m_sockListen != INVALID_SOCKET)
        {
            closesocket(m_sockListen);
        }

        m_clientMap.clear();
        m_listAccept.clear();
    }

    int CTcpServer::Working()
    {
        DWORD dwTrans{ 0 };
        DWORD dwCompleteKey{ 0 };
        CPeerIoData* ioDataPtr{ NULL };
        OVERLAPPED* pOverlap{ NULL };
        BOOL bResult{ FALSE };

        while (m_run)
        {
            // bResult = GetQueuedCompletionStatus(m_hIOCompletionPort, &dwTrans, (PULONG_PTR)&dwCompleteKey, (LPOVERLAPPED*)&pOverlap, INFINITE);
            bResult = m_pCompletionPort->GetQueuedStatus(dwTrans, dwCompleteKey, (LPOVERLAPPED*)&pOverlap);
            if (!bResult)
            {
                DWORD lastError = GetLastError();
                if (NULL == pOverlap)
                {
                    // 如果由于未完成调用而关闭与其关联的完成端口句柄而导致
                    // 对GetQueuedCompletionStatus的调用失败，
                    // 则该函数返回FALSE，* lpOverlapped将为NULL，
                    // 并且GetLastError将返回ERROR_ABANDONED_WAIT_0。
                    if (lastError == ERROR_ABANDONED_WAIT_0)
                    {
                        // 关闭了关联的完成端口
                        TRACE("Completion port closed, Lasterror is %d!\n", lastError);
                        break;
                    }
                    else
                    {
                        // 根据官方说明，不应该走到这里。
                        ASSERT(FALSE);
                        continue;
                    }
                }
                // 往一个已经断开的客户端发数据就会到这里。
                TRACE("client is close! Lasterror is %d!\n", lastError);
                ioDataPtr = CONTAINING_RECORD(pOverlap, CPeerIoData, m_overlapped);
                ClientPtr ctx = nullptr;
                if (m_clientMap.end() != m_clientMap.find(ioDataPtr->m_client))
                {
                    ctx = m_clientMap[ioDataPtr->m_client];
                    DoShutdown(ctx, ioDataPtr);
                }
                continue;
            }
            if (NULL == pOverlap)
            {
                if (m_pCompletionPort == (HANDLE)dwCompleteKey)
                {
                    TRACE("post complete process end!\n");
                    break;
                }
                else if (m_sockListen == (SOCKET)dwCompleteKey)
                {
                    TRACE("LISTEN ERROR!\n");
                    ASSERT(FALSE);
                    break;
                }
                TRACE("GetQueuedCompletionStatus error key is %d!\n", dwCompleteKey);
                break;
            }
            ioDataPtr = CONTAINING_RECORD(pOverlap, CPeerIoData, m_overlapped);
            SOCKET client{ ioDataPtr->m_client };
            // lock
            m_lockMap[client].Lock();
            ClientPtr ctx = nullptr;
            if (m_clientMap.end() != m_clientMap.find(client))
            {
                ctx = m_clientMap[client];
                if ((0 == dwTrans) && (POST_RECV == ioDataPtr->m_operationType
                    || POST_SEND == ioDataPtr->m_operationType))
                {
                    DoShutdown(ctx, ioDataPtr);
                }
                else if (POST_RECV == ioDataPtr->m_operationType)
                {
                    DoRecv(ctx, ioDataPtr, dwTrans);
                }
                else if (POST_SEND == ioDataPtr->m_operationType)
                {
                    TRACE("send %d\n", ioDataPtr->m_buffer.GetSize());
                    DoSend(ctx, ioDataPtr, dwTrans);
                }
                else if (POST_ACCEPT == ioDataPtr->m_operationType)
                {
                    DoAcceptEx(ctx, ioDataPtr);
                }
                else
                {
                    ASSERT(FALSE);
                }
            }
            else
            {
                // 走到这里就是有BUG
                TRACE("socket:%d is shutdown\n", client);
                ASSERT(FALSE);
            }
            // unlock
            m_lockMap[client].Unlock();
        }

        return 0;
    }

    int CTcpServer::PostSend(SOCKET nSocket, const char* buf, long nSize)
    {
        int nResult = SOCKET_ERROR;
        do
        {
            auto it = m_clientMap.find(nSocket);
            if (it == m_clientMap.end())
            {
                break;
            }
            auto cliPtr = m_clientMap[nSocket];
            auto peerPtr = m_pool.Take();

            peerPtr->m_dwTransfered = peerPtr->m_dwLength = 0;
            peerPtr->m_operationType = EmOperationType::POST_SEND;
            peerPtr->m_flags = 0;
            peerPtr->m_dwLength = nSize;
            peerPtr->m_dwTransfered = 0;
            peerPtr->m_dwNeedTrans = nSize;
            peerPtr->m_buffer.Resize(nSize + 1);
            peerPtr->m_buffer.Write(buf, nSize);
            peerPtr->m_client = nSocket;
            peerPtr->m_wsabuf.buf = peerPtr->m_buffer.Data();
            peerPtr->m_wsabuf.len = nSize;

            cliPtr->m_listSend.push_back(peerPtr);

            nResult = WSASend(cliPtr->m_socket, &peerPtr->m_wsabuf, 1, &peerPtr->m_dwTransfered
                , peerPtr->m_flags, &peerPtr->m_overlapped, NULL);

            if (0 != nResult)
            {
                nResult = WSAGetLastError();
                if (WSA_IO_PENDING == nResult)
                {
                    nResult = 0;
                }
            }
        } while (0);


        return nResult;
    }

    int CTcpServer::RegRecv(SOCKET nSocket)
    {
        int nResult = SOCKET_ERROR;
        do
        {
            auto it = m_clientMap.find(nSocket);
            if (it == m_clientMap.end())
            {
                break;
            }
            auto cliPtr = m_clientMap[nSocket];
            auto peerPtr = m_pool.Take();

            peerPtr->m_dwTransfered = peerPtr->m_dwLength = 0;
            peerPtr->m_operationType = POST_RECV;
            peerPtr->m_flags = 0;
            peerPtr->m_buffer.Resize(BUFFER_SIZE);
            peerPtr->m_wsabuf.buf = peerPtr->m_buffer.Data();
            peerPtr->m_wsabuf.len = BUFFER_SIZE;
            peerPtr->m_client = nSocket;
            cliPtr->m_listRecv.push_back(peerPtr);
            nResult = WSARecv(cliPtr->m_socket, &peerPtr->m_wsabuf, 1, &peerPtr->m_dwTransfered
                , &peerPtr->m_flags, &peerPtr->m_overlapped, NULL);
            if (0 != nResult)
            {
                nResult = WSAGetLastError();
                if (WSA_IO_PENDING == nResult)
                {
                    nResult = 0;
                }
            }
        } while (0);
        return nResult;
    }

    int CTcpServer::RegAcceptEx()
    {
        int nResult = -1;
        do
        {
            SOCKET s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
            if (INVALID_SOCKET == s)
            {
                break;
            }
            auto ctx = std::make_shared<CClient>();
            ctx->m_socket = s;
            m_clientMap[s] = ctx;

            auto m_acceptPtr = m_pool.Take();
            m_listAccept.push_back(m_acceptPtr);

            m_acceptPtr->m_buffer.Resize(4096);
            m_acceptPtr->m_wsabuf.buf = m_acceptPtr->m_buffer.Data();
            m_acceptPtr->m_wsabuf.len = 4096;
            m_acceptPtr->m_operationType = POST_ACCEPT;
            m_acceptPtr->m_client = s;
            
            //BOOL bResult = m_lpfnAcceptEx(m_sockListen, s, m_acceptPtr->m_buffer.Data(), 0,
            //    sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16
            //    , &m_acceptPtr->m_dwTransfered,
            //    &m_acceptPtr->m_overlapped);

            BOOL bResult = m_acceptExHelper.AcceptEx(s, m_acceptPtr->m_buffer.Data(), 0,
                sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16
                , &m_acceptPtr->m_dwTransfered,
                &m_acceptPtr->m_overlapped);

            if (!bResult)
            {
                if (WSAGetLastError() == ERROR_IO_PENDING)
                    nResult = 0;
            }
            else
            {
                nResult = 0;
            }

        } while (0);
        return nResult;
    }

    int CTcpServer::RegSend(SOCKET nSocket, CPeerIoData* peer)
    {
        int nResult = SOCKET_ERROR;
        peer->m_wsabuf.buf = peer->m_buffer.Data() + peer->m_dwLength - peer->m_dwNeedTrans;
        peer->m_wsabuf.len = peer->m_dwNeedTrans;

        nResult = WSASend(nSocket, &peer->m_wsabuf, 1, &peer->m_dwTransfered
            , peer->m_flags, &peer->m_overlapped, NULL);

        return nResult;
    }

    ms::mstring CTcpServer::GetRemote(SOCKET nSocket)
    {
        auto it = m_clientMap.find(nSocket);
        if (it == m_clientMap.end())
        {
            return _T("");
        }
        TCHAR num[10];
        ms::mstring remote(it->second->ip);
        _itot_s(it->second->nPort, num, 10);
        remote = remote + _T(":") + num;
        return remote;
    }

    inline void CTcpServer::CloseComplete()
    {
        for (int i = 0; i < m_nProcessors; ++i)
        {
            m_pCompletionPort->PostQueuedStatus(0, (ULONG_PTR)m_pCompletionPort, NULL);
        }
    }

    int CTcpServer::DoRecv(ClientPtr ctx, CPeerIoData* peer, DWORD dwTransfered)
    {
        peer->m_buffer.SetSize(dwTransfered);

        if (m_funRecv)
        {
            m_funRecv(ctx->m_socket, peer);
        }
        if (m_funRecvComplete)
        {
            m_funRecvComplete(ctx->m_socket, peer);
        }
        // 
        for (auto it = ctx->m_listRecv.begin();
            it != ctx->m_listRecv.end(); ++it)
        {
            if (&(*(*it)) == peer)
            {
                auto p = *it;
                ctx->m_listRecv.erase(it);
                m_pool.Put(p);
                break;
            }
        }
        return 0;
    }

    int CTcpServer::DoSend(ClientPtr ctx, CPeerIoData* peer, DWORD dwTransfered)
    {
        int nResult = -1;
        if (peer->m_dwNeedTrans - peer->m_dwTransfered > 0)
        {
            peer->m_dwNeedTrans -= peer->m_dwTransfered;
            nResult = RegSend(ctx->m_socket, peer);
        }
        else
        {
            peer->m_dwNeedTrans -= peer->m_dwTransfered;
            if (m_funSendComplete)
            {
                m_funSendComplete(ctx->m_socket, peer);
            }

            for (auto it = ctx->m_listSend.begin();
                it != ctx->m_listSend.end(); ++it)
            {
                if (&(*(*it)) == peer)
                {
                    auto p = *it;
                    ctx->m_listSend.erase(it);
                    m_pool.Put(p);
                    break;
                }
            }
        }
        nResult = 0;
        return nResult;
    }

    int CTcpServer::DoAcceptEx(ClientPtr ctx, CPeerIoData* peer)
    {
        int nResult = -1;
        do
        {
            int iResult = setsockopt(ctx->m_socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
                (char*)&m_sockListen, sizeof(m_sockListen));

            SOCKADDR_IN* pLocal{ 0 };
            SOCKADDR_IN* pRemote{ 0 };
            int nLocal{ 0 }, nRemote{ 0 };
            //m_lpfnGetAcceptExSockAddrs(peer->m_buffer.Data(), 0,
            //    sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16
            //    , (sockaddr**)&pLocal, &nLocal, (sockaddr**)&pRemote, &nRemote);

            m_acceptExHelper.GetAcceptExSockAddrs(peer->m_buffer.Data(), 0,
                sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16
                , (sockaddr**)&pLocal, &nLocal, (sockaddr**)&pRemote, &nRemote);
            ctx->client_addr = *pRemote;
            InetNtop(ctx->client_addr.sin_family, &ctx->client_addr.sin_addr, ctx->ip, CClient::IP_BUF);
            ctx->nPort = ntohs(ctx->client_addr.sin_port);
            
            TRACE(_T("accept %s:%d"), ctx->ip, ctx->nPort);
            // reg complete
            BOOL bResule = m_pCompletionPort->Bind((HANDLE)ctx->m_socket, (DWORD)ctx->m_socket);
            if (!bResule)
            {
                nResult = GENERAL_ERROR;
                break;
            }
            if (m_funAccept)
            {
                m_funAccept(ctx->m_socket, peer);
            }
            nResult = RegAcceptEx();
        } while (0);

        // back peer
        for (auto it = m_listAccept.begin(); it != m_listAccept.end(); ++it)
        {
            if (peer == &(**it))
            {
                m_pool.Put(*it);
                m_listAccept.erase(it);
                break;
            }
        }

        return nResult;
    }

    int CTcpServer::DoShutdown(ClientPtr ctx, CPeerIoData* peer)
    {
        TCHAR ip[1024];
        InetNtop(ctx->client_addr.sin_family, &ctx->client_addr.sin_addr, ip, 1024);
        TRACE(_T("客户端 %s:%d 断开连接."), ip
            , ntohs(ctx->client_addr.sin_port));
        for (auto& it : ctx->m_listRecv)
        {
            m_pool.Put(it);
        }
        ctx->m_listRecv.clear();
        for (auto& it : ctx->m_listSend)
        {
            m_pool.Put(it);
        }
        ctx->m_listSend.clear();
        m_clientMap.erase(ctx->m_socket);
        if (m_funShutdownComplete)
        {
            m_funShutdownComplete(ctx->m_socket, peer);
        }
        closesocket(ctx->m_socket);
        return 0;
    }


}; // ms