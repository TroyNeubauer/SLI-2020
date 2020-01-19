
#include "Module.h"


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

