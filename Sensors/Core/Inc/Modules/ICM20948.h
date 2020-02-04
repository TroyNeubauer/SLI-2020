#pragma once

#include "Modules/ICM_20948_IMPL.h"

#include "Module.h"

#define ACCELEROMETER_CS GPIOC, 2

class ICM20948 : public SLIModule
{
public:

	ICM20948(SLICoreModule* core, SPI_HandleTypeDef* spi)
		: SLIModule(core, ModuleID::ACCEL), m_SPI(spi)
	{
		Info("Constructed ICM20948");
	}

	virtual void Init()
	{
		m_Impl.begin(m_SPI, ACCELEROMETER_CS);
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

};

