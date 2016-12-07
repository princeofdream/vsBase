// AdbStat.cpp : 实现文件
//

#include "stdafx.h"
#include "FactoryTools.h"
#include "AdbStat.h"
#include "afxdialogex.h"


// AdbStat 对话框

IMPLEMENT_DYNAMIC(AdbStat, CDialogEx)

AdbStat::AdbStat(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ADBSTAT, pParent)
{
	m_serial_number = "";
}

AdbStat::~AdbStat()
{
}

void AdbStat::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(AdbStat, CDialogEx)
	ON_BN_CLICKED(IDOK, &AdbStat::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &AdbStat::OnBnClickedCancel)
END_MESSAGE_MAP()


// AdbStat 消息处理程序


void AdbStat::OnBnClickedOk()
{
	char m_sn[1024];
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
	memset(m_sn, 0x0, sizeof(m_sn));
	GetDlgItem(IDC_SERIAL_NUMBER)->GetWindowText(m_sn, sizeof(m_sn));
	m_serial_number = _T(m_sn);
	
	this->DestroyWindow();
}


void AdbStat::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
	this->DestroyWindow();
}


CString AdbStat::get_serial_number()
{
	return m_serial_number;
}

void AdbStat::set_serial_number_empty()
{
	m_serial_number = "";
}