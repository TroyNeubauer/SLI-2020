
#include "CommunicationsBoard.h"

#include "InvokeCpp.h"

extern "C"
{

	void InvokeCpp(SPI_HandleTypeDef* spi, UART_HandleTypeDef* uart1, UART_HandleTypeDef* uart2)
	{
		CommunicationsBoard board(spi, uart1, uart2);
		board.Init();

		while(true)
		{
			board.Update();
		}
	}

}
