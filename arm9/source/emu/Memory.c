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
#include "ProSystem.h"
#include "Memory.h"
#include "Maria.h"

byte memory_ram[MEMORY_SIZE] = {0};
byte memory_rom[MEMORY_SIZE] = {0};

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

// ----------------------------------------------------------------------------
// Read
// ----------------------------------------------------------------------------
#ifndef DS_LITE    
ITCM_CODE byte memory_Read(word address) 
{ 
  if (cartridge_pokey)
  {
      if (cartridge_pokey == POKEY_AT_4000)
      {
        if ((address & 0xFFF0) == 0x4000) return pokey_GetRegister(address);            
      }
      else
      {
          // Not quite accurate as it will catch anything from 0x440 to 0x4C0 but that's 
          // good enough as nothing else should be mapped in this region except POKEY.
          if ((address & 0xFFC0) == 0x440) return pokey_GetRegister(0x4000 + (address - 0x0450));
      }
  }
  if ((address & 0xFFFC) == 0x284)
  {
      if (address & 0x1)
      {
        byte tmp_byte = memory_ram[INTFLG];
        memory_ram[INTFLG] &= 0x7f;
        return tmp_byte; 
      }
      else
      {
        memory_ram[INTFLG] &= 0x7f;
        return memory_ram[INTIM];
      }
  }
  return memory_ram[address];
}
#endif

// ----------------------------------------------------------------------------
// Write
// ----------------------------------------------------------------------------
ITCM_CODE void memory_Write(word address, byte data) 
{
#ifndef DS_LITE        
  if (cartridge_pokey)
  {
      if (cartridge_pokey == POKEY_AT_4000)
      {
        if ((address & 0xFFF0) == 0x4000)
        {
            pokey_SetRegister(address, data);
            return;
        }
      }
      else
      {
          // Not quite accurate as it will catch anything from 0x440 to 0x4C0 but that's 
          // good enough as nothing else should be mapped in this region except POKEY.
          if ((address & 0xFFC0) == 0x440)
          {
            pokey_SetRegister(0x4000 + (address - 0x0450), data);
            return;
          }          
      }
  }
#endif
    
  if(!memory_rom[address]) 
  {
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
      case WSYNC:
        if (cartridge_uses_wsync)
        {
          memory_ram[WSYNC] = true;
        }
        break;
      case SWCHB:
        /*gdement:  Writing here actually writes to DRB inside the RIOT chip.
					This value only indirectly affects output of SWCHB.*/
        riot_SetDRB(data);
        break;
      case SWCHA:	
        riot_SetDRA(data);
        break;
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
        if (address >= 8256)
        {
          // 0x2040 -> 0x20ff    (0x2000)
          if(address >= 8256 && address <= 8447) 
          {
            memory_ram[address - 8192] = data;
          }
          // 0x2140 -> 0x21ff    (0x2000)
          else if(address >= 8512 && address <= 8703) 
          {
            memory_ram[address - 8192] = data;
          }
        }
        else if (address <= 511)
        {
          // 0x40 -> 0xff    (0x2000)
          if(address >= 64 && address <= 255) 
          {
            memory_ram[address + 8192] = data;
          }
          // 0x140 -> 0x1ff    (0x2000)
          else if(address >= 320 && address <= 511) 
          {
            memory_ram[address + 8192] = data;
          }
        }
        break;
    }
  }
  else 
  {
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
