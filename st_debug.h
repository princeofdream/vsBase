#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <iostream>


#define JCG() printf("---[By James]---[%s:%d]--%s---\n",__FILE__,__LINE__,__func__)

class st_debug
{
public:
	st_debug();
	~st_debug();

public:
	int init_debug_env(void);
	int release_debug_env(void);
};

