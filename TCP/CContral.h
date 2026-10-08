#pragma once

#include "tcp/TcpServer.h"
#include <map>
#include <queue>


class CContral
{
public:
    CContral();
    virtual ~CContral();

    void SetCallback(ms::AcceptCallbackFunc afunc
        , ms::RecvCallbackFunc rfunc, ms::RecvCompleteCallbackFunc rcfunc
        , ms::SendCompleteCallbackFunc scfunc
        , ms::ShutdownCompleteCallbackFunc sdcfunc
    )
    {
        m_server.SetCallback(afunc, rfunc, rcfunc, scfunc, sdcfunc);
    }
         
    int Init();

    int Start();
    
    int PostSend(SOCKET nSocket, const char * buf, long nSize);
    const CString GetRemote(SOCKET nSocket);

    int InsertDoc(SOCKET nSocket, CDocument* pDoc);
    int RemoveDoc(SOCKET nSocket);
    int InsertView(SOCKET nSocket, CView* pDoc);
    int RemoveView(SOCKET nSocket);

    ms::spBuffer TakeNew(SOCKET nSocket)
    {
        if (m_newsMap[nSocket].empty())
        {
            return nullptr;
        }
        auto sp = m_newsMap[nSocket].front();
        m_newsMap[nSocket].pop();
        return sp;
    }
    void PutBack(ms::spBuffer sp)
    {
        m_pool.Put(sp);
    }
    
private:
    int AcceptCall(SOCKET nSocket, ms::CPeerIoData * ptr);
    int RecvMsg(SOCKET nSocket, ms::CPeerIoData* ptr);
    int RecvMsgComplete(SOCKET nSocket, ms::CPeerIoData* ptr);
    int SendMsgComplete(SOCKET nSocket, ms::CPeerIoData* ptr);
    int CloseComplete(SOCKET nSocket, ms::CPeerIoData* ptr);

private:
    ms::ObjectPool<ms::CBuffer> m_pool;

    std::map<SOCKET, std::queue<ms::spBuffer>> m_newsMap;
    std::map<SOCKET, CDocument*> m_docMap;
    std::map<SOCKET, CView*> m_viewMap;
    HWND m_hWnd {NULL};
    enum {em_normal, em_init, em_start };
    int m_nStatus{ em_normal };
    ms::CTcpServer m_server;
};

