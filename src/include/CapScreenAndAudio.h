#include <basedef.h>

#include "tchar.h"
#include <stdarg.h>


#ifdef ENABLE_FFMPEG_AND_RECORD

int OpenVideoCapture();
int Start_Rec(int argc, _TCHAR* argv[]);
DWORD WINAPI SetRecStat(LPVOID lpParam);

#endif