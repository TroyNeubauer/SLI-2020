#pragma once

#include <stdint.h>

enum ModuleID
{
	NONE = 0, GROUND_STATION = 1, STM32F205 = 2, STM32F103 = 3, ALT1 = 10, ALT2 = 11, GPS = 12, RADIO = 13, ACCEL = 14, SD_CARD = 15;
};

using PacketType = uint8_t;

//Indicates that a certain module has initialized
//Only sent once
//This is the first packet that will be sent for a device
constexpr PacketType INIT = 0;

//The module has entered a different status (GPS lock, accelerometer mode set, etc.)
constexpr PacketType STATUS = 1;

//This module is sending new data (altitude reading, velocity, etc.)
constexpr PacketType DATA = 2;

//Some type of error has occurred
constexpr PacketType ERROR = 3;

//A device is asking another device for something
constexpr PacketType REQUEST = 4

//A device is asking another device for something
constexpr PacketType RESPONSE = 5;

constexpr PacketType MAX_PACKET_TYPE = 6;


using RequestType = uint8_t;

constexpr RequestType NONE = 0;
constexpr RequestType WAKE = 1;


struct PacketHeader
{
	//Where the packet originally came from
	ModuleID Destination;
	ModuleID Forwarder;

	ModuleID From;
	PacketType Type;

	union
	{
		//1 sub union for each packet type
		union
		{

		} InitData;

		union
		{

		} StatusData;

		union
		{
			union
			{

			} Alt1;

			union
			{

			} Accel;

			union
			{

			} Alt2;

			union
			{

			} GPS;

			union
			{

			} Radio;

			union
			{

			} SDCard;

		} Data;

		union
		{

		} ErrorData;

		union
		{

		} RequestData;

		union
		{

		} ResponseData;


	};

};



