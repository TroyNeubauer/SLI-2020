#include "stm32f1xx.h"

#include <cstdlib>
#include <cstdio>
#include <chrono>

#include "Modules/CommunicationsBoard.h"
#include "Modules/GPS.h"
#include "main.h"

#include "Core.h"
#include "Packet.h"

CommunicationsBoard* boardPtr = nullptr;

USART_TypeDef* s_RadioUart = nullptr;
USART_TypeDef* s_GPSUart = nullptr;

GPS* gpsPtr = nullptr;

void SerialPrint(Formatter& formatter, LogLevelType level);
void SerialPrint(Formatter&& formatter, LogLevelType level);

const char* GetParentModuleName();

extern "C"
{
	SizedFormatter<256> cLog;

	void InvokeCpp(USART_TypeDef* radioUart, USART_TypeDef* GPSUart)
	{
		s_RadioUart = radioUart;
		s_GPSUart = GPSUart;
		Lights(1);

		SerialPrint(cLog, LL_INFO);
		HAL_Delay(100);

		Lights(2);

		CommunicationsBoard board(radioUart, GPSUart);
		boardPtr = &board;
		board.Init();

		GPS gps(&board, GPSUart);

		board.AddModule(&gps);
		board.Info("Starting loop");
		LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_13);

		int last = HAL_GetTick();
		while (true)
		{
			board.Update();
			board.Info("Looping1");
			board.Info("Looping2");
			board.Info("Looping3");

			if (HAL_GetTick() - last > 1250)
			{
				LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_12);
				last = HAL_GetTick();
			}
			HAL_Delay(200);
			board.Info("Delayed");
			HAL_Delay(5);
		}
	}

	void CLog(const char* message)
	{
		cLog << '[' << GetParentModuleName() << " (C code)]: ";
		cLog << message << '\n';
	}

	std::array<std::array<bool, 8>, 3> s_ActiveDMA = { };

	int GetDMAIndex(DMA_TypeDef* dma)
	{
		if (dma == DMA1)
			return 1;
//	else if (dma == DMA2)
//		return 2;
		else
		{
			SLI_ASSERT(false, "Unknown DMA!");
			return 0;
		}
	}

	void UART_DMA_Interupt(DMA_TypeDef* dma, uint8_t dmaChannel)
	{
		//s_ActiveDMA[GetDMAIndex(dma)][dmaChannel] = false;
		LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_12);
	}

	void UARTWrite(USART_TypeDef* usart, DMA_TypeDef* dma, uint8_t dmaChannel, const void* data, uint32_t length)
	{
		while (s_ActiveDMA[GetDMAIndex(dma)][dmaChannel])
		{
		}

		s_ActiveDMA[GetDMAIndex(dma)][dmaChannel] = true;

		LL_DMA_DisableChannel(dma, dmaChannel);

		// set length to be tranmitted
		LL_DMA_SetDataLength(dma, dmaChannel, length);

		LL_USART_EnableDMAReq_TX(usart);

		// configure address to be transmitted by DMA
		LL_DMA_ConfigAddresses(dma, dmaChannel, (uint32_t) data, (uint32_t) LL_USART_DMA_GetRegAddr(usart),
				LL_DMA_GetDataTransferDirection(dma, dmaChannel));

		// Enable DMA again
		LL_DMA_EnableChannel(dma, dmaChannel);
		LL_DMA_EnableIT_TC(dma, dmaChannel);

	}

	void UARTRead(USART_TypeDef* usart, DMA_TypeDef* dma, uint8_t dmaChannel, void* data, uint32_t length)
	{
		while (s_ActiveDMA[GetDMAIndex(dma)][dmaChannel])
		{
		}

		s_ActiveDMA[GetDMAIndex(dma)][dmaChannel] = true;

		LL_DMA_DisableChannel(dma, dmaChannel);

		LL_DMA_SetDataLength(dma, dmaChannel, length);

		LL_USART_EnableDMAReq_RX(usart);

		LL_DMA_ConfigAddresses(dma, dmaChannel, (uint32_t) data, (uint32_t) LL_USART_DMA_GetRegAddr(usart),
				LL_DMA_GetDataTransferDirection(dma, dmaChannel));

		LL_DMA_EnableIT_TC(dma, dmaChannel);

		LL_DMA_EnableChannel(dma, dmaChannel);

	}

}

void SerialPrint(Formatter& formatter, LogLevelType level)
{
	SerialPrint(std::move(formatter), level);
}

void SerialPrint(Formatter&& formatter, LogLevelType level)
{
	if (s_RadioUart == nullptr)
	{
		My_Error_Handler();
	}

	StackBuffer < sizeof(PacketHeader) + sizeof(MessagePacket) + 256 > buf;

	PacketHeader* header = buf.Header();
	header->ID = 0;
	header->UnixSeconds = 420;
	header->NanoSeconds = 69;

	header->Destination = ModuleID::GROUND_STATION;
	header->Forwarder = ModuleID::NONE;
	header->From = ModuleID::STM32F103;
	header->Type = PacketType::MESSAGE;

	MessagePacket* messagePacket = buf.Ptr<MessagePacket>();
	messagePacket->Level = level;
	messagePacket->MessageLength = formatter.Size();

	char* destMessage = buf.As<char>(messagePacket->MessageLength);
	memcpy(destMessage, formatter.Data(), messagePacket->MessageLength);

	header->CRC32 = buf.CalculateCRC32();

	UARTWrite(RADIO_UART, DMA1, RADIO_DMA_CHANNEL_TX, buf.Begin(), buf.Offset());

}

void SLIAssertFailed(const char* message)
{
	SizedFormatter<256> formatter;
	formatter << "ASSERTION FAILED!!\n" << message;
	UARTWriteSync(s_RadioUart, formatter.c_str(), formatter.Size());
	My_Error_Handler();
}

const char* GetParentModuleName()
{
	return "stmF103";
}
