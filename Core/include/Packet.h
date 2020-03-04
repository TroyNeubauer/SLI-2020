#pragma once

#include <cstdint>

#include "Module.h"

using PacketTypeValue = uint8_t;

namespace PacketType {
	//Indicates that a certain module has initialized
	//Only sent once
	//This is the first packet that will be sent for a device
	constexpr PacketTypeValue INIT = 1;

	//The module has entered a different status (GPS lock, accelerometer mode set, etc.)
	constexpr PacketTypeValue STATUS = 2;

	//This module is sending new data (altitude reading, velocity, etc.)
	constexpr PacketTypeValue DATA = 3;

	//A string message of a particular log level
	constexpr PacketTypeValue MESSAGE = 4;

	//A device is asking another device for something
	constexpr PacketTypeValue REQUEST = 5;

	//A device is asking another device for something
	constexpr PacketTypeValue RESPONSE = 6;


	//Used to identify array sizes (not an actual enum value)
	constexpr PacketTypeValue MAX_PACKET_TYPE = 7;
}

using RequestTypeValue = uint8_t;

namespace RequestType {

	constexpr RequestTypeValue NONE = 0;
	constexpr RequestTypeValue WAKE = 1;
}

using StatusTypeValue = uint8_t;

namespace StatusValue {

	constexpr StatusTypeValue GPS_LOCK = 0;
	constexpr StatusTypeValue GPS_10HZ = 1;
	constexpr StatusTypeValue GPS_115200_BAUD_RATE = 1;

}


struct PacketHeader
{
	uint32_t CRC32;

	uint32_t ID;
	uint32_t UnixSeconds;//The time this packet was sent at
	uint32_t NanoSeconds;

	//Where the packet originally came from
	ModuleID Destination;
	ModuleID Forwarder;

	ModuleID From;
	PacketTypeValue Type;

};

static_assert(offsetof(PacketHeader, CRC32) == 0, "CRC must start at offset 0");


class Buffer;

const int MAX_PACKET_DATA_SIZE = 256;

//Returns the number of bytes of data in buffer
int16_t SizeOfPacketData(const PacketHeader& header, const Buffer& buffer);

uint32_t CalculateCRC32(const PacketHeader& header, const Buffer& buffer);

uint32_t CRC32Impl(const uint8_t* data, std::size_t bytes);


//All the data needed is inside the packet header
struct InitPacket
{
	uint32_t Reserved = 0xDEADBEEF;
};

struct StatusPacket
{
	StatusTypeValue Status;
};


const uint32_t MAX_NMEA_LENGTH = 90;//TODO. Double check this

struct DataPacket_GPS
{
	uint16_t NMEASentenceLength;//How many bytes the string written after this struct is
	/*
	const char* NMEASentence;//Encoded after this struct
	*/
};

struct DataPacket_STMF103
{

};

struct DataPacket_STMF205
{

};

const uint32_t MAX_MESSAGE_LENGTH = 256;

struct MessagePacket
{
	LogLevelType Level;
	uint16_t MessageLength;//How many bytes the string written after this struct is
	/*
	const char* Message;//Encoded after this struct
	*/
};
