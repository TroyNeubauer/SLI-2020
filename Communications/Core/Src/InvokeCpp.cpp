

#include "stm32f1xx.h"
#include "stm32f1xx_hal_crc.h"

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
CRC_HandleTypeDef* s_CRC32 = nullptr;

GPS* gpsPtr = nullptr;

void SerialPrint(Formatter& formatter, LogLevelType level);
void SerialPrint(Formatter&& formatter, LogLevelType level);

const char* GetParentModuleName();

extern "C"
{
	SizedFormatter<256> cLog;

	void InvokeCpp(USART_TypeDef* radioUart, USART_TypeDef* GPSUart, CRC_HandleTypeDef* crc32)
	{
		s_RadioUart = radioUart;
		s_GPSUart = GPSUart;
		s_CRC32 = crc32;

		int last = HAL_GetTick();

		SerialPrint(cLog, LL_INFO);

		CommunicationsBoard board(radioUart, GPSUart);
		boardPtr = &board;
		board.Init();

		GPS gps(&board, GPSUart);

		board.AddModule(&gps);
		board.Info("Starting loop");
		while(true)
		{
			board.Info("Looping...");
			Lights(1);
			board.Update();

			if (HAL_GetTick() - last > 1250)
			{
				LL_GPIO_TogglePin(GPIOB, LL_GPIO_AF_EVENTOUT_PIN_12);
				last = HAL_GetTick();
			}

		}
	}


	void CLog(const char* message)
	{
		cLog << '[' << GetParentModuleName() << " (C code)]: ";
		cLog << message << '\n';
	}

	void DMA1_Channel5_IRQHandler_USER()
	{
		gpsPtr->DMA1_C5_IRQHandler();
	}

	void USART1_IRQHandler_USER()
	{
		gpsPtr->UART_IRQHandler();
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

	StackBuffer<sizeof(PacketHeader) + sizeof(MessagePacket) + 256> buf;

	PacketHeader* header = buf.Header();
	header->Destination = ModuleID::GROUND_STATION;
	header->Forwarder = ModuleID::NONE;
	header->From = ModuleID::STM32F103;
	header->NanoSeconds = 69;
	header->UnixSeconds = 420;
	header->Type = PacketType::MESSAGE;
	header->ID = 0;

	MessagePacket* messagePacket = buf.Ptr<MessagePacket>();
	messagePacket->Level = level;
	messagePacket->MessageLength = formatter.Size();

	char* destMessage = buf.As<char>(formatter.Size());
	memcpy(destMessage, formatter.Data(), formatter.Size());

	header->CRC32 = buf.CalculateCRC32();


	uint32_t length = buf.Offset() + formatter.Size();
	UARTWrite(RADIO_UART, DMA1, RADIO_DMA_CHANNEL_TX, reinterpret_cast<const char*>(buf.Begin()), length);


}

void SLIAssertFailed(const char* message)
{
	SizedFormatter<256> formatter;
	formatter << "ASSERTION FAILED!!\n" << message;
	SerialPrint(formatter, LL_ERROR);
	My_Error_Handler();
}

const char* GetParentModuleName()
{
	return "stmF103";
}
