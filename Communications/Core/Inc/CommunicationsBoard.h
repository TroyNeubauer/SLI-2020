#pragma once

#include "Module.h"

#include "stm32f1xx_hal.h"

class CommunicationsBoard : public SLICoreModule
{
public:
	CommunicationsBoard(SPI_HandleTypeDef* spi1, UART_HandleTypeDef* radioUART, UART_HandleTypeDef* GPSUART)
		: SLICoreModule(ModuleID::STM32F103), m_SPI1(spi1), m_RadioUART(radioUART), m_GPSUART(GPSUART) {}

	virtual void Init();
	virtual void Update();


private:
	virtual void RoutePacket(const PacketHeader& header, Buffer& packet);

private:
	SPI_HandleTypeDef* m_SPI1;

	UART_HandleTypeDef* m_RadioUART;
	UART_HandleTypeDef* m_GPSUART;
};

