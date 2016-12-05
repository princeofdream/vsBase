
// FactoryToolsDlg.h : ͷ�ļ�
//

#pragma once

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
};
