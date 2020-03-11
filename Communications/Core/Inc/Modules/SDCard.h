#pragma once

#include "fatfs.h"
#include "fatfs_sd.h"
#include <map>

class SDCard {
private:
	FATFS fs;
	FIL fil;
	std::map<FRESULT, const char *> tresult;

public:
	SDCard();
	const char * Init();
	const char * Open(const char * path);
	const char * Close();
	const char * Write(const char* buff);
	~SDCard();
};
