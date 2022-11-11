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
#include "Database.h"

byte memory_ram[MEMORY_SIZE] ALIGN(32) = {0};
u8 *is_memory_writable = (u8*)0x06820000;

extern bool ram_dirty[];
extern bool write_only_pokey_at_4000;

// ----------------------------------------------------------------------------
// Reset
// ----------------------------------------------------------------------------
void memory_Reset( ) 
{
  uint index;
  for(index = 0; index < MEMORY_SIZE; index++) {
    memory_ram[index] = 0;
    is_memory_writable[index] = 0;
  }
  u16 *ptr = (u16*)is_memory_writable;
  for(index = 0; index < 16384/2; index++) {
    ptr[index] = 0xFFFF;
  }
}


// ----------------------------------------------------------------------------
// Read
// ----------------------------------------------------------------------------
ITCM_CODE byte memory_Read_Slower(word address) 
{ 
  if (address & 0x8000) return memory_ram[address];
  else if ((address & 0xFFFC) == 0x284)
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
  else if (myCartInfo.pokeyType)
  {
      if (myCartInfo.pokeyType == POKEY_AT_4000)
      {
          if (((address & 0xFFF0) == 0x4000) && (!write_only_pokey_at_4000)) return pokey_GetRegister(address);            
      }
      else
      {
          // Not quite accurate as it will catch anything from 0x440 to 0x4C0 but that's 
          // good enough as nothing else should be mapped in this region except POKEY.
          if ((address & 0xFFC0) == 0x440) return pokey_GetRegister(0x4000 | (address & 0xF));
          if ((address & 0xFFF0) == 0x800) return pokey_GetRegister(0x4000 | (address & 0xF));
      }
  }
  return memory_ram[address];
}

// ----------------------------------------------------------------------------
// Write
// ----------------------------------------------------------------------------
ITCM_CODE void memory_Write(word address, byte data) 
{
  extern u32 bg32, maria_charbase;
  extern u8 bg8;
    
  if (myCartInfo.pokeyType)
  {
      if (myCartInfo.pokeyType == POKEY_AT_4000)
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
            pokey_SetRegister(0x4000 | (address & 0x0F), data);
            return;
          }
          if ((address & 0xFFF0) == 0x800)  // Pokey @800
          {
            pokey_SetRegister(0x4000 | (address & 0x0F), data);
            return;
          }          
      }
  }
  
  if (((u8*)0x06820000)[address]) // We use 64K of faster VRAM to hold the bits to tell us if this area is writable... speeds up things slightly...
  {
    // ---------------------------------------------------------------------------------------
    // If this write would be in normal (non bankset) memory, we need to keep the bankset 
    // memory up to date as well... This speeds up processing in Maria for banksets handling.
    // ---------------------------------------------------------------------------------------
    extern byte banksets_memory[]; extern u16 banksets_mask;
    if (!(address & banksets_mask)) banksets_memory[address] = data;
      
    if ((address & 0xF800))     // Base RAM is at 0x1800 so this will find anything that is RAM... 
    {
        // For banking RAM we need to keep the shadow up to date.
        if ((address & 0xC000) == 0x4000)
        { 
            extern u8 *shadow_ram;
            shadow_ram[address] = data;
            
            if (myCartInfo.cardtype == CARTRIDGE_TYPE_FRACTALUS)
            {
                // Special EXRAM/A8 handling... mirror ram
                memory_ram[address ^0x0100] = data;        
            }
        }
        memory_ram[address] = data;
        return;
    }
    
    if (address >= 0x460 && address < 0x480) return;    // XM/Yamaha is mapped into this area... do not respond to it as we are not XM capable (yet)
      
    switch(address) {
      case INPTCTRL:
        if(data == 22 && cartridge_IsLoaded( )) { 
          cartridge_Store( ); 
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
      case BACKGRND:
        memory_ram[BACKGRND] = data;
        bg8 = data;
        bg32 =  data | (data<<8) | (data<<16) | (data<<24);
        break;
      case CHARBASE:
        memory_ram[CHARBASE] = data;
        maria_charbase = data;
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
        memory_ram[WSYNC] = true;
        riot_and_wsync |= 0x01;
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
#ifdef RAM_MIRRORS_ENABLED  // Technically the RAM mirrors are here but we don't really care... we assume anyone using a mirror will read back from that same mirror location.
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
#endif            
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
ITCM_CODE void memory_WriteROM(word address, u32 size, const byte* data) 
{
  u32* ramPtr = (u32*)&memory_ram[address];
  u32* romPtr = (u32*)&is_memory_writable[address];
  u32* dataPtr = (u32*)data;
  for (u32 i=0; i<(size>>2); i++)
  {
      *ramPtr++ = *dataPtr++;
      *romPtr++ = 0x00000000;
  }
}

// ----------------------------------------------------------------------------
// WriteROMFast (assumes is_memory_writable[] already set properly)
// size is already in multiples of u32 dwords
// ----------------------------------------------------------------------------
ITCM_CODE void memory_WriteROMFast(word address, u32 size, const u32* data) 
{
  u32* ramPtr = (u32*)&memory_ram[address];
  u32* dataPtr = (u32*)data;
  u16 size2 = size;
  do
  {
      *ramPtr++ = *dataPtr++;
      *ramPtr++ = *dataPtr++;
      *ramPtr++ = *dataPtr++;
      *ramPtr++ = *dataPtr++;
  }
  while (--size2);
}

// ----------------------------------------------------------------------------
// ClearROM
// ----------------------------------------------------------------------------
void memory_ClearROM(word address, word size) 
{
    memset(&memory_ram[address], 0x00, size);
    memset(&is_memory_writable[address], 0xFF, size);
}
