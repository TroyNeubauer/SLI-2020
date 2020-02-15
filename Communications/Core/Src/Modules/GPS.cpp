#include "Modules/GPS.h"
#include "Formatter.h"

#include "Modules/CommunicationsBoard.h"

#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"

//The max NMEA sentence length should be 82 but we need to verify this
uint8_t gpsBuf[128];

GPS *gpsPtr = nullptr;

void GPS::Init()
{
	gpsPtr = this;
	Trace("GPS::Init");
	NMEASend("PMTK251,115200");
	Trace("Sent GPS baud rate change command");

	HAL_UART_DeInit(m_GPSUART);

	m_GPSUART->Init.BaudRate = 115200;
	if (HAL_UART_Init(m_GPSUART) != HAL_OK)
	{
		Error("Failed to change GPS baud rate to 115200 b/s!");
	} else
	{
		Info("Successfully changed GPS baud rate to 115200 b/s");
	}

	NMEASend("PMTK220,100");
	Info("Set GPS to 10Hz");

	NMEASend("PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0");
}

uint32_t startTicks;

void transferComplete(UART_HandleTypeDef *def)
{
	SizedFormatter<64> formatter;
	formatter << "TRANSFER COMPLETE Took: " << (HAL_GetTick() - startTicks) << "ms | ";
	bool fix = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_SET;
	formatter << "Has fix: " << fix << '\n' << reinterpret_cast<const char*>(gpsBuf);
	CommunicationsBoard::GetInstance().GetModule(ModuleID::GPS)->Info(formatter.c_str());

}

void transferHalfComplete(UART_HandleTypeDef *def)
{
	CommunicationsBoard::GetInstance().GetModule(ModuleID::GPS)->Info("Half complete");
}

void transferError(UART_HandleTypeDef *def)
{
	CommunicationsBoard::GetInstance().GetModule(ModuleID::GPS)->Info("Transfer error");
}

void transferAbort(UART_HandleTypeDef *def)
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

	} else
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
	SLI_ASSERT(chars < (int ) sizeof(buf), "NMEASend buffer overflow");
	Info("Sending command to GPS:");
	Info(buf);
	HAL_UART_Transmit(m_GPSUART, reinterpret_cast<uint8_t*>(buf), chars, HAL_MAX_DELAY);

}

void GPS::HandleLine(const char *buf, uint32_t size)
{
	Info("\n\nRecieved NEMA line!!!!");
	Info(buf);
}

extern "C"
{
	#define USART_GPS LPUART1
	#define USART_GPS_IRQn LPUART1_IRQn
	#define USART_GPS_IRQHandler LPUART1_IRQHandler

	#ifdef USART_CR1_TXEIE_TXFNFIE // FIFO Support (L4R9)
	#define USART_CR1_TXEIE USART_CR1_TXEIE_TXFNFIE
	#define USART_ISR_TXE USART_ISR_TXE_TXFNF
	#define USART_CR1_RXNEIE USART_CR1_RXNEIE_RXFNEIE
	#define USART_ISR_RXNE USART_ISR_RXNE_RXFNE
	#endif

	#define LINEMAX 255 // Maximal allowed/expected line length
	void USART_GPS_IRQHandler(void) // Sync and Queue NMEA Sentences
	{
		static char rx_buffer[LINEMAX + 1]; // Local holding buffer to build line, w/NUL
		static int rx_index = 0;
		if (USART_GPS->ISR & USART_ISR_ORE) // Overrun Error
			USART_GPS->ICR = USART_ICR_ORECF;
		if (USART_GPS->ISR & USART_ISR_NE) // Noise Error
			USART_GPS->ICR = USART_ICR_NCF;
		if (USART_GPS->ISR & USART_ISR_FE) // Framing Error
			USART_GPS->ICR = USART_ICR_FECF;
		if (USART_GPS->ISR & USART_ISR_RXNE) // Received character?
		{
			char rx = (char) (USART_GPS->RDR & 0xFF);
			if ((rx == '\r') || (rx == '\n')) // Is this an end-of-line condition, either will suffice?
			{
				if (rx_index != 0) // Line has some content?
				{
					rx_buffer[rx_index++] = 0; // Add NUL if required down stream
					gpsPtr->HandleLine(rx_buffer, rx_index); // Copy to queue from live dynamic receive buffer
					rx_index = 0; // Reset content pointer
				}
			} else
			{
				if ((rx == '$') || (rx_index == LINEMAX)) // If resync or overflows pull back to start
					rx_index = 0;
				rx_buffer[rx_index++] = rx; // Copy to buffer and increment
			}
		}
	}
}

