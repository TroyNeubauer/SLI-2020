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

	//Sends a command to the GPS after adding the $ character at the beginning and adding the calculated checksum at the end
	void NMEASend(const char* command);

	virtual ~GPS() {}

private:
	UART_HandleTypeDef* m_GPSUART;
};

