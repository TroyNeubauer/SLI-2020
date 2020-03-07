#include "Buffer.h"
#include "gspch.h"

#include "Buffer.h"

#include "Packet.h"

bool PacketBuffer::Read(void* dest, std::size_t bytes)
{
	if (m_Offset + bytes > m_Capacity)
	{
		return false;
	}
	
	memcpy(dest, m_Buf + m_Offset, bytes);
	m_Offset += bytes;
	return true;
}

uint32_t PacketBuffer::CalculateCRC32()
{
	//Dont count the magic or the checksum itself
	uint8_t* begin = m_Buf + sizeof(uint32_t) + sizeof(m_Header->CRC32);
	uint8_t* end = m_Buf + m_Offset;
	return CRC32Impl(begin, end - begin);
}
