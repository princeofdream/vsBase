#include "stdafx.h"
#include <ProcessManager.h>


ProcessManager::ProcessManager()
{
}


ProcessManager::~ProcessManager()
{
}

UINT ProcessManager::Start_New_Process(char* m_cmd, char* arg_main, char* arg_sec, long* pParam)
{
#if 1	// transfer exe
	char cmd_full[1024];
	SECURITY_ATTRIBUTES sa;
	HANDLE hRead, hWrite;


	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
		printf("Error in creat pipe!\n");
		return -1;
	}

	mStartupInfo.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&mStartupInfo);
	mStartupInfo.hStdError = hWrite;
	mStartupInfo.hStdOutput = hWrite;
	mStartupInfo.wShowWindow = SW_HIDE;
	mStartupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	memset(cmd_full, 0x0, sizeof(cmd_full));
	sprintf(cmd_full,"%s %s %s",m_cmd, arg_main,arg_sec);

	if (CreateProcess(NULL, cmd_full
		, NULL, NULL, TRUE, NULL, NULL, NULL, &mStartupInfo, &mProcessInfo))
	{
		if (pParam != NULL)
		{
			char *get_param = (char*)pParam;
			if (strcmpi(get_param, "wait_for_single") == 0)
			{
				WaitForSingleObject(mProcessInfo.hProcess, INFINITE);
				CloseHandle(mProcessInfo.hThread);
				CloseHandle(mProcessInfo.hProcess);
			}
		}
	}
	else
	{
		printf("Error in creat process!\n");
		return -1;
	}
	CloseHandle(hWrite);


	if (pParam != NULL)
	{
		char *get_param = (char*)pParam;
		if (strcmpi(get_param,"read") == 0)
		{
			char buffer[4096] = { 0 };
			DWORD bytesRead;

			printf("++++[%s:%d]++++\n",__func__,__LINE__);
			while (true) {
				if (ReadFile(hRead, buffer, 4095, &bytesRead, NULL) == NULL)
					break;
				printf("%s", buffer);
				memset(buffer, 0x0, sizeof(buffer));
			}
		}
	}

#endif
	return 0;
}
