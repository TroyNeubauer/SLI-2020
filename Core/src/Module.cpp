
#include "Module.h"

void SLIModule::SendPacket(const PacketHeader& header, const Buffer& packet)
{

}


bool SLICoreModule::HasModule(ModuleID id)
{
	int intID = (int) id;
	if (intID >= ModuleID::MAX_MODULE_ID || id < 0)
	{
		//TODO handle error
	}
	else
	{
		return m_ContainedModules[intID] != nullptr;
	}
}
