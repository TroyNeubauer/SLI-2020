#pragma once

#include "fatfs.h"
#include "fatfs_sd.h"

#include "Module.h"

class SDCard : public SLIModule {
public:
	SDCard(SLICoreModule* core) : SLIModule(core, ModuleID::SD_CARD) {}
	virtual void Init();
	virtual void Update();
	virtual void RecievePacket(PacketBuffer& packet);

	void Open(const char* path);
	void Close();

	void Write(const void* buff, uint32_t length);
	inline void Write(const char* buff) { return Write(buff, strlen(buff)); }

	virtual ~SDCard();

private:
	void CheckError(FRESULT code, uint8_t operation);

private:
	FATFS fs;
	FIL fil;
};
