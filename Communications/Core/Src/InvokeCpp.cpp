
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

SPI_HandleTypeDef* s_SPI1 = nullptr;
UART_HandleTypeDef* s_RadioUart = nullptr;
UART_HandleTypeDef* s_GPSUart = nullptr;

void SerialPrint(Formatter& formatter);

const char* GetParentModuleName();

extern "C"
{
	SizedFormatter<256> cLog;

	void InvokeCpp(SPI_HandleTypeDef* spi1, UART_HandleTypeDef* radioUart, UART_HandleTypeDef* GPSUart)
	{
		int last = HAL_GetTick();
		s_SPI1 = spi1;
		s_RadioUart = radioUart;
		s_GPSUart = GPSUart;
		SerialPrint(cLog);

		CommunicationsBoard board(spi1, radioUart, GPSUart);
		boardPtr = &board;
		board.Init();

		GPS gps(&board, GPSUart);

		board.AddModule(&gps);
		while(true)
		{
			board.Update();

			if (HAL_GetTick() - last > 1250)
			{
				HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_12);
				last = HAL_GetTick();
			}
			HAL_Delay(20);

		}
	}


	void CLog(const char* message)
	{
		cLog << '[' << GetParentModuleName() << " (C code)]: ";
		cLog << message << '\n';
	}

}

template<typename T>
uint8_t* ToDumbType(const T* ptr)
{
	return reinterpret_cast<uint8_t*>(const_cast<T*>(ptr));
}

void SerialPrint(Formatter& formatter)
{
	if (s_RadioUart == nullptr)
	{
		My_Error_Handler();
	}
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);

	formatter << '\n';
	HAL_UART_Transmit(s_RadioUart, ToDumbType(formatter.Data()), formatter.Size(), HAL_MAX_DELAY);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
}

void SLIAssertFailed(const char* message)
{
	SizedFormatter<256> formatter;
	formatter << "ASSERTION FAILED!!\n" << message;
	SerialPrint(formatter);
	My_Error_Handler();
}

const char* GetParentModuleName()
{
	return "stmF103";
}

