#pragma once


// AdbStat �Ի���

class AdbStat : public CDialogEx
{
	DECLARE_DYNAMIC(AdbStat)

public:
	AdbStat(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~AdbStat();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADBSTAT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	CString m_serial_number;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CString get_serial_number();
	void set_serial_number_empty();
	afx_msg void OnChangeSerialNumber();
	virtual BOOL OnInitDialog();
};
