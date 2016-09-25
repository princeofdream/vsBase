#pragma once
#include "afxwin.h"
#include "ProcessManager.h"
#include "CapScreenAndAudio.h"

UINT __cdecl ThreadProc(LPVOID pParam);

class PThread_Main :
	public CWinThread
{
public:
	PThread_Main();
	~PThread_Main();

public:
	UINT Start_PThread(long* pParam);

};

