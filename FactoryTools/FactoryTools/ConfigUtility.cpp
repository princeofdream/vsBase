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

CString ConfigUtility::start_burn_local_config_to_machine()
{
	CString get_info;
	CString m_ret;
	get_info = check_machine_stat();

	m_ret = get_info;
	return m_ret;
}

bool ConfigUtility::check_local_config_md5()
{
	return TRUE;
}

bool ConfigUtility::check_machine_config_md5()
{
	return TRUE;
}

CString ConfigUtility::check_machine_serialnumber()
{
	return NULL;
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
