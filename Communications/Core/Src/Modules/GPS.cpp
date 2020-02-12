#include "Modules/GPS.h"
#include "Formatter.h"

#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"

//The max NMEA sentence length should be 82 but we need to verify this
uint8_t gpsBuf[96];

void GPS::Init()
{
	Info("GPS::Init");
	NMEASend("PMTK251,115200");
	Info("Sent GPS baud rate change command");

	HAL_UART_DeInit(m_GPSUART);

	m_GPSUART->Init.BaudRate = 115200;
	if (HAL_UART_Init(m_GPSUART) != HAL_OK)
	{
		Error("Failed to change GPS baud rate to 115200 b/s!");
	}
	else
	{
		Info("Successfully changed GPSbaud rate to 115200 b/s");
	}

	NMEASend("PMTK220,100");
	Info("Sent GPS to 10Hz");

	NMEASend("PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0");
}

void GPS::Update()
{
	Info("Called GPS::Update");
	uint32_t startTicks = HAL_GetTick();
	HAL_UART_Receive(m_GPSUART, (uint8_t*) gpsBuf, sizeof(gpsBuf), HAL_MAX_DELAY);


	SizedFormatter<64> formatter;
	formatter << "Read GPS! Took: " << (HAL_GetTick() - startTicks) << "ms | ";
	bool fix = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_SET;
	formatter << "Has fix: " << fix << '\n';
	Info(formatter.c_str());

}

void GPS::RecievePacket(const PacketHeader &header, Buffer &packet)
{

}

void GPS::NMEASend(const char *command)
{
	//Make sure the user excluded the dollar sign because we add it
	SLI_ASSERT(command[0] != '$', "$ in NMEASend");

	//Calculate NMEA checksum
	unsigned int checksum = 0;
	const char *copy = command;
	while (char c = *copy)
	{
		SLI_ASSERT(c != '*', "* in NMEASend"); //Make sure no checksum is specified
		checksum ^= c;
		copy++;
	}

	//Encode the actual command
	char buf[128];
	int chars = snprintf(buf, sizeof(buf), "$%s*%02X\r\n", command, checksum);
	SLI_ASSERT(chars != -1, "NMEASend snprintf error");
	SLI_ASSERT(chars < (int) sizeof(buf), "NMEASend buffer overflow");
	Info("Sending command to GPS:");
	Info(buf);
	HAL_UART_Transmit(m_GPSUART, reinterpret_cast<uint8_t*>(buf), chars, HAL_MAX_DELAY);

}

