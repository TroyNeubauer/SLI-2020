#pragma once

#include "Module.h"

#include "stm32f2xx_hal.h"
#include "stm32f2xx_hal_spi.h"
#include "stm32f2xx_hal_uart.h"

class SensorsBoard : public SLICoreModule
{
public:
	SensorsBoard();

	void Init(SPI_HandleTypeDef* spi1, UART_HandleTypeDef* uart4, UART_HandleTypeDef* uart5);

	virtual void Update() override;

private:
	virtual void RoutePacket(const PacketHeader& header, Buffer& packet);


};
