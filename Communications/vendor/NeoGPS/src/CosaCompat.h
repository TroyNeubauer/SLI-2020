#ifndef COSACOMPAT_H
#define COSACOMPAT_H

//  Copyright (C) 2014-2017, SlashDevin
//
//  This file is part of NeoGPS
//
//  NeoGPS is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  NeoGPS is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with NeoGPS.  If not, see <http://www.gnu.org/licenses/>.

#include <cstdint>

#ifdef __AVR__

  #include <avr/pgmspace.h>

#else

  #define PGM_P const char *
  #define __FlashStringHelper char
  uint8_t pgm_read_byte(const void* address);
  void* pgm_read_ptr(const void* address);
  #define F(str) str

#endif

typedef PGM_P str_P;
//#define __PROGMEM PROGMEM
#define __PROGMEM

#endif
