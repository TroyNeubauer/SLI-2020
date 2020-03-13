#include "Modules/GPS.h"
#include "Formatter.h"
#include "main.h"

#include "Modules/CommunicationsBoard.h"

#include "stm32f1xx.h"

//The max NMEA sentence length should be 82 but we need to verify this
uint8_t gpsBuf[512];

void GPS::Init()
{
	Trace("GPS::Init");
	NMEASend("PMTK251,115200");
	Trace("Sent GPS baud rate change command");
	DelayUS(100);

	//Re init gps
	HAL_UART_DeInit(m_gpsUART);
	m_gpsUART->Init.BaudRate = 115200;
	if (HAL_UART_Init(m_gpsUART) != HAL_OK)
	{
		Error("Failed to change GPS baud rate to 115200 b/s");
		return;
	}
	Info("Changed GPS baud rate to 115200 b/s");
	NMEASend("PMTK220,100");
	NMEASend("PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0");
	UARTRead(m_gpsUART, &gpsBuf, sizeof(gpsBuf));
}

uint32_t lastPos = 0;

void GPS::Update()
{
	uint32_t currentPos = m_gpsUART->RxXferSize - __HAL_DMA_GET_COUNTER(m_gpsUART->hdmarx);
	if (currentPos != lastPos)
	{
		if (gpsBuf[currentPos - 1] == '\n')
		{
			StackBuffer<sizeof(PacketHeader) + sizeof(DataPacket_GPS) + 5 * MAX_NMEA_LENGTH> buf;

			PacketHeader* header = buf.Header();
			header->ID = 0;
			header->UnixSeconds = 420;
			header->NanoSeconds = 69;

			header->Destination = ModuleID::GROUND_STATION;
			header->Forwarder = ModuleID::NONE;
			header->From = ModuleID::GPS;
			header->Type = PacketType::DATA;

			DataPacket_GPS* messagePacket = buf.Ptr<DataPacket_GPS>();

			if (currentPos > lastPos)
			{
				uint32_t length = currentPos - lastPos;
				messagePacket->NMEASentenceLength = length;

				char* destSentence = buf.As<char>(messagePacket->NMEASentenceLength);
				memcpy(destSentence, gpsBuf + lastPos, messagePacket->NMEASentenceLength);

			}
			else
			{
				//Hard case. the data wraps around
				uint32_t endLength = sizeof(gpsBuf) - lastPos;
				uint32_t beginLength = currentPos;
				messagePacket->NMEASentenceLength = endLength + beginLength;

				char* destSentence = buf.As<char>(messagePacket->NMEASentenceLength);
				memcpy(destSentence, gpsBuf + lastPos, endLength);
				memcpy(destSentence + endLength, gpsBuf, beginLength);
			}

			header->CRC32 = buf.CalculateCRC32();

			SendPacket(buf);
			lastPos = currentPos;
		}
	}
}

void GPS::RecievePacket(PacketBuffer &packet)
{

}

void GPS::CheckRX()
{
}

void GPS::DMA1_C5_IRQHandler()
{

}

void GPS::UART_IRQHandler()
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
	UARTWriteSync(m_gpsUART, buf, chars);

}

