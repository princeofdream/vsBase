#pragma once


// AdbStat 对话框

class AdbStat : public CDialogEx
{
	DECLARE_DYNAMIC(AdbStat)

public:
	AdbStat(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~AdbStat();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADBSTAT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
