
//#include "CommunicationsBoard.h"

#include "stm32f1xx.h"
#include "stm32f1xx_ll_usart.h"

#include <cstdlib>
#include <cstdio>
#include <chrono>

#include "Modules/CommunicationsBoard.h"
#include "Modules/GPS.h"
#include "main.h"

#include "Core.h"


CommunicationsBoard* boardPtr = nullptr;

USART_TypeDef* s_RadioUart = nullptr;
USART_TypeDef* s_GPSUart = nullptr;

void SerialPrint(Formatter& formatter);

const char* GetParentModuleName();

extern "C"
{
	SizedFormatter<256> cLog;

	void InvokeCpp(USART_TypeDef* radioUart, USART_TypeDef* GPSUart)
	{
		int last = HAL_GetTick();
		s_RadioUart = radioUart;
		s_GPSUart = GPSUart;
		SerialPrint(cLog);

		CommunicationsBoard board(radioUart, GPSUart);
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
			board.Info("...");
			HAL_Delay(200);

		}
	}


	void CLog(const char* message)
	{
		cLog << '[' << GetParentModuleName() << " (C code)]: ";
		cLog << message << '\n';
	}

}

void SerialPrint(Formatter& formatter)
{
	if (s_RadioUart == nullptr)
	{
		My_Error_Handler();
	}

	formatter << '\n';

	SerialPrintImpl(formatter.Data(), formatter.Size());

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
