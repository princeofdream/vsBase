
// FactoryToolsDlg.cpp : ʵ���ļ�
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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CFactoryToolsDlg �Ի���


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
	// ����öԻ������Զ���������
	//  ���˴���ָ��öԻ���ĺ���ָ������Ϊ NULL���Ա�
	//  �˴���֪���öԻ����ѱ�ɾ����
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


// CFactoryToolsDlg ��Ϣ�������

BOOL CFactoryToolsDlg::OnInitDialog()
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

	//ShowWindow(SW_MINIMIZE);
	ShowWindow(SW_SHOW);

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	
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

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CFactoryToolsDlg::OnPaint()
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
HCURSOR CFactoryToolsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ���û��ر� UI ʱ������������Ա���������ĳ��
//  �������Զ�����������Ӧ�˳���  ��Щ
//  ��Ϣ�������ȷ����������: �����������ʹ�ã�
//  ������ UI�������ڹرնԻ���ʱ��
//  �Ի�����Ȼ�ᱣ�������

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
	// �����������Ա�����������Զ���
	//  �������Իᱣ�ִ�Ӧ�ó���
	//  ʹ�Ի���������������� UI ����������
	if (m_pAutoProxy != NULL)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}



void CFactoryToolsDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	m_output_msg += "Note��\r\n��¼�豸���뱣֤�����������������ֵ�����һ����\r\n1����������TF����������������TF���ڸ�Ŀ¼�����˿��ļ�enable_hg_ibox_product��";
	m_output_msg += "\r\n2���о߰Ѷ�ӦGPIO�ڼкò��ӵء�\r\n�� Recovery ģʽ�¼�����¼�豸�š�";

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
			m_output_msg += "�Ҳ����豸��";
		}
		else
		{
			m_output_msg += "Recoveryģʽ���豸��Recoveryģʽ��";

			m_ver = m_ctrlcent.StartSingleCommand(_T("adb shell getprop ro.mediatek.version.release"));
			m_fin_prt = m_ctrlcent.StartSingleCommand(_T("adb shell getprop ro.build.fingerprint"));

			m_output_msg += "\r\n\r\n\r\n\r";
			m_output_msg += "==========================================\r\n";
			m_output_msg += "����汾�ţ�" + m_ver;
			m_output_msg += "���ָ�ƣ�" + m_fin_prt;
			m_output_msg += "==========================================";
		}
	}
	else
	{
		//m_output_msg += check_stat;

		check_boot = m_ctrlcent.StartSingleCommand(_T("adb shell getprop dev.bootcomplete"));

		if (check_boot.Find("1", 0) < 0)
		{
			m_output_msg += "Android ģʽ�����ڿ�����";
		}
		else
		{
			m_output_msg += "Android ģʽ��������ɣ�";
		}

		m_ver = m_ctrlcent.StartSingleCommand(_T("adb shell getprop ro.mediatek.version.release"));
		m_fin_prt = m_ctrlcent.StartSingleCommand(_T("adb shell getprop ro.build.fingerprint"));

		m_output_msg += "\r\n\r\n\r\n\r";
		m_output_msg += "==========================================\r\n";
		m_output_msg += "����汾�ţ�" + m_ver;
		m_output_msg += "���ָ�ƣ�" + m_fin_prt;
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
		m_stat_info = "Check ADB Stat: Fail\r\n\r\n�밲װ�����ú�ADB����������";
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
		m_output_msg += "\r\nû�������豸�ţ�ֹͣ��¼��";
		return;
	}
	m_output_msg += "\r\n������豸��Ϊ��	";
	m_output_msg += get_sn;

	get_burn_stat = m_confutil.start_burn_local_config_to_machine(get_sn);
	m_output_msg += "\r\n��¼״̬��";
	m_output_msg += get_burn_stat;

#else
	if (audo_detect_device_to_burn_sn)
	{
		CWnd *pWnd;
		
		audo_detect_device_to_burn_sn = FALSE;

		pWnd = GetDlgItem(IDC_START_BURN);
		pWnd->SetWindowText(_T("��ʼ��¼�豸��"));
		m_output_msg += "\r\n�� <��ʼ��¼�豸��> ��ʼ�ȴ��豸���벢��¼��";
	}
	else
	{
		//from false to true, this status is true
		CWnd *pWnd;
		
		audo_detect_device_to_burn_sn = TRUE;

		pWnd = GetDlgItem(IDC_START_BURN);
		pWnd->SetWindowText(_T("ֹͣ��¼�豸��"));
		m_output_msg += "\r\n�ȴ��豸���벢���뵽Recoveryģʽ....";
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
		m_output_msg = "û��Ӧ�������Ѿ�ִ�гɹ���God bless you!";
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
	m_output_msg += "\r\n��¼״̬��\r\n";
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
		m_output_msg += "�豸δ��ɿ�������ȴ��豸������ɺ��ٲ�����";
		return;
	}

	m_output_msg += "\r\n�������ģʽ����";
	m_ctrlcent.StartSingleCommand(_T("adb shell touch /sdcard/disable_hg_ibox_camera"));
	m_ctrlcent.StartSingleCommand(_T("adb shell touch /sdcard/disable_hg_ibox_wifiap"));
	m_ctrlcent.StartSingleCommand(_T("adb shell touch /sdcard/disable_hg_ibox_ap"));
	m_ctrlcent.StartSingleCommand(_T("adb shell touch /sdcard/disable_hg_ibox_timeout"));

	m_ctrlcent.StartSingleCommand(_T("adb shell input keyevent 4"));
	m_ctrlcent.StartSingleCommand(_T("adb shell \"am start -n com.android.sim/com.android.sim.CITMain\""));
	


}


void CFactoryToolsDlg::OnBnClickedPlayMusic()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

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

	::GetCurrentDirectory(300, (LPTSTR)chPath);//�õ���ǰĿ¼


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
		m_output_msg += "�豸δ��ɿ�������ȴ��豸������ɺ��ٲ�����";
		return;
	}
	m_output_msg += "\r\n�������֣���4���ӡ���";


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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	m_output_msg += "����������Recoveryģʽ";

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
	m_output_msg += "��������..";

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
		m_output_msg += "�豸δ��ɿ�������ȴ��豸������ɺ��ٲ�����";
		return;
	}

	m_output_msg += "\r\n�������ģʽ����";
	m_ctrlcent.StartSingleCommand(_T("adb shell rm -rf  /sdcard/disable_hg_ibox_camera"));
	m_ctrlcent.StartSingleCommand(_T("adb shell rm -rf  /sdcard/disable_hg_ibox_wifiap"));
	m_ctrlcent.StartSingleCommand(_T("adb shell rm -rf  /sdcard/disable_hg_ibox_ap"));
	m_ctrlcent.StartSingleCommand(_T("adb shell rm -rf /sdcard/disable_hg_ibox_timeout"));

	m_ctrlcent.StartSingleCommand(_T("adb shell input keyevent 4"));
//	m_ctrlcent.StartSingleCommand(_T("adb shell \"am start -n com.android.sim/com.android.sim.CITMain\""));
}


void CFactoryToolsDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
				m_output_msg += "\r\nû�������豸�ţ�ֹͣ��¼��";
				return ;
			}
			m_output_msg += "\r\n������豸��Ϊ��	";
			m_output_msg += get_sn;

			get_burn_stat = m_confutil.start_burn_local_config_to_machine(get_sn);
			m_output_msg += "\r\n�����ļ���¼״̬��";
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