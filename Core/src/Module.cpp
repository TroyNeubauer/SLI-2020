
#include "Module.h"
#include "Formatter.h"

void SLIModule::SendPacket(const PacketHeader& header, Buffer& packet)
{
	m_CoreModule->SendPacket(header, packet);
}

void SLICoreModule::SendPacket(const PacketHeader& header, Buffer& packet)
{
	if (HasModule(header.Destination))
	{
		DeliverLocalPacket(header, packet);
	}
	else
	{
		RoutePacket(header, packet);
	}
}

void SLICoreModule::RecievePacket(const PacketHeader& header, Buffer& packet)
{
	if (HasModule(header.Destination))
	{
		DeliverLocalPacket(header, packet);
	}
	else
	{
		RoutePacket(header, packet);
	}
}


//Delivers a packet to its local destination
void SLICoreModule::DeliverLocalPacket(const PacketHeader& header, Buffer& packet)
{
	int intID = static_cast<int>(header.Destination);
	if (intID >= static_cast<int>(ModuleID::MAX_MODULE_ID) || intID < 0 || !HasModule(header.Destination))
	{
		//TODO. Fail
	}
	else
	{
		m_ContainedModules[intID]->RecievePacket(header, packet);
	}
}



bool SLICoreModule::HasModule(ModuleID id)
{
	int intID = static_cast<int>(id);
	if (intID >= static_cast<int>(ModuleID::MAX_MODULE_ID) || intID < 0)
	{
		//TODO handle error
		return false;
	}
	else
	{
		return m_ContainedModules[intID] != nullptr;
	}
}

void SLICoreModule::UpdateLocalModules()
{
	for (unsigned int i = 0; i < m_ContainedModules.size(); i++)
	{
		SLIModule* module = m_ContainedModules[i];
		if (module) module->Update();
	}
}

uint16_t SizeOfPacket(const PacketHeader& header, Buffer& packet)
{
	return 0;
}

void LogInfo(ModuleID module, const char* message)
{
	SizedFormatter<256> formatter;
	formatter << '[' << GetModuleIDName(module) << "]: " << message;
}

void LogWarn(ModuleID module, const char* message)
{
	SizedFormatter<256> formatter;
	formatter << '[' << GetModuleIDName(module) << "] warn: " << message;
}

void LogError(ModuleID module, const char* message)
{
	SizedFormatter<256> formatter;
	formatter << '[' << GetModuleIDName(module) << "] ERROR!: " << message;
}

const char* GetModuleIDName(ModuleID id)
{
	switch(id)
	{
		case ModuleID::NONE:				return "None";
		case ModuleID::GROUND_STATION:		return "Ground Station";
		case ModuleID::STM32F205:			return "STM F205";
		case ModuleID::STM32F103:			return "STM F103";
		case ModuleID::ALT1:				return "Altimeter 1";
		case ModuleID::ALT2:				return "Altimeter 2";
		case ModuleID::GPS:					return "GPS";
		case ModuleID::RADIO:				return "Radio";
		case ModuleID::ACCEL:				return "Accelerometer";
		case ModuleID::SD_CARD:				return "SD Card";
		case ModuleID::MAX_MODULE_ID:		return "INVALID Max module id";
		default:
		{
			static SizedFormatter<64> buf;
			buf << "Unknown module ID! value: " << static_cast<uint16_t>(id);
			return buf.c_str();
		}
	}
}

