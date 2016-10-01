; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CFloatWnd
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "vsBase.h"

ClassCount=4
Class1=CMainApp
Class2=CMainDlg
Class3=CAboutDlg

ResourceCount=5
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_FLOATWND
Class4=CFloatWnd
Resource4=IDD_MAIN_DIALOG
Resource5=IDR_MENU1

[CLS:CMainApp]
Type=0
HeaderFile=vsBase.h
ImplementationFile=vsBase.cpp
Filter=N

[CLS:CMainDlg]
Type=0
HeaderFile=vsBaseDlg.h
ImplementationFile=vsBaseDlg.cpp
Filter=D
LastObject=CMainDlg
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=vsBaseDlg.h
ImplementationFile=vsBaseDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_MAIN_DIALOG]
Type=1
Class=CMainDlg
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_SLIDER1,msctls_trackbar32,1342242840

[DLG:IDD_FLOATWND]
Type=1
Class=CFloatWnd
ControlCount=1
Control1=IDC_LOGO,static,1342177294

[CLS:CFloatWnd]
Type=0
HeaderFile=FloatWnd.h
ImplementationFile=FloatWnd.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CFloatWnd

[MNU:IDR_MENU1]
Type=1
Class=CFloatWnd
Command1=ID_SHOW
Command2=ID_EXIT
CommandCount=2

