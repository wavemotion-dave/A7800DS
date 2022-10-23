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
// Cartridge.cpp
// ----------------------------------------------------------------------------
#include "Cartridge.h"
#include "HighScore.h"
#include "ProSystem.h"
#include "Database.h"

char cartridge_title[128];
char cartridge_description[128];
char cartridge_year[128];
char cartridge_maker[128];
byte cartridge_digest[128];
char cartridge_filename[128];


// -------------------------------------------------------------------------------------------------
// We allow cart sizes up to 512K which is pretty huge - I've not seen any ROMs bigger than this.
// -------------------------------------------------------------------------------------------------
static byte cartridge_buffer[512 * 1024] ALIGN(32);
uint cartridge_size __attribute__((section(".dtcm"))) = 0;
static uint maxbank = 9;

// ----------------------------------------------------------------------------
// HasHeader
// ----------------------------------------------------------------------------
static bool cartridge_HasHeader(const byte* header) {
  uint index;
  
  const char HEADER_ID[ ] = {"ATARI7800"};
  for(index = 0; index < 9; index++) {
    if(HEADER_ID[index] != header[index + 1]) {
      return false;
    }
  }
  return true;
}

inline static uint cartridge_GetBank(byte bank) {
  if ((myCartInfo.cardtype == CARTRIDGE_TYPE_SUPERCART || myCartInfo.cardtype == CARTRIDGE_TYPE_SUPERCART_ROM || myCartInfo.cardtype == CARTRIDGE_TYPE_SUPERCART_RAM) && cartridge_size <= 65536) {
    // for some of these carts, there are only 4 banks. in this case we ignore bit 3
    // previously, games of this type had to be doubled. The first 4 banks needed to be duplicated at the end of the ROM
      return (bank & 3);
  }
  return bank;
}

// ----------------------------------------------------------------------------
// GetBankOffset
// ----------------------------------------------------------------------------
inline static uint cartridge_GetBankOffset(byte bank) {
  return cartridge_GetBank(bank) * 16384;
}

byte last_bank = 255;
// ----------------------------------------------------------------------------
// WriteBank
// ----------------------------------------------------------------------------
inline static void cartridge_WriteBank(word address, byte bank) 
{
  if (bank != last_bank)
  {
    last_bank = bank;
    uint offset = cartridge_GetBank(bank) * 16384;
    if(offset < cartridge_size) 
    {
        if (offset < (256*1024))    // If we are in fast VRAM memory
            memory_WriteROM_DMA((u32*)&memory_ram[address], (u32*)(0x06860000 + offset), 16384);
        else    // Normal memory
            memory_WriteROMFast(address, (16384/4), cartridge_buffer + offset);
    }
  }
}

// ----------------------------------------------------------------------------
// ReadHeader
// ----------------------------------------------------------------------------
static void cartridge_ReadHeader(const byte* header) {
  uint index;
  char temp[33] = {0};
  for(index = 0; index < 32; index++) {
    temp[index] = header[index + 17];  
  }
  strcpy(cartridge_title,temp);
  
  cartridge_size  = header[49] << 24;
  cartridge_size |= header[50] << 16;
  cartridge_size |= header[51] << 8;
  cartridge_size |= header[52];
  
//  bit 0    = pokey at $4000
//  bit 1    = supergame bank switched
//  bit 2    = supergame ram at $4000
//  bit 3    = rom at $4000
//  bit 4    = bank 6 at $4000
//  bit 5    = supergame banked ram
//  bit 6    = pokey at $450
//  bit 7    = mirror ram at $4000  
  
  if(header[53] == 0) 
  {
    if (header[54] & 0x04)
    {
      myCartInfo.cardtype = CARTRIDGE_TYPE_SUPERCART_RAM;
    }
    else if(header[54] & 0x08) 
    {
      if (cartridge_size > 131072)
        myCartInfo.cardtype = CARTRIDGE_TYPE_SUPERCART_LARGE;
      else
        myCartInfo.cardtype = CARTRIDGE_TYPE_SUPERCART_ROM;
    }
    else if(header[54] & 0x02) 
    {
      myCartInfo.cardtype = CARTRIDGE_TYPE_SUPERCART;
    }
    else if (cartridge_size > 131072)
    {
      myCartInfo.cardtype = CARTRIDGE_TYPE_SUPERCART_LARGE;
    }
    else 
    {
      myCartInfo.cardtype = CARTRIDGE_TYPE_NORMAL;
    }
  }
  else 
  {
    if(header[53] == 2) 
    {
      myCartInfo.cardtype = CARTRIDGE_TYPE_ABSOLUTE;
    }
    else if(header[53] == 1) 
    {
      myCartInfo.cardtype = CARTRIDGE_TYPE_ACTIVISION;
    }
    else 
    {
      myCartInfo.cardtype = CARTRIDGE_TYPE_NORMAL;
    }
  }
  
  
  if (header[54] & 0x01) myCartInfo.pokeyType = POKEY_AT_4000;
  if (header[54] & 0x40) myCartInfo.pokeyType = POKEY_AT_450;
  
  myCartInfo.cardctrl1 = header[55];
  myCartInfo.cardctrl2 = header[56];
  myCartInfo.region = header[57];
  myCartInfo.hsc = (header[0x3A]&1 ? HSC_YES:HSC_NO);
  myCartInfo.steals_cycles = true;       // By default, assume the cart steals cycles
  myCartInfo.uses_wsync = true;          // By default, assume the cart uses wsync
  last_bank = 255;
}

// ----------------------------------------------------------------------------
// Load
// ----------------------------------------------------------------------------
static bool _cartridge_Load(const byte* data, uint size) 
{
  uint index;
  if(size <= 128) {
    return false;
  }

  cartridge_Release();
  
  byte header[128] = {0};
  for(index = 0; index < 128; index++) 
  {
    header[index] = data[index];
  }

  uint offset = 0;
  if(cartridge_HasHeader(header)) 
  {
    cartridge_ReadHeader(header);   // cartridge_size will get filled in from this
    size -= 128;
    offset = 128;
  }
  else 
  {
    strcpy(cartridge_title, "NoTitle*#!.)_");
    cartridge_size = size;
  }

  // -----------------------------------------------------------------------------
  // Copy up to 256K bytes of cart into the fast memory - used for bank swap only
  // -----------------------------------------------------------------------------
  u32 *fast_mem = (u32*)0x06860000;
  memcpy(fast_mem, &data[offset], (256 * 1024));
    
  // ----------------------------------------------------  
  // And copy the full ROM into the main cart area...
  // ----------------------------------------------------  
  for(index = 0; index < cartridge_size; index++) {
    cartridge_buffer[index] = data[index + offset];
  }
  
  hash_Compute(cartridge_buffer, cartridge_size,cartridge_digest);
  return true;
}
extern unsigned long crc32 (unsigned int crc, const unsigned char *buf, unsigned int len);

// ----------------------------------------------------------------------------
// Load
// ----------------------------------------------------------------------------
bool cartridge_Load(char *filename) {
  if(strlen(filename) == 0) {
    return false;
  }
  
  cartridge_Release();
 
  byte* data = NULL;
  uint size = archive_GetUncompressedFileSize(filename);
  if(size == 0) {
    FILE *file = fopen(filename, "rb");
    if(file == NULL) {
      return false;  
    }

    if(fseek(file, 0L, SEEK_END)) {
    }
    size = ftell(file);
    if(fseek(file, 0L, SEEK_SET)) {
    }

    data = (byte *) malloc(size);
    if(fread(data, 1, size, file) != size && ferror(file)) {
      fclose(file);
      cartridge_Release( );
      free(data);
      return false;
    }    

    fclose(file);    
  }
  else {
    data = (byte *) malloc(size);
    archive_Uncompress(filename, data, size);
  }

  if(!_cartridge_Load(data, size)) {
    free(data);
    return false;
  }
  if(data != NULL) {
    free(data);
  }
  
  
  strcpy(cartridge_filename, filename);
  return true;
}

bool cartridge_Load_buffer(char* rom_buffer, int rom_size) {
  cartridge_Release();
  byte* data = (byte *)rom_buffer;
  uint size = rom_size;

  if(!_cartridge_Load(data, size)) {
    return false;
  }
  strcpy(cartridge_filename ,"");
  return true;
}

// ----------------------------------------------------------------------------
// Store
// ----------------------------------------------------------------------------
void cartridge_Store( ) {
  switch(myCartInfo.cardtype) {
    case CARTRIDGE_TYPE_NORMAL:
      memory_WriteROM(65536 - cartridge_size, cartridge_size, cartridge_buffer);
      break;
    case CARTRIDGE_TYPE_SUPERCART: {
      uint offset = cartridge_size - 16384;
      if(offset < cartridge_size) {
        memory_WriteROM(49152, 16384, cartridge_buffer + offset);
      }
    } break;
    case CARTRIDGE_TYPE_SUPERCART_LARGE: {
      uint offset = cartridge_size - 16384;
      if(offset < cartridge_size) {
        memory_WriteROM(49152, 16384, cartridge_buffer + offset);
        memory_WriteROM(16384, 16384, cartridge_buffer + cartridge_GetBankOffset(0));
      }
    } break;
    case CARTRIDGE_TYPE_SUPERCART_RAM: {
      uint offset = cartridge_size - 16384;
      if(offset < cartridge_size) {
        memory_WriteROM(49152, 16384, cartridge_buffer + offset);
        memory_ClearROM(16384, 16384);
      }
    } break;
    case CARTRIDGE_TYPE_SUPERCART_ROM: {
      uint offset = cartridge_size - 16384;
      if(offset < cartridge_size && cartridge_GetBankOffset(6) < cartridge_size) {
        memory_WriteROM(49152, 16384, cartridge_buffer + offset);        
        memory_WriteROM(16384, 16384, cartridge_buffer + cartridge_GetBankOffset(6));
      }
    } break;
    case CARTRIDGE_TYPE_ABSOLUTE:
      memory_WriteROM(16384, 16384, cartridge_buffer);
      memory_WriteROM(32768, 32768, cartridge_buffer + cartridge_GetBankOffset(2));
      break;
    case CARTRIDGE_TYPE_ACTIVISION:
      if(122880 < cartridge_size) {
        memory_WriteROM(40960, 16384, cartridge_buffer);
        memory_WriteROM(16384, 8192, cartridge_buffer + 106496);
        memory_WriteROM(24576, 8192, cartridge_buffer + 98304);
        memory_WriteROM(32768, 8192, cartridge_buffer + 122880);
        memory_WriteROM(57344, 8192, cartridge_buffer + 114688);
      }
      break;
  }
  maxbank = cartridge_size / 16384;
}

// ----------------------------------------------------------------------------
// Write
// ----------------------------------------------------------------------------
void cartridge_Write(word address, byte data) {
  switch(myCartInfo.cardtype) 
  {
    case CARTRIDGE_TYPE_SUPERCART:
    case CARTRIDGE_TYPE_SUPERCART_RAM:
    case CARTRIDGE_TYPE_SUPERCART_ROM:
      if ((address & 0xC000) == 0x8000) // Is this a bankswitching write?
      {
        cartridge_WriteBank(32768, data);
      }
      break;
    case CARTRIDGE_TYPE_SUPERCART_LARGE:
      if ((address & 0xC000) == 0x8000) // Is this a bankswitching write?
      {
        cartridge_WriteBank(32768, data+1);
      }
      break;
    case CARTRIDGE_TYPE_ABSOLUTE:
      if(address == 32768 && (data == 1 || data == 2)) {
        cartridge_WriteBank(16384, data-1);
      }
      break;
    case CARTRIDGE_TYPE_ACTIVISION:
      if(address >= 65408) {
        cartridge_WriteBank(40960, (address & 7));
      }
      break;
  }
}

// ----------------------------------------------------------------------------
// StoreBank
// ----------------------------------------------------------------------------
void cartridge_StoreBank(byte bank) {
  switch(myCartInfo.cardtype) {
    case CARTRIDGE_TYPE_SUPERCART:
    case CARTRIDGE_TYPE_SUPERCART_RAM:
    case CARTRIDGE_TYPE_SUPERCART_ROM:
    case CARTRIDGE_TYPE_SUPERCART_LARGE:
      cartridge_WriteBank(32768, bank);        
      break;
    case CARTRIDGE_TYPE_ABSOLUTE:
      cartridge_WriteBank(16384, bank);
      break;
    case CARTRIDGE_TYPE_ACTIVISION:
      cartridge_WriteBank(40960, bank);
      break;
  }  
}


// ----------------------------------------------------------------------------
// IsLoaded
// ----------------------------------------------------------------------------
bool cartridge_IsLoaded( ) {
  return (cartridge_buffer != NULL)? true: false;
}

// ----------------------------------------------------------------------------
// Release
// ----------------------------------------------------------------------------
void cartridge_Release( ) 
{
  if(cartridge_buffer != NULL) 
  {
    // Snap out the High Score SRAM (if used)
    cartridge_SaveHighScoreSram();
    cartridge_size = 0;

    // These values need to be reset so that moving between carts works
    // consistently. This seems to be a ProSystem emulator bug.
    //
    myCartInfo.cardtype = CT_NORMAL;
    myCartInfo.region = NTSC;
    myCartInfo.pokeyType = POKEY_NONE;
    myCartInfo.hsc = false;
    myCartInfo.cardctrl1 = 0;
    myCartInfo.cardctrl2 = 0;
    myCartInfo.steals_cycles = false;
    myCartInfo.uses_wsync = false;
  }
}
