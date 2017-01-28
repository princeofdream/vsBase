#pragma once

#include <def.h>

class ProcessManager
{
public:
	ProcessManager();
	~ProcessManager();

public:
	UINT Start_New_Process(char* m_cmd, char* arg_main, char* arg_sec, long* pParam);

protected:
	STARTUPINFO mStartupInfo;
	PROCESS_INFORMATION mProcessInfo;

};

