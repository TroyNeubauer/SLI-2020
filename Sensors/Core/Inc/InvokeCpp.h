
#include "stm32f2xx_hal.h"
#include "stm32f2xx_hal_spi.h"
#include "stm32f2xx_hal_uart.h"

extern "C"
{
	void InvokeCpp(SPI_HandleTypeDef* spi1, UART_HandleTypeDef* uart4, UART_HandleTypeDef* uart5);

}



