#pragma once

#include "Module.h"

class SensorsBoard : public SLICoreModule
{
public:
	SensorsBoard() {}

	virtual void Init();
	virtual void Update();

private:
	virtual void RoutePacket(const PacketHeader& header, Buffer& packet);


};
