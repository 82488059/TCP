
// TCP.cpp: 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "TCP.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "TCPDoc.h"
#include "TCPView.h"
#include "core/Charset.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTCPApp

BEGIN_MESSAGE_MAP(CTCPApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CTCPApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CTCPApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CTCPApp::OnFilePrintSetup)
    ON_COMMAND(ID_SERVER_START, &CTCPApp::OnServerStart)
    ON_COMMAND(ID_SERVER_STOP, &CTCPApp::OnServerStop)
END_MESSAGE_MAP()


// CTCPApp 构造

CTCPApp::CTCPApp() noexcept
{
	m_bHiColorIcons = TRUE;

	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则: 
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("TCP.AppID.NoVersion"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的 CTCPApp 对象

CTCPApp theApp;


// CTCPApp 初始化

BOOL CTCPApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// 使用 RichEdit 控件需要 AfxInitRichEdit2()
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

    //
    m_pDocManager = new CDocManager;

	// 注册应用程序的文档模板。  文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_TCPTYPE,
		RUNTIME_CLASS(CTCPDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CTCPView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

	// 创建主 MDI 框架窗口
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;


	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// 调度在命令行中指定的命令。  如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// 主窗口已初始化，因此显示它并对其进行更新
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

int CTCPApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CTCPApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CTCPApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CTCPApp::OnFileNew()
{
    if (NULL == m_pDocManager)
        return;
    // first template position 
    POSITION posTemplate =  m_pDocManager->GetFirstDocTemplatePosition();
    // if NULL no template
    if (NULL == posTemplate)
    {
        return;
    }
    // first template 
    CDocTemplate* pTemplate = m_pDocManager->GetNextDocTemplate(posTemplate);
    if (!pTemplate)
        return;
    // create new doc and add to manager
    CDocument* pDoc = pTemplate->OpenDocumentFile(NULL);
    if (!pDoc)
    {
        return;
    }
    // set doc title 
    pDoc->SetTitle(_T("Readme"));
    // Test is not the first document
    POSITION posDoc =  pTemplate->GetFirstDocPosition();
    if (posDoc)
    {
        auto p = pTemplate->GetNextDoc(posDoc);
        if (p == pDoc)
        {
            TRACE(_T("new doc title is %s\n"), p->GetTitle());
        }
        else
        {
            TRACE(_T("first doc title is %s\n"), p->GetTitle());
            TRACE(_T("new doc title is %s\n"), pDoc->GetTitle());
        }
    }
    //POSITION vpos = pDoc->GetFirstViewPosition();
    //CEditView* pView  = (CEditView*)pDoc->GetNextView(vpos);
    //if (pView)
    //{
    //    pView->SetWindowText(_T("xxxxxxxx"));
    //    ((CTCPView*) (pView))->AddString(_T("1232321"));
    //}

}

int CTCPApp::NewClient(SOCKET nSocket)
{
    if (NULL == m_pDocManager)
        return -1;
    // first template position 
    POSITION posTemplate = m_pDocManager->GetFirstDocTemplatePosition();
    // if NULL no template
    if (NULL == posTemplate)
    {
        return -1;
    }
    // first template 
    CDocTemplate* pTemplate = m_pDocManager->GetNextDocTemplate(posTemplate);
    if (!pTemplate)
        return -1;
    // create new doc and add to manager
    CDocument* pDoc = pTemplate->OpenDocumentFile(NULL);
    if (!pDoc)
    {
        return -1;
    }
    // set doc title 
    CString title = m_contral.GetRemote(nSocket);
    pDoc->SetTitle(title);
    m_contral.InsertDoc(nSocket, pDoc);
    // Test is not the first document
    POSITION posDoc = pTemplate->GetFirstDocPosition();
    if (posDoc)
    {
        auto p = pTemplate->GetNextDoc(posDoc);
        if (p == pDoc)
        {
            TRACE(_T("new doc title is %s\n"), p->GetTitle());
        }
        else
        {
            TRACE(_T("first doc title is %s\n"), p->GetTitle());
            TRACE(_T("new doc title is %s\n"), pDoc->GetTitle());
        }
    }
    POSITION vpos = pDoc->GetFirstViewPosition();
    if (vpos)
    {
        CView* pV = pDoc->GetNextView(vpos);
        m_contral.InsertView(nSocket, pV);
        pV->GetDocument();
    }
    return 0;
}

// CTCPApp 自定义加载/保存方法

int CTCPApp::StartServer(const CString & szIP, int nPort)
{
    if (NULL != m_tcpServer)
    {
        return GENERAL_ERROR;
    }
    int nResult = GENERAL_ERROR;
    do 
    {
        m_tcpServer = new ms::CTcpServer;
        std::string ip;
#ifdef _UNICODE
        ip = ms::Charset::UnicodeToANSI(szIP);
#else
        ip = szIP;
#endif
        if (RESULT_OK != m_contral.Init())
        {
            break;
        }
        nResult = m_contral.Start();
    } while (0);
    return nResult;
}

void CTCPApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CTCPApp::LoadCustomState()
{
}

void CTCPApp::SaveCustomState()
{
}

// CTCPApp 消息处理程序





void CTCPApp::OnServerStart()
{
    // TODO: 在此添加命令处理程序代码
    if (NULL == m_pMainWnd)
    {
        return;
    }
    CMainFrame* pFrame = static_cast<CMainFrame*>(m_pMainWnd);
    if (NULL == pFrame)
    {
        return;
    }
    if (NULL == theApp.m_tcpServer)
    {
        CString szIp = pFrame->GetIp();
        CString szType = pFrame->GetType();
        CString szPort = pFrame->GetPort();

        int nResult = theApp.StartServer(szIp, _tstoi(szPort));
        if (RESULT_OK == nResult)
        {
            MessageBox(NULL, _T("启动服成功！"), _T("提示"), MB_OK);
        }
    }
    else
    {

    }

}


void CTCPApp::OnServerStop()
{
    // TODO: 在此添加命令处理程序代码
}
