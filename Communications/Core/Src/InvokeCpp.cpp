
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


extern "C"
{
	void InvokeCpp(SPI_HandleTypeDef* spi1, UART_HandleTypeDef* radioUart, UART_HandleTypeDef* GPSUart)
	{

		CommunicationsBoard board(spi1, radioUart, GPSUart);
		board.Init();

		GPS gps(&board, GPSUart);

		board.AddModule(&gps);

		while(true)
		{
			board.Update();
		}
	}

}
