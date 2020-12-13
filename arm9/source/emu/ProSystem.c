// ----------------------------------------------------------------------------
//   ___  ___  ___  ___       ___  ____  ___  _  _
//  /__/ /__/ /  / /__  /__/ /__    /   /_   / |/ /
// /    / \  /__/ ___/ ___/ ___/   /   /__  /    /  emulator
//
// ----------------------------------------------------------------------------
// Copyright 2003, 2004 Greg Stanton
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
// ProSystem.cpp
// ----------------------------------------------------------------------------

#include <malloc.h>
#include "ProSystem.h"
#include "Sound.h"
#define PRO_SYSTEM_SOURCE "ProSystem.cpp"
#define PRO_SYSTEM_STATE_HEADER "PRO-SYSTEM STATE"

#define CYCLES_PER_SCANLINE 454

bool prosystem_active = false;
bool prosystem_paused = false;
word prosystem_frequency = 60;
word prosystem_scanlines = 262;
uint prosystem_cycles = 0;

// Whether the last CPU operation resulted in a half cycle (need to take it
// into consideration)
extern bool half_cycle;

// ----------------------------------------------------------------------------
// Reset
// ----------------------------------------------------------------------------
void prosystem_Reset( ) {
  if(cartridge_IsLoaded( )) {
    prosystem_paused = false;

    sally_Reset( ); // WII

    region_Reset( );
    tia_Clear( );
    tia_Reset( );
    pokey_Clear( );
    pokey_Reset( );
    memory_Reset( );
    maria_Clear( );
    maria_Reset( );
	  riot_Reset ( );
    if(bios_enabled) {
      bios_Store( );
    }
    else {
      cartridge_Store( );
    }

    cartridge_LoadHighScoreCart();
    prosystem_cycles = sally_ExecuteRES( );
    prosystem_active = true;
  }
}
uint32 bg32 = 0;
// ----------------------------------------------------------------------------
// ExecuteFrame
// ----------------------------------------------------------------------------
ITCM_CODE void prosystem_ExecuteFrame(const byte* input) 
{
  static byte last_background = 254;
  extern int gTotalAtariFrames;
  extern word *framePtr;
  bool bRenderScanline = false;

  gTotalAtariFrames++;

  riot_SetInput(input);
  
  for(maria_scanline = 1; maria_scanline <= prosystem_scanlines; maria_scanline++) 
  {
    if(maria_scanline == maria_displayArea.top) 
    {
      memory_ram[MSTAT] = 0;
      framePtr = (word*)(maria_surface + ((maria_scanline - maria_displayArea.top) * 256));
      bRenderScanline = true;
    }
    else if(maria_scanline == maria_displayArea.bottom) 
    {
      memory_ram[MSTAT] = 128;
      bRenderScanline = false;
    }
    
    uint cycles=0;
    prosystem_cycles %= CYCLES_PER_SCANLINE;
    sally_Execute(34);
    
    if (bRenderScanline)
    {
      // If our background has changed... set our global 32-bit version of that now... speeds up scanline renders
      if (memory_ram[BACKGRND] != last_background)
      {
          bg32 =  memory_ram[BACKGRND] | (memory_ram[BACKGRND] << 8) | (memory_ram[BACKGRND]<<16) | (memory_ram[BACKGRND]<<24);
          last_background = memory_ram[BACKGRND];
      }
      cycles = maria_RenderScanline( );
    }
    
    if(cartridge_cycle_stealing) 
    {
      prosystem_cycles += cycles;
      if(riot_timing) riot_UpdateTimer( cycles >> 2 );
    }
 
    sally_Execute(CYCLES_PER_SCANLINE);
    tia_Process(2);
    if(cartridge_pokey) pokey_Process(2);      
  }  
}

byte *loc_buffer = 0;

// ----------------------------------------------------------------------------
// Save
// ----------------------------------------------------------------------------
bool prosystem_Save(char * filename, bool compress) 
{

  if(strlen(filename) == 0) {
#if 0
    logger_LogError("Filename is invalid.", PRO_SYSTEM_SOURCE);
#endif
    return false;
  }

  if (! loc_buffer) loc_buffer = (byte *)malloc(33000 * sizeof(byte));

#if 0
  logger_LogInfo("Saving game state to file " + filename + ".");
#endif
  
  uint size = 0;
  
  uint index;
  for(index = 0; index < 16; index++) {
    loc_buffer[size + index] = PRO_SYSTEM_STATE_HEADER[index];
  }
  size += 16;
  
  loc_buffer[size++] = 1;
  for(index = 0; index < 4; index++) {
    loc_buffer[size + index] = 0;
  }
  size += 4;

  for(index = 0; index < 32; index++) {
    loc_buffer[size + index] = cartridge_digest[index];
  }
  size += 32;

  loc_buffer[size++] = sally_a;
  loc_buffer[size++] = sally_x;
  loc_buffer[size++] = sally_y;
  loc_buffer[size++] = sally_p;
  loc_buffer[size++] = sally_s;
  loc_buffer[size++] = sally_pc.b.l;
  loc_buffer[size++] = sally_pc.b.h;
  loc_buffer[size++] = cartridge_bank;

  for(index = 0; index < 16384; index++) {
    loc_buffer[size + index] = memory_ram[index];
  }
  size += 16384;
  
  if(cartridge_type == CARTRIDGE_TYPE_SUPERCART_RAM) {
    for(index = 0; index < 16384; index++) {
      loc_buffer[size + index] = memory_ram[16384 + index];
    } 
    size += 16384;
  }
  
  if(!compress) {
    FILE* file = fopen(filename, "wb");
    if(file == NULL) {
#if 0
      logger_LogError("Failed to open the file " + filename + " for writing.", PRO_SYSTEM_SOURCE);
#endif
      return false;
    }
  
    if(fwrite(loc_buffer, 1, size, file) != size) {
      fclose(file);
#if 0
      logger_LogError("Failed to write the save state data to the file " + filename + ".", PRO_SYSTEM_SOURCE);
#endif
      return false;
    }
  
    fclose(file);
  }
  else {
    if(!archive_Compress(filename, "Save.sav", loc_buffer, size)) {
#if 0
      logger_LogError("Failed to compress the save state data to the file " + filename + ".", PRO_SYSTEM_SOURCE);
#endif
      return false;
    }
  }
  return true;
}

// ----------------------------------------------------------------------------
// Load
// ----------------------------------------------------------------------------
bool prosystem_Load(char * filename) {

  if(strlen(filename) == 0) {
#if 0
    logger_LogError("Filename is invalid.", PRO_SYSTEM_SOURCE);    
#endif
    return false;
  }

  if (! loc_buffer) loc_buffer = (byte *)malloc(33000 * sizeof(byte));

#if 0
  logger_LogInfo("Loading game state from file " + filename + ".");
#endif
  
  uint size = archive_GetUncompressedFileSize(filename);
  if(size == 0) {
    FILE* file = fopen(filename, "rb");
    if(file == NULL) {
#if 0
      logger_LogError("Failed to open the file " + filename + " for reading.", PRO_SYSTEM_SOURCE);
#endif
      return false;
    }

    fseek(file, 0, SEEK_END);
#if 0
    if(fseek(file, 0, SEEK_END)) {
      fclose(file);
      logger_LogError("Failed to find the end of the file.", PRO_SYSTEM_SOURCE);
      return false;
    }
#endif
  
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
#if 0
    if(fseek(file, 0, SEEK_SET)) {
      fclose(file);
      logger_LogError("Failed to find the size of the file.", PRO_SYSTEM_SOURCE);
      return false;
    }
#endif

    if(size != 16445 && size != 32829) {
      fclose(file);
#if 0
      logger_LogError("Save state file has an invalid size.", PRO_SYSTEM_SOURCE);
#endif
      return false;
    }
  
    fread(loc_buffer, 1, size, file);
#if 0
    if(fread(loc_buffer, 1, size, file) != size && ferror(file)) {
      fclose(file);
      logger_LogError("Failed to read the file data.", PRO_SYSTEM_SOURCE);
      return false;
    }
#endif
    fclose(file);
  }  
  else if(size == 16445 || size == 32829) {
    archive_Uncompress(filename, loc_buffer, size);
  }
  else {
#if 0
    logger_LogError("Save state file has an invalid size.", PRO_SYSTEM_SOURCE);
#endif
    return false;
  }

  uint offset = 0;
  uint index;
  for(index = 0; index < 16; index++) {
    if(loc_buffer[offset + index] != PRO_SYSTEM_STATE_HEADER[index]) {
#if 0
      logger_LogError("File is not a valid ProSystem save state.", PRO_SYSTEM_SOURCE);
#endif
      return false;
    }
  }
  offset += 16;
  
  for(index = 0; index < 4; index++) {
  }
  offset += 4;
  
  prosystem_Reset( );
  
  char digest[33] = {0};
  for(index = 0; index < 32; index++) {
    digest[index] = loc_buffer[offset + index];
  }
  offset += 32;
  if (strcmp((const char*)cartridge_digest,digest)) {
  //if(cartridge_digest != std::string(digest)) {
#if 0
    logger_LogError("Load state digest [" + std::string(digest) + "] does not match loaded cartridge digest [" + cartridge_digest + "].", PRO_SYSTEM_SOURCE);
#endif
    return false;
  }
  
  sally_a = loc_buffer[offset++];
  sally_x = loc_buffer[offset++];
  sally_y = loc_buffer[offset++];
  sally_p = loc_buffer[offset++];
  sally_s = loc_buffer[offset++];
  sally_pc.b.l = loc_buffer[offset++];
  sally_pc.b.h = loc_buffer[offset++];
  
  cartridge_StoreBank(loc_buffer[offset++]);

  for(index = 0; index < 16384; index++) {
    memory_ram[index] = loc_buffer[offset + index];
  }
  offset += 16384;

  if(cartridge_type == CARTRIDGE_TYPE_SUPERCART_RAM) {
    if(size != 32829) {
#if 0
      logger_LogError("Save state file has an invalid size.", PRO_SYSTEM_SOURCE);
#endif
      return false;
    }
    for(index = 0; index < 16384; index++) {
      memory_ram[16384 + index] = loc_buffer[offset + index];
    }
    offset += 16384; 
  }  

  return true;
}

// ----------------------------------------------------------------------------
// Pause
// ----------------------------------------------------------------------------
void prosystem_Pause(bool pause) {
  if(prosystem_active) {
    prosystem_paused = pause;
  }
}

// ----------------------------------------------------------------------------
// Close
// ----------------------------------------------------------------------------
void prosystem_Close( ) {
  prosystem_active = false;
  prosystem_paused = false;
  cartridge_Release( );
  maria_Reset( );
  maria_Clear( );
  memory_Reset( );
  tia_Reset( );
  tia_Clear( );
}
