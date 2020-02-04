
#include "InvokeCpp.h"
#include "SensorsBoard.h"

extern "C"
{

	void InvokeCpp(SPI_HandleTypeDef* spi1, UART_HandleTypeDef* uart4, UART_HandleTypeDef* uart5)
	{
		SensorsBoard board;

		board.Init(spi1, uart4, uart5);

		while (true)
		{
			board.Update();
		}


	}
}
