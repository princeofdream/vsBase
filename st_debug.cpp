#include "stdafx.h"
#include "st_debug.h"

st_debug::st_debug()
{
}


st_debug::~st_debug()
{
}

int st_debug::init_debug_env(void)
{
	// TODO: Add your control notification handler code here
	HANDLE HConsole;
	AllocConsole();
	HConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dw;
	//std::string str = "my test";

	//WriteConsole(HConsole, str.c_str(), strlen(str.c_str()), &dw, NULL);
	//WriteConsole(HConsole, "\n", strlen("\n"), &dw, NULL);

	AllocConsole();
	freopen("CONOUT$", "w+t", stdout);
	freopen("CONIN$", "r+t", stdin);

	set_console_flag(true);
	return 0;
}

int st_debug::release_debug_env(void)
{
	fclose(stdout);
	fclose(stdin);
	FreeConsole();
	set_console_flag(false);
	return 0;
}

void st_debug::set_console_flag(bool stat)
{
	console_flag = stat;
}

bool st_debug::get_console_flag()
{
	return console_flag;
}


