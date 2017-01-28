#pragma once


#include <st_debug.h>
#include <ProcessManager.h>

class node_js
{
public:
	node_js();
	~node_js();

public:
	UINT start_nodejs_proc(long* argc);
	UINT check_nodejs_proc(long* argc);

protected:
	UINT m_stat;
};

