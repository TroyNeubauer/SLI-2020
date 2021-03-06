
#include "Formatter.h"
#if defined(RKT_DEBUG)
extern void SLIAssertFailed(const char* message);
#define SLI_ASSERT(value, message) { if(!(value)) { SLIAssertFailed(message); }}

#define SLI_FASSERT(value, formatterCode)			\
{													\
	if(!(value))									\
	{												\
		SizedFormatter<256> f;						\
		formatterCode;								\
		SLIAssertFailed(f.c_str());					\
	}												\
}													\

#else

#define SLI_ASSERT(value, message)

#define SLI_FASSERT(value, formatterCode)

#endif
