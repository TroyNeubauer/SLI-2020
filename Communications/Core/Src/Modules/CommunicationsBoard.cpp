#include "Modules/CommunicationsBoard.h"

#include "stm32f1xx_hal_uart.h"

static CommunicationsBoard* s_Instance = nullptr;


CommunicationsBoard& CommunicationsBoard::GetInstance()
{
	SLI_ASSERT(s_Instance, "Cannot call CommunicationsBoard::GetInstance when instance is null!");

	return *s_Instance;
}

CommunicationsBoard::CommunicationsBoard(SPI_HandleTypeDef* spi1, UART_HandleTypeDef* radioUART, UART_HandleTypeDef* GPSUART)
	: SLICoreModule(ModuleID::STM32F103), m_SPI1(spi1), m_RadioUART(radioUART), m_GPSUART(GPSUART)
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
		uint8_t *buffer = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&header));
		HAL_UART_Transmit(m_RadioUART, buffer, sizeof(header) + SizeOfPacket(header, packet), 0);
	}
	else
	{
		//Forward to the sensor board

	}
}

