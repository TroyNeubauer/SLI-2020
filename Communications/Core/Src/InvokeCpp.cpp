
//#include "CommunicationsBoard.h"

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx.h"

#include <cstdlib>
#include <cstdio>
#include <chrono>

#include "Modules/CommunicationsBoard.h"
#include "Modules/GPS.h"
#include "main.h"

#include "Core.h"


CommunicationsBoard* boardPtr = nullptr;

extern "C"
{
	void InvokeCpp(SPI_HandleTypeDef* spi1, UART_HandleTypeDef* radioUart, UART_HandleTypeDef* GPSUart)
	{

		CommunicationsBoard board(spi1, radioUart, GPSUart);
		boardPtr = &board;
		board.Init();

		GPS gps(&board, GPSUart);

		board.AddModule(&gps);

		while(true)
		{
			board.Update();
		}
	}

}

void SerialPrint(Formatter& formatter)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);

	formatter << '\n';
	HAL_UART_Transmit(RADIO_UART, formatter.Data(), formatter.Size(), HAL_MAX_DELAY);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
}

void SLIAssertFailed(const char* message)
{
	boardPtr->Error("SLI ASSERT FAILED!!!");
	boardPtr->Error(message);
}

void const char* GetParentModuleName()
{
	return "stmF103";
}

