// vsBase.h : main header file for the MAIN application
//

#if !defined(AFX_MAIN_H__1F10FC0D_E07A_42AD_95DC_DF7B9C129A80__INCLUDED_)
#define AFX_MAIN_H__1F10FC0D_E07A_42AD_95DC_DF7B9C129A80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#define ST_DEBUG
#ifdef ST_DEBUG
#include "st_debug.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainApp:
// See vsBase.cpp for the implementation of this class
//

class CMainApp : public CWinApp
{
public:
	CMainApp();
	~CMainApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMainApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAIN_H__1F10FC0D_E07A_42AD_95DC_DF7B9C129A80__INCLUDED_)
