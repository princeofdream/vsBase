
// FactoryToolsDlg.h : 头文件
//

#pragma once
#include "AdbStat.h"
#include "ControlCenter.h"
#include "ConfigUtility.h"

class CFactoryToolsDlgAutoProxy;


// CFactoryToolsDlg 对话框
class CFactoryToolsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFactoryToolsDlg);
	friend class CFactoryToolsDlgAutoProxy;

// 构造
public:
	CFactoryToolsDlg(CWnd* pParent = NULL);	// 标准构造函数
	virtual ~CFactoryToolsDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FACTORYTOOLS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	CFactoryToolsDlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;

	BOOL CanExit();

	// 生成的消息映射函数
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
	afx_msg void OnBnClickedClearTest();
	BOOL StartBurn();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnMyDeviceChange(WPARAM wParam, LPARAM lParam);
};

static const GUID GUID_DEVINTERFACE_LIST[] =
{
	// GUID_DEVINTERFACE_USB_DEVICE
	{ 0xA5DCBF10, 0x6530, 0x11D2,{ 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } },

	// GUID_DEVINTERFACE_DISK
	{ 0x53f56307, 0xb6bf, 0x11d0,{ 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } },

	// GUID_DEVINTERFACE_HID, 
	{ 0x4D1E55B2, 0xF16F, 0x11CF,{ 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } },

	// GUID_NDIS_LAN_CLASS
	{ 0xad498944, 0x762f, 0x11d0,{ 0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c } }

	//// GUID_DEVINTERFACE_COMPORT
	//{ 0x86e0d1e0, 0x8089, 0x11d0, { 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73 } },

	//// GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR
	//{ 0x4D36E978, 0xE325, 0x11CE, { 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18 } },

	//// GUID_DEVINTERFACE_PARALLEL
	//{ 0x97F76EF0, 0xF883, 0x11D0, { 0xAF, 0x1F, 0x00, 0x00, 0xF8, 0x00, 0x84, 0x5C } },

	//// GUID_DEVINTERFACE_PARCLASS
	//{ 0x811FC6A5, 0xF728, 0x11D0, { 0xA5, 0x37, 0x00, 0x00, 0xF8, 0x75, 0x3E, 0xD1 } }
};


