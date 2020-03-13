#include "Modules/SDCard.h"
#include "main.h"
#include <string.h>

SDCard::SDCard() {
	tresult[FR_OK] = "Succeeded";
	tresult[FR_DISK_ERR] = "A hard error occurred in the low level disk I/O layer";
	tresult[FR_INT_ERR] = "Assertion failed";
	tresult[FR_NOT_READY] = "The physical drive cannot work";
	tresult[FR_NO_FILE] = "Could not find the file";
	tresult[FR_NO_PATH] = "Could not find the path";
	tresult[FR_INVALID_NAME] = "The path name format is invalid";
	tresult[FR_DENIED] = "Access denied due to prohibited access or directory full";
	tresult[FR_EXIST] = "Access denied due to prohibited access";
	tresult[FR_INVALID_OBJECT] = "The file/directory object is invalid";
	tresult[FR_WRITE_PROTECTED] = "The physical drive is write protected";
	tresult[FR_INVALID_DRIVE] = "The logical drive number is invalid";
	tresult[FR_NOT_ENABLED] = "The volume has no work area";
	tresult[FR_NO_FILESYSTEM] = "There is no valid FAT volume";
	tresult[FR_MKFS_ABORTED] = "The f_mkfs() aborted due to any parameter error";
	tresult[FR_TIMEOUT] = "Could not get a grant to access the volume within defined period";
	tresult[FR_LOCKED] = "The operation is rejected according to the file sharing policy";
	tresult[FR_NOT_ENOUGH_CORE] = "LFN working buffer could not be allocated";
	tresult[FR_TOO_MANY_OPEN_FILES] = "Number of open files > _FS_SHARE";
	tresult[FR_INVALID_PARAMETER] = "Given parameter is invalid";
}

const char * SDCard::Init() {
	FRESULT fresult = f_mount(&fs, "", 0);
	Error_Lights(fresult);
	return tresult[fresult];
}

const char * SDCard::Open(const char * path) {
	FRESULT fresult = f_open(&fil, path, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
	Error_Lights(fresult);
	return tresult[fresult];
}

const char * SDCard::Close() {
	FRESULT fresult = f_close(&fil);
	Error_Lights(fresult);
	return tresult[fresult];
}
const char * SDCard::Write(const char* buff) {
	return Write(buff, strlen(buff));
}

const char * SDCard::Write(const void* buff, int length) {
	UINT bw;
	FRESULT fresult = f_write(&fil, buff, length, &bw);
	f_sync(&fil);
	Error_Lights(fresult);
	return tresult[fresult];
}

SDCard::~SDCard() {

}
