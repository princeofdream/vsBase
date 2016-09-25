#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <iostream>


#define JCG() printf("---[By James]---[%s:%d]--%s---",__FILE__,__LINE__,__func__)

class st_debug
{
public:
	st_debug();
	~st_debug();

public:
	int init_debug_env(void);
	int release_debug_env(void);
	void set_console_flag(bool stat);
	bool get_console_flag();
protected:
	bool console_flag;
};

