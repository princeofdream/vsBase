#include "stdafx.h"
#include <PThread_vsBase.h>
#include <grabScreen.h>


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
#if 0
	Start_Rec(0, NULL);
#else
	Start_SDL_Rec(0, NULL);
#endif
	printf("=================== Stop Rec ================\n");
#endif
	AfxEndThread(ret,TRUE/*�Ƿ�ɾ���ֳ���ռ�õ��ڴ�*/ ); //��ǰ�˳��̺߳���
	return 0;
}



UINT PThread_vsBase::Start_PThread(long* pParam)
{
#if 1	//thread test
	CWinThread *pThread = NULL;
	CString strArg = _T("");

	pThread = AfxBeginThread(
		ThreadProc,         //�߳���������
		&strArg,          //�߳���������
		THREAD_PRIORITY_NORMAL,       //�߳����ȼ�
		0,            //Windowsϵͳһ���߳�ջ��СΪ1 MB�������̵߳���Ŀ�������ڴ��ջ�ռ��С�й�
		0,            //�̴߳�����־���磺CREATE_SUSPENDED
		NULL);           //ϵͳ��ȫ������NULL

	if (pThread)
	{
		printf("Start PThread Sucess!\n");
		//pThread->m_bAutoDelete = TRUE;     //���߳̽������Զ�����̶߳���,Ĭ����TRUE
		//WaitForSingleObject(pThread->m_hThread, INFINITE); //�ȴ��߳̽���
														   //AfxMessageBox(strArg);
	}
#endif
	return 0;
}


