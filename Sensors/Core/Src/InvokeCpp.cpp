#include "stm32f205xx.h"

#include <cstdlib>
#include <cstdio>
#include <chrono>

#include "main.h"

#include "Core.h"
#include "Packet.h"
#include "Modules/SensorsBoard.h"
#include "Modules/ICM20948.h"

SensorsBoard* boardPtr = nullptr;

UART_HandleTypeDef* s_F103Uart = nullptr;
UART_HandleTypeDef* s_DebugUart = nullptr;

void SerialPrint(Formatter& formatter, LogLevelType level);
void SerialPrint(Formatter&& formatter, LogLevelType level);

const char* GetParentModuleName();

extern "C"
{
SizedFormatter<256> cLog;

void InvokeCpp(UART_HandleTypeDef* f103Uart, UART_HandleTypeDef* debugUart, SPI_HandleTypeDef* spi)
{
	s_F103Uart = f103Uart;
	s_DebugUart = debugUart;
	SerialPrint(cLog, LL_INFO);

	SensorsBoard board(f103Uart);
	boardPtr = &board;
	board.Init();

	ICM20948 icm(&board, spi);
	board.AddModule(&icm);


	board.Info("Starting loop");

	int last = HAL_GetTick();
	while (true)
	{
		board.Update();

		if (HAL_GetTick() - last > 1250)
		{
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
			last = HAL_GetTick();
		}
	}
}

void CLog(const char* message)
{
	cLog << '[' << GetParentModuleName() << " (C code)]: ";
	cLog << message << '\n';
}

void UART_DMA_Interupt(DMA_HandleTypeDef* dma)
{
	//s_ActiveDMA[GetDMAIndex(dma)][dmaChannel] = false;
	//LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_12);
}

void UARTWrite(UART_HandleTypeDef* channel, const void* data, uint32_t length)
{
	HAL_UART_Transmit(channel, reinterpret_cast<uint8_t*>(const_cast<void*>(data)), length, HAL_MAX_DELAY);
}

void UARTRead(UART_HandleTypeDef* uart, void* data, uint32_t length)
{
	HAL_UART_Receive_DMA(uart, reinterpret_cast<uint8_t*>(data), length);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
	DefaultFormatter formatter;
	formatter << "HAL_UART_TxCpltCallback: Read " << " bytes";
	UARTWriteSync(s_F103Uart, formatter.c_str(), formatter.Size());
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{

}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef* huart)
{

}

}

void SerialPrint(Formatter& formatter, LogLevelType level)
{
	SerialPrint(std::move(formatter), level);
}

void SerialPrint(Formatter&& formatter, LogLevelType level)
{
	if (s_F103Uart == nullptr)
	{
		My_Error_Handler();
	}
/*
	StackBuffer<sizeof(PacketHeader) + sizeof(MessagePacket) + 256> buf;

	PacketHeader* header = buf.Header();
	header->ID = 0;
	header->UnixSeconds = 420;
	header->NanoSeconds = 69;

	header->Destination = ModuleID::GROUND_STATION;
	header->Forwarder = ModuleID::NONE;
	header->From = ModuleID::STM32F205;
	header->Type = PacketType::MESSAGE;

	MessagePacket* messagePacket = buf.Ptr<MessagePacket>();
	messagePacket->Level = level;
	messagePacket->MessageLength = formatter.Size();

	char* destMessage = buf.As<char>(messagePacket->MessageLength);
	memcpy(destMessage, formatter.Data(), messagePacket->MessageLength);


	header->CRC32 = buf.CalculateCRC32();

	UARTWrite(s_F103Uart, buf.Begin(), buf.Offset());
*/
	formatter << "..\n\r";
	UARTWrite(s_DebugUart, formatter.Data(), formatter.Size());
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);

}

void SLIAssertFailed(const char* message)
{
	SizedFormatter<256> formatter;
	formatter << "ASSERTION FAILED!!\n" << message;
	UARTWriteSync(s_F103Uart, formatter.c_str(), formatter.Size());
	My_Error_Handler();
}

const char* GetParentModuleName()
{
	return "stmF205";
}
