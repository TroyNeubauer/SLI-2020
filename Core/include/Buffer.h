#pragma once

#include "Core.h"

#include <stdint.h>
#include <stdlib.h>

class Buffer
{
public:
	Buffer(uint8_t* buf, uint32_t offset, uint32_t capacity)
		: m_Buf(buf), m_Offset(offset), m_Capacity(capacity) {}

	template<typename T>
	void Read(T& value)
	{
		SLI_ASSERT(m_Offset + sizeof(T) > m_Capacity, "Buffer read out exceeds capacity!");
		memcpy(&value, m_Buf + m_Offset);
		m_Offset += sizeof(T);
	}

public:

#ifdef STM
	static thread_local char BUF[128];
#else//Ground Station
	static thread_local char BUF[1024];

#endif

private:
	uint8_t* m_Buf;
	uint32_t m_Offset;
	uint32_t m_Capacity;

};

