
// DlgProxy.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FactoryTools.h"
#include "DlgProxy.h"
#include "FactoryToolsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFactoryToolsDlgAutoProxy

IMPLEMENT_DYNCREATE(CFactoryToolsDlgAutoProxy, CCmdTarget)

CFactoryToolsDlgAutoProxy::CFactoryToolsDlgAutoProxy()
{
	EnableAutomation();
	
	// ΪʹӦ�ó������Զ��������ڻ״̬ʱһֱ���� 
	//	���У����캯������ AfxOleLockApp��
	AfxOleLockApp();

	// ͨ��Ӧ�ó����������ָ��
	//  �����ʶԻ���  ���ô�����ڲ�ָ��
	//  ָ��Ի��򣬲����öԻ���ĺ���ָ��ָ��
	//  �ô���
	ASSERT_VALID(AfxGetApp()->m_pMainWnd);
	if (AfxGetApp()->m_pMainWnd)
	{
		ASSERT_KINDOF(CFactoryToolsDlg, AfxGetApp()->m_pMainWnd);
		if (AfxGetApp()->m_pMainWnd->IsKindOf(RUNTIME_CLASS(CFactoryToolsDlg)))
		{
			m_pDialog = reinterpret_cast<CFactoryToolsDlg*>(AfxGetApp()->m_pMainWnd);
			m_pDialog->m_pAutoProxy = this;
		}
	}
}

CFactoryToolsDlgAutoProxy::~CFactoryToolsDlgAutoProxy()
{
	// Ϊ������ OLE �Զ����������ж������ֹӦ�ó���
	//	������������ AfxOleUnlockApp��
	//  ���������������⣬�⻹���������Ի���
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void CFactoryToolsDlgAutoProxy::OnFinalRelease()
{
	// �ͷ��˶��Զ�����������һ�����ú󣬽�����
	// OnFinalRelease��  ���ཫ�Զ�
	// ɾ���ö���  �ڵ��øû���֮ǰ�����������
	// ��������ĸ���������롣

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CFactoryToolsDlgAutoProxy, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CFactoryToolsDlgAutoProxy, CCmdTarget)
END_DISPATCH_MAP()

// ע��: ��������˶� IID_IFactoryTools ��֧��
//  ��֧������ VBA �����Ͱ�ȫ�󶨡�  �� IID ����ͬ���ӵ� .IDL �ļ��е�
//  ���Ƚӿڵ� GUID ƥ�䡣

// {E62A0629-2E37-4A17-AF37-1308C57E7B20}
static const IID IID_IFactoryTools =
{ 0xE62A0629, 0x2E37, 0x4A17, { 0xAF, 0x37, 0x13, 0x8, 0xC5, 0x7E, 0x7B, 0x20 } };

BEGIN_INTERFACE_MAP(CFactoryToolsDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CFactoryToolsDlgAutoProxy, IID_IFactoryTools, Dispatch)
END_INTERFACE_MAP()

// IMPLEMENT_OLECREATE2 ���ڴ���Ŀ�� StdAfx.h �ж���
// {4EB704E8-412E-439F-9572-3535FB1EE89C}
IMPLEMENT_OLECREATE2(CFactoryToolsDlgAutoProxy, "FactoryTools.Application", 0x4eb704e8, 0x412e, 0x439f, 0x95, 0x72, 0x35, 0x35, 0xfb, 0x1e, 0xe8, 0x9c)


// CFactoryToolsDlgAutoProxy ��Ϣ�������
