#include "gspch.h"

#include "Module.h"
#include "Packet.h"
#include "Formatter.h"

#include <algorithm>

#include "Buffer.h"

SLICoreModule::SLICoreModule(ModuleIDType self) : m_ModuleID(self)
{
	std::fill(m_ContainedModules.begin(), m_ContainedModules.end(), nullptr);
}

void SLIModule::SendPacket(PacketBuffer& packet)
{
	m_CoreModule->SendPacket(packet);
}

void SLICoreModule::SendPacket(PacketBuffer& packet)
{
	if (HasModule(packet.Header()->Destination))
	{
		DeliverLocalPacket(packet);
	}
	else
	{
		RoutePacket(packet);
	}
}

void SLICoreModule::RecievePacket(PacketBuffer& packet)
{
	if (HasModule(packet.Header()->Destination))
	{
		DeliverLocalPacket(packet);
	}
	else
	{
		RoutePacket(packet);
	}
}


//Delivers a packet to its local destination
void SLICoreModule::DeliverLocalPacket(PacketBuffer& packet)
{
	int intID = static_cast<int>(packet.Header()->Destination);
	if (intID >= static_cast<int>(ModuleID::MAX_MODULE_ID) || intID < 0 || !HasModule(packet.Header()->Destination))
	{
		//TODO. Fail
	}
	else
	{
		m_ContainedModules[intID]->RecievePacket(packet);
	}
}



bool SLICoreModule::HasModule(ModuleIDType id)
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


void SLICoreModule::AddModule(SLIModule* module)
{
	SLI_FASSERT(m_ContainedModules[module->GetIntID()] == nullptr, f << "Module already exists! " << GetModuleIDName(module->GetID()));
	m_ContainedModules[module->GetIntID()] = module;
	DefaultFormatter formatter;
	formatter << "Added module! ID " << module->GetIntID();
	Info(formatter);
	module->Init();
}

SLIModule* SLICoreModule::GetModule(ModuleIDType id)
{
	int intId = static_cast<int>(id);
	SLI_FASSERT(m_ContainedModules[intId], f << "Module " << GetModuleIDName(id) << "Doesnt exist on board!");

	return m_ContainedModules[intId];
}



Formatter BeginMessage(const char* device, LogLevelType level)
{
	SizedFormatter<256> result;
	result << '[';
	result << device;
	if (level != LL_TRACE)
	{
		result << ' ';
		if (level == LL_INFO)
			result << "info";
		else if (level == LL_WARN)
			result << "Warn";
		else if (level == LL_ERROR)
			result << "ERROR";
		else
			result << "Unknown level";
	}

	result << "]: ";
	return result;
}


void SLILogable::SendDebugMessage(Formatter&& formatter, LogLevelType level)
{
	SerialPrint(std::move(formatter), level);
}


void SLILogable::Trace(Formatter&& formatter)
{
	SendDebugMessage(std::move(formatter), LL_TRACE);
}

void SLILogable::Info(Formatter&& formatter)
{
	SendDebugMessage(std::move(formatter), LL_INFO);

}

void SLILogable::Warn(Formatter&& formatter)
{
	SendDebugMessage(std::move(formatter), LL_WARN);

}

void SLILogable::Error(Formatter&& formatter)
{
	SendDebugMessage(std::move(formatter), LL_ERROR);
}


const char* GetModuleIDName(ModuleIDType id)
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

