
#include "CommunicationsBoard.h"


void CommunicationsBoard::Init()
{

}

void CommunicationsBoard::Update()
{
	//Update all the modules
	UpdateLocalModules();
}

void CommunicationsBoard::RoutePacket(const PacketHeader& header, Buffer& packet)
{
	if (header.Destination == ModuleID::GROUND_STATION)
	{
		uint8_t* buffer = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&header));
		HAL_SPI_Transmit(SPI, buffer, sizeof(header) + SizeOfPacket(header, packet), 0);
	}
	else
	{
		//Forward to the sensor board


	}
}

