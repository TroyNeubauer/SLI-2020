#pragma once

#include "Module.h"

//#include "stm32f1xx_ll_usart.h"

class CommunicationsBoard : public SLICoreModule
{
public:
//	CommunicationsBoard(USART_TypeDef* radioUART, USART_TypeDef* GPSUART);

	virtual void Init();
	virtual void Update();

public:
	static CommunicationsBoard& GetInstance();


private:
	virtual void RoutePacket(const PacketHeader& header, Buffer& packet);

private:
//	USART_TypeDef* m_RadioUART;
//	USART_TypeDef* m_GPSUART;
};

