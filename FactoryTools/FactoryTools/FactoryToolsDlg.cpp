
// FactoryToolsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FactoryTools.h"
#include "FactoryToolsDlg.h"
#include "DlgProxy.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static bool draw_runing = FALSE;

DWORD __stdcall CFactoryToolsDlgThreadProc(void *pVoid)
{
	if (pVoid != NULL) {
		CFactoryToolsDlg *pShell = (CFactoryToolsDlg *)pVoid;
		pShell->Loop();
	}
	return 0;
}


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

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

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
	EnableActiveAccessibility();
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFactoryToolsDlg 对话框


IMPLEMENT_DYNAMIC(CFactoryToolsDlg, CDialogEx);

CFactoryToolsDlg::CFactoryToolsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FACTORYTOOLS_DIALOG, pParent)
{
	EnableActiveAccessibility();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy = NULL;
}

CFactoryToolsDlg::~CFactoryToolsDlg()
{
	// 如果该对话框有自动化代理，则
	//  将此代理指向该对话框的后向指针设置为 NULL，以便
	//  此代理知道该对话框已被删除。
	if (m_pAutoProxy != NULL)
		m_pAutoProxy->m_pDialog = NULL;
}

void CFactoryToolsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFactoryToolsDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &CFactoryToolsDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK_ADB, &CFactoryToolsDlg::OnBnClickedCheckAdb)
	ON_BN_CLICKED(IDC_START_BURN, &CFactoryToolsDlg::OnBnClickedStartBurn)
	ON_BN_CLICKED(IDC_RUN_CMD, &CFactoryToolsDlg::OnBnClickedRunCmd)
	ON_BN_CLICKED(IDC_RECHECK_STAT, &CFactoryToolsDlg::OnBnClickedRecheckStat)
END_MESSAGE_MAP()


// CFactoryToolsDlg 消息处理程序

BOOL CFactoryToolsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//ShowWindow(SW_MINIMIZE);
	ShowWindow(SW_SHOW);

	// TODO: 在此添加额外的初始化代码
	
	//CheckAdbStat();
	DWORD dwThread = FALSE;
	hThread = CreateThread(NULL, 0, CFactoryToolsDlgThreadProc, this, 0, &dwThread);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CFactoryToolsDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFactoryToolsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFactoryToolsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 当用户关闭 UI 时，如果控制器仍保持着它的某个
//  对象，则自动化服务器不应退出。  这些
//  消息处理程序确保如下情形: 如果代理仍在使用，
//  则将隐藏 UI；但是在关闭对话框时，
//  对话框仍然会保留在那里。

void CFactoryToolsDlg::OnClose()
{
	if (CanExit())
		CDialogEx::OnClose();
}

void CFactoryToolsDlg::OnOK()
{
	if (CanExit())
		CDialogEx::OnOK();
}

void CFactoryToolsDlg::OnCancel()
{
	if (CanExit())
		CDialogEx::OnCancel();
}

BOOL CFactoryToolsDlg::CanExit()
{
	// 如果代理对象仍保留在那里，则自动化
	//  控制器仍会保持此应用程序。
	//  使对话框保留在那里，但将其 UI 隐藏起来。
	if (m_pAutoProxy != NULL)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}



void CFactoryToolsDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


void CFactoryToolsDlg::OnBnClickedCheckAdb()
{
	// TODO: 在此添加控件通知处理程序代码
#if 0
	TRACE("James's Debug....");
	m_adbstat.Create(IDD_ADBSTAT, GetDlgItem(IDC_CHECK_ADB));
	m_adbstat.ShowWindow(TRUE);
	UpdateData(false);
#else
	CheckAdbStat();
#endif
}


void CFactoryToolsDlg::CheckAdbStat()
{
	//CEdit* pEdit = (CEdit*)GetDlgItem(IDC_STAT);
	CString m_stat_info;
	CString get_adb_stat;

	draw_runing = TRUE;
	m_output_msg = "";

	get_adb_stat = m_ctrlcent.StartSingleCommand("adb");
	if (get_adb_stat.IsEmpty())
		m_stat_info = "Check ADB Stat: Fail";
	else
		m_stat_info = "Check ADB Stat: OK";

	//pEdit->SetWindowText((LPCTSTR)m_stat_info);
	m_output_msg = m_stat_info;

	return;
}



void CFactoryToolsDlg::OnBnClickedStartBurn()
{
	CString get_info;
	CString get_sn;
	CString get_burn_stat;

	draw_runing = TRUE;
	m_output_msg = "";
	get_sn = "";
	
	m_adbstat.set_serial_number_empty();
	get_info = m_confutil.check_machine_stat();
	m_output_msg = get_info;
	
#if 1 //JamesL
	if (get_info.Find(_T("recovery")) < 0)
	{
		m_output_msg = get_info;
		return;
	}
#endif

	/*new a dialog to get input serial number*/
	m_adbstat.DoModal();
	get_sn = m_adbstat.get_serial_number();
	
	if (get_sn.IsEmpty())
	{
		m_output_msg += "\r\n没有输入设备号，停止烧录！";
		return;
	}
	m_output_msg += "\r\n输入的设备号为：	";
	m_output_msg += get_sn;

	get_burn_stat = m_confutil.start_burn_local_config_to_machine(get_sn);
	m_output_msg += "\r\n烧录状态：";
	m_output_msg += get_burn_stat;

	return;
}


void CFactoryToolsDlg::OnBnClickedRunCmd()
{
	CString get_info;
	char *get_cmd;
	char get_full_cmd[1024];
	//CEdit* pEdit = (CEdit*)GetDlgItem(IDC_INFO);
	TCHAR get_win_cmd[1024];

	draw_runing = TRUE;
	m_output_msg = "";

	//memset(get_cmd, 0x0, sizeof(get_cmd));
	memset(get_full_cmd, 0x0, sizeof(get_full_cmd));

	GetDlgItem(IDC_SELF_CMD)->GetWindowText(get_win_cmd, sizeof(get_win_cmd));
	get_cmd = m_ctrlcent.ConvertLPWSTRToLPSTR(get_win_cmd);


	if (strlen(get_cmd) == 0)
		sprintf_s(get_full_cmd, "adb", "");
	else if ( _stricmp(get_cmd, "devices") == 0)
		sprintf_s(get_full_cmd, "adb devices", "");
	else
		sprintf_s(get_full_cmd, "adb shell \"%s\"", get_cmd);
	TRACE(get_full_cmd);

	//get_info = m_ctrlcent.StartSingleCommand("adb shell ls");
	get_info = m_ctrlcent.StartSingleCommand(get_full_cmd);


	if (get_info.IsEmpty())
		m_output_msg = "Error! Can not Find ADB. Please Check if ADB is installed!";
	else
		m_output_msg += get_info;

	TRACE(get_info);

}

bool CFactoryToolsDlg::Loop()
{
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_INFO);
	TRACE(_T(" Debug by JamesL "));
#if 1
	while (TRUE)
	{
		if (draw_runing)
		{
			draw_runing = FALSE;
			if (m_output_msg.IsEmpty())
				m_output_msg = "Running......\n";
		}
		pEdit->SetWindowText(m_output_msg);
		Sleep(1000);
	}
#endif
	return TRUE;
}




void CFactoryToolsDlg::OnBnClickedRecheckStat()
{
	CString get_info;
	CString get_sn;
	CString get_burn_stat;

	CString check_stat;

	draw_runing = TRUE;
	m_output_msg = "";
	get_sn = "";

	get_info = m_confutil.check_machine_stat();
	m_output_msg = get_info;

	if (get_info.Find(_T("recovery")) < 0)
	{
		m_output_msg = get_info;
		return;
	}

	check_stat = "CheckOnly";
	get_burn_stat = m_confutil.check_burned_data(check_stat);
	m_output_msg += "\r\n烧录状态：\r\n";
	m_output_msg += get_burn_stat;

	return;
}
