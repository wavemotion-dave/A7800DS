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
#define CARTRIDGE_SOURCE "Cartridge.cpp"

char cartridge_title[128];
char cartridge_description[128];
char cartridge_year[128];
char cartridge_maker[128];
byte cartridge_digest[128];
char cartridge_filename[128];
byte cartridge_type;
byte cartridge_region;
byte cartridge_pokey;
bool cartridge_hsc_enabled;
byte cartridge_controller[2];
byte cartridge_bank;
bool cartridge_steals_cycles;
bool cartridge_uses_wsync;
int  cartridge_xOffset = 0;
int  cartridge_yOffset = 22;
int  cartridge_xScale  = 256;
int  cartridge_yScale  = 220;
  
extern int debug[];  
static byte* cartridge_buffer = NULL;
static uint cartridge_size = 0;

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

static uint cartridge_GetBank(byte bank) {
  if ((cartridge_type == CARTRIDGE_TYPE_SUPERCART || cartridge_type == CARTRIDGE_TYPE_SUPERCART_ROM || cartridge_type == CARTRIDGE_TYPE_SUPERCART_RAM) && cartridge_size <= 65536) {
    // for some of these carts, there are only 4 banks. in this case we ignore bit 3
    // previously, games of this type had to be doubled. The first 4 banks needed to be duplicated at the end of the ROM
      return (bank & 3);
  }
  return bank;
}

// ----------------------------------------------------------------------------
// GetBankOffset
// ----------------------------------------------------------------------------
static uint cartridge_GetBankOffset(byte bank) {
  return cartridge_GetBank(bank) * 16384;
}

// ----------------------------------------------------------------------------
// WriteBank
// ----------------------------------------------------------------------------
static void cartridge_WriteBank(word address, byte bank) {
  uint offset = cartridge_GetBankOffset(bank);
  if(offset < cartridge_size) {
    memory_WriteROM(address, 16384, cartridge_buffer + offset);
    cartridge_bank = bank;
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
    if(cartridge_size > 131072) 
    {
      cartridge_type = CARTRIDGE_TYPE_SUPERCART_LARGE;
    }
    else if (header[54] & 0x04)
    {
      cartridge_type = CARTRIDGE_TYPE_SUPERCART_RAM;
    }
    else if(header[54] & 0x08) 
    {
      cartridge_type = CARTRIDGE_TYPE_SUPERCART_ROM;
    }
    else if(header[54] & 0x02) 
    {
      cartridge_type = CARTRIDGE_TYPE_SUPERCART;
    }
    else 
    {
      cartridge_type = CARTRIDGE_TYPE_NORMAL;
    }
  }
  else 
  {
    if(header[53] == 2) 
    {
      cartridge_type = CARTRIDGE_TYPE_ABSOLUTE;
    }
    else if(header[53] == 1) 
    {
      cartridge_type = CARTRIDGE_TYPE_ACTIVISION;
    }
    else 
    {
      cartridge_type = CARTRIDGE_TYPE_NORMAL;
    }
  }
  
  if (header[54] & 0x01) cartridge_pokey = POKEY_AT_4000;
  if (header[54] & 0x40) cartridge_pokey = POKEY_AT_450;
  
  cartridge_controller[0] = header[55];
  cartridge_controller[1] = header[56];
  cartridge_region = header[57];
  cartridge_hsc_enabled = (header[0x3A]&1 ? HSC_YES:HSC_NO);
  cartridge_steals_cycles = true;       // By default, assume the cart steals cycles
  cartridge_uses_wsync = true;          // By default, assume the cart uses wsync
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
    cartridge_ReadHeader(header);
    size -= 128;
    offset = 128;
  }
  else 
  {
    cartridge_size = size;
  }
  
  cartridge_buffer = (byte *) malloc(cartridge_size);
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
#if 0
    logger_LogError("Cartridge filename is invalid.", CARTRIDGE_SOURCE);
#endif
    return false;
  }
  
  cartridge_Release();
#if 0
  logger_LogInfo("Opening cartridge file " + filename + ".");
#endif
  
  byte* data = NULL;
  uint size = archive_GetUncompressedFileSize(filename);
  if(size == 0) {
    FILE *file = fopen(filename, "rb");
    if(file == NULL) {
#if 0
      logger_LogError("Failed to open the cartridge file " + filename + " for reading.", CARTRIDGE_SOURCE);
#endif
      return false;  
    }

    if(fseek(file, 0L, SEEK_END)) {
#if 0
      fclose(file);
      logger_LogError("Failed to find the end of the cartridge file.", CARTRIDGE_SOURCE);
      return false;
#endif
    }
    size = ftell(file);
    if(fseek(file, 0L, SEEK_SET)) {
#if 0
      fclose(file);
      logger_LogError("Failed to find the size of the cartridge file.", CARTRIDGE_SOURCE);
      return false;
#endif
    }

    data = (byte *) malloc(size);
    if(fread(data, 1, size, file) != size && ferror(file)) {
      fclose(file);
#if 0
      logger_LogError("Failed to read the cartridge data.", CARTRIDGE_SOURCE);
#endif
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
#if 0
    logger_LogError("Failed to load the cartridge data into memory.", CARTRIDGE_SOURCE);
#endif
    free(data);
    return false;
  }
  if(data != NULL) {
    free(data);
  }
  
  gameCRC = crc32(0, data, size);
  
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
  switch(cartridge_type) {
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
}

// ----------------------------------------------------------------------------
// Write
// ----------------------------------------------------------------------------
void cartridge_Write(word address, byte data) {
  switch(cartridge_type) {
    case CARTRIDGE_TYPE_SUPERCART:
    case CARTRIDGE_TYPE_SUPERCART_RAM:
    case CARTRIDGE_TYPE_SUPERCART_ROM:
      {
      uint maxbank = cartridge_size / 16384;
      if(address >= 32768 && address < 49152 && cartridge_GetBank(data) < maxbank /*9*/) {
        cartridge_StoreBank(data);
      }
      }
      break;
    case CARTRIDGE_TYPE_SUPERCART_LARGE:
      {
      uint maxbank = cartridge_size / 16384;
      if(address >= 32768 && address < 49152 && cartridge_GetBank(data) < maxbank /*9*/) {
        cartridge_StoreBank(data + 1);
      }
      }
      break;
    case CARTRIDGE_TYPE_ABSOLUTE:
      if(address == 32768 && (data == 1 || data == 2)) {
        cartridge_StoreBank(data - 1);
      }
      break;
    case CARTRIDGE_TYPE_ACTIVISION:
      if(address >= 65408) {
        cartridge_StoreBank(address & 7);
      }
      break;
  }
}

// ----------------------------------------------------------------------------
// StoreBank
// ----------------------------------------------------------------------------
void cartridge_StoreBank(byte bank) {
  switch(cartridge_type) {
    case CARTRIDGE_TYPE_SUPERCART:
      cartridge_WriteBank(32768, bank);
      break;
    case CARTRIDGE_TYPE_SUPERCART_RAM:
      cartridge_WriteBank(32768, bank);
      break;
    case CARTRIDGE_TYPE_SUPERCART_ROM:
      cartridge_WriteBank(32768, bank);
      break;
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



// The memory location of the high score cartridge SRAM
#define HS_SRAM_START 0x1000
// The size of the high score cartridge SRAM
#define HS_SRAM_SIZE 2048

byte high_score_cart_loaded = false;

/*
 * Saves the high score cartridge SRAM
 *
 * return   Whether the save was successful
 */
bool cartridge_SaveHighScoreSram() 
{    
    bool status = false;
    byte sram[HS_SRAM_SIZE];
    word retries = 3;
    if(!high_score_cart_loaded || !cartridge_hsc_enabled)
    {
        // If we didn't load the high score cartridge, or didn't access the HSC ROM, or don't have an HSC enabled cart: don't save.
        return false;
    }
  
    while (status == false)
    {
      u32 chksum = 0;
      for( uint i = 0; i < HS_SRAM_SIZE; i++ ) 
      {
          sram[i] = memory_ram[HS_SRAM_START+i];
          chksum += sram[i];
      }

      if (chksum != 0)
      {
          FILE* file = fopen("A7800DS.sram", "wb+");
          if( file != NULL ) 
          {
            if( fwrite( sram, 1, HS_SRAM_SIZE, file ) != HS_SRAM_SIZE ) 
            {
              status = false;              
            }
            fflush(file);
            fclose(file);
          }
      }
      if (--retries == 0) break;
    }

    return status;
}

/*
 * Loads the high score cartridge SRAM
 *
 * return   Whether the load was successful
 */
static bool cartridge_LoadHighScoreSram() 
{    
    bool status = false;
    byte sram[HS_SRAM_SIZE];
    word retries=3;
    
    while (status == false)
    {
        FILE* file = fopen("A7800DS.sram", "rb" );
        if( file == NULL ) 
        {
            status = false;
        }

        if( fread( sram, 1, HS_SRAM_SIZE, file ) == HS_SRAM_SIZE ) 
        {
            fclose( file );
            for( uint i = 0; i < HS_SRAM_SIZE; i++ ) 
            {
                memory_Write( HS_SRAM_START + i, sram[i] );
            }
            status = true;
        }
        fclose(file);
        
        if (--retries == 0) break;
    }
    
    return status;
}

/*
 * Loads the high score cartridge
 *
 * return   Whether the load was successful
 */
#define HSC_CART_ROM_SIZE 4096
byte high_score_buffer[HSC_CART_ROM_SIZE];
bool cartridge_LoadHighScoreCart() 
{
    if( !cartridge_hsc_enabled || cartridge_region != 0 ) 
    {
        // Only load the cart if it is enabled and the region is NTSC
        return false;
    }

    FILE* file = fopen("highscore.rom", "rb" );

    if( file != NULL )
    {
      fread(high_score_buffer, 1, HSC_CART_ROM_SIZE, file );
      cartridge_LoadHighScoreSram();
      for( uint i = 0; i < HSC_CART_ROM_SIZE; i++ )
      {
          memory_Write( 0x3000 + i, high_score_buffer[i] );
      }
      high_score_cart_loaded = true;
    }
    else 
    {
      high_score_cart_loaded = false;
    }

    return high_score_cart_loaded;
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

    free(cartridge_buffer);
    cartridge_size = 0;
    cartridge_buffer = NULL;

    // These values need to be reset so that moving between carts works
    // consistently. This seems to be a ProSystem emulator bug.
    //
    cartridge_type = 0;
    cartridge_region = 0;
    cartridge_pokey = POKEY_NONE;
    cartridge_hsc_enabled = false;
   
    memset( cartridge_controller, 0, sizeof( cartridge_controller ) );
    cartridge_bank = 0;
    cartridge_steals_cycles = false;
    cartridge_uses_wsync = false;
  }
}
