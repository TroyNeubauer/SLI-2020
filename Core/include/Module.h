#pragma once

#include <cstdint>

#include <type_traits>
#include <array>

#include "Buffer.h"
#include "Formatter.h"

enum class ModuleID
{
	NONE = 0, GROUND_STATION = 1, STM32F205 = 2, STM32F103 = 3,
	ALT1 = 10, ALT2 = 11, GPS = 12, RADIO = 13, ACCEL = 14, SD_CARD = 15,
	MAX_MODULE_ID = 16,
};

const char* GetModuleIDName(ModuleID id);

using PacketTypeValue = uint8_t;

namespace PacketType {
	//Indicates that a certain module has initialized
	//Only sent once
	//This is the first packet that will be sent for a device
	constexpr PacketTypeValue INIT = 0;

	//The module has entered a different status (GPS lock, accelerometer mode set, etc.)
	constexpr PacketTypeValue STATUS = 1;

	//This module is sending new data (altitude reading, velocity, etc.)
	constexpr PacketTypeValue DATA = 2;

	//A string message of a particular log level
	constexpr PacketTypeValue MESSAGE = 3;

	//A device is asking another device for something
	constexpr PacketTypeValue REQUEST = 4;

	//A device is asking another device for something
	constexpr PacketTypeValue RESPONSE = 5;


	//Used to identify
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

using LogLevelType = uint8_t;

namespace LogLevel {

	constexpr LogLevelType TRACE = 0;
	constexpr LogLevelType INFO = 1;
	constexpr LogLevelType WARN = 2;
	constexpr LogLevelType ERROR = 3;

}

struct PacketHeader
{
	uint32_t ID;
	uint32_t UnixSeconds;//The time this packet was sent at
	uint32_t NanoSeconds;

	//Where the packet originally came from
	ModuleID Destination;
	ModuleID Forwarder;

	ModuleID From;
	PacketTypeValue Type;

};


Formatter BeginMessage(const char* device, LogLevelType level);

//Implemented by each parent-module
void SerialPrint(Formatter& formatter);
const char* GetParentModuleName();


class SLILogable
{
public:
	virtual ModuleID GetID() const = 0;

	virtual ~SLILogable() {}

protected:
	inline Formatter BeginDeviceMessage(LogLevelType level) { return BeginMessage(GetModuleIDName(GetID()), level); }
	void SendDebugMessage(Formatter& formatter);

	void Trace(const char* message);
	void Info(const char* message);
	void Warn(const char* message);
	void Error(const char* message);
};

class SLICoreModule;

//An SLI module represents a sensor or processor that can send and receive packets
//Eg. the radio, GPS, altimeter, etc
class SLIModule : public SLILogable
{
public:
	SLIModule(SLICoreModule* core, ModuleID moduleID) : m_CoreModule(core), m_ModuleID(moduleID) {}

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void RecievePacket(const PacketHeader& header, Buffer& packet) = 0;
	virtual inline ModuleID GetID() const { return m_ModuleID; }

	inline int32_t GetIntID() const { return static_cast<int32_t>(m_ModuleID); }
	void SendPacket(const PacketHeader& header, Buffer& packet);
	void Log(const char* message);

	virtual ~SLIModule() {}


private:

	SLICoreModule* m_CoreModule;
	ModuleID m_ModuleID;
};



//A core module represents a module that can physically send and receive packets
//Eg. the ground station, the STM32F103, and the STM32F205
class SLICoreModule : public SLILogable
{
public:
	SLICoreModule(ModuleID self);

	virtual void Update() = 0;

	//Called by modules to send packets
	virtual void SendPacket(const PacketHeader& header, Buffer& packet);

	//Called by the user in Update to handle when a packet is received
	//Manages calling RecievePacket on the local module or RoutePacket
	//to get the packet to its final destination
	virtual void RecievePacket(const PacketHeader& header, Buffer& packet);

	//Returns true if this module is directly connected to this device
	bool HasModule(ModuleID id);
	void AddModule(SLIModule* module);

	virtual ModuleID GetID() const { return m_ModuleID; }


	virtual ~SLICoreModule() {}

	inline Formatter BeginDeviceMessage(LogLevelType level) { return BeginMessage(GetModuleIDName(m_ModuleID), level); }

protected:
	void UpdateLocalModules();

private:
	//Re-sends a packet across the wire/radio to the intended recipient
	virtual void RoutePacket(const PacketHeader& header, Buffer& packet) = 0;

	//Delivers a packet to its local destination
	void DeliverLocalPacket(const PacketHeader& header, Buffer& packet);


protected:
	//Either a pointer to the module if is packets can be sent directly to it
	//Or nullptr if a packet being sent there needs routing
	std::array<SLIModule*, static_cast<int>(ModuleID::MAX_MODULE_ID)> m_ContainedModules;

private:
	ModuleID m_ModuleID;
};

class Buffer;

uint16_t SizeOfPacket(const PacketHeader& header, Buffer& buffer);


