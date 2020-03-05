#include "gspch.h"

#include "Buffer.h"

bool Buffer::Read(void* dest, std::size_t bytes)
{
	if (m_Offset + bytes > m_Capacity)
	{
		return false;
	}
	
	memcpy(dest, m_Buf + m_Offset, bytes);
	m_Offset += bytes;
	return true;
}
