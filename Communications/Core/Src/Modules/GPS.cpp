#include "Modules/GPS.h"
#include "Formatter.h"

#include "Modules/CommunicationsBoard.h"

#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"

//The max NMEA sentence length should be 82 but we need to verify this
uint8_t gpsBuf[128];

void GPS::Init()
{
	Trace("GPS::Init");
	NMEASend("PMTK251,115200");
	Trace("Sent GPS baud rate change command");

	HAL_UART_DeInit(m_GPSUART);

	m_GPSUART->Init.BaudRate = 115200;
	if (HAL_UART_Init(m_GPSUART) != HAL_OK)
	{
		Error("Failed to change GPS baud rate to 115200 b/s!");
	}
	else
	{
		Info("Successfully changed GPS baud rate to 115200 b/s");
	}

	NMEASend("PMTK220,100");
	Info("Set GPS to 10Hz");

	NMEASend("PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0");
}

uint32_t startTicks;


void transferComplete(UART_HandleTypeDef * def)
{
	SizedFormatter<64> formatter;
	formatter << "TRANSFER COMPLETE Took: " << (HAL_GetTick() - startTicks) << "ms | ";
	bool fix = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_SET;
	formatter << "Has fix: " << fix << '\n' << reinterpret_cast<const char*>(gpsBuf);
	CommunicationsBoard::GetInstance().GetModule(ModuleID::GPS)->Info(formatter.c_str());

}

void transferHalfComplete(UART_HandleTypeDef * def)
{
	CommunicationsBoard::GetInstance().GetModule(ModuleID::GPS)->Info("Half complete");
}

void transferError(UART_HandleTypeDef * def)
{
	CommunicationsBoard::GetInstance().GetModule(ModuleID::GPS)->Info("Transfer error");
}

void transferAbort(UART_HandleTypeDef * def)
{
	CommunicationsBoard::GetInstance().GetModule(ModuleID::GPS)->Info("Transfer Abort");
}


void GPS::Update()
{
	if (m_GPSUART->RxState == HAL_UART_STATE_READY)
	{
		Info("Calling HAL_UART_IRQHandler");
		HAL_UART_IRQHandler(m_GPSUART);
		Info("Starting GPS transfer");
		startTicks = HAL_GetTick();
		m_GPSUART->RxHalfCpltCallback = transferHalfComplete;
		m_GPSUART->AbortReceiveCpltCallback = transferAbort;
		m_GPSUART->RxCpltCallback = transferComplete;

		Info("Calling HAL_UART_Receive_DMA");
		HAL_UART_Receive_DMA(m_GPSUART, gpsBuf, sizeof(gpsBuf));
		Info("doing __HAL_UART_ENABLE_IT");
		__HAL_UART_ENABLE_IT(m_GPSUART, UART_IT_IDLE);
		Info("done");

	}
	else
	{
		Info("...");
	}

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

