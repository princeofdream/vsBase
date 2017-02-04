// vsBaseDlg.cpp : implementation file
//

#include "stdafx.h"
#include <vsBase.h>
#include <vsBaseDlg.h>
#include <PathBrowser.h>

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
	m_strImageDir = _T("");
}

CMainDlg::~CMainDlg()
{
	bool flag = m_debug.get_console_flag();
	if (flag)
	{
		m_debug.release_debug_env();
	}

	GdiplusShutdown(m_gdiplusToken);

}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMainDlg)
	DDX_Control(pDX, IDC_SLIDER1, m_Slider);
	DDX_Control(pDX, IDC_LIST_THUMB, m_ListThumbnail);
	DDX_Control(pDX, IDC_IMAGE_RECT, m_ImageRect);
	DDX_Text(pDX, IDC_PATH_INFO, m_strImageDir);
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
	ON_BN_CLICKED(IDC_BROWSER_PHOTO_PATH, &CMainDlg::OnBnClickedBrowserPhotoPath)
	ON_BN_CLICKED(IDC_LOAD_PHOTOS, &CMainDlg::OnBnClickedLoadPhotos)
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
	
#if 1
	// initialize GDI+
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	printf("-----------------------------------------------\n");
	TRACE("++++++++++++++++++++++++++++++++++++++++++++\n\n");
#endif

#if 1 //for thumbnail
	m_ImageListThumb.Create(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, ILC_COLOR24, 0, 1);
	m_ListThumbnail.SetImageList(&m_ImageListThumb, LVSIL_NORMAL);
	m_nSelectedItem = 0;
#endif


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
	pWnd->SetBitmap(hBmp);
	pWnd->SetWindowPos(NULL, 0, 0, 720, 160, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
}


void CMainDlg::OnBnClickedBrowserPhotoPath()
{
	// TODO: Add your control notification handler code here
	CPathDialog  dlg(TEXT("Folder Selection"),
		TEXT("Select Image Directory to Browse"),
		m_strImageDir,
		this);

	// show path dialog
	if (dlg.DoModal() == IDOK)
	{
		CString strPath = dlg.GetPathName();
		if (strPath != m_strImageDir)
		{
			m_strImageDir = strPath;
			UpdateData(FALSE);
		}
		printf("Get path: %s",strPath);
	}
}


void  CMainDlg::DrawSelectedImage()
{
#if 1
	CString		strPath;
	Rect		DesRect;
	RECT		clRect;

	// read the image file
	if (m_strImageDir.Right(1) == TEXT("\\"))
		strPath.Format(TEXT("%s%s"), m_strImageDir, m_VectorImageNames[m_nSelectedItem]);
	else
		strPath.Format(TEXT("%s\\%s"), m_strImageDir, m_VectorImageNames[m_nSelectedItem]);

	USES_CONVERSION;
	Image img(A2W(strPath));

	// get destination rectangle
	m_ImageRect.GetClientRect(&clRect);
	DesRect.X = clRect.left;
	DesRect.Y = clRect.top;
	DesRect.Width = clRect.right - clRect.left;
	DesRect.Height = clRect.bottom - clRect.top;

	// draw the image
	Graphics gc(m_ImageRect.GetDC()->GetSafeHdc());
	gc.DrawImage(&img, DesRect);
#endif
}



BOOL  CMainDlg::GetImageFileNames()
{
	CString	strExt;
	CString	strName;
	CString	strPattern;
	BOOL	bRC = TRUE;

	HANDLE					hFind = NULL;
	WIN32_FIND_DATA			FindFileData;
	std::vector<CString>	VectorImageNames;

	if (m_strImageDir[m_strImageDir.GetLength() - 1] == TCHAR('\\'))
		strPattern.Format(TEXT("%s*.*"), m_strImageDir);
	else
		strPattern.Format(TEXT("%s\\*.*"), m_strImageDir);

	hFind = ::FindFirstFile(strPattern, &FindFileData);	// strat search	
	if (hFind == INVALID_HANDLE_VALUE)
	{
		LPVOID  msg;
		::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&msg,
			0,
			NULL);
#if 0
		MessageBox((LPTSTR)msg, CString((LPCSTR)IDS_TITLE), MB_OK | MB_ICONSTOP);
#endif
		::LocalFree(msg);
		return FALSE;
	}

	// filter off the system files and directories
	if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
		!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) &&
		!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) &&
		!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY))
	{
		// test file extension
		strName = FindFileData.cFileName;
		strExt = strName.Right(3);

		if ((strExt.CompareNoCase(TEXT("bmp")) == 0) ||
			(strExt.CompareNoCase(TEXT("jpg")) == 0) ||
			(strExt.CompareNoCase(TEXT("gif")) == 0) ||
			(strExt.CompareNoCase(TEXT("tif")) == 0) ||
			(strExt.CompareNoCase(TEXT("png")) == 0))
		{
			// save the image file name
			VectorImageNames.push_back(strName);
		}
	}

	printf("==== [%s:%d] ====\n", __FILE__, __LINE__);
	// loop through to add all of them to our vector	
	while (bRC)
	{
		bRC = ::FindNextFile(hFind, &FindFileData);
		if (bRC)
		{
			// filter off the system files and directories
			if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
				!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) &&
				!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) &&
				!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY))
			{
				// test file extension
				strName = FindFileData.cFileName;
				strExt = strName.Right(3);

				if ((strExt.CompareNoCase(TEXT("bmp")) == 0) ||
					(strExt.CompareNoCase(TEXT("jpg")) == 0) ||
					(strExt.CompareNoCase(TEXT("gif")) == 0) ||
					(strExt.CompareNoCase(TEXT("tif")) == 0) ||
					(strExt.CompareNoCase(TEXT("png")) == 0))
				{
					// save the image file name
					VectorImageNames.push_back(strName);
				}
			}
		}
		else
		{
			DWORD err = ::GetLastError();
			if (err != ERROR_NO_MORE_FILES)
			{
				LPVOID msg;
				::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
					NULL, err,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(LPTSTR)&msg, 0, NULL);
#if 0
				MessageBox((LPTSTR)msg, CString((LPCSTR)IDS_TITLE), MB_OK | MB_ICONSTOP);
#endif
				::LocalFree(msg);
				::FindClose(hFind);
				return FALSE;
			}
		}
	} // end of while loop

	printf("==== [%s:%d] ====\n", __FILE__, __LINE__);
	  // close the search handle
	::FindClose(hFind);

	// update the names, if any
	if (!VectorImageNames.empty())
	{
		printf("==== [%s:%d] ====\n", __FILE__, __LINE__);
		// reset the image name vector
		m_VectorImageNames.clear();
		m_VectorImageNames = VectorImageNames;
		return TRUE;
	}
	printf("==== [%s:%d] ====\n", __FILE__, __LINE__);

	return FALSE;
}

void CMainDlg::OnBnClickedLoadPhotos()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	// validate image directory
	if (m_strImageDir.IsEmpty())
	{
		printf("==== [%s:%d] ====\n",__FILE__,__LINE__);
#if 0
		MessageBox(CString((LPCSTR)IDS_DIR_ERROR), CString((LPCSTR)IDS_TITLE), MB_OK | MB_ICONSTOP);
#endif
		GetDlgItem(IDC_PATH_INFO)->SetFocus();
		return;
	}

	// show hour glass cursor
	BeginWaitCursor();

	// get the names of bitmap files
	if (!GetImageFileNames())
	{
		EndWaitCursor();
		return;
	}

	// draw thumbnail images in list control
	DrawThumbnails();

	// draw the selected image in its full size
	DrawSelectedImage();

	// set focus and select the first thumbnail in the list control
	m_ListThumbnail.SetFocus();
	m_ListThumbnail.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	EndWaitCursor();

}





void  CMainDlg::DrawThumbnails()
{
	CBitmap*    pImage = NULL;
	HBITMAP		hBmp = NULL;
	POINT		pt;
	CString		strPath;
	int			i;

	// no images
	if (m_VectorImageNames.empty())
		return;

	// set the length of the space between thumbnails
	// you can also calculate and set it based on the length of your list control
	int nGap = 6;

	// hold the window update to avoid flicking
	m_ListThumbnail.SetRedraw(FALSE);

	// reset our image list
	for (i = 0; i<m_ImageListThumb.GetImageCount(); i++)
		m_ImageListThumb.Remove(i);

	// remove all items from list view
	if (m_ListThumbnail.GetItemCount() != 0)
		m_ListThumbnail.DeleteAllItems();

	// set the size of the image list
	m_ImageListThumb.SetImageCount(m_VectorImageNames.size());
	i = 0;

	// draw the thumbnails
	std::vector<CString>::iterator	iter;

	for (iter = m_VectorImageNames.begin(); iter != m_VectorImageNames.end(); iter++)
	{
		// load the bitmap
		strPath.Format(TEXT("%s\\%s"), m_strImageDir, *iter);
		printf("==== [%s:%d] get path: %s ====\n", __FILE__, __LINE__, strPath);

		USES_CONVERSION;
#if 1	//Must start gdi first
		Bitmap img(A2W(strPath));
		Bitmap* pThumbnail = static_cast<Bitmap*>(img.GetThumbnailImage(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, NULL, NULL));
#else
		//Image img(A2W(strPath));
		Bitmap img(A2W("F:\\Work_Space\\Develop\\vsBase\\load_img.jpg"));
		Bitmap* pThumbnail = static_cast<Bitmap*>(img.GetThumbnailImage(40, 40, NULL, NULL));

		Bitmap img_new(A2W("F:\\Work_Space\\Develop\\vsBase\\load_img.jpg"));
		printf("==== [%s:%d] Image OK , get H: %d , get W: %d .====\n", __FILE__, __LINE__, img_new.GetHeight(), img_new.GetWidth());
#endif

		if (pThumbnail == NULL)
		{
			printf("==== [%s:%d] pThumbnail is NULL! ====\n", __FILE__, __LINE__);
			continue;
			//return;
		}

#if 1
		// attach the thumbnail bitmap handle to an CBitmap object
		pThumbnail->GetHBITMAP(NULL, &hBmp);
		pImage = new CBitmap();
		pImage->Attach(hBmp);

		// add bitmap to our image list
		m_ImageListThumb.Replace(i, pImage, NULL);

		// put item to display
		// set the image file name as item text
		m_ListThumbnail.InsertItem(i, m_VectorImageNames[i], i);

		// get current item position	 
		m_ListThumbnail.GetItemPosition(i, &pt);

		// shift the thumbnail to desired position
		pt.x = nGap + i*(THUMBNAIL_WIDTH + nGap);
		m_ListThumbnail.SetItemPosition(i, pt);
		i++;
#endif
		delete pImage;
		delete pThumbnail;
	}

	// let's show the new thumbnails
	m_ListThumbnail.SetRedraw();
}



