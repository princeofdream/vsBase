#pragma once
#include "afxwin.h"
#include <ProcessManager.h>
#include <CapScreenAndAudio.h>
#include <node_js.h>

UINT __cdecl ThreadProc(LPVOID pParam);

class PThread_vsBase :
	public CWinThread
{
public:
	PThread_vsBase();
	~PThread_vsBase();

public:
	UINT Start_PThread(long* pParam);
	UINT Check_Pthread(long* pParam);

};

