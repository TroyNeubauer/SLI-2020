#include "gspch.h"

#include "Buffer.h"


#ifdef STM
	thread_local char Buffer::BUF[128];
#else//Ground Station
	thread_local char Buffer::BUF[1024];
#endif

