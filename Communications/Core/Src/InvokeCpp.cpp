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

void SerialPrint(Formatter& formatter, LogLevelType level);
void SerialPrint(Formatter&& formatter, LogLevelType level);

const char* GetParentModuleName();

extern "C"
{
SizedFormatter<256> cLog;

void InvokeCpp(UART_HandleTypeDef* radioUart, UART_HandleTypeDef* gpsUart)
{
	Lights(1);
	Error_Lights(1);

	s_RadioUart = radioUart;
	s_gpsUart = gpsUart;

	CommunicationsBoard board(radioUart, gpsUart);

	boardPtr = &board;
	board.Init();

	SerialPrint(cLog, LL_INFO);

	GPS gps(&board, gpsUart);
	board.AddModule(&gps);

	SDCard sdcard;
	sdcard.Init();
	sdcard.Open("logTest1.txt");
	board.SetSDCard(&sdcard);

	board.Info("Starting loop");

	int last = HAL_GetTick();
	while (true)
	{
		board.Update();

		if (HAL_GetTick() - last > 1250)
		{
			HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
			last = HAL_GetTick();
		}
	}
}

void CLog(const char* message)
{
	cLog << '[' << GetParentModuleName() << " (C code)]: ";
	cLog << message << '\n';
}


void UARTWrite(UART_HandleTypeDef* channel, const void* data, uint32_t length)
{
	HAL_UART_Transmit(s_RadioUart, reinterpret_cast<uint8_t*>(const_cast<void*>(data)), length, HAL_MAX_DELAY);
}

void UARTRead(UART_HandleTypeDef* uart, void* data, uint32_t length)
{
	HAL_UART_Receive_DMA(uart, reinterpret_cast<uint8_t*>(data), length);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
	DefaultFormatter formatter;
	formatter << "HAL_UART_TxCpltCallback: Read " << " bytes";
	UARTWriteSync(s_RadioUart, formatter.c_str(), formatter.Size());
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{

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

	boardPtr->SendPacket(buf);
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
