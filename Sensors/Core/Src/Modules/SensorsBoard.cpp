#include "Modules/SensorsBoard.h"
#include "Packet.h"
#include "Module.h"
#include "Core.h"

#include "main.h"

static SensorsBoard* s_Instance = nullptr;


SensorsBoard& SensorsBoard::GetInstance()
{
	SLI_ASSERT(s_Instance, "Cannot call CommunicationsBoard::GetInstance when instance is null!");

	return *s_Instance;
}

SensorsBoard::SensorsBoard(UART_HandleTypeDef* f103UART)
	: SLICoreModule(ModuleID::STM32F103), m_F103UART(f103UART)
{
	s_Instance = this;
}


void SensorsBoard::Init()
{
	Info("Calling CommunicationsBoard::Init");
}

void SensorsBoard::Update()
{
	UpdateLocalModules();

}

void SensorsBoard::RoutePacket(PacketBuffer& packet)
{
	PacketHeader* header = packet.Header();
	if (header->Destination == ModuleID::GROUND_STATION || header->Destination == ModuleID::STM32F103)
	{
		UARTWrite(m_F103UART, packet.Begin(), packet.Offset());

	}
	else
	{
		//TODO: handle error
		DefaultFormatter formatter;
		formatter << "Unknown destination " << static_cast<uint32_t>(header->Destination) << ", Corrupt packet?";
		Warn(formatter);
	}
}

