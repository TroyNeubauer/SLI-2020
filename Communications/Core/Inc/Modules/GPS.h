#pragma once

#include "Module.h"
#include "stm32f1xx_ll_usart.h"

class GPS : public SLIModule
{
public:

	GPS(SLICoreModule* core, USART_TypeDef* gps) : SLIModule(core, ModuleID::GPS), m_GPSUART(gps) {}
	virtual void Init();
	virtual void Update();
	virtual void RecievePacket(const PacketHeader& header, Buffer& packet);

	//Sends a command to the GPS after adding the $ character at the beginning and adding the calculated checksum at the end
	void NMEASend(const char* command);

	virtual ~GPS() {}

	void IRQHandler();


private:
	USART_TypeDef* m_GPSUART;

};

