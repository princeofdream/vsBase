#pragma once

#include "ControlCenter.h"

#define MD5_SIZE 32

class ConfigUtility
{
public:
	ConfigUtility();
	~ConfigUtility();

	bool check_machine_config_stat();
	CString start_burn_local_config_to_machine(CString m_sn);
	CString check_local_config_md5();
	CString check_machine_config_md5(CString m_protect);
	CString check_machine_serialnumber();
	CString check_machine_stat();
	CString check_devices_recovery_online();
	CString get_devices_recovery_machine_id();
	bool check_machine_mount_stat();
	bool compare_md5_sum();
	bool compare_serail_number(CString m_sn);
	CString check_burned_data(CString m_sn);

protected:
	ControlCenter m_ctrlcent;

};

