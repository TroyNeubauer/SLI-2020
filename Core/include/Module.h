#pragma once

#include <cstdint>

#include <type_traits>
#include <array>
#include <cstddef>


#include "Buffer.h"
#include "Formatter.h"

enum class ModuleID
{
	NONE = 0, GROUND_STATION = 1, STM32F205 = 2, STM32F103 = 3,
	ALT1 = 10, ALT2 = 11, GPS = 12, RADIO = 13, ACCEL = 14, SD_CARD = 15,
	MAX_MODULE_ID = 16,
};

const char* GetModuleIDName(ModuleID id);


using LogLevelType = uint8_t;

constexpr LogLevelType LL_TRACE = 0;
constexpr LogLevelType LL_INFO = 1;
constexpr LogLevelType LL_WARN = 2;
constexpr LogLevelType LL_ERROR = 3;


//Implemented by each parent-module
extern void SerialPrint(Formatter&& formatter, LogLevelType level);
extern const char* GetParentModuleName();


class SLILogable
{
public:
	virtual ModuleID GetID() const = 0;

	virtual ~SLILogable() {}

	void SendDebugMessage(Formatter&& formatter, LogLevelType level);

	void Trace(Formatter&& formatter);
	void Info(Formatter&& formatter);
	void Warn(Formatter&& formatter);
	void Error(Formatter&& formatter);

	inline void Trace(Formatter& formatter) { Trace(std::move(formatter)); }
	inline void Info(Formatter& formatter) { Info(std::move(formatter)); }
	inline void Warn(Formatter& formatter) { Warn(std::move(formatter)); }
	inline void Error(Formatter& formatter) { Error(std::move(formatter)); }

};

class SLICoreModule;
class PacketHeader;

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
//Eg. the Ground Station, the STM32F103, and the STM32F205
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
	SLIModule* GetModule(ModuleID id);

	virtual ModuleID GetID() const { return m_ModuleID; }


	virtual ~SLICoreModule() {}

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

extern SLICoreModule* GetDefaultCoreModule();

