#pragma once

#include <stdint.h>

#include <type_traits>
#include <array>

#include "Buffer.h"

enum class ModuleID
{
	NONE = 0, GROUND_STATION = 1, STM32F205 = 2, STM32F103 = 3,
	ALT1 = 10, ALT2 = 11, GPS = 12, RADIO = 13, ACCEL = 14, SD_CARD = 15,
	MAX_MODULE_ID = 16,
};

using PacketType = uint8_t;

//Indicates that a certain module has initialized
//Only sent once
//This is the first packet that will be sent for a device
constexpr PacketType INIT = 0;

//The module has entered a different status (GPS lock, accelerometer mode set, etc.)
constexpr PacketType STATUS = 1;

//This module is sending new data (altitude reading, velocity, etc.)
constexpr PacketType DATA = 2;

//Some type of error has occurred
constexpr PacketType ERROR = 3;

//A device is asking another device for something
constexpr PacketType REQUEST = 4;

//A device is asking another device for something
constexpr PacketType RESPONSE = 5;

//Used to identify
constexpr PacketType MAX_PACKET_TYPE = 6;


using RequestType = uint8_t;

constexpr RequestType NONE = 0;
constexpr RequestType WAKE = 1;


struct PacketHeader
{
	uint32_t ID;
	uint32_t UnixSeconds;//The time this packet was sent at
	uint32_t NanoSeconds;
	//Where the packet originally came from
	ModuleID Destination;
	ModuleID Forwarder;

	ModuleID From;
	PacketType Type;

};

class SLICoreModule;

//An SLI module represents a sensor or processor that can send and receive packets
//Eg. the radio, GPS, altimeter, etc
class SLIModule
{
public:
	SLIModule(SLICoreModule* core) : m_CoreModule(core) {}

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void SendPacket(const PacketHeader& header, const Buffer& packet);
	virtual void RecievePacket(const PacketHeader& header, Buffer& packet) = 0;
	virtual ModuleID GetID() = 0;

private:
	SLICoreModule* m_CoreModule;
};


//A core module represents a module that can physically send and receive packets
//Eg. the ground station, the STM32F103, and the STM32F205
class SLICoreModule
{
public:
	virtual void Init() = 0;
	virtual void Update() = 0;

	//Called by modules to send packets
	virtual void SendPacket(const PacketHeader& header, const Buffer& packet);
	virtual void RecievePacket(const PacketHeader& header, Buffer& packet);

	//Returns true if modules can be
	bool HasModule(ModuleID id);

private:
	//Re-sends a packet across the wire/radio to the intended recipient
	void RoutePacket(const PacketHeader& header, const Buffer& packet) = 0;

private:
	//Delivers a packet to its local destination
	void DeliverPacket(const PacketHeader& header, const Buffer& packet);


protected:
	//Either a pointer to the module if is packets can be sent directly to it
	//Or nullptr if a packet being sent there needs routing
	std::array<SLIModule*, (int) ModuleID::MAX_MODULE_ID> m_ContainedModules;
};


