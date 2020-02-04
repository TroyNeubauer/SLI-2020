#pragma once

#include "Module.h"

#include "stm32f1xx_hal.h"

class CommunicationsBoard : public SLICoreModule
{
public:
	CommunicationsBoard(SPI_HandleTypeDef* spi, UART_HandleTypeDef* uart1, UART_HandleTypeDef* uart2)
		: SLICoreModule(ModuleID::STM32F103), SPI(spi), UART1(uart1), UART2(uart2) {}

	virtual void Init();
	virtual void Update();


private:
	virtual void RoutePacket(const PacketHeader& header, Buffer& packet);

private:
	SPI_HandleTypeDef* SPI;

	UART_HandleTypeDef* UART1;
	UART_HandleTypeDef* UART2;
};

