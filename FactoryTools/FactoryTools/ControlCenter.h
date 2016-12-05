#pragma once
class ControlCenter
{
public:
	ControlCenter();
	~ControlCenter();

public:
	bool Loop();
	bool StartCommand(char* m_cmd);
	bool RunCmd(const CString& strCmdline);
	CString GetOutput();

protected:
	
	

private:
	CString m_strOutput;

	HANDLE  hThread;

	HANDLE  m_hEvent;

	HANDLE  m_hReadPipe;
	HANDLE  m_hReadPipe2;
	HANDLE  m_hWritePipe;
	HANDLE  m_hWritePipe2;

};

