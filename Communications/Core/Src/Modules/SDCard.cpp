#include "Modules/SDCard.h"
#include "main.h"
#include <string.h>


void SDCard::Init()
{
	FRESULT fresult = f_mount(&fs, "", 0);
}

void SDCard::Open(const char * path)
{
	FRESULT errorCode = f_open(&fil, path, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
	CheckError(errorCode, 1);
}

void SDCard::Close()
{
	FRESULT errorCode = f_close(&fil);
	CheckError(errorCode, 2);
}

//Make sure that we don't log SDCard errors to the SD Card...
bool s_Recursion = false;

void SDCard::Write(const void* buff, uint32_t length)
{
	if (s_Recursion)
		return;
	s_Recursion = true;

	UINT bw;
	FRESULT errorCode = f_write(&fil, buff, length, &bw);
	CheckError(errorCode, 3);

	errorCode = f_sync(&fil);
	CheckError(errorCode, 4);

	s_Recursion = false;
}

SDCard::~SDCard()
{
	f_close(&fil);
}

void SDCard::CheckError(FRESULT code, uint8_t operation)
{
	if (code != FR_OK)
	{
		StackBuffer<sizeof(StatusPacket) + sizeof(ErrorStatusPacket)> buf;
		NewPacket(buf, PacketType::STATUS);
		StatusPacket* status = buf.Ptr<StatusPacket>();
		status->Status = StatusValue::MODULE_ERROR;

		ErrorStatusPacket* errorStatus = buf.Ptr<ErrorStatusPacket>();
		errorStatus->ErrorCode = code;
		errorStatus->Operation = operation;

		SendPacket(buf);
	}
}

void SDCard::RecievePacket(PacketBuffer& packet)
{

}

void SDCard::Update()
{

}


