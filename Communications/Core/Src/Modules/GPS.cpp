
#include "Modules/GPS.h"
#include "Formatter.h"

#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"

char gpsBuf[96];

void GPS::Init()
{

}

void GPS::Update()
{
	Info("Called GPS::Update");
	uint32_t startTicks = HAL_GetTick();
	HAL_UART_Receive(m_GPSUART, (uint8_t*) gpsBuf, sizeof(gpsBuf) - 4, HAL_MAX_DELAY);
	SizedFormatter<64> formatter;
	formatter << "Read GPS! Took: " << (HAL_GetTick() - startTicks) << "ms. Data is " << (uint16_t) gpsBuf[0] << (uint16_t) gpsBuf[1] << (uint16_t) gpsBuf[2] << (uint16_t) gpsBuf[3] << (uint16_t) gpsBuf[4];
	Info(formatter.c_str());
	gpsBuf[sizeof(gpsBuf) - 4] = 0x00;
	Info(gpsBuf);

}

void GPS::SendPacket(const PacketHeader& header, Buffer& packet)
{

}

void GPS::RecievePacket(const PacketHeader& header, Buffer& packet)
{

}

