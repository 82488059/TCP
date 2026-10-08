#include "stdafx.h"
#include "CContral.h"
#include "TCP.h"
#include <WS2tcpip.h>


CContral::CContral()
{
}


CContral::~CContral()
{
}

int CContral::AcceptCall(SOCKET nSocket, ms::CPeerIoData * ptr)
{
    CWnd* pWnd = theApp.GetMainWnd();
    HWND handle{ NULL };
    if (pWnd)
    {
        handle = pWnd->GetSafeHwnd();
    }
    SendMessage(handle, MSG_SOCKET_CONNECT, 0, nSocket);

    m_server.RegRecv(nSocket);
    m_server.RegAcceptEx();

    return 0;
}

int CContral::RecvMsg(SOCKET nSocket, ms::CPeerIoData * ptr)
{   
    auto sp = m_pool.Take();
    sp->Resize(ptr->m_buffer.GetSize());
    sp->SetData(ptr->m_buffer.Data(), ptr->m_buffer.GetSize());
    // add news queue
    m_newsMap[nSocket].push(sp);
    // msg
    auto m = m_docMap[nSocket];
    // m->GetNextView();
    SendMessage(m_hWnd, MSG_SOCKET_RECV, nSocket, (LPARAM)ptr);
    // continue recv
    m_server.RegRecv(nSocket);
    return 0;
}

int CContral::RecvMsgComplete(SOCKET nSocket, ms::CPeerIoData * ptr)
{
    m_server.PostSend(nSocket, "1234", 4);
    return 0;
}

int CContral::SendMsgComplete(SOCKET nSocket, ms::CPeerIoData * ptr)
{
    return 0;
}

int CContral::CloseComplete(SOCKET nSocket, ms::CPeerIoData * ptr)
{
    CWnd* pWnd = theApp.GetMainWnd();
    HWND handle{ NULL };
    if (pWnd)
    {
        handle = pWnd->GetSafeHwnd();
    }
    SendMessage(handle, MSG_SOCKET_CLOSE, 0, nSocket);
    return 0;
}

int CContral::Init()
{
    if (m_nStatus != em_normal)
    {
        return RESULT_OK;
    }
    int nResult = GENERAL_ERROR;
    do 
    {
        nResult = m_server.Init("127.0.0.1", 6688);
        if (RESULT_OK != nResult)
        {
            break;
        }

        ms::AcceptCallbackFunc acfunc =
            std::bind(&CContral::AcceptCall, this, std::placeholders::_1, std::placeholders::_2);
        ms::RecvCallbackFunc func =
            std::bind(&CContral::RecvMsg, this, std::placeholders::_1, std::placeholders::_2);
        ms::RecvCompleteCallbackFunc rcfunc =
            std::bind(&CContral::RecvMsgComplete, this, std::placeholders::_1, std::placeholders::_2);
        ms::SendCompleteCallbackFunc sfunc =
            std::bind(&CContral::SendMsgComplete, this, std::placeholders::_1, std::placeholders::_2);
        ms::ShutdownCompleteCallbackFunc cfunc =
            std::bind(&CContral::CloseComplete, this, std::placeholders::_1, std::placeholders::_2);

        m_server.SetCallback(acfunc, func, rcfunc, sfunc, cfunc);

        m_nStatus = em_init;
        nResult = RESULT_OK;
    } while (0);
    return nResult;
}


int CContral::Start()
{
    int nResult = m_server.Start();
    return nResult;
}

int CContral::PostSend(SOCKET nSocket, const char * buf, long nSize)
{
    int nResult = m_server.PostSend(nSocket, buf, nSize);
    return nResult;
}

const CString CContral::GetRemote(SOCKET nSocket)
{
    return m_server.GetRemote(nSocket).c_str();
}

int CContral::InsertDoc(SOCKET nSocket, CDocument * pDoc)
{
    m_docMap[nSocket] = pDoc;
    return 0;
}

int CContral::RemoveDoc(SOCKET nSocket)
{
    if (m_docMap.find(nSocket) == m_docMap.end())
    {
        return -1;
    }
    m_docMap.erase(nSocket);
    return 0;
}

int CContral::InsertView(SOCKET nSocket, CView * pDoc)
{
    m_viewMap[nSocket] = pDoc;
    return 0;
}

int CContral::RemoveView(SOCKET nSocket)
{
    if (m_viewMap.find(nSocket) == m_viewMap.end())
    {
        return -1;
    }
    m_viewMap.erase(nSocket);
    return 0;
}
