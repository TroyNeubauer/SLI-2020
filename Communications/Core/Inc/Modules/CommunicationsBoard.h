#pragma once

#include "Module.h"

#include "stm32f1xx_hal.h"

class CommunicationsBoard : public SLICoreModule
{
public:
	CommunicationsBoard(SPI_HandleTypeDef* spi1, UART_HandleTypeDef* radioUART, UART_HandleTypeDef* GPSUART);

	virtual void Init();
	virtual void Update();

public:
	static CommunicationsBoard& GetInstance();


private:
	virtual void RoutePacket(const PacketHeader& header, Buffer& packet);

private:
	SPI_HandleTypeDef* m_SPI1;

	UART_HandleTypeDef* m_RadioUART;
	UART_HandleTypeDef* m_GPSUART;
};

