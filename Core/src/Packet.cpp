
#include "Packet.h"


int16_t SizeOfPacketData(const PacketHeader& header, const Buffer& packet)
{
	switch (header.Type)
	{
		case PacketType::INIT:
			return sizeof(InitPacket);
			
		case PacketType::STATUS:
			return sizeof(StatusPacket);

		case PacketType::DATA:
		{
			if (header.From == ModuleID::GPS)
			{
				uint16_t sentenceLength = packet.As<DataPacket_GPS>()->NMEASentenceLength;
				if (sentenceLength > MAX_NMEA_LENGTH)
				{
					return -1;
				}

				return sizeof(DataPacket_GPS) + sentenceLength;
			}
			else if (header.From == ModuleID::STM32F103)
			{
				return sizeof(DataPacket_STMF103);
			}
			else if (header.From == ModuleID::STM32F205)
			{
				return sizeof(DataPacket_STMF205);
			}
			else
			{
				//Unknown sender. Corrupt packet?
				return -1;
			}

		}
		case PacketType::MESSAGE:
		{
			uint16_t messageLength = packet.As<MessagePacket>()->MessageLength;
			if (messageLength > MAX_MESSAGE_LENGTH)
			{
				return -1;
			}
			return sizeof(MessagePacket) + messageLength;
		}
		case PacketType::REQUEST:
			//TODO
			//return sizeof(RequestPacket);
			return 0;

		case PacketType::RESPONSE:
			//TODO
			//return sizeof(ResponsePacket);
			return 0;

		default:
			return -1;
	}
}

uint32_t CalculateCRC32(const PacketHeader& header, const Buffer& buffer)
{
	SLI_ASSERT(reinterpret_cast<const uint8_t*>(&header) + sizeof(header) == buffer.Begin(), "Packet is non contigious!");
	//Don't count the CRC32 when calculating the checksum
	const uint8_t* begin = reinterpret_cast<const uint8_t*>(&header) + sizeof(header.CRC32);

	uint32_t size = SizeOfPacketData(header, buffer);
	const uint8_t* end = buffer.Begin() + size;
	return CRC32Impl(begin, end - begin);
}

