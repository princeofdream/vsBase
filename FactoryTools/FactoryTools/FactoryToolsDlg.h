
// FactoryToolsDlg.h : ͷ�ļ�
//

#pragma once
#include "AdbStat.h"
#include "ControlCenter.h"
#include "ConfigUtility.h"

class CFactoryToolsDlgAutoProxy;


// CFactoryToolsDlg �Ի���
class CFactoryToolsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFactoryToolsDlg);
	friend class CFactoryToolsDlgAutoProxy;

// ����
public:
	CFactoryToolsDlg(CWnd* pParent = NULL);	// ��׼���캯��
	virtual ~CFactoryToolsDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FACTORYTOOLS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	CFactoryToolsDlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;

	BOOL CanExit();

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()

	AdbStat m_adbstat;
	ControlCenter m_ctrlcent;
	ConfigUtility m_confutil;
	HANDLE  hThread;
	CString m_output_msg;
	CString m_tmp;

public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedCheckAdb();
	void CheckAdbStat();
	afx_msg void OnBnClickedStartBurn();
	afx_msg void OnBnClickedRunCmd();
	bool Loop();
	afx_msg void OnBnClickedRecheckStat();
	afx_msg void OnBnClickedAndroidTest();
	afx_msg void OnBnClickedPlayMusic();
	afx_msg void OnBnClickedRebootToRecovery();
	afx_msg void OnBnClickedButton1();
};
