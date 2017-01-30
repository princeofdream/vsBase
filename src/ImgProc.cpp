#include "stdafx.h"
#include <ImgProc.h>


ImgProc::ImgProc()
{
	m_stat = 0;
}


ImgProc::~ImgProc()
{
}



UINT ImgProc::load_img_proc(long* argc)
{
	CStatic* pWnd = (CStatic*)GetDlgItem(IDC_PIC_CTRL); // 得到 Picture Control 句柄
	CImage image;
	image.Load(_T("logo.png"));
	HBITMAP hBmp = image.Detach();
	pWnd->SetBitmap(hBmp);
	pWnd->SetWindowPos(NULL,0,0,96,96, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
#if 0
	CStatic* pWnd = (CStatic*)GetDlgItem(IDC_PIC_CTRL); // 得到 Picture Control 句柄
	pWnd->ModifyStyle(0, SS_BITMAP); // 修改它的属性为位图
	pWnd->SetBitmap((HBITMAP)::LoadImage(NULL, _T("logo.bmp"),
		IMAGE_BITMAP,
		96,96,
		LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE));
#endif
	return 0;
}

UINT ImgProc::check_img_proc(long* argc)
{
	return 0;
}

