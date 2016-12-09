
// FactoryToolsDlg.cpp : ʵ���ļ�
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
		m_output_msg += "\r\nû�������豸�ţ�ֹͣ��¼��";
		return;
	}
	m_output_msg += "\r\n������豸��Ϊ��	";
	m_output_msg += get_sn;

	get_burn_stat = m_confutil.start_burn_local_config_to_machine(get_sn);
	m_output_msg += "\r\n��¼״̬��";
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
	m_output_msg += "\r\n��¼״̬��\r\n";
	m_output_msg += get_burn_stat;

	return;
}
