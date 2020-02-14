#include "Modules/GPS.h"
#include "Formatter.h"

#include "Modules/CommunicationsBoard.h"

#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"

//The max NMEA sentence length should be 82 but we need to verify this
uint8_t gpsBuf[128];

uint8_t temp;
uint8_t lastTemp = 0;

uint32_t lastTicks;

uint32_t gpsIndex;

void transferComplete(UART_HandleTypeDef* def)
{
	gpsBuf[gpsIndex++] = temp;
	if (gpsIndex >= sizeof(gpsBuf)) gpsIndex = 0;

	lastTemp = temp;
	HAL_UART_Receive_IT(def, &temp, 1);

	SizedFormatter<128> formatter;
	formatter << "index " << gpsIndex << ", char " << (char) temp;
	if (lastTemp == '\r' && temp == '\n')
	{
		formatter << "\nEND OF SEQUENCE!!!";
	}

	CommunicationsBoard::GetInstance().GetModule(ModuleID::GPS)->Info(formatter.c_str());



}

void transferError(UART_HandleTypeDef* def)
{

}

void GPS::Init()
{
	Trace("GPS::Init");
	NMEASend("PMTK251,115200");
	Trace("Sent GPS baud rate change command");

	HAL_UART_DeInit(m_GPSUART);
	HAL_Delay(1);

	m_GPSUART->Init.BaudRate = 115200;
	if (HAL_UART_Init(m_GPSUART) != HAL_OK)
	{
		Error("Failed to change GPS baud rate to 115200 b/s!");
	}
	else
	{
		Info("Successfully changed GPS baud rate to 115200 b/s");
	}
	HAL_Delay(1);

	//NMEASend("PMTK220,100");
	//Info("Set GPS to 10Hz");

	NMEASend("PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0");

	lastTicks = HAL_GetTick();
	m_GPSUART->RxCpltCallback = transferComplete;
	HAL_UART_Receive_IT(m_GPSUART, &temp, 1);
}



void GPS::Update()
{

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

