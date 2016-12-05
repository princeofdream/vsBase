
// DlgProxy.cpp : 实现文件
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
	
	// 为使应用程序在自动化对象处于活动状态时一直保持 
	//	运行，构造函数调用 AfxOleLockApp。
	AfxOleLockApp();

	// 通过应用程序的主窗口指针
	//  来访问对话框。  设置代理的内部指针
	//  指向对话框，并设置对话框的后向指针指向
	//  该代理。
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
	// 为了在用 OLE 自动化创建所有对象后终止应用程序，
	//	析构函数调用 AfxOleUnlockApp。
	//  除了做其他事情外，这还将销毁主对话框
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void CFactoryToolsDlgAutoProxy::OnFinalRelease()
{
	// 释放了对自动化对象的最后一个引用后，将调用
	// OnFinalRelease。  基类将自动
	// 删除该对象。  在调用该基类之前，请添加您的
	// 对象所需的附加清理代码。

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CFactoryToolsDlgAutoProxy, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CFactoryToolsDlgAutoProxy, CCmdTarget)
END_DISPATCH_MAP()

// 注意: 我们添加了对 IID_IFactoryTools 的支持
//  以支持来自 VBA 的类型安全绑定。  此 IID 必须同附加到 .IDL 文件中的
//  调度接口的 GUID 匹配。

// {E62A0629-2E37-4A17-AF37-1308C57E7B20}
static const IID IID_IFactoryTools =
{ 0xE62A0629, 0x2E37, 0x4A17, { 0xAF, 0x37, 0x13, 0x8, 0xC5, 0x7E, 0x7B, 0x20 } };

BEGIN_INTERFACE_MAP(CFactoryToolsDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CFactoryToolsDlgAutoProxy, IID_IFactoryTools, Dispatch)
END_INTERFACE_MAP()

// IMPLEMENT_OLECREATE2 宏在此项目的 StdAfx.h 中定义
// {4EB704E8-412E-439F-9572-3535FB1EE89C}
IMPLEMENT_OLECREATE2(CFactoryToolsDlgAutoProxy, "FactoryTools.Application", 0x4eb704e8, 0x412e, 0x439f, 0x95, 0x72, 0x35, 0x35, 0xfb, 0x1e, 0xe8, 0x9c)


// CFactoryToolsDlgAutoProxy 消息处理程序
