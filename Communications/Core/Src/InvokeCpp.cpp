
//#include "CommunicationsBoard.h"

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx.h"

#include <cstdlib>
#include <cstdio>
#include <chrono>

#include "CommunicationsBoard.h"
#include "main.h"


extern "C"
{
	void InvokeCpp(SPI_HandleTypeDef* spi1, UART_HandleTypeDef* radioUart, UART_HandleTypeDef* GPSUart)
	{
		Log("Starting InvokeCpp");
		int i = 0;
		while (i < 4)
		{
			Log("cpp loop");
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);

			std::chrono::microseconds micros(100000);
			uint32_t count = std::chrono::duration_cast<std::chrono::milliseconds>(micros).count();
			HAL_Delay(count);

			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);

			printf("Trying printf! %d", i++);


			HAL_Delay(500);

		}
		Log("cpp loop end");
		Log("About to construct obj");
		CommunicationsBoard board(spi1, radioUart, GPSUart);
		Log("obj constructed");
		Log("About to call init");
		board.Init();
		Log("Init done");

		while(true)
		{
			Log("Updaitng");
			board.Update();
		}
	}

}
