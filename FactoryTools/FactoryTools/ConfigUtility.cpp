#include "stdafx.h"
#include "ConfigUtility.h"




ConfigUtility::ConfigUtility()
{
}


ConfigUtility::~ConfigUtility()
{
}


bool ConfigUtility::check_machine_config_stat()
{
	m_ctrlcent.StartSingleCommand("");
	return TRUE;
}

CString ConfigUtility::start_burn_local_config_to_machine(CString m_sn)
{
	CString get_info;
	CString m_ret;
	CString get_write_sn_stat;
	CString get_write_conf_stat;
	bool get_mount_stat;


	CString get_local_md5;
	CString get_machine_md5;

	CString get_burned_data;

	/*check protect partition mount stat*/
	get_mount_stat = check_machine_mount_stat();
	if (!get_mount_stat)
	{
		m_ret = "�����޷��������ر���������";
		return m_ret;
	}
	get_write_sn_stat = write_serial_number_to_machine(m_sn);
	get_write_conf_stat = write_config_files_to_machine();
	m_ret = m_sn;
	if (get_write_conf_stat.Compare("OK") == 0)
	{
		m_ret += "\r\n" + get_write_conf_stat + "\r\n";
		get_burned_data = check_burned_data(m_sn);
		m_ret += get_burned_data;
	}

	return m_ret;
}

CString ConfigUtility::check_local_config_md5()
{
	CString get_info;
	get_info = m_ctrlcent.StartSingleCommand("md5sum IBoxConfig/*");
	return get_info;
}

CString ConfigUtility::check_machine_config_md5(CString m_protect)
{
	CString get_info;

	if (m_protect.CompareNoCase("protect_s") == 0)
		get_info = m_ctrlcent.StartSingleCommand("adb shell busybox md5sum \/protect_s\/IBoxConfig\/*");
	else if (m_protect.CompareNoCase("protect_f") == 0)
		get_info = m_ctrlcent.StartSingleCommand("adb shell busybox md5sum \/protect_f\/IBoxConfig\/*");
	else
		get_info = "Get md5 Fail!";
	return get_info;
}

CString ConfigUtility::check_machine_serialnumber()
{
	CString get_info_s, get_info_f;
	CString m_ret;
	get_info_s = m_ctrlcent.StartSingleCommand("adb shell cat \/protect_s\/IBoxDeviceID.config");
	get_info_f = m_ctrlcent.StartSingleCommand("adb shell cat \/protect_s\/IBoxDeviceID.config");
	get_info_s.TrimRight(" ");
	get_info_f.TrimRight(" ");
	if (get_info_s.Compare(get_info_f) != 0)
	{
		m_ret = "\r\nprotect_s��" + get_info_s;
		m_ret += "\r\nprotect_f��" + get_info_f;
		m_ret += "\r\n���������豸 ID ��һ��!!";
	}
	else
		m_ret = get_info_s;
	return m_ret;
}

CString ConfigUtility::check_machine_stat()
{
	CString get_stat;
	CString m_ret;

	for (int i0 = 0; i0 <= 3; i0++)
	{
		get_stat = check_devices_recovery_online();
		if (get_stat.CompareNoCase("OK") == 0)
		{
			m_ret = get_devices_recovery_machine_id();
			break;
		}
		else if (get_stat.CompareNoCase("HOST") == 0)
		{
			m_ctrlcent.StartSingleCommand("adb kill-server");
			continue;
		}
		else
		{
			m_ret = get_stat;
			break;
		}
	}

	return m_ret;
}


CString ConfigUtility::check_devices_recovery_online()
{
	CString get_stat;
	CString m_ret;
	int m_pos, m_count_recovery, m_count_devices, m_count_host;
	CString str_debug;

	/* for example, we have a devices in first line
	List of devices attached
	emulator-5584   host
	AAYTT8IRS49TGEHY        device
	AAYTT8IRS49TGEHZ        device
	AAYTT8IRS49TGEHX        recovery
	*/
	get_stat = m_ctrlcent.StartSingleCommand("adb devices");
	m_count_recovery = 0;
	m_count_devices = 0;
	m_count_host=0;

	m_pos = 0;
	for (int i0 = 0; i0 < 5; i0++)
	{
		m_pos = get_stat.Find("recovery", m_pos + 1);
		if (m_pos < 0)
			break;
		++m_count_recovery;
		str_debug.Format("\n=====>%d == %d<=====", m_pos, m_count_recovery);
		TRACE(str_debug);
	}
	m_pos = 0;
	for (int i0 = 0; i0 < 5; i0++)
	{
		m_pos = get_stat.Find("device", m_pos + 1);
		if (m_pos < 0)
			break;
		++m_count_devices;
		str_debug.Format("\n=====>%d == %d<=====", m_pos, m_count_devices);
		TRACE(str_debug);
	}
	--m_count_devices;/*Because we have one more key word.*/

	m_pos = 0;
	for (int i0 = 0; i0 < 5; i0++)
	{
		m_pos = get_stat.Find("host", m_pos + 1);
		if (m_pos < 0)
			break;
		++m_count_host;
		str_debug.Format("\n=====>%d == %d<=====", m_pos, m_count_host);
		TRACE(str_debug);
	}

	if (m_count_host > 0)
	{
		m_ret = "HOST";
		return m_ret;
	}
	if (m_count_recovery == 0)
	{
		if (m_count_devices >= 1)
		{
			m_ret = "�豸���� Recovery ģʽ�������½��� Recovery ģʽ������¼��";
		}
		else
		{
			m_ret = "�Ҳ����豸���������ߣ�";
		}
	}
	else if (m_count_recovery == 1)
	{
		if (m_count_devices >= 1)
		{
			m_ret = "����ж���豸���뱣ֻ֤��һ���豸���룡";
		}
		else
		{
			m_ret = "OK";
		}
	}
	else
	{
		m_ret = "�ж���豸�����豸�쳣���������ߣ�";
	}
	TRACE(m_ret);
	return m_ret;
}


CString ConfigUtility::get_devices_recovery_machine_id()
{
	CString get_stat;
	CString m_ret;
	int m_pos, m_pos_recovery;
	CString str_debug;

	/* for example, we have a devices in first line
	List of devices attached
	emulator-5584   host
	AAYTT8IRS49TGEHY        device
	AAYTT8IRS49TGEHZ        device
	AAYTT8IRS49TGEHX        recovery
	*/
	get_stat = m_ctrlcent.StartSingleCommand("adb devices");

	m_pos = get_stat.Find("\n", 0);
	m_pos_recovery = get_stat.Find("recovery", m_pos);

	m_ret = get_stat.Mid(m_pos + 1, m_pos_recovery - m_pos - 1 + 8);
	str_debug.Format("\n=====>%d == %d<=====", m_pos,m_pos_recovery);
	TRACE(str_debug);
	TRACE(m_ret);
	
	//m_ret.TrimRight(" ");
	//m_ret.TrimRight("	");
	return m_ret;
}



bool ConfigUtility::check_machine_mount_stat()
{
	CString get_stat;
	int m_pos_s,m_pos_f;

	get_stat = m_ctrlcent.StartSingleCommand("adb shell mount");
	m_pos_s = get_stat.Find("protect_s", 0);
	m_pos_f = get_stat.Find("protect_f", 0);

	if (m_pos_s > 0 && m_pos_f > 0)
	{
		return TRUE;
	}
	return FALSE;
}



bool ConfigUtility::compare_md5_sum()
{
	CString get_local_md5;
	CString get_machine_md5_s;
	CString get_machine_md5_f;
	CString get_single_md5;
	int m_pos_start, m_pos_end;
	CString get_single_md5_tmp;

	CString debug_str;

	get_local_md5 = check_local_config_md5();
	get_machine_md5_s = check_machine_config_md5("protect_s");
	get_machine_md5_s = check_machine_config_md5("protect_f");

	m_pos_start = 0;
	m_pos_end = 0;
	for (int i0 = 0; i0 < 10; i0++)
	{
		if (m_pos_start < 0 || m_pos_end < 0)
			break;

		if (m_pos_start == 0 && m_pos_end == 0)
		{
			m_pos_end = get_local_md5.Find(" ", m_pos_start);
			get_single_md5_tmp = get_local_md5.Mid(m_pos_start, m_pos_end - m_pos_start);

			debug_str.Format("--s->%d<-e->%d<---delta->%d<--", m_pos_start, m_pos_end, m_pos_end - m_pos_start);
			//TRACE("\n" + debug_str + "--->" + get_single_md5_tmp + "<----");
		}
		else
		{

			m_pos_start = get_local_md5.Find("\n", m_pos_end);
			m_pos_end = get_local_md5.Find(" ", m_pos_start);
			//if (m_pos_start < 0 || m_pos_end < 0)
			//	break;
			get_single_md5_tmp = get_local_md5.Mid(m_pos_start + 1, m_pos_end - m_pos_start);

			debug_str.Format("--s->%d<-e->%d<---delta->%d<--", m_pos_start, m_pos_end, m_pos_end - m_pos_start);
			//TRACE("\n" + debug_str + "--->" + get_single_md5_tmp + "<----");
		}
		get_single_md5_tmp.TrimRight(" ");
		get_single_md5_tmp.TrimRight("	");
		get_single_md5_tmp.TrimLeft("\n");
		get_single_md5 = "";
		if (get_single_md5_tmp.GetLength() == 32)
		{
			get_single_md5 = get_single_md5_tmp;
		}
		TRACE("\n--->" + get_single_md5 + "<----");


		if (get_single_md5.GetLength() == 0)
		{
			break;
		}

		if (get_machine_md5_s.Find(get_single_md5, 0) < 0)
		{
			return FALSE;
		}
		if (get_machine_md5_s.Find(get_single_md5, 0) < 0)
		{
			return FALSE;
		}
	}
	return TRUE;
}



bool ConfigUtility::compare_serail_number(CString m_sn)
{
	CString get_info_s;
	CString get_info_f;

	get_info_s = m_ctrlcent.StartSingleCommand("adb shell cat \/protect_s\/IBoxDeviceID.config");
	get_info_f = m_ctrlcent.StartSingleCommand("adb shell cat \/protect_s\/IBoxDeviceID.config");

	get_info_s.TrimRight(" ");
	get_info_f.TrimRight(" ");
	m_sn.TrimRight(" ");
	
	TRACE("---s--->"+ get_info_s + "<---f--->"+ get_info_f + "<---get--->" + m_sn + "<---");
	if (get_info_s.Compare(get_info_f) != 0)
	{
		return FALSE;
	}
	if (get_info_s.Compare(m_sn) != 0)
	{
		return FALSE;
	}
	return TRUE;
	
}

CString ConfigUtility::check_burned_data(CString m_sn)
{
	CString m_ret;
	bool get_check_md5_stat;
	bool get_check_sn_stat;
	CString get_sn;
	CString get_md5;

	get_check_md5_stat = compare_md5_sum();

	get_md5 = check_local_config_md5();
	get_md5.Replace("\n", "\r\n");
	get_md5.Replace(" ", "\t");

	if (!get_check_md5_stat)
	{
		m_ret = "��������ļ�MD5�����";
		return m_ret;
	}

	m_ret = "�����ļ�MD5У�飺\t\tOK��\r\n";

	if (m_sn.CompareNoCase("CheckOnly") == 0)
	{
		get_sn = check_machine_serialnumber();
		m_ret += "IBox�豸ID��\t\t\t" + get_sn;

		m_ret += "\r\nIBox�����ļ�MD5��ֵ��\r\n" + get_md5;

		return m_ret;
	}
	else
	{
		get_check_sn_stat = compare_serail_number(m_sn);
		if (!get_check_md5_stat)
		{
			m_ret += "����豸 ID ����";
			return m_ret;
		}
		m_ret += "IBox�豸ID��" + m_sn;
		m_ret += "\r\nIBox�����ļ�MD5��ֵ��\r\n" + get_md5;
	}



	return m_ret;
}



CString ConfigUtility::write_serial_number_to_machine(CString m_sn)
{
	char m_cmd[1024];

	memset(m_cmd, 0x0, sizeof(m_cmd));
	sprintf_s(m_cmd, "adb shell \"echo %s > /protect_s/IBoxDeviceID.config\"", m_sn);
	m_ctrlcent.StartSingleCommand(m_cmd);

	memset(m_cmd, 0x0, sizeof(m_cmd));
	sprintf_s(m_cmd, "adb shell \"echo %s > /protect_f/IBoxDeviceID.config\"", m_sn);
	m_ctrlcent.StartSingleCommand(m_cmd);

	m_ctrlcent.StartSingleCommand("adb shell sync");
	return "";
}
CString ConfigUtility::write_config_files_to_machine()
{
	char chPath[301];
	TCHAR command[1024] = _T("adb");
	TCHAR command_arg[1024] = _T("push FactoryTools.cpp /protect_s/IBoxConfig/");

	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;

	si.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	
	::GetCurrentDirectory(300, (LPTSTR)chPath);//�õ���ǰĿ¼

	m_ctrlcent.StartSingleCommand("adb shell \"rm -rf /protect_f/IBoxConfig/ /protect_s/IBoxConfig/ \"");
	m_ctrlcent.StartSingleCommand("adb shell \"mkdir -p /protect_s/IBoxConfig/\"");
	m_ctrlcent.StartSingleCommand("adb shell \"mkdir -p /protect_f/IBoxConfig/\"");

	if (CreateProcess(NULL, "adb push FactoryTools.cpp /protect_s/IBoxConfig/", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi) == FALSE)
	{
		return FALSE;
	}

	CreateProcess(NULL, "adb push IBoxConfig\\CaptureConfig.txt /protect_s/IBoxConfig/", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	CreateProcess(NULL, "adb push IBoxConfig\\FolderConfig.txt /protect_s/IBoxConfig/", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	CreateProcess(NULL, "adb push IBoxConfig\\RTSPConfig.txt /protect_s/IBoxConfig/", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	CreateProcess(NULL, "adb push IBoxConfig\\VideoRecordConfig.txt /protect_s/IBoxConfig/", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	
	

	CreateProcess(NULL, "adb push IBoxConfig\\CaptureConfig.txt /protect_f/IBoxConfig/", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	CreateProcess(NULL, "adb push IBoxConfig\\FolderConfig.txt /protect_f/IBoxConfig/", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	CreateProcess(NULL, "adb push IBoxConfig\\RTSPConfig.txt /protect_f/IBoxConfig/", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	CreateProcess(NULL, "adb push IBoxConfig\\VideoRecordConfig.txt /protect_f/IBoxConfig/", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);

	m_ctrlcent.StartSingleCommand("adb shell sync");
	return "OK";
}

