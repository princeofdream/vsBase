// vsBaseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vsBase.h"
#include "vsBaseDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

st_debug m_debug;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainDlg dialog

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMainDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_debug.init_debug_env();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	iTransparent = 255;
	pFloatWnd = NULL;
}

CMainDlg::~CMainDlg()
{
	bool flag = m_debug.get_console_flag();
	if (flag)
	{
		m_debug.release_debug_env();
	}
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMainDlg)
	DDX_Control(pDX, IDC_SLIDER1, m_Slider);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
	//{{AFX_MSG_MAP(CMainDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_START_CMD, &CMainDlg::OnBnClickedStartCmd)
	ON_BN_CLICKED(IDC_STOP_CMD, &CMainDlg::OnBnClickedStopCmd)
	ON_BN_CLICKED(IDC_RUN_REC, &CMainDlg::OnBnClickedRunRec)
	ON_BN_CLICKED(IDC_IMGPROC, &CMainDlg::OnBnClickedImgproc)
	ON_BN_CLICKED(IDC_REFRASH, &CMainDlg::OnBnClickedRefrash)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainDlg message handlers

BOOL CMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	pFloatWnd = new CFloatWnd;
	pFloatWnd->Create(IDD_FLOATWND,this);
	pFloatWnd->ShowWindow(SW_SHOW);

	pFloatWnd->OnUpdateTransparent(iTransparent);

	m_Slider.SetRangeMax(255,TRUE);
	m_Slider.SetRangeMin(1,TRUE);
	m_Slider.SetPageSize(10);
	m_Slider.SetPos(255);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMainDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMainDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMainDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMainDlg::OnOK() 
{
	// TODO: Add extra validation here

	ShowWindow(SW_HIDE);
	//CDialog::OnOK();
}

void CMainDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	ShowWindow(SW_HIDE);
	
	//CDialog::OnCancel();
}

void CMainDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	int iCurPos = m_Slider.GetPos();	

	int nMax = m_Slider.GetRangeMax();
	int nMin = m_Slider.GetRangeMin();

	int nPageSize = m_Slider.GetPageSize();

	switch(nSBCode)
	{
	case SB_LINELEFT:
		if(iCurPos > nMin)
			iCurPos --;
		break;
	case SB_LINERIGHT:
		if(iCurPos < nMax)
			iCurPos ++;
		break;
	case SB_PAGELEFT:
		if(iCurPos > nMin)
			iCurPos = max(nMin,iCurPos - nPageSize);
		break;
	case SB_PAGERIGHT:
		if(iCurPos < nMax)
			iCurPos = min(nMax,iCurPos + nPageSize);
		break;
	case SB_THUMBTRACK:
		iCurPos = nPos;
		break;
	case SB_THUMBPOSITION:
		iCurPos = nPos;
		break;		
	}

	m_Slider.SetPos(iCurPos);

	pFloatWnd->OnUpdateTransparent(iCurPos);
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CMainDlg::OnBnClickedStartCmd()
{
	// TODO: 在此添加控件通知处理程序代码
	bool flag = m_debug.get_console_flag();
	if (!flag)
	{
		m_debug.init_debug_env();
	}
}


void CMainDlg::OnBnClickedStopCmd()
{
	// TODO: 在此添加控件通知处理程序代码
	bool flag = m_debug.get_console_flag();
	if (flag)
	{
		m_debug.release_debug_env();
	}
}


void CMainDlg::OnBnClickedRunRec()
{
	long pParam = NULL;
	PThread_vsBase mpthread;
	mpthread.Start_PThread(&pParam);
	printf("============ END =============\n");
}




void CMainDlg::OnBnClickedImgproc()
{
#if 1
	//ImgProc m_imgproc;
	//m_imgproc.load_img_proc(NULL);
	CStatic* pWnd = (CStatic*)GetDlgItem(IDC_PIC_CTRL); // 得到 Picture Control 句柄
	pWnd->ModifyStyle(0, SS_BITMAP); // 修改它的属性为位图
	pWnd->SetBitmap((HBITMAP)::LoadImage(NULL, _T("load_img.bmp"),
		IMAGE_BITMAP,
		720, 160,
		LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE));
#endif
}


void CMainDlg::OnBnClickedRefrash()
{
	CStatic* pWnd = (CStatic*)GetDlgItem(IDC_PIC_CTRL); // 得到 Picture Control 句柄
	CImage image;
	image.Load(_T("load_img.jpg"));
	HBITMAP hBmp = image.Detach();
	//pWnd->SetBitmap(hBmp);
	//pWnd->SetWindowPos(NULL, 0, 0, 720, 160, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
}
