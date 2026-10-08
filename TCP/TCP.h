
// TCP.h: TCP 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号
#include "tcp/TcpServer.h"
#include "CContral.h"
#include <map>

// CTCPApp:
// 有关此类的实现，请参阅 TCP.cpp
//

class CTCPApp : public CWinAppEx
{
public:
	CTCPApp() noexcept;


// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
	ms::CTcpServer* m_tcpServer{ NULL };
private:
    CContral m_contral;

public:
    CContral& GetContral() { return m_contral; }

    int StartServer(const CString& szIP, int port);

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();


    afx_msg void OnFileNew();
    int NewClient(SOCKET nSocket);

    //afx_msg void OnFileOpen();


	DECLARE_MESSAGE_MAP()
	afx_msg void OnServerStart();
	afx_msg void OnServerStop();
};

extern CTCPApp theApp;
