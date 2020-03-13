#pragma once

#include "Module.h"
#include "Buffer.h"

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_uart.h"
#include "stm32f1xx_hal_dma.h"
#include "stm32f1xx_hal_dma_ex.h"

class SDCard;

class CommunicationsBoard : public SLICoreModule
{
public:
	CommunicationsBoard(UART_HandleTypeDef* radioUART, UART_HandleTypeDef* GPSUART);

	virtual void Init();
	virtual void Update();
	inline void SetSDCard(SDCard* sd) {m_SDCard = sd;}

public:
	static CommunicationsBoard& GetInstance();


private:
	virtual void RoutePacket(PacketBuffer& packet);

private:
	UART_HandleTypeDef* m_RadioUART;
	UART_HandleTypeDef* m_GPSUART;
	SDCard* m_SDCard = nullptr;
};

