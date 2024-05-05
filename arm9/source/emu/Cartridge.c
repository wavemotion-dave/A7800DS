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

char cartridge_title[256];
byte cartridge_digest[256];
char cartridge_filename[256];

word cardtype = 0x0000;
bool write_only_pokey_at_4000 = false;

// -------------------------------------------------------------------------------------------------
// We allow cart sizes up to 1024K which is pretty huge - I've not seen any ROMs bigger than this.
// -------------------------------------------------------------------------------------------------
static byte cartridge_buffer[MAX_CART_SIZE+128] ALIGN(32); // The extra 128 bytes is for the possible .a78 header

uint cartridge_size         __attribute__((section(".dtcm"))) = 0;
byte last_bank              __attribute__((section(".dtcm"))) = 255;
byte ex_ram_bank            __attribute__((section(".dtcm"))) = 0;
byte last_ex_ram_bank       __attribute__((section(".dtcm"))) = 0;
byte ex_ram_bank_df         __attribute__((section(".dtcm"))) = 0;
byte last_ex_ram_bank_df    __attribute__((section(".dtcm"))) = 0;
u8 *shadow_ram              __attribute__((section(".dtcm"))) = 0;

// ---------------------------------------------------------------------------------------
// We support EX RAM which is 2x16K banks which are switched on bit 5 of banking writes.
// ---------------------------------------------------------------------------------------
byte ex_ram_buffer[0x8000] ALIGN(32) = {0};

// ------------------------------------------------------------------------------------
// For the new banksets scheme, we use a 64K buffer to mirror the 7800 address space.
// ------------------------------------------------------------------------------------
byte banksets_memory[64*1024] ALIGN(32);

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

// ----------------------------------------------------------------------------
// GetBankOffset
// ----------------------------------------------------------------------------
inline static uint cartridge_GetBankOffset(byte bank) 
{
  return (bank * 16384);
}

// ----------------------------------------------------------------------------
// WriteBank - this is the bankswitching code!
// ----------------------------------------------------------------------------
ITCM_CODE void cartridge_WriteBank(word address, byte bank) 
{
  if (myCartInfo.cardtype == CARTRIDGE_TYPE_SUPERCART_RAMX2) // Banking RAM
  {
    ex_ram_bank = ((bank >> 5) & 0x01);
    bank &= 0x1F;   // Limit to 32 banks (512K ROM) as the upper bit is for 2X RAM
      
    if (last_ex_ram_bank != ex_ram_bank)
    {
        u32 *src  = ex_ram_bank ? (u32*)0x06830000 : (u32*)0x06834000;  // Only for the DSi.. see DS_LITE handling below
        u32 *dest = (u32*)(memory_ram+0x4000);
        shadow_ram = ex_ram_bank ? (u8*)0x06830000 : (u8*)0x06834000;   // // Only for the DSi.. see DS_LITE handling below

        if (isDS_LITE) // Unfortunately non DSi can't write 8-bit values to LCD RAM... so we have to do this the slow way
        {
            src  = ex_ram_bank ? (u32*)(ex_ram_buffer+0x0000) : (u32*)(ex_ram_buffer+0x4000);  // 
            shadow_ram = ex_ram_bank ? (u8*)(ex_ram_buffer+0x0000) : (u8*)(ex_ram_buffer+0x4000);  // 
        }
        shadow_ram -= 0x4000; // Makes for faster indexing in Memory.c
        
        for (uint i=0; i<0x1000/4; i++)
        {
            *dest++ = *src++;
            *dest++ = *src++;
            *dest++ = *src++;
            *dest++ = *src++;
        }
        // Set the new EX RAM bank...
        last_ex_ram_bank = ex_ram_bank;
    }
  }
    
  if (bank != last_bank)
  {
    last_bank = bank;
      
    uint offset = bank * 16384;
    if (offset < (272*1024))    // If we are in fast VRAM memory
        memory_WriteROMFast(address, (16384/(4*4)), (u32*)(0x06860000 + offset));
    else    // Normal memory - a little slower but that's the best we can do...
        memory_WriteROMFast(address, (16384/(4*4)), (u32*)(cartridge_buffer + offset));
  }
}

void cartridge_SwapRAM_DragonFlyStyle(u8 data)
{
    ex_ram_bank_df = data & 0x01;

    if (last_ex_ram_bank_df != ex_ram_bank_df)
    {
        u32 *src  = ex_ram_bank_df ? (u32*)0x06830000 : (u32*)0x06834000;  // Only for the DSi.. see DS_LITE handling below
        u32 *dest = (u32*)(memory_ram+0x4000);
        shadow_ram = ex_ram_bank_df ? (u8*)0x06830000 : (u8*)0x06834000;   // Only for the DSi.. see DS_LITE handling below

        if (isDS_LITE) // Unfortunately non DSi can't write 8-bit values to LCD RAM... so we have to do this the slow way
        {
            src  = ex_ram_bank_df ? (u32*)(ex_ram_buffer+0x0000) : (u32*)(ex_ram_buffer+0x4000);  // 
            shadow_ram = ex_ram_bank_df ? (u8*)(ex_ram_buffer+0x0000) : (u8*)(ex_ram_buffer+0x4000);  // 
        }
        shadow_ram -= 0x4000; // Makes for faster indexing in Memory.c

        for (uint i=0; i<0x1000/4; i++)
        {
            *dest++ = *src++;
            *dest++ = *src++;
            *dest++ = *src++;
            *dest++ = *src++;
        }
        // Set the new EX RAM bank...
        last_ex_ram_bank_df = ex_ram_bank_df;
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

  // -------------------------------------------------------
  // A78 Card Type is a 16-bit word at header offset 53+54
  // -------------------------------------------------------
  //  bit 0     = pokey at $4000
  //  bit 1     = supergame bank switched
  //  bit 2     = ram at $4000
  //  bit 3     = rom at $4000
    
  //  bit 4     = second to last bank at $4000
  //  bit 5     = banked ram
  //  bit 6     = pokey at $450
  //  bit 7     = mirror ram at $4000
  
  //  bit 8     = activision banking
  //  bit 9     = absolute banking
  //  bit 10    = pokey at $440
  //  bit 11    = ym2151 at $460/$461
    
  //  bit 12    = souper
  //  bit 13    = banksets
  //  bit 14    = halt banked ram
  //  bit 15    = pokey at $800
  
  cardtype = (header[53] << 8) | header[54];
  
  myCartInfo.cardtype = (cartridge_size <= 65536) ? CARTRIDGE_TYPE_NORMAL : CARTRIDGE_TYPE_SUPERCART;    // Default guess... may be overridden below
    
  if (cardtype & 0x0002) myCartInfo.cardtype  = CARTRIDGE_TYPE_SUPERCART;
  if (cardtype & 0x0004) myCartInfo.cardtype  = ((cardtype & 0x0002) ? CARTRIDGE_TYPE_SUPERCART_RAM : CARTRIDGE_TYPE_FLAT_WITH_RAM);
  if (cardtype & 0x0008) myCartInfo.cardtype  = CARTRIDGE_TYPE_SUPERCART_LARGE;
  if (cardtype & 0x0010) myCartInfo.cardtype  = CARTRIDGE_TYPE_SUPERCART_ROM;
  if (cardtype & 0x0020) myCartInfo.cardtype  = CARTRIDGE_TYPE_SUPERCART_RAMX2;
  if (cardtype & 0x0100) myCartInfo.cardtype  = CARTRIDGE_TYPE_ACTIVISION;
  if (cardtype & 0x0200) myCartInfo.cardtype  = CARTRIDGE_TYPE_ABSOLUTE;
  if (cardtype & 0x2000) // Banksets
  {
      myCartInfo.cardtype = CARTRIDGE_TYPE_BANKSETS;                                    // Default to Banksets (no RAM)
      if (cardtype & 0x0004) myCartInfo.cardtype  = CARTRIDGE_TYPE_BANKSETS_RAM;        // RAM @4000 enabled
      if (cardtype & 0x4000) myCartInfo.cardtype  = CARTRIDGE_TYPE_BANKSETS_HALTRAM;    // Banked Halt RAM @4000 enabled
  }

  if (cardtype & 0x0040) myCartInfo.pokeyType = POKEY_AT_450;
  if (cardtype & 0x8000) myCartInfo.pokeyType = POKEY_AT_800;
  if (cardtype & 0x0001) myCartInfo.pokeyType = POKEY_AT_4000;

    // ========================
    // 0 = none
    // 1 = 7800 joystick
    // 2 = lightgun
    // 3 = paddle
    // 4 = trakball
    // 5 = 2600 joystick
    // 6 = 2600 driving
    // 7 = 2600 keypad
    // 8 = ST mouse
    // 9 = Amiga mouse
    // 10 = AtariVox/SaveKey
    // 11 = SNES2Atari
    // ========================
  switch(header[55])
  {
      case 1:
          myCartInfo.cardctrl1 = CARTRIDGE_CONTROLLER_JOYSTICK;
          break;
      case 2:
          myCartInfo.cardctrl1 = CARTRIDGE_CONTROLLER_LIGHTGUN;
          break;
      case 3:
          myCartInfo.cardctrl1 = CARTRIDGE_CONTROLLER_PADDLES;
          break;
      case 11:
          myCartInfo.cardctrl1 = CARTRIDGE_CONTROLLER_SNES2ATARI;
          break;
      default:
          myCartInfo.cardctrl1 = CARTRIDGE_CONTROLLER_JOYSTICK;
          break;
  }

  switch(header[56])
  {
      case 1:
          myCartInfo.cardctrl2 = CARTRIDGE_CONTROLLER_JOYSTICK;
          break;
      case 2:
          myCartInfo.cardctrl2 = CARTRIDGE_CONTROLLER_LIGHTGUN;
          break;
      case 3:
          myCartInfo.cardctrl2 = CARTRIDGE_CONTROLLER_PADDLES;
          break;
      case 11:
          myCartInfo.cardctrl2 = CARTRIDGE_CONTROLLER_SNES2ATARI;
          break;
      default:
          myCartInfo.cardctrl2 = CARTRIDGE_CONTROLLER_JOYSTICK;
          break;
  }
  
  // Region:  
  //  bit 0 : 0=NTSC, 1=PAL
  //  bit 1 : 0=component, 1=composite
  //  bit 2 : 0=single region, 1=multi-region
  myCartInfo.region = header[57] & 1;
  
  // High Score Support
  //  bit 0 : HSC
  //  bit 1 : AtariVox/SaveKey 
  myCartInfo.hsc = (header[58]&1 ? HSC_YES:HSC_NO); 
  
  myCartInfo.dma_adjust    = 0;
  last_bank = 255;
  last_ex_ram_bank = 0;
  ex_ram_bank = 0;
  last_ex_ram_bank_df = 0;
  write_only_pokey_at_4000 = false;
  ex_ram_bank_df = 0;
  if (isDS_LITE) shadow_ram = ex_ram_bank ? (u8*)(ex_ram_buffer+0x0000) : (u8*)(ex_ram_buffer+0x4000);  // for DS-Lite
  else shadow_ram = ex_ram_bank ? (u8*)0x06830000 : (u8*)0x06834000;   // // Only for the DSi.. see DS_LITE handling above
  shadow_ram -= 0x4000; // Makes for faster indexing in Memory.c
}

// ----------------------------------------------------------------------------
// Load
// ----------------------------------------------------------------------------
static bool _cartridge_Load(uint size) 
{
  uint index;
  if(size <= 128) {
    return false;
  }

  cartridge_Release();
    
  memset(ex_ram_buffer, 0x00, sizeof(ex_ram_buffer));   // Clear the EX RAM memory
    
  memset(banksets_memory, 0xFF, sizeof(banksets_memory));   // Clear the banksets ROM memory area
  memset(banksets_memory+0x4000, 0x00, 0x4000);             // Clear banksets RAM memory area
  
  static byte header[128] = {0};                   // We might have a header... this will buffer it
  for(index = 0; index < 128; index++) 
  {
    header[index] = cartridge_buffer[index];
  }

  uint offset = 0;
  if(cartridge_HasHeader(header)) 
  {
    cartridge_ReadHeader(header);   // cartridge_size will get filled in from this
    size -= 128;
    offset = 128;
    myCartInfo.hasHeader = true;
  }
  else 
  {
    strcpy(cartridge_title, "NoTitle*#!.)_");   // No header... no title.
    cartridge_size = size;
    myCartInfo.hasHeader = false;
  }
    

  // --------------------------------------------------------------------
  // If there was a header... shift the entire cart down to remove it...
  // --------------------------------------------------------------------
  if (offset > 0)
  {
      // ----------------------------------------------------  
      // And copy the full ROM into the main cart area...
      // ----------------------------------------------------  
      for(index = 0; index < cartridge_size; index++) 
      {
        cartridge_buffer[index] = cartridge_buffer[index + offset];
      }
      
  }
    
  // For small carts, double the size to avoid having to check banks
  if (cartridge_size <= 65536)
  {
      for(index = 0; index < cartridge_size; index++) 
      {
        cartridge_buffer[index + 65536] = cartridge_buffer[index];
      }
  }
    

  // -----------------------------------------------------------------------------
  // Copy up to 272K bytes of cart into the fast memory - used for bank swap only
  // -----------------------------------------------------------------------------
  u32 *fast_mem = (u32*)0x06860000;
  memcpy(fast_mem, cartridge_buffer, (272 * 1024));
    
  memset((u8*)0x06830000, 0x00, (64*1024));   // Clear this 128K chunk of fast VRAM as we use it for RAM bankswitch
  
  hash_Compute(cartridge_buffer, cartridge_size, cartridge_digest);
  return true;
}
extern unsigned long crc32 (unsigned int crc, const unsigned char *buf, unsigned int len);

// ----------------------------------------------------------------------------
// Load
// ----------------------------------------------------------------------------
bool cartridge_Load(char *filename) 
{
    if(strlen(filename) == 0) return false;  
    uint size = 0;
    cartridge_Release();
 
    FILE *file = fopen(filename, "rb");
    if(file == NULL)  return false;  

    if(fseek(file, 0L, SEEK_END)) { }
    size = ftell(file);
    if(fseek(file, 0L, SEEK_SET)) { }
    
    if (size > (MAX_CART_SIZE+128)) {fclose(file); return false;}   // Cart is too big... can't handle it
    
    memset(cartridge_buffer, 0xFF, MAX_CART_SIZE+128);  // Set all to 0xFF before we read the ROM in

    if(fread(cartridge_buffer, 1, size, file) != size && ferror(file)) 
    {
      fclose(file);
      return false;
    }    

    fclose(file);    

    if(!_cartridge_Load(size)) return false;

    strcpy(cartridge_filename, filename);
    return true;
}


// ------------------------------------------------------------------------------------------
// Here are the main 7800 Bankswitching schemes (ignoring Absolute, Activistion and Fractalus):
//
// NORMAL           Anything 48K or less... fits into memory (0xffff downwards) without switching.
// SUPERCART        Games that are 128+K in size with nothing mapped in at 0x4000
// SUPERCART_LARGE  Games that are 144+K in size with the extra 16K bank 0 fixed at 0x4000
// SUPERCART_RAM    Games that are 128+K in size with extra 16K of RAM at 0x4000
// SUPERCART_ROM    Games that are 128+K in size with the second-to-last bank fixed at 0x4000
//
// For the "Super Carts" the 16K at 0xC000 is the last bank in the ROM.
// For the "Super Carts" the 16K at 0x8000 is the bankswapping bank and is switched by writing
// the bank # to any address in that region.  For Supercart "Large" there are actually two
// chips (16K fixed and 128K bankswapped) and the bank is relative to the 128K chip so emulators
// will use (bank+1) to compensate for the extra 16K fixed bank 0 at 0x4000.
//
// In theory, since we can write any bank number 0-255 that would allow up to 255 banks of 16k
// which is a whopping 4096K (4 Megabytes) of ROM but in practice carts seem to limit to 512K
// or less for practical reasons with a few outstanding tech-demos reaching 1024K. 
// ------------------------------------------------------------------------------------------
void cartridge_Store( ) 
{
  uint offset, lastBank;
    
  switch(myCartInfo.cardtype) 
  {
    case CARTRIDGE_TYPE_NORMAL:
      if (cartridge_size <= 49152) memory_WriteROM(65536 - cartridge_size, cartridge_size, cartridge_buffer);
      break;
          
    case CARTRIDGE_TYPE_FLAT_WITH_RAM:
      if (cartridge_size <= 49152) memory_WriteROM(65536 - cartridge_size, cartridge_size, cartridge_buffer);
      memory_ClearROM(16384, 16384);
      break;
          
    case CARTRIDGE_TYPE_SUPERCART:
      offset = cartridge_size - 16384;
      memory_WriteROM(49152, 16384, cartridge_buffer + offset);
      break;
          
    case CARTRIDGE_TYPE_SUPERCART_LARGE: 
      offset = cartridge_size - 16384;
      memory_WriteROM(49152, 16384, cartridge_buffer + offset);
      memory_WriteROM(16384, 16384, cartridge_buffer + cartridge_GetBankOffset(0));
      break;
          
    case CARTRIDGE_TYPE_SUPERCART_RAM: 
      offset = cartridge_size - 16384;
      memory_WriteROM(49152, 16384, cartridge_buffer + offset);
      memory_ClearROM(16384, 16384);
      break;
          
    case CARTRIDGE_TYPE_SUPERCART_RAMX2:
      offset = cartridge_size - 16384;
      memory_WriteROM(49152, 16384, cartridge_buffer + offset);
      memory_ClearROM(16384, 16384);
      break;          
          
    case CARTRIDGE_TYPE_SUPERCART_ROM:
      offset = cartridge_size - 16384;
      lastBank = (cartridge_size/16384)-1;
      memory_WriteROM(49152, 16384, cartridge_buffer + offset);        
      memory_WriteROM(16384, 16384, cartridge_buffer + cartridge_GetBankOffset(lastBank-1));
      break;
          
    case CARTRIDGE_TYPE_ABSOLUTE:
      memory_WriteROM(16384, 16384, cartridge_buffer);
      memory_WriteROM(32768, 32768, cartridge_buffer + cartridge_GetBankOffset(2));
      break;
          
    case CARTRIDGE_TYPE_ACTIVISION:
      memory_WriteROM(40960, 16384, cartridge_buffer);
      memory_WriteROM(16384, 8192, cartridge_buffer + 106496);
      memory_WriteROM(24576, 8192, cartridge_buffer + 98304);
      memory_WriteROM(32768, 8192, cartridge_buffer + 122880);
      memory_WriteROM(57344, 8192, cartridge_buffer + 114688);
      break;
          
    case CARTRIDGE_TYPE_FRACTALUS:
      if (cartridge_size <= 49152) memory_WriteROM(65536 - cartridge_size, cartridge_size, cartridge_buffer);
      memory_ClearROM(0x4000, 0x4000);
      break;
          
    case CARTRIDGE_TYPE_BANKSETS:
      if ((cartridge_size/2) <= (52 * 1024))
      {
          offset = 0;
          memory_WriteROM(65536 - (cartridge_size/2), (cartridge_size/2), cartridge_buffer + offset);
          memcpy(&banksets_memory[65536 - (cartridge_size/2)], cartridge_buffer + (cartridge_size/2), (cartridge_size/2));
          if ((cartridge_size/2) >= (48*1024)) write_only_pokey_at_4000 = true;
      }
      else
      {
          offset = (cartridge_size/2) - 0x4000;
          memory_WriteROM(0xC000, 0x4000, cartridge_buffer + offset);
          memcpy(&banksets_memory[0xC000], &cartridge_buffer[cartridge_size - 0x4000], 0x4000);
      }
      break;
          
    case CARTRIDGE_TYPE_BANKSETS_RAM:
    case CARTRIDGE_TYPE_BANKSETS_HALTRAM:
      if ((cartridge_size/2) <= (52 * 1024))
      {
          memory_WriteROM(65536 - (cartridge_size/2), (cartridge_size/2), cartridge_buffer);
          memcpy(&banksets_memory[65536 - (cartridge_size/2)], cartridge_buffer + (cartridge_size/2), (cartridge_size/2));
          if ((cartridge_size/2) >= (48*1024)) write_only_pokey_at_4000 = true;
      }
      else
      {
          memory_WriteROM(0xC000, 0x4000, cartridge_buffer + (cartridge_size/2) - 0x4000);
          memcpy(&banksets_memory[0xC000], cartridge_buffer + (cartridge_size) - 0x4000, 0x4000);
      }
      memory_ClearROM(0x4000, 0x4000);
      memset(&banksets_memory[0x4000], 0x00, 0x4000);
      break;
  }
}

// ----------------------------------------------------------------------------
// Cart Write - this may cause a bankswitch
// ----------------------------------------------------------------------------
ITCM_CODE void cartridge_Write(word address, byte data) {
  switch(myCartInfo.cardtype) 
  {
    case CARTRIDGE_TYPE_SUPERCART:
    case CARTRIDGE_TYPE_SUPERCART_RAM:
    case CARTRIDGE_TYPE_SUPERCART_ROM:
    case CARTRIDGE_TYPE_SUPERCART_RAMX2:
      if ((address & 0xC000) == 0x8000) // Is this a bankswitching write?
      {
        cartridge_WriteBank(32768, data);
      }
      else if (address == 0xFFFF) cartridge_SwapRAM_DragonFlyStyle(data); // For the Dragonfly way of RAM banking
      break;
          
    case CARTRIDGE_TYPE_SUPERCART_LARGE:
      if ((address & 0xC000) == 0x8000) // Is this a bankswitching write?
      {
        cartridge_WriteBank(32768, data+1);
      }
      break;
          
    case CARTRIDGE_TYPE_ABSOLUTE:
      if(address == 32768 && (data == 1 || data == 2)) 
      {
        cartridge_WriteBank(16384, data-1);
      }
      break;
          
    case CARTRIDGE_TYPE_ACTIVISION:
      if(address >= 65408) 
      {
        cartridge_WriteBank(40960, (address & 7));
      }
      break;
          
    case CARTRIDGE_TYPE_BANKSETS:
    case CARTRIDGE_TYPE_BANKSETS_RAM:
      if ((address & 0xC000) == 0x8000) // Is this a bankswitching write?
      {
          // We need to swap in the main Sally memory...
          cartridge_WriteBank(0x8000, data);
          // And also swap in the Maria memory... this ROM starts half-way up the main cartridge_buffer[]
          u32 offset = (cartridge_size/2) + (data*0x4000);
          if (offset < (272*1024))    // If we are in fast VRAM memory
              memcpy(&banksets_memory[0x8000], (u32*)(0x06860000 + offset), 0x4000);
          else
              memcpy(&banksets_memory[0x8000], &cartridge_buffer[offset], 0x4000);
      }
      break;
          
    case CARTRIDGE_TYPE_BANKSETS_HALTRAM:
      if ((address & 0xC000) == 0x8000) // Is this a bankswitching write?
      {
          // We need to swap in the main Sally memory...
          cartridge_WriteBank(0x8000, data);
          // And also swap in the Maria memory... this ROM starts half-way up the main cartridge_buffer[]
          u32 offset = (cartridge_size/2) + (data*0x4000);
          if (offset < (272*1024))    // If we are in fast VRAM memory
              memcpy(&banksets_memory[0x8000], (u32*)(0x06860000 + offset), 0x4000);
          else
              memcpy(&banksets_memory[0x8000], &cartridge_buffer[offset], 0x4000);
      }
      else if ((address & 0xC000) == 0xC000) // Are we writing to MARIA HALT RAM?
      {
          // Write the data into the 0x4000-0x7FFF region - for Sally, this is write only but will be seen by Maria
          banksets_memory[0x4000 + (address & 0x3FFF)] = data;
      }
      break;
  }
}

// ----------------------------------------------------------------------------
// IsLoaded
// ----------------------------------------------------------------------------
bool cartridge_IsLoaded( ) {
  return (cartridge_size > 0) ? true:false;
}

// ----------------------------------------------------------------------------
// Release cart, save SRAM if used, set common vars back to system default.
// ----------------------------------------------------------------------------
void cartridge_Release( ) 
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
    myCartInfo.hasHeader = false;
    myCartInfo.dma_adjust = 0;
    last_bank = 255;
    last_ex_ram_bank = 0;
    ex_ram_bank = 0;
    last_ex_ram_bank_df = 0;
    ex_ram_bank_df = 0;
    write_only_pokey_at_4000 = false;
    if (isDS_LITE) shadow_ram = ex_ram_bank ? (u8*)(ex_ram_buffer+0x0000) : (u8*)(ex_ram_buffer+0x4000);  // for DS-Lite
    else shadow_ram = ex_ram_bank ? (u8*)0x06830000 : (u8*)0x06834000;   // // Only for the DSi.. see DS_LITE handling above
    shadow_ram -= 0x4000; // Makes for faster indexing in Memory.c
}
