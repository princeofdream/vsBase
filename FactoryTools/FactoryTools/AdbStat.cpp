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
	ON_EN_CHANGE(IDC_SERIAL_NUMBER, &AdbStat::OnChangeSerialNumber)
END_MESSAGE_MAP()


// AdbStat 消息处理程序


void AdbStat::OnBnClickedOk()
{
	TCHAR m_sn[1024];
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
	memset(m_sn, 0x0, sizeof(m_sn));
	GetDlgItem(IDC_SERIAL_NUMBER)->GetWindowText(m_sn, sizeof(m_sn));
	m_serial_number = m_sn;
	
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

void AdbStat::OnChangeSerialNumber()
{
	CString str_tmp;
	GetDlgItemText(IDC_SERIAL_NUMBER, str_tmp);

	str_tmp.TrimLeft();
	str_tmp.TrimRight();



	if (str_tmp.GetLength() ==15)
	{
		m_serial_number = str_tmp;
		OnOK();
	}
}


BOOL AdbStat::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	this->GetDlgItem(IDC_SERIAL_NUMBER)->SetFocus();
	((CEdit*)(GetDlgItem(IDC_SERIAL_NUMBER)))->SetSel(0, -1);

	return FALSE;
}
