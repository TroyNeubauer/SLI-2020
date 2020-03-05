
#include "Packet.h"
#include "Buffer.h"

#include "CRC.h"

uint32_t CRC32Impl(const uint8_t* data, size_t bytes)
{
	return CRC::Calculate(data, bytes, CRC::CRC_32());

}
