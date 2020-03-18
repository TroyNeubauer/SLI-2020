#pragma once

#include <cstdint>
#include <cstddef>

using PacketTypeValue = uint8_t;

namespace PacketType {
	//Indicates that a certain module has initialized
	//Only sent once
	//This is the first packet that will be sent for a device
	constexpr PacketTypeValue INIT = 1;

	//The module has entered a different status (GPS lock, accelerometer mode set, error status, etc.)
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
	constexpr PacketTypeValue MAX_PACKET_TYPE = 8;
}

using RequestTypeValue = uint8_t;

namespace RequestType {

	constexpr RequestTypeValue NONE = 0;
	constexpr RequestTypeValue WAKE = 1;
}

using StatusTypeValue = uint8_t;

namespace StatusValue {

	constexpr StatusTypeValue MODULE_ERROR = 0;//A module defined error code is encoded after this struct
	constexpr StatusTypeValue GPS_LOCK = 1;
	constexpr StatusTypeValue GPS_10HZ = 2;
	constexpr StatusTypeValue GPS_115200_BAUD_RATE = 3;

}

using ModuleIDType = uint8_t;

struct PacketHeader
{
	uint32_t UnixSeconds;//The time this packet was sent at
	uint32_t NanoSeconds;

	//Where the packet originally came from
	ModuleIDType Destination;
	ModuleIDType Forwarder;

	ModuleIDType From;
	PacketTypeValue Type;

};

const int MAX_PACKET_SIZE = 256;



//All the data needed is inside the packet header so there is no point in having an empty struct
/*
struct InitPacket
{
};

*/


struct StatusPacket
{
	StatusTypeValue Status;
};

static_assert(sizeof(StatusPacket) == 1, "StatusPacket is wrong size");

struct ErrorStatusPacket
{
	int32_t ErrorCode : 24;
	int32_t Operation : 8;
};

static_assert(sizeof(ErrorStatusPacket) == 4, "StatusPacket is wrong size");




struct DataPacket_GPS
{
	uint8_t NMEASentenceLength;//How many bytes the string written after this struct is
	/*
	const char* NMEASentence;//Encoded after this struct
	*/
};

static_assert(sizeof(DataPacket_GPS) == 1, "DataPacket_GPS is wrong size");


struct DataPacket_STMF103
{
	uint32_t UpdateLoopHZ;
};

struct DataPacket_STMF205
{
	uint32_t UpdateLoopHZ;
};

const uint32_t MAX_MESSAGE_LENGTH = 256;

using LogLevelType = uint8_t;

constexpr LogLevelType LL_TRACE = 0;
constexpr LogLevelType LL_INFO = 1;
constexpr LogLevelType LL_WARN = 2;
constexpr LogLevelType LL_ERROR = 3;
constexpr LogLevelType MAX_MESSAGE_LEVEL = 4;

struct MessagePacket
{
	uint16_t MessageLength;//How many bytes the string written after this struct is
	LogLevelType Level;
	uint8_t Reserved;
	/*
	const char* Message;//Encoded after this struct
	*/
};

static_assert(sizeof(MessagePacket) == 4, "MessagePacket is wrong size");


