#include "stm32f1xx.h"

#include <cstdlib>
#include <cstdio>
#include <chrono>

#include "Modules/CommunicationsBoard.h"
#include "Modules/GPS.h"
#include "Modules/SDCard.h"
#include "main.h"

#include "Core.h"
#include "Packet.h"

CommunicationsBoard* boardPtr = nullptr;

UART_HandleTypeDef* s_RadioUart = nullptr;
UART_HandleTypeDef* s_gpsUart = nullptr;

GPS* gpsPtr = nullptr;
#define min(a, b) (a < b ? a : b)

extern void SerialPrint(Formatter& formatter, ModuleIDType from, LogLevelType level);
extern void SerialPrint(Formatter&& formatter, ModuleIDType from, LogLevelType level);

extern const char* GetParentModuleName();

extern "C"
{
	void InvokeCpp(UART_HandleTypeDef* radioUart, UART_HandleTypeDef* gpsUart)
	{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ERROR_LED_GPIO_Port, ERROR_LED_Pin, GPIO_PIN_SET);
		HAL_Delay(1000);
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(ERROR_LED_GPIO_Port, ERROR_LED_Pin, GPIO_PIN_RESET);


		s_RadioUart = radioUart;
		s_gpsUart = gpsUart;

		CommunicationsBoard board(radioUart, gpsUart);
		boardPtr = &board;
		board.Init();
		SDCard sdcard(&board);
		sdcard.Open("logTest1.bin");

		board.AddModule(&sdcard);
		board.SetSDCard(&sdcard);

		GPS gps(&board, gpsUart);
		board.AddModule(&gps);

		board.Info("Starting loop");

		int last = HAL_GetTick();
		uint32_t updateCount = 0;
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		while (true)
		{
			board.Update();
			updateCount++;

			if (HAL_GetTick() - last >= 1000)
			{
				last = HAL_GetTick();
				HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

				StackBuffer<sizeof(PacketHeader) + sizeof(DataPacket_STMF103)> buf;
				board.NewPacket(buf, PacketType::DATA);

				DataPacket_STMF103* data = buf.Ptr<DataPacket_STMF103>();
				data->UpdateLoopHZ = updateCount;

				board.SendPacket(buf);
				updateCount = 0;
			}
		}
	}


	void UARTWrite(UART_HandleTypeDef* channel, const void* data, uint32_t length)
	{
		HAL_UART_Transmit(s_RadioUart, reinterpret_cast<uint8_t*>(const_cast<void*>(data)), length, HAL_MAX_DELAY);
	}

	void UARTRead(UART_HandleTypeDef* uart, void* data, uint32_t length)
	{
		HAL_UART_Receive_DMA(uart, reinterpret_cast<uint8_t*>(data), length);
	}

}

void SerialPrint(Formatter& formatter, ModuleIDType from, LogLevelType level)
{
	SerialPrint(std::move(formatter), from, level);
}

void SerialPrint(Formatter&& formatter, ModuleIDType from, LogLevelType level)
{
	if (s_RadioUart == nullptr)
	{
		My_Error_Handler();
	}

	StackBuffer<sizeof(MessagePacket) + 64> buf;
	NewPacketImpl(buf, PacketType::MESSAGE, from);

	MessagePacket* messagePacket = buf.Ptr<MessagePacket>();
	messagePacket->Level = level;
	messagePacket->MessageLength = min(formatter.Size(), 64);

	char* destMessage = buf.As<char>(messagePacket->MessageLength);
	memcpy(destMessage, formatter.Data(), messagePacket->MessageLength);

	boardPtr->SendPacket(buf);
}

void NewPacketImpl(PacketBuffer& buffer, PacketTypeValue type, ModuleIDType from)
{
	PacketHeader* header = buffer.Header();
	header->UnixSeconds = 69;
	header->NanoSeconds = 420;

	header->Destination = ModuleID::GROUND_STATION;
	header->Forwarder = ModuleID::NONE;
	header->Type = type;
	header->From = from;
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

