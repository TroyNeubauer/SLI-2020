#pragma once

#include "Modules/ICM_20948_IMPL.h"

#include "Module.h"

#define ACCELEROMETER_CS GPIOB, 14

class ICM20948 : public SLIModule
{
public:

	ICM20948(SLICoreModule* core, SPI_HandleTypeDef* spi)
		: SLIModule(core, ModuleID::ACCEL), m_SPI(spi)
	{
		Info("Constructed ICM20948");
	}

	void PrintICMStatus(ICM20948* icm, ICM_20948_Status_e status, const char* message)
	{
		const char* error;
		switch (status)
		{
			case ICM_20948_Stat_Err: error = "error"; break;
			case ICM_20948_Stat_NotImpl: error = "ok"; break;
			case ICM_20948_Stat_ParamErr: error = "param error"; break;
			case ICM_20948_Stat_WrongID: error = "wrong id"; break;
			case ICM_20948_Stat_InvalSensor: error = "invalid sensor"; break;
			case ICM_20948_Stat_NoData: error = "no data"; break;
			case ICM_20948_Stat_SensorNotSupported: error = "sensor not supported"; break;
			default: error = "unknown"; break;
		}
		DefaultFormatter formatter;
		formatter << "Result of: " << message << " - " << error << " (" << (int32_t) status << ")";
		Info(formatter);
	}

	virtual void Init()
	{
		Info("ICM20948::Init() Called!");
		ICM_20948_Status_e status = m_Impl.begin(m_SPI, ACCELEROMETER_CS);
 		PrintICMStatus(this, status, "init");

 		ICM_20948_Status_e retval = ICM_20948_Stat_Ok;
 		uint8_t whoami = 0x00;
 		retval = ICM_20948_get_who_am_i(&m_Impl._device, &whoami);
	}

	virtual void Update()
	{
		if (m_Impl.dataReady())
		{

		}
	}

	virtual void RecievePacket(PacketBuffer& packet)
	{

	}
private:
	ICM_20948_SPI m_Impl;

	SPI_HandleTypeDef* m_SPI;

};

