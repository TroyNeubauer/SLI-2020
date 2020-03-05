#include "SensorsBoard.h"

#include "Modules/ICM20948.h"
#include <cstring>

UART_HandleTypeDef* uart4 = nullptr;


#define LED GPIOE, 3

void SerialPrint(const char* message)
{
	HAL_GPIO_TogglePin(LED);
	HAL_Delay(10);
	HAL_GPIO_TogglePin(LED);
	if (!uart4)
	{
		//Were screwed. Maybe blink LED? FIXME
	}
	else
	{
		HAL_UART_Transmit(uart4, reinterpret_cast<uint8_t*>(const_cast<char*>(message)), strlen(message), HAL_MAX_DELAY);
	}
}


SensorsBoard::SensorsBoard() : SLICoreModule(ModuleID::STM32F205)
{

}


void SensorsBoard::Init(SPI_HandleTypeDef* spi1, UART_HandleTypeDef* uart4, UART_HandleTypeDef* uart5)
{
	int i = 0;
	m_ContainedModules[i++] = new ICM20948(this, spi1);
}

void SensorsBoard::Update()
{
	HAL_GPIO_TogglePin(LED);
	HAL_Delay(500);

	HAL_GPIO_TogglePin(LED);
	HAL_Delay(500);


}


void SensorsBoard::RoutePacket(PacketBuffer& packet)
{

}

