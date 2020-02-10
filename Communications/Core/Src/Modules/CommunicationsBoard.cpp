#include "Modules/CommunicationsBoard.h"

#include "stm32f1xx_hal_uart.h"


//See http://stm32f4-discovery.net/2014/07/library-21-read-sd-card-fatfs-stm32f4xx-devices/


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
		uint8_t *buffer = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&header));
		HAL_UART_Transmit(m_RadioUART, buffer, sizeof(header) + SizeOfPacket(header, packet), 0);
	}
	else
	{
		//Forward to the sensor board

	}
}

