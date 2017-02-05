// vsBaseDlg.h : header file
//

#if !defined(AFX_MAINDLG_H__9DE2C7F5_0D02_48F7_A1D0_89D7626F1323__INCLUDED_)
#define AFX_MAINDLG_H__9DE2C7F5_0D02_48F7_A1D0_89D7626F1323__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMainDlg dialog

#include <FloatWnd.h>
#include <PThread_vsBase.h>
#include <ProcessManager.h>
#include <ImgProc.h>
#include <ImageArea.h>
#include <vector>



//extern UINT __cdecl AfxThreadProc(LPVOID pParam);

#define	THUMBNAIL_WIDTH		32
#define	THUMBNAIL_HEIGHT	32



class CMainDlg : public CDialog
{
// Construction
public:
	CMainDlg(CWnd* pParent = NULL);	// standard constructor
	~CMainDlg();

// Dialog Data
	//{{AFX_DATA(CMainDlg)
	enum { IDD = IDD_MAIN_DIALOG };
	CSliderCtrl	m_Slider;
	//}}AFX_DATA

	int iTransparent;

	CFloatWnd *pFloatWnd;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMainDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStartCmd();
	afx_msg void OnBnClickedStopCmd();
	afx_msg void OnBnClickedRunRec();
	afx_msg void OnBnClickedImgproc();
	afx_msg void OnBnClickedRefrash();
	afx_msg void OnBnClickedBrowserPhotoPath();

public:
#ifdef ENABLE_GDI_PLUS_IMAGE_DRAWING
	ULONG_PTR	m_gdiplusToken;
#endif
	CString	m_strImageDir;
	CListCtrl	m_ListThumbnail;
	CImageArea	m_ImageRect;

	CImageList				m_ImageListThumb;		// image list holding the thumbnails
	std::vector<CString>	m_VectorImageNames;		// vector holding the image names
	int						m_nSelectedItem;

public:
	void  DrawSelectedImage();
	BOOL  GetImageFileNames();
	void  DrawThumbnails();

	afx_msg void OnBnClickedLoadPhotos();
	afx_msg void OnLvnKeydownListThumb(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListThumb(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINDLG_H__9DE2C7F5_0D02_48F7_A1D0_89D7626F1323__INCLUDED_)
