#include "CommunicationsBoard.h"

#include "stm32f1xx_hal_uart.h"


//See http://stm32f4-discovery.net/2014/07/library-21-read-sd-card-fatfs-stm32f4xx-devices/


void CommunicationsBoard::Init()
{
	Info("Calling CommunicationsBoard::Init");
}

void CommunicationsBoard::Update()
{
	Info("CommunicationsBoard::Update");
	UpdateLocalModules();
	Info("CommunicationsBoard::Update done");

	const char* message = "test!";
	HAL_UART_Transmit(m_RadioUART, reinterpret_cast<uint8_t*>(const_cast<char*>(message)), 5, HAL_MAX_DELAY);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_Delay(10);

	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);


	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_SET);


	HAL_Delay(1500);

	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_12);
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_13);

	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_14);
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_15);

	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_0);
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_1);

	HAL_Delay(1500);


}

void CommunicationsBoard::RoutePacket(const PacketHeader &header, Buffer &packet)
{
	if (header.Destination == ModuleID::GROUND_STATION)
	{
		uint8_t *buffer = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&header));
		HAL_UART_Transmit(m_RadioUART, buffer, sizeof(header) + SizeOfPacket(header, packet), 0);
	}
	else
	{
		//Forward to the sensor board

	}
}

