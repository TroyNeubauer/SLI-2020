#pragma once

#include "Module.h"

#include "stm32f1xx_hal.h"


class GPS : public SLIModule
{
public:

	GPS(SLICoreModule* core, UART_HandleTypeDef* gps) : SLIModule(core, ModuleID::GPS), m_GPSUART(gps) {}
	virtual void Init();
	virtual void Update();
	virtual void SendPacket(const PacketHeader& header, Buffer& packet);
	virtual void RecievePacket(const PacketHeader& header, Buffer& packet);

	virtual ~GPS() {}

private:
	UART_HandleTypeDef* m_GPSUART;
};

