
// FactoryToolsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FactoryTools.h"
#include "FactoryToolsDlg.h"
#include "DlgProxy.h"
#include "afxdialogex.h"
#include <dbt.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static bool draw_runing = FALSE;
static bool audo_detect_device_to_burn_sn = FALSE;

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
	ON_BN_CLICKED(IDC_ANDROID_TEST, &CFactoryToolsDlg::OnBnClickedAndroidTest)
	ON_BN_CLICKED(IDC_PLAY_MUSIC, &CFactoryToolsDlg::OnBnClickedPlayMusic)
	ON_BN_CLICKED(IDC_REBOOT_TO_RECOVERY, &CFactoryToolsDlg::OnBnClickedRebootToRecovery)
	ON_BN_CLICKED(IDC_BUTTON1, &CFactoryToolsDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_CLEAR_TEST, &CFactoryToolsDlg::OnBnClickedClearTest)
	ON_WM_TIMER()
	ON_MESSAGE(WM_DEVICECHANGE, OnMyDeviceChange)
	//ON_WM_DEVICECHANGE()
	//ON_MESSAGE(WM_DEVICECHANGE, &CFactoryToolsDlg::OnDeviceChange)
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

	SetTimer(1, 1000, NULL);

	HDEVNOTIFY hDevNotify;
	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
	ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
	NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	for (int i = 0; i<sizeof(GUID_DEVINTERFACE_LIST) / sizeof(GUID); i++) {
		NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_LIST[i];
		hDevNotify = RegisterDeviceNotification(this->GetSafeHwnd(), &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
		if (!hDevNotify) {
			AfxMessageBox(CString("Can't register device notification: ")
				+ _com_error(GetLastError()).ErrorMessage(), MB_ICONEXCLAMATION);
			return FALSE;
		}
	}

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
	CString get_device_info;
	CString check_stat;
	CString check_boot;
	CString m_ver;
	CString m_fin_prt;
#if 0
	TRACE("James's Debug....");
	m_adbstat.Create(IDD_ADBSTAT, GetDlgItem(IDC_CHECK_ADB));
	m_adbstat.ShowWindow(TRUE);
	UpdateData(false);
#else
	CheckAdbStat();
#endif

	m_output_msg += "\r\n\r\n";
	m_output_msg += "Note：\r\n烧录设备号请保证满足以下两个条件种的任意一个：\r\n1，插入外置TF卡，并事先在外置TF卡内根目录建立了空文件enable_hg_ibox_product；";
	m_output_msg += "\r\n2，夹具把对应GPIO口夹好并接地。\r\n到 Recovery 模式下即可烧录设备号。";

	get_device_info = m_ctrlcent.StartSingleCommand("adb devices");
	m_output_msg += "\r\n\r\n";
	m_output_msg += get_device_info;

	
	TRACE("\nEntering OnBnClickedCheckAdb ...");

	check_stat = m_confutil.check_machine_stat(_T("devices"));
	if (check_stat.Find("device", 0) < 0)
	{
		check_stat = m_confutil.check_machine_stat(_T("recovery"));
		if (check_stat.Find("recovery", 0) < 0)
		{
			m_output_msg += "找不到设备！";
		}
		else
		{
			m_output_msg += "Recovery模式：设备在Recovery模式！";

			m_ver = m_ctrlcent.StartSingleCommand(_T("adb shell getprop ro.mediatek.version.release"));
			m_fin_prt = m_ctrlcent.StartSingleCommand(_T("adb shell getprop ro.build.fingerprint"));

			m_output_msg += "\r\n\r\n\r\n\r";
			m_output_msg += "==========================================\r\n";
			m_output_msg += "软件版本号：" + m_ver;
			m_output_msg += "软件指纹：" + m_fin_prt;
			m_output_msg += "==========================================";
		}
	}
	else
	{
		//m_output_msg += check_stat;

		check_boot = m_ctrlcent.StartSingleCommand(_T("adb shell getprop dev.bootcomplete"));

		if (check_boot.Find("1", 0) < 0)
		{
			m_output_msg += "Android 模式：正在开机！";
		}
		else
		{
			m_output_msg += "Android 模式：开机完成！";
		}

		m_ver = m_ctrlcent.StartSingleCommand(_T("adb shell getprop ro.mediatek.version.release"));
		m_fin_prt = m_ctrlcent.StartSingleCommand(_T("adb shell getprop ro.build.fingerprint"));

		m_output_msg += "\r\n\r\n\r\n\r";
		m_output_msg += "==========================================\r\n";
		m_output_msg += "软件版本号：" + m_ver;
		m_output_msg += "软件指纹：" + m_fin_prt;
		m_output_msg += "==========================================";
	}


	return;
	
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
		m_stat_info = "Check ADB Stat: Fail\r\n\r\n请安装并配置好ADB环境变量！";
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
	
	TRACE("\nEntering OnBnClickedStartBurn ...");

	m_adbstat.set_serial_number_empty();
	get_info = m_confutil.check_machine_stat(_T("recovery"));
	m_output_msg = get_info;
	
	

#if 0 //JamesL
	if (get_info.Find(_T("recovery")) < 0)
	{
		m_output_msg = get_info;
		return;
	}


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

#else
	if (audo_detect_device_to_burn_sn)
	{
		CWnd *pWnd;
		
		audo_detect_device_to_burn_sn = FALSE;

		pWnd = GetDlgItem(IDC_START_BURN);
		pWnd->SetWindowText(_T("开始烧录设备号"));
		m_output_msg += "\r\n按 <开始烧录设备号> 开始等待设备插入并烧录。";
	}
	else
	{
		//from false to true, this status is true
		CWnd *pWnd;
		
		audo_detect_device_to_burn_sn = TRUE;

		pWnd = GetDlgItem(IDC_START_BURN);
		pWnd->SetWindowText(_T("停止烧录设备号"));
		m_output_msg += "\r\n等待设备插入并进入到Recovery模式....";
	}
#endif

	return;

	
}

BOOL CFactoryToolsDlg::StartBurn()
{
	CString get_info;
	CString get_sn;
	CString get_burn_stat;
	BOOL ret = FALSE;

	//draw_runing = TRUE;
	//m_output_msg = "";
	get_sn = "";

	TRACE("\nEntering StartBurn ...");

	m_adbstat.set_serial_number_empty();
	get_info = m_confutil.check_machine_stat("recovery");
	//m_output_msg = get_info;

#if 1 //JamesL
	if (get_info.Find(_T("recovery")) > 0)
	{
		//m_output_msg = get_info;
		ret = TRUE;

	}
	return ret;
#endif
}


void CFactoryToolsDlg::OnBnClickedRunCmd()
{
	CString get_info;
	char get_cmd[1024];
	char get_full_cmd[1024];
	//CEdit* pEdit = (CEdit*)GetDlgItem(IDC_INFO);
	TCHAR get_win_cmd[1024];

	draw_runing = TRUE;
	m_output_msg = "";

	memset(get_cmd, 0x0, sizeof(get_cmd));
	memset(get_full_cmd, 0x0, sizeof(get_full_cmd));

	GetDlgItem(IDC_SELF_CMD)->GetWindowText(get_win_cmd, sizeof(get_win_cmd));
#ifdef UNICODE  
	//MultiByteToWideChar(CP_ACP, 0, m_cmd, -1, command, sizeof(command));
	WideCharToMultiByte(CP_ACP, 0, get_cmd, -1, get_win_cmd, sizeof(get_win_cmd));
#else
	strcpy_s(get_full_cmd, get_win_cmd);
#endif


	if (strlen(get_win_cmd) == 0)
	{
		TRACE("-------1-------->");
		sprintf_s(get_full_cmd, "adb", "");
	}
	else if (_stricmp(get_win_cmd, "devices") == 0)
	{
		TRACE("--------2------->");
		sprintf_s(get_full_cmd, "adb devices", "");
	}
	else
	{
		TRACE("-------3-------->");
		sprintf_s(get_full_cmd, "adb shell \"%s\"", get_win_cmd);
	}
	TRACE("--------------->");
	TRACE(get_full_cmd);
	TRACE("<---------------");

	//get_info = m_ctrlcent.StartSingleCommand("adb shell ls");
	get_info = m_ctrlcent.StartSingleCommand(_T(get_full_cmd));


	if (get_info.GetLength() == 0)
		m_output_msg = "没回应，可能已经执行成功，God bless you!";
	else
		m_output_msg += get_info;

	TRACE(get_info);

}

bool CFactoryToolsDlg::Loop()
{
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_INFO);
	TRACE(_T(" Debug by JamesL \n"));
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

	get_info = m_confutil.check_machine_stat(_T("recovery"));
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


void CFactoryToolsDlg::OnBnClickedAndroidTest()
{
	CString check_stat;
	CString check_boot;
	check_stat = m_confutil.check_machine_stat(_T("devices"));
	if (check_stat.Find("device",0) < 0 )
	{
		m_output_msg = check_stat;
		return;
	}
	m_output_msg = check_stat;

	check_boot = m_ctrlcent.StartSingleCommand(_T("adb shell getprop dev.bootcomplete"));

	if (check_boot.Find("1",0) < 0)
	{
		m_output_msg += "设备未完成开机，请等待设备开机完成后再操作！";
		return;
	}

	m_output_msg += "\r\n进入测试模式……";
	m_ctrlcent.StartSingleCommand(_T("adb shell touch /sdcard/disable_hg_ibox_camera"));
	m_ctrlcent.StartSingleCommand(_T("adb shell touch /sdcard/disable_hg_ibox_wifiap"));
	m_ctrlcent.StartSingleCommand(_T("adb shell touch /sdcard/disable_hg_ibox_ap"));
	m_ctrlcent.StartSingleCommand(_T("adb shell touch /sdcard/disable_hg_ibox_timeout"));

	m_ctrlcent.StartSingleCommand(_T("adb shell input keyevent 4"));
	m_ctrlcent.StartSingleCommand(_T("adb shell \"am start -n com.android.sim/com.android.sim.CITMain\""));
	


}


void CFactoryToolsDlg::OnBnClickedPlayMusic()
{
	// TODO: 在此添加控件通知处理程序代码

	char chPath[301];

	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;

	CString check_stat;
	CString check_boot;

	TRACE("\nEntering OnBnClickedPlayMusic ...");
	si.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;

	::GetCurrentDirectory(300, (LPTSTR)chPath);//得到当前目录


	check_stat = m_confutil.check_machine_stat(_T("devices"));
	if (check_stat.Find("device", 0) < 0)
	{
		m_output_msg = check_stat;
		return;
	}
	m_output_msg = check_stat;
	check_boot = m_ctrlcent.StartSingleCommand(_T("adb shell getprop dev.bootcomplete"));
	if (check_boot.Find("1", 0) < 0)
	{
		m_output_msg += "设备未完成开机，请等待设备开机完成后再操作！";
		return;
	}
	m_output_msg += "\r\n播放音乐，共4分钟……";


	m_ctrlcent.StartSingleCommand(_T("adb shell touch /sdcard/disable_hg_ibox_camera"));
	m_ctrlcent.StartSingleCommand(_T("adb shell touch /sdcard/disable_hg_ibox_wifiap"));
	m_ctrlcent.StartSingleCommand(_T("adb shell touch /sdcard/disable_hg_ibox_ap"));
	m_ctrlcent.StartSingleCommand(_T("adb shell touch /sdcard/disable_hg_ibox_timeout"));

	m_ctrlcent.StartSingleCommand(_T("adb shell input keyevent 4"));

	//m_ctrlcent.StartSingleCommand(_T("adb shell input keyevent 86"));

	CreateProcess(NULL, _T("adb push test.mp3 /sdcard/"), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);

	m_ctrlcent.StartSingleCommand("adb shell \"am start -n com.android.music/com.android.music.MediaPlaybackActivity -d /sdcard/test.mp3\"");
	//m_ctrlcent.StartSingleCommand("adb shell \"am start -n com.android.ringtune/com.android.ringtune.MediaPlaybackActivity -d /sdcard/test.mp3\"");
}


void CFactoryToolsDlg::OnBnClickedRebootToRecovery()
{
	// TODO: 在此添加控件通知处理程序代码
	CString check_stat;
	CString check_boot;

	TRACE("\nEntering OnBnClickedRebootToRecovery ...");
	check_stat = m_confutil.check_machine_stat(_T("devices"));
	if (check_stat.Find("device", 0) < 0)
	{
		m_output_msg = check_stat;
		return;
	}
	m_output_msg = check_stat;
	m_output_msg += "正在重启到Recovery模式";

	m_ctrlcent.StartSingleCommand("adb shell \"reboot recovery\"");
}


void CFactoryToolsDlg::OnBnClickedButton1()
{
	CString check_stat;
	CString check_boot;

	TRACE("\nEntering OnBnClickedButton1 ...");
	check_stat = m_confutil.check_machine_stat(_T("recovery"));
	if (check_stat.Find("recovery", 0) < 0)
	{
		m_output_msg = check_stat;
		return;
	}
	m_output_msg = check_stat;
	m_output_msg += "正在重启..";

	m_ctrlcent.StartSingleCommand("adb shell reboot");
}


void CFactoryToolsDlg::OnBnClickedClearTest()
{
	CString check_stat;
	CString check_boot;

	TRACE("\nEntering OnBnClickedClearTest ...");
	check_stat = m_confutil.check_machine_stat(_T("devices"));
	if (check_stat.Find("device", 0) < 0)
	{
		m_output_msg = check_stat;
		return;
	}
	m_output_msg = check_stat;

	check_boot = m_ctrlcent.StartSingleCommand(_T("adb shell getprop dev.bootcomplete"));

	if (check_boot.Find("1", 0) < 0)
	{
		m_output_msg += "设备未完成开机，请等待设备开机完成后再操作！";
		return;
	}

	m_output_msg += "\r\n进入测试模式……";
	m_ctrlcent.StartSingleCommand(_T("adb shell rm -rf  /sdcard/disable_hg_ibox_camera"));
	m_ctrlcent.StartSingleCommand(_T("adb shell rm -rf  /sdcard/disable_hg_ibox_wifiap"));
	m_ctrlcent.StartSingleCommand(_T("adb shell rm -rf  /sdcard/disable_hg_ibox_ap"));
	m_ctrlcent.StartSingleCommand(_T("adb shell rm -rf /sdcard/disable_hg_ibox_timeout"));

	m_ctrlcent.StartSingleCommand(_T("adb shell input keyevent 4"));
//	m_ctrlcent.StartSingleCommand(_T("adb shell \"am start -n com.android.sim/com.android.sim.CITMain\""));
}


void CFactoryToolsDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	BOOL isok = FALSE;
	CString get_info;
	CString get_sn;
	CString get_burn_stat;
	AdbStat dlgadbstat;

	if (!audo_detect_device_to_burn_sn)
	{
		return;
	}


	switch (nIDEvent)
	{
	case 1:
		isok=StartBurn();
		if(isok)
		{
			KillTimer(1);

			dlgadbstat.DoModal();
			get_sn = dlgadbstat.get_serial_number();

			m_output_msg = "";
			if (get_sn.IsEmpty())
			{
				m_output_msg += "\r\n没有输入设备号，停止烧录！";
				return ;
			}
			m_output_msg += "\r\n输入的设备号为：	";
			m_output_msg += get_sn;

			get_burn_stat = m_confutil.start_burn_local_config_to_machine(get_sn);
			m_output_msg += "\r\n配置文件烧录状态：";
			m_output_msg += get_burn_stat;				
		}
		break;
     
	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

LRESULT CFactoryToolsDlg::OnMyDeviceChange(WPARAM wParam, LPARAM lParam)
{
	CString szTmp;

	if (!audo_detect_device_to_burn_sn)
	{
		return 0;
	}


	if (DBT_DEVICEARRIVAL == wParam || DBT_DEVICEREMOVECOMPLETE == wParam)
	{
		PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
		PDEV_BROADCAST_DEVICEINTERFACE pDevInf;
		PDEV_BROADCAST_HANDLE pDevHnd;
		PDEV_BROADCAST_OEM pDevOem;
		PDEV_BROADCAST_PORT pDevPort;
		PDEV_BROADCAST_VOLUME pDevVolume;

		switch (pHdr->dbch_devicetype) 
		{
		case DBT_DEVTYP_DEVICEINTERFACE:
			pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)pHdr;
			//UpdateDevice(pDevInf, wParam);
			if (DBT_DEVICEARRIVAL == wParam) 
			{
				szTmp.Format(_T("Adding \r\n"));
				SetTimer(1, 1000, NULL);
				
			}
			else 
			{
				szTmp.Format(_T("Removing \r\n"));
				KillTimer(1);
			}

			TRACE(szTmp);
			//AfxMessageBox(szTmp);
			break;

		case DBT_DEVTYP_HANDLE:
			pDevHnd = (PDEV_BROADCAST_HANDLE)pHdr;
			break;

		case DBT_DEVTYP_OEM:
			pDevOem = (PDEV_BROADCAST_OEM)pHdr;
			break;

		case DBT_DEVTYP_PORT:
			pDevPort = (PDEV_BROADCAST_PORT)pHdr;
			break;

		case DBT_DEVTYP_VOLUME:
			pDevVolume = (PDEV_BROADCAST_VOLUME)pHdr;
			break;
		}
	}
	return 0;
}