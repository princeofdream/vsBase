
// FactoryToolsDlg.h : 头文件
//

#pragma once

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
};
