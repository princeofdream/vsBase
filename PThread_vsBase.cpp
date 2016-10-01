#include "stdafx.h"
#include "PThread_vsBase.h"


PThread_vsBase::PThread_vsBase()
{
}


PThread_vsBase::~PThread_vsBase()
{
}



UINT __cdecl ThreadProc(LPVOID pParam)
{
	int i0 = 0;
	int ret = 0;
	CString * pStr = (CString*)pParam;
	//*pStr = _T("Hello World!");
#if 0
	while (true)
	{
		printf("Thread test loop: %d.\n", i0);
		i0++;
		if (i0 > 20)
			break;
		Sleep(500);
	}
#else
	//ProcessManager mproc;
	//mproc.Start_New_Process("ping", "127.0.0.1", "-n 10", NULL);
	printf("=================== Start Rec ================\n");
#if 1
	Start_Rec(0, NULL);
#else
	Start_SDL_Rec(0, NULL);
#endif
	printf("=================== Stop Rec ================\n");
#endif
	AfxEndThread(ret,TRUE/*是否删除现成所占用的内存*/ ); //提前退出线程函数
	return 0;
}



UINT PThread_vsBase::Start_PThread(long* pParam)
{
#if 1	//thread test
	CWinThread *pThread = NULL;
	CString strArg = _T("");

	pThread = AfxBeginThread(
		ThreadProc,         //线程启动函数
		&strArg,          //线程启动函数
		THREAD_PRIORITY_NORMAL,       //线程优先级
		0,            //Windows系统一般线程栈大小为1 MB，创建线程的数目与物理内存和栈空间大小有关
		0,            //线程创建标志，如：CREATE_SUSPENDED
		NULL);           //系统安全描述，NULL

	if (pThread)
	{
		printf("Start PThread Sucess!");
		//pThread->m_bAutoDelete = TRUE;     //当线程结束是自动清除线程对象,默认是TRUE
		//WaitForSingleObject(pThread->m_hThread, INFINITE); //等待线程结束
														   //AfxMessageBox(strArg);
	}
#endif
	return 0;
}


