

extern void SLIAssertFailed(const char* message);
#define SLI_ASSERT(value, message) { if(!value) { SLIAssertFailed(message); }}
