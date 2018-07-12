
// TCPDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "TcpClient.h"
#include "TcpServer.h"
#include "afxcmn.h"

// CTCPDlg �Ի���
class CTCPDlg : public CDialogEx
{
// ����
public:
	CTCPDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TCP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
    CTcpServer m_tcpserver;
    CTcpClient m_tcpclient;

    CButton m_ctServer;
    CButton m_ctClient;
public:
    afx_msg void OnBnClickedButtonServer();
    afx_msg void OnBnClickedButtonClient();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    CEdit m_ctPort;
    CIPAddressCtrl m_ctIP;
    int m_port;
    DWORD m_ip;
};
