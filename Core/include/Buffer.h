#pragma once

#include "Core.h"

#include <cstdint>
#include <cstdlib>

class Buffer
{
public:
	Buffer(uint8_t* buf, uint32_t capacity, uint32_t offset = 0)
		: m_Buf(buf), m_Capacity(capacity), m_Offset(offset) {}

	Buffer(const Buffer& other, uint32_t offset = 0)
		: m_Buf(other.m_Buf + offset), m_Capacity(other.m_Capacity - offset), m_Offset(0) {}


	Buffer(const Buffer& other) : m_Buf(other.m_Buf), m_Capacity(other.m_Capacity), m_Offset(other.m_Offset) {}


	template<typename T>
	void Read(T& value)
	{
		SLI_ASSERT(m_Offset + sizeof(T) > m_Capacity, "Buffer read out exceeds capacity!");
		memcpy(&value, m_Buf + m_Offset);
		m_Offset += sizeof(T);
	}

	inline uint8_t* Begin() { return m_Buf; }
	inline const uint8_t* Begin() const { return m_Buf; }

	template<typename T> T* As() { return reinterpret_cast<T*>(m_Buf); }
	template<typename T> const T* As() const { return reinterpret_cast<T*>(m_Buf); }


private:
	uint8_t* m_Buf;
	uint32_t m_Capacity;
	uint32_t m_Offset;

};

template<std::size_t Cap>
class StackBuffer : public Buffer
{
public:
	StackBuffer() : Buffer(m_Buffer, Cap, 0) {}

private:
	uint8_t m_Buffer[Cap];
};

