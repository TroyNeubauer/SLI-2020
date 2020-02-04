#pragma once

#include "Modules/ICM_20948_IMPL.h"

#include "Module.h"

class ICM20948 : public SLIModule
{
public:

	ICM20948(SPI_HandleTypeDef* spi, GPIO_TypeDef* bank, uint16_t csPin) : m_SPI(spi), m_Bank(bank), m_CSPin(csPin)
	{
	}

	virtual void Init()
	{
		m_Impl.begin(spi, bank, csPin);
	}

	virtual void Update()
	{
		if (m_Impl.dataReady())
		{

		}
	}

	virtual void RecievePacket(const PacketHeader& header, Buffer& packet)
	{

	}
private:
	ICM_20948_SPI m_Impl;

	SPI_HandleTypeDef* m_SPI;
	GPIO_TypeDef* m_Bank;
	uint16_t m_CSPin;

};

