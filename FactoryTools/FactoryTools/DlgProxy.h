
// DlgProxy.h: ͷ�ļ�
//

#pragma once

class CFactoryToolsDlg;


// CFactoryToolsDlgAutoProxy ����Ŀ��

class CFactoryToolsDlgAutoProxy : public CCmdTarget
{
	DECLARE_DYNCREATE(CFactoryToolsDlgAutoProxy)

	CFactoryToolsDlgAutoProxy();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��

// ����
public:
	CFactoryToolsDlg* m_pDialog;

// ����
public:

// ��д
	public:
	virtual void OnFinalRelease();

// ʵ��
protected:
	virtual ~CFactoryToolsDlgAutoProxy();

	// ���ɵ���Ϣӳ�亯��

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CFactoryToolsDlgAutoProxy)

	// ���ɵ� OLE ����ӳ�亯��

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

