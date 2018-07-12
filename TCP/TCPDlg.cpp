
// TCPDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TCP.h"
#include "TCPDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib ")

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTCPDlg �Ի���



CTCPDlg::CTCPDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTCPDlg::IDD, pParent)
    , m_port(8080)
    , m_ip((127<<24)+1)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTCPDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON_SERVER, m_ctServer);
    DDX_Control(pDX, IDC_BUTTON_CLIENT, m_ctClient);
    DDX_Control(pDX, IDC_EDIT_PORT, m_ctPort);
    DDX_Control(pDX, IDC_IPADDRESS_SERVER, m_ctIP);
    DDX_Text(pDX, IDC_EDIT_PORT, m_port);
    DDV_MinMaxInt(pDX, m_port, 0, 65535);
    DDX_IPAddress(pDX, IDC_IPADDRESS_SERVER, m_ip);
}

BEGIN_MESSAGE_MAP(CTCPDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_SERVER, &CTCPDlg::OnBnClickedButtonServer)
    ON_BN_CLICKED(IDC_BUTTON_CLIENT, &CTCPDlg::OnBnClickedButtonClient)
    ON_WM_TIMER()
    ON_WM_DESTROY()
END_MESSAGE_MAP()


// CTCPDlg ��Ϣ�������

BOOL CTCPDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
    WSAData wsa;
    ::WSAStartup(MAKEWORD(2, 2), &wsa);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CTCPDlg::OnDestroy()
{
    CDialogEx::OnDestroy();

    // TODO:  �ڴ˴������Ϣ����������
    WSACleanup();
}

void CTCPDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTCPDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CTCPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


#define TCP_SERVER 2
#define TCP_CLIENT 4


void CTCPDlg::OnBnClickedButtonServer()
{
    // TODO:  �ڴ���ӿؼ�֪ͨ����������
    m_ctClient.EnableWindow(FALSE);
    m_ctServer.EnableWindow(FALSE);
    m_ctServer.SetWindowText(_T("���ڼ��"));
    CString ip;
    m_ctIP.GetWindowText(ip);
    CString port;
    m_ctPort.GetWindowText(port);
    m_tcpserver.run();
    SetTimer(TCP_SERVER, 1000, NULL);
}


void CTCPDlg::OnBnClickedButtonClient()
{
    // TODO:  �ڴ���ӿؼ�֪ͨ����������
    m_ctServer.EnableWindow(FALSE);
    m_ctClient.EnableWindow(FALSE);
    m_ctClient.SetWindowText(_T("��������"));
    m_tcpserver.run();
    SetTimer(TCP_CLIENT, 1000, NULL);
}


void CTCPDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
    switch (nIDEvent)
    {
    case TCP_SERVER:
        
        break;

    case TCP_CLIENT:

        break;
    }

    CDialogEx::OnTimer(nIDEvent);
}

