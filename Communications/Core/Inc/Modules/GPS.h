#pragma once

#include "Modules/CommunicationsBoard.h"

class GPS : public SLIModule
{
public:

	GPS(SLICoreModule* core, UART_HandleTypeDef* gps) : SLIModule(core, ModuleID::GPS), m_gpsUART(gps) {}
	virtual void Init();
	virtual void Update();
	virtual void RecievePacket(PacketBuffer& packet);

	//Sends a command to the GPS after adding the $ character at the beginning and adding the calculated checksum at the end
	void NMEASend(const char* command);

	virtual ~GPS() {}

//private:
//Only public for the sake of calling from C
	void DMA1_C5_IRQHandler();
	void UART_IRQHandler();

private:
	void CheckRX();


private:
	UART_HandleTypeDef* m_gpsUART;

};

