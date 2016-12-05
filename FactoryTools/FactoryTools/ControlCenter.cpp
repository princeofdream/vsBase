#include "stdafx.h"
#include "ControlCenter.h"

char m_info[1024];

DWORD __stdcall ThreadAdbshellProc(void *pVoid)
{
	if (pVoid != NULL) {
		ControlCenter *pShell = (ControlCenter *)pVoid;
		pShell->Loop();
	}
	return 0;
}


ControlCenter::ControlCenter()
{
}


ControlCenter::~ControlCenter()
{
}


bool ControlCenter::StartCommand(char* m_cmd)
{
	m_strOutput.Empty();
	SECURITY_ATTRIBUTES sat;
	STARTUPINFO startupinfo;
	PROCESS_INFORMATION pinfo;

	sat.nLength = sizeof(SECURITY_ATTRIBUTES);
	sat.bInheritHandle = TRUE;
	sat.lpSecurityDescriptor = NULL;
	if (CreatePipe(&m_hReadPipe, &m_hWritePipe, &sat, NULL) == FALSE) {
		return FALSE;
	}
	if (CreatePipe(&m_hReadPipe2, &m_hWritePipe2, &sat, NULL) == FALSE) {
		return FALSE;
	}
	startupinfo.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&startupinfo);
	startupinfo.hStdError = m_hWritePipe;
	startupinfo.hStdOutput = m_hWritePipe;
	startupinfo.hStdInput = m_hReadPipe2;
	startupinfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	startupinfo.wShowWindow = SW_HIDE;

	TCHAR command[1024] = _T("adb.exe shell");

	if (CreateProcess(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &startupinfo, &pinfo) == FALSE)
	{
		return FALSE;
	}
	CloseHandle(m_hWritePipe);
	CloseHandle(m_hReadPipe2);
	CloseHandle(pinfo.hThread);
	CloseHandle(pinfo.hProcess);
	//m_hProcess = pinfo.hProcess;  

	DWORD dwThread = FALSE;
	hThread = CreateThread(NULL, 0, ThreadAdbshellProc, this, 0, &dwThread);//�����������������Ҫ����Ҫ��Ҫ  
	if (hThread == NULL) {
		return FALSE;
	}

	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	return TRUE;
}

bool ControlCenter::Loop()
{
	CHAR buffer[1024] = { 0 };
	DWORD dwRead = 0;

	memset(m_info, 0x0, sizeof(m_info));
	sprintf_s(m_info, "---<%s:%d>---\n", __func__, __LINE__);
	TRACE(_T(m_info));
	while (TRUE)
	{
		RtlZeroMemory(buffer, _countof(buffer));
		SetEvent(m_hEvent);

		if (ReadFile(m_hReadPipe, buffer, _countof(buffer), &dwRead, NULL) == FALSE)
		{
			break;
		}
		else
		{
			m_strOutput += buffer;
			if (StrStrA(buffer, "# exit") != NULL)
			{
				break;
			}
		}
	}

	memset(m_info, 0x0, sizeof(m_info));
	sprintf_s(m_info, "---<%s:%d>---\n", __func__, __LINE__);
	TRACE(_T(m_info));

	CloseHandle(m_hReadPipe);
	CloseHandle(m_hWritePipe2);

	CloseHandle(m_hEvent);
	m_hEvent = NULL;
	CloseHandle(hThread);
	hThread = NULL;

	
	memset(m_info,0x0,sizeof(m_info));
	sprintf_s(m_info,"---<%s:%d>---\n",__func__,__LINE__);
	TRACE(_T(m_info));
	TRACE(m_strOutput);
	memset(m_info, 0x0, sizeof(m_info));
	sprintf_s(m_info, "---<%s:%d>---\n", __func__, __LINE__);
	TRACE(_T(m_info));

	return TRUE;
}



//�������������adb shell״̬�µ��������������ж�������\n�ָ�  
bool ControlCenter::RunCmd(const CString& strCmdline)
{
	BOOL bSuccess = TRUE;
	CString strOneCmd;
	int nPos1 = 0;
	int nPos2 = 0;
	DWORD dwWrite = 0;

	if (strCmdline.GetLength() < 2) {
		return FALSE;
	}
	//�ȵȴ��̺߳���׼����  
	WaitForSingleObject(m_hEvent, INFINITE);

	while (TRUE) {
		nPos1 = nPos2;
		nPos2 = strCmdline.Find('\n', nPos1);
		if (nPos2 == -1) {
			nPos2 = strCmdline.GetLength();
		}
		strOneCmd = strCmdline.Mid(nPos1, nPos2 - nPos1).Trim();

		//���������Ϊ2  
		if (strOneCmd.GetLength() >= 2) {
			strOneCmd += "\r\n";
			//ִ��  
			bSuccess = WriteFile(m_hWritePipe2, strOneCmd, strOneCmd.GetLength(), &dwWrite, NULL);
		}

		++nPos2;
		if (nPos2 >= strCmdline.GetLength()) {
			break;
		}
	}

	return bSuccess;
}


CString ControlCenter::GetOutput()
{
	//WaitForSingleObject(hThread, INFINITE);
	memset(m_info, 0x0, sizeof(m_info));
	sprintf_s(m_info, "---<%s:%d>---\n", __func__, __LINE__);
	TRACE(_T(m_info));
	TRACE(m_strOutput);
	memset(m_info, 0x0, sizeof(m_info));
	sprintf_s(m_info, "---<%s:%d>---\n", __func__, __LINE__);
	TRACE(_T(m_info));

	return m_strOutput;
}