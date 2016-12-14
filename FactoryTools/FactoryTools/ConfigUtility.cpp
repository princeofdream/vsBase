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
		m_ret = "错误：无法正常挂载保护分区。";
		return m_ret;
	}
	get_write_sn_stat = write_serial_number_to_machine(m_sn);
	get_write_conf_stat = write_config_files_to_machine();
	m_ret = m_sn;
	if (get_write_conf_stat.Compare(_T("OK")) == 0)
	{
		m_ret += "\r\n";
		m_ret += get_write_conf_stat;
		m_ret += "\r\n";
		get_burned_data = check_burned_data(m_sn);
		m_ret += get_burned_data;
	}

	return m_ret;
}

CString ConfigUtility::check_local_config_md5()
{
	CString get_info;
	CString m_ret;
	int m_pos;

	get_info = m_ctrlcent.StartSingleCommand("md5sums.exe -e IBoxConfig\/*");
	TRACE(get_info);
	get_info.Delete(0, 200);
	get_info.TrimLeft(_T("-"));
	get_info.TrimLeft(_T(" "));
	get_info.TrimLeft(_T("\t"));
	get_info.TrimLeft(_T("\r"));
	get_info.TrimLeft(_T("\n"));
	m_pos = get_info.Find("\n", 0);
	get_info.Delete(0, m_pos);

	return get_info;
}

CString ConfigUtility::check_machine_config_md5(CString m_protect)
{
	CString get_info;

	if (m_protect.CompareNoCase(_T("protect_s")) == 0)
		get_info = m_ctrlcent.StartSingleCommand("adb shell busybox md5sum \/protect_s\/IBoxConfig\/*");
	else if (m_protect.CompareNoCase(_T("protect_f")) == 0)
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
	get_info_s.TrimRight(_T(" "));
	get_info_f.TrimRight(_T(" "));
	if (get_info_s.Compare(get_info_f) != 0)
	{
		m_ret = "\r\nprotect_s：";
		m_ret += get_info_s;
		m_ret += "\r\nprotect_f：";
		m_ret += get_info_f;
		m_ret += "\r\n保护分区设备 ID 不一致!!";
	}
	else
		m_ret = get_info_s;
	return m_ret;
}

CString ConfigUtility::check_machine_stat(CString get_mode)
{
	CString get_stat;
	CString m_ret;

	for (int i0 = 0; i0 <= 3; i0++)
	{
		if (get_mode.Compare(_T("recovery")) == 0)
		{
			get_stat = check_devices_recovery_online();
			if (get_stat.CompareNoCase(_T("OK")) == 0)
			{
				m_ret = get_devices_recovery_machine_id();
				break;
			}
			else if (get_stat.CompareNoCase(_T("HOST")) == 0)
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
		else
		{
			get_stat = check_devices_online();
			if (get_stat.CompareNoCase(_T("OK")) == 0)
			{
				m_ret = get_devices_machine_id();
				break;
			}
			else if (get_stat.CompareNoCase(_T("HOST")) == 0)
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
	}

	return m_ret;
}




CString ConfigUtility::check_devices_online()
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
	m_count_host = 0;

	m_pos = 0;
	m_pos = get_stat.Find(_T("recovery"), m_pos + 1);
	if (m_pos >= 0)
	{
		m_ret =  "IBox处于Recovery模式，请按VCR按键重启进入正常开机模式！";
		return m_ret;
	}
	m_pos = 0;
	for (int i0 = 0; i0 < 5; i0++)
	{
		m_pos = get_stat.Find(_T("device"), m_pos + 1);
		if (m_pos < 0)
			break;
		++m_count_devices;
		str_debug.Format(_T("\n=====>%d == %d<====="), m_pos, m_count_devices);
		TRACE(str_debug);
	}
	--m_count_devices;/*Because we have one more key word.*/
	if (m_count_devices > 1)
	{
		m_ret = "找到多个设备，请确保只接入一个设备!";
	}
	else if (m_count_devices < 1)
	{
		m_ret = "找不到设备，请确保设备已经接入！";
	}

	m_pos = 0;
	for (int i0 = 0; i0 < 5; i0++)
	{
		m_pos = get_stat.Find(_T("host"), m_pos + 1);
		if (m_pos < 0)
			break;
		++m_count_host;
		str_debug.Format(_T("\n=====>%d == %d<====="), m_pos, m_count_host);
		TRACE(str_debug);
	}

	if (m_count_host > 0)
	{
		m_ret = "HOST";
		return m_ret;
	}
	
	m_ret = "OK";

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
		m_pos = get_stat.Find(_T("recovery"), m_pos + 1);
		if (m_pos < 0)
			break;
		++m_count_recovery;
		str_debug.Format(_T("\n=====>%d == %d<====="), m_pos, m_count_recovery);
		TRACE(str_debug);
	}
	m_pos = 0;
	for (int i0 = 0; i0 < 5; i0++)
	{
		m_pos = get_stat.Find(_T("device"), m_pos + 1);
		if (m_pos < 0)
			break;
		++m_count_devices;
		str_debug.Format(_T("\n=====>%d == %d<====="), m_pos, m_count_devices);
		TRACE(str_debug);
	}
	--m_count_devices;/*Because we have one more key word.*/

	m_pos = 0;
	for (int i0 = 0; i0 < 5; i0++)
	{
		m_pos = get_stat.Find(_T("host"), m_pos + 1);
		if (m_pos < 0)
			break;
		++m_count_host;
		str_debug.Format(_T("\n=====>%d == %d<====="), m_pos, m_count_host);
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
			m_ret = "设备不在 Recovery 模式，请重新进入 Recovery 模式进行烧录！";
		}
		else
		{
			m_ret = "找不到设备，请检查连线！";
		}
	}
	else if (m_count_recovery == 1)
	{
		if (m_count_devices >= 1)
		{
			m_ret = "检测有多个设备，请保证只有一个设备插入！";
		}
		else
		{
			m_ret = "OK";
		}
	}
	else
	{
		m_ret = "有多个设备或者设备异常，请检查连线！";
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

	m_pos = get_stat.Find(_T("\n"), 0);
	m_pos_recovery = get_stat.Find(_T("recovery"), m_pos);

	m_ret = get_stat.Mid(m_pos + 1, m_pos_recovery - m_pos - 1 + 8);
	str_debug.Format(_T("\n=====>%d == %d<====="), m_pos,m_pos_recovery);
	TRACE(str_debug);
	TRACE(m_ret);
	
	//m_ret.TrimRight(" ");
	//m_ret.TrimRight("	");
	return m_ret;
}

CString ConfigUtility::get_devices_machine_id()
{
	CString get_stat;
	CString m_ret;
	int m_pos, m_pos_device;
	CString str_debug;

	/* for example, we have a devices in first line
	List of devices attached
	emulator-5584   host
	AAYTT8IRS49TGEHY        device
	AAYTT8IRS49TGEHZ        device
	AAYTT8IRS49TGEHX        recovery
	*/
	get_stat = m_ctrlcent.StartSingleCommand("adb devices");

	m_pos = get_stat.Find(_T("\n"), 0);
	m_pos_device = get_stat.Find(_T("device"), m_pos);

	m_ret = get_stat.Mid(m_pos + 1, m_pos_device - m_pos - 1 + 8);
	str_debug.Format(_T("\n=====>%d == %d<====="), m_pos, m_pos_device);
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
	m_pos_s = get_stat.Find(_T("protect_s"), 0);
	m_pos_f = get_stat.Find(_T("protect_f"), 0);

	if (m_pos_s > 0 && m_pos_f > 0)
	{
		return TRUE;
	}
	return FALSE;
}



CString ConfigUtility::compare_md5_sum()
{
	CString get_local_md5;
	CString get_machine_md5_s;
	CString get_machine_md5_f;
	CString get_single_md5;
	int m_pos_start, m_pos_end;
	CString get_single_md5_tmp;

	CString debug_str;

	get_local_md5 = check_local_config_md5();
	get_machine_md5_s = check_machine_config_md5(_T("protect_s"));
	get_machine_md5_s = check_machine_config_md5(_T("protect_f"));

	m_pos_start = 0;
	m_pos_end = 0;
	for (int i0 = 0; i0 < 10; i0++)
	{
		m_pos_start = get_local_md5.Find(_T(" "), m_pos_end);
		if (m_pos_start < 0 || m_pos_end < 0)
			break;
		m_pos_end = get_local_md5.Find(_T("\n"), m_pos_start);
		if (m_pos_start < 0 || m_pos_end < 0)
			break;

		debug_str.Format(_T("--s->%d<-e->%d<---delta->%d<--"), m_pos_start, m_pos_end, m_pos_end - m_pos_start);


		get_single_md5_tmp = get_local_md5.Mid(m_pos_start + 1, m_pos_end - m_pos_start);
		TRACE("\n" + debug_str + "--->" + get_single_md5_tmp + "<----");

		get_single_md5_tmp.TrimLeft(_T(" "));
		get_single_md5_tmp.TrimLeft(_T("	"));
		get_single_md5_tmp.TrimLeft(_T("\n"));
		get_single_md5_tmp.TrimRight(_T(" "));
		get_single_md5_tmp.TrimRight(_T("	"));
		get_single_md5_tmp.TrimRight(_T("\n"));
		get_single_md5_tmp.TrimRight(_T("\r"));



		get_single_md5 = "";
		if (get_single_md5_tmp.GetLength() == 32)
		{
			get_single_md5 = get_single_md5_tmp;
		}
		TRACE(get_single_md5);


		if (get_single_md5.GetLength() == 0)
		{
			break;
		}

		if (get_machine_md5_s.Find(get_single_md5, 0) < 0)
		{
			return "NG";
		}
		if (get_machine_md5_s.Find(get_single_md5, 0) < 0)
		{
			return "NG";
		}
	}
	return "OK";
}



bool ConfigUtility::compare_serail_number(CString m_sn)
{
	CString get_info_s;
	CString get_info_f;

	get_info_s = m_ctrlcent.StartSingleCommand("adb shell cat \/protect_s\/IBoxDeviceID.config");
	get_info_f = m_ctrlcent.StartSingleCommand("adb shell cat \/protect_s\/IBoxDeviceID.config");

	get_info_s.TrimRight(_T(" "));
	get_info_f.TrimRight(_T(" "));
	m_sn.TrimRight(_T(" "));
	
	//TRACE("---s--->"+ get_info_s + "<---f--->"+ get_info_f + "<---get--->" + m_sn + "<---");
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
	CString get_check_md5_stat;
	bool get_check_sn_stat;
	CString get_sn;
	CString get_local_md5;
	CString get_machine_md5_s, get_machine_md5_f;

	get_check_md5_stat = compare_md5_sum();

	get_local_md5 = check_local_config_md5();
	get_machine_md5_s = check_machine_config_md5(_T("protect_s"));
	get_machine_md5_f = check_machine_config_md5(_T("protect_f"));

	get_local_md5.Replace(_T("\r\n"), _T("\n"));
	get_local_md5.Replace(_T("\n"), _T("\r\n"));
	//get_local_md5.Replace(_T(" "), _T("\t"));

	get_machine_md5_s.Replace(_T("\n"), _T("\r\n"));
	get_machine_md5_s.Replace(_T(" "), _T("\t"));

	get_machine_md5_f.Replace(_T("\n"), _T("\r\n"));
	get_machine_md5_f.Replace(_T(" "), _T("\t"));

	if (get_check_md5_stat.Find("OK",0) != 0)
	{
		m_ret = "检查配置文件MD5码出错！";
		m_ret += "\r\nIBox配置文件MD5码值：\r\n";
		m_ret += get_local_md5;
		m_ret += "\r\n==============>>s\r\n";
		m_ret += get_machine_md5_s;
		m_ret += "\r\n==============>>f\r\n";
		m_ret += get_machine_md5_f;


		return m_ret;
	}

	m_ret = "配置文件MD5校验：\t\tOK！\r\n";

	if (m_sn.CompareNoCase(_T("CheckOnly")) == 0)
	{
		get_sn = check_machine_serialnumber();
		m_ret += "IBox设备ID：\t\t\t";
		m_ret += get_sn;

		m_ret += "\r\nIBox配置文件MD5码值：\r\n";
		m_ret += get_local_md5;

		return m_ret;
	}
	else
	{
		get_check_sn_stat = compare_serail_number(m_sn);
		if (!get_check_md5_stat)
		{
			m_ret += "检查设备 ID 出错！";
			return m_ret;
		}
		m_ret += "IBox设备ID：";
		m_ret += m_sn;
		m_ret += "\r\nIBox配置文件MD5码值：\r\n";
		m_ret += get_local_md5;
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
	return NULL;
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
	
	::GetCurrentDirectory(300, (LPTSTR)chPath);//得到当前目录

	m_ctrlcent.StartSingleCommand("adb shell \"rm -rf /protect_f/IBoxConfig/ /protect_s/IBoxConfig/ \"");
	m_ctrlcent.StartSingleCommand("adb shell \"mkdir -p /protect_s/IBoxConfig/\"");
	m_ctrlcent.StartSingleCommand("adb shell \"mkdir -p /protect_f/IBoxConfig/\"");


	CreateProcess(NULL, _T("adb push IBoxConfig\\CaptureConfig.txt /protect_s/IBoxConfig/"), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	CreateProcess(NULL, _T("adb push IBoxConfig\\FolderConfig.txt /protect_s/IBoxConfig/"), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	CreateProcess(NULL, _T("adb push IBoxConfig\\RTSPConfig.txt /protect_s/IBoxConfig/"), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	CreateProcess(NULL, _T("adb push IBoxConfig\\VideoRecordConfig.txt /protect_s/IBoxConfig/"), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	
	

	CreateProcess(NULL, _T("adb push IBoxConfig\\CaptureConfig.txt /protect_f/IBoxConfig/"), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	CreateProcess(NULL, _T("adb push IBoxConfig\\FolderConfig.txt /protect_f/IBoxConfig/"), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	CreateProcess(NULL, _T("adb push IBoxConfig\\RTSPConfig.txt /protect_f/IBoxConfig/"), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	CreateProcess(NULL, _T("adb push IBoxConfig\\VideoRecordConfig.txt /protect_f/IBoxConfig/"), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);

	m_ctrlcent.StartSingleCommand("adb shell sync");
	return _T("OK");
}

