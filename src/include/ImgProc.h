#pragma once


#include <basedef.h>

class ImgProc
{
public:
	ImgProc();
	~ImgProc();

public:
	UINT load_img_proc(long* argc);
	UINT check_img_proc(long* argc);

protected:
	UINT m_stat;
};

