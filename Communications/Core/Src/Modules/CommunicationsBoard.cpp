#include "Modules/CommunicationsBoard.h"
#include "Packet.h"
#include "Module.h"

#include "main.h"

static CommunicationsBoard* s_Instance = nullptr;


CommunicationsBoard& CommunicationsBoard::GetInstance()
{
	SLI_ASSERT(s_Instance, "Cannot call CommunicationsBoard::GetInstance when instance is null!");

	return *s_Instance;
}

CommunicationsBoard::CommunicationsBoard(USART_TypeDef* radioUART, USART_TypeDef* GPSUART)
	: SLICoreModule(ModuleID::STM32F103), m_RadioUART(radioUART), m_GPSUART(GPSUART)
{
	s_Instance = this;
}


void CommunicationsBoard::Init()
{
	Info("Calling CommunicationsBoard::Init");
}

void CommunicationsBoard::Update()
{
	UpdateLocalModules();

}

void CommunicationsBoard::RoutePacket(const PacketHeader &header, Buffer &packet)
{
	if (header.Destination == ModuleID::GROUND_STATION)
	{
		const char* buffer = reinterpret_cast<const char*>(&header);
		UARTWrite(m_RadioUART, DMA1, RADIO_DMA_CHANNEL_TX, buffer, sizeof(header) + SizeOfPacketData(header, packet));

	}
	else if (header.Destination == ModuleID::STM32F205)
	{
		//Forward to the sensor board

	}
	else
	{
		//TODO: handle error
		DefaultFormatter formatter;
		formatter << "Unknown destination " << static_cast<uint32_t>(header.Destination) << ", Corrupt packet?";
		Warn(formatter);
	}
}

