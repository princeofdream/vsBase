#pragma once


#include <def.h>
#include <resource.h>
#include <vsBase.h>
#include <FloatWnd.h>
#include <vsBaseDlg.h>

class ImgProc : public CDialog
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

