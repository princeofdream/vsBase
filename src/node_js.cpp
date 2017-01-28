#include "stdafx.h"
#include <node_js.h>


node_js::node_js()
{
	m_stat = 0;
}


node_js::~node_js()
{
}

UINT node_js::start_nodejs_proc(long* argc)
{
	ProcessManager m_proc;
	char *param = "wait_for_single";

	printf("=================== [%s:%d] Start JS ================\n", __FILE__, __LINE__);
	//m_proc.Start_New_Process("node.exe", "node.js/hello.js", NULL, (long*)param);
	printf("=================== [%s:%d] End of JS ================\n",__FILE__,__LINE__);

	m_stat++;
	printf("=================== [%s:%d] stat: %d ================\n", __FILE__, __LINE__, m_stat);
	return 0;
}

UINT node_js::check_nodejs_proc(long* argc)
{
	m_stat++;
	printf("=================== [%s:%d] stat: %d ================\n", __FILE__, __LINE__, m_stat);
	return 0;
}
