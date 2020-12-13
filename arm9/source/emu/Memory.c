// ----------------------------------------------------------------------------
//   ___  ___  ___  ___       ___  ____  ___  _  _
//  /__/ /__/ /  / /__  /__/ /__    /   /_   / |/ /
// /    / \  /__/ ___/ ___/ ___/   /   /__  /    /  emulator
//
// ----------------------------------------------------------------------------
// Copyright 2005 Greg Stanton
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
// ----------------------------------------------------------------------------
// Memory.cpp
// ----------------------------------------------------------------------------
#include "Memory.h"
#include "Maria.h"

byte memory_ram[MEMORY_SIZE] = {0};
byte memory_rom[MEMORY_SIZE] = {0};
//ALEK byte *memory_rom = memory_ram;

// ----------------------------------------------------------------------------
// Reset
// ----------------------------------------------------------------------------
void memory_Reset( ) {
  uint index;
  for(index = 0; index < MEMORY_SIZE; index++) {
    memory_ram[index] = 0;
    memory_rom[index] = 1;
  }
  for(index = 0; index < 16384; index++) {
    memory_rom[index] = 0;
  }
}

	// AM_RANGE(0x0020, 0x003f) AM_MIRROR(0x300) AM_READWRITE(a7800_MARIA_r, a7800_MARIA_w)
// ----------------------------------------------------------------------------
// Read
// ----------------------------------------------------------------------------
ITCM_CODE byte memory_Read(word address) {
  byte tmp_byte;

  switch ( address ) {
  case INTIM:
  case INTIM | 0x2:
	  memory_ram[INTFLG] &= 0x7f;
    return memory_ram[INTIM];
	  break;
  case INTFLG:
  case INTFLG | 0x2:
	  tmp_byte = memory_ram[INTFLG];
	  memory_ram[INTFLG] &= 0x7f;
	  return tmp_byte; 
    break;
  default:
    return memory_ram[address];
    break;
  }
}

// ----------------------------------------------------------------------------
// Write
// ----------------------------------------------------------------------------
ITCM_CODE void memory_Write(word address, byte data) {
//ALEK void memory_Write(byte data,word address) {

  if(!memory_rom[address]) {
    switch(address) {
      case INPTCTRL:
        if(data == 22 && cartridge_IsLoaded( )) { 
          cartridge_Store( ); 
        }
        else if(data == 2 && bios_enabled) {
          bios_Store( );
        }
        break;
      case INPT0:
        break;
      case INPT1:
        break;
      case INPT2:
        break;
      case INPT3:
        break;
      case INPT4:
        break;
      case INPT5:
        break;
      case AUDC0:
        tia_audc[0] = data & 15;
        tia_MemoryChannel(0);
        break;
      case AUDC1:
        tia_audc[1] = data & 15;
        tia_MemoryChannel(1);
        break;
      case AUDF0:
        tia_audf[0] = data & 31;
        tia_MemoryChannel(0);
        break;
      case AUDF1:
        tia_audf[1] = data & 31;
        tia_MemoryChannel(1);
        break;
      case AUDV0:
        tia_audv[0] = (data & 15) << 2;
        tia_MemoryChannel(0);
        break;
      case AUDV1:
        tia_audv[1] = (data & 15) << 2;
        tia_MemoryChannel(1);
        break;
//#define WSYNC       36 -> 0x24      
      case WSYNC:
        if(!(cartridge_flags & 128)) {
          memory_ram[WSYNC] = true;
        }
        break;
// #define SWCHB       642 -> 0x282
//#define CTLSWB      643
      case SWCHB:
        /*gdement:  Writing here actually writes to DRB inside the RIOT chip.
					This value only indirectly affects output of SWCHB.*/
		    riot_SetDRB(data);
        break;
      case SWCHA:	
		    riot_SetDRA(data);
//      case CTLSWB:
        break;
// #define TIM1T       660
      case TIM1T:
      case TIM1T | 0x8:
        riot_SetTimer(TIM1T, data);
        break;
      case TIM8T:
      case TIM8T | 0x8:
        riot_SetTimer(TIM8T, data);
        break;
      case TIM64T:
      case TIM64T | 0x8:
        riot_SetTimer(TIM64T, data);
        break;
      case T1024T:
      case T1024T | 0x8:
        riot_SetTimer(T1024T, data);
        break;
      default:
        memory_ram[address] = data;
        // 0x2040 -> 0x20ff    (0x2000)
        if(address >= 8256 && address <= 8447) {
          memory_ram[address - 8192] = data;
        }
        // 0x2140 -> 0x21fe    (0x2000)
        else if(address >= 8512 && address <= 8702) {
          memory_ram[address - 8192] = data;
        }
        // 0x40 -> 0xff    (0x2000)
        else if(address >= 64 && address <= 255) {
          memory_ram[address + 8192] = data;
        }
        // 0x140 -> 0x1ff    (0x2000)
        else if(address >= 320 && address <= 511) {
          memory_ram[address + 8192] = data;
        }
      cartridge_Write(address, data);
        break;
    }
  }
  else {
    cartridge_Write(address, data);
  }
}

// ----------------------------------------------------------------------------
// WriteROM
// ----------------------------------------------------------------------------
void memory_WriteROM(word address, word size, const byte* data) {
  uint index;
  if((address + size) <= MEMORY_SIZE && data != NULL) {
    for(index = 0; index < size; index++) {
      memory_ram[address + index] = data[index];
      memory_rom[address + index] = 1;
    }
  }
}

// ----------------------------------------------------------------------------
// ClearROM
// ----------------------------------------------------------------------------
void memory_ClearROM(word address, word size) {
  uint index;
  if((address + size) <= MEMORY_SIZE) {
    for(index = 0; index < size; index++) {
      memory_ram[address + index] = 0;
       memory_rom[address + index] = 0;
    }
  }
}
