#pragma once

#include "ControlCenter.h"

class ConfigUtility
{
public:
	ConfigUtility();
	~ConfigUtility();

	bool check_machine_config_stat();
	CString start_burn_local_config_to_machine();
	bool check_local_config_md5();
	bool check_machine_config_md5();
	CString check_machine_serialnumber();
	CString check_machine_stat();
	CString check_devices_recovery_online();
	CString get_devices_recovery_machine_id();

protected:
	ControlCenter m_ctrlcent;

};

