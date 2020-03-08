#pragma once

#include "Module.h"
#include "Buffer.h"

#include "stm32f2xx_hal.h"
#include "stm32f2xx_hal_uart.h"
#include "stm32f2xx_hal_dma.h"
#include "stm32f2xx_hal_dma_ex.h"

class SensorsBoard : public SLICoreModule
{
public:
	SensorsBoard(UART_HandleTypeDef* f103UART);

	virtual void Init();
	virtual void Update();

public:
	static SensorsBoard& GetInstance();


private:
	virtual void RoutePacket(PacketBuffer& packet);

private:
	UART_HandleTypeDef* m_F103UART;
};

