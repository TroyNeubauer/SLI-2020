#include "Modules/GPS.h"
#include "Formatter.h"

#include "Modules/CommunicationsBoard.h"

#include "stm32f1xx.h"

//The max NMEA sentence length should be 82 but we need to verify this
uint8_t gpsBuf[128];

GPS *gpsPtr = nullptr;

void GPS::Init()
{
	gpsPtr = this;
	Trace("GPS::Init");
	NMEASend("PMTK251,115200");
	Trace("Sent GPS baud rate change command");

	LL_USART_DeInit(m_GPSUART);

	LL_USART_InitTypeDef USART_InitStruct =
	{ 0 };

	LL_USART_Disable(USART1);
	USART_InitStruct.BaudRate = 115200;
	USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
	USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
	USART_InitStruct.Parity = LL_USART_PARITY_NONE;
	USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
	USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
	LL_USART_Init(USART1, &USART_InitStruct);
	LL_USART_Enable(USART1);

	Info("Changed GPS baud rate to 115200 b/s");

	HAL_Delay(1);

	//NMEASend("PMTK220,100");
	//Info("Set GPS to 10Hz");

	NMEASend("PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0");
}

uint32_t startTicks;

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
	SLI_ASSERT(chars < (int ) sizeof(buf), "NMEASend buffer overflow");
	Info("Sending command to GPS:");
	Info(buf);
//	HAL_UART_Transmit(m_GPSUART, reinterpret_cast<uint8_t*>(buf), chars, HAL_MAX_DELAY);

}

