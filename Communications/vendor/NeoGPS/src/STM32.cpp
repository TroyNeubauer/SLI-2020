#include "CosaCompat.h"

uint8_t pgm_read_byte(const void* address)
{
	uint8_t* ptr = (uint8_t*) address;
	return *ptr;
}

void* pgm_read_ptr(const void* address)
{
	void** ptr = (void**) address;
	return *ptr;
}
