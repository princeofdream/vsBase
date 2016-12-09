// AdbStat.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FactoryTools.h"
#include "AdbStat.h"
#include "afxdialogex.h"


// AdbStat �Ի���

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


// AdbStat ��Ϣ�������


void AdbStat::OnBnClickedOk()
{
	TCHAR m_sn[1024];
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnOK();
	memset(m_sn, 0x0, sizeof(m_sn));
	GetDlgItem(IDC_SERIAL_NUMBER)->GetWindowText(m_sn, sizeof(m_sn));
	m_serial_number = m_sn;
	
	this->DestroyWindow();
}


void AdbStat::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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