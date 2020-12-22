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
void prosystem_Reset( ) 
{
  if(cartridge_IsLoaded( )) 
  {
    prosystem_paused = false;
    sally_Reset( );
    region_Reset( );
    tia_Clear( );
    tia_Reset( );
    pokey_Clear( );
    pokey_Reset( );
    memory_Reset( );
    maria_Clear( );
    maria_Reset( );
    riot_Reset ( );
    cartridge_LoadHighScoreCart();
    if(bios_enabled) 
    {
      bios_Store( );
    }
    else 
    {
      cartridge_Store( );
    }

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
          last_background = memory_ram[BACKGRND];
          bg32 =  last_background | (last_background<<8) | (last_background<<16) | (last_background<<24);
      }
      cycles = maria_RenderScanline( );
    }
    
    if(cartridge_steals_cycles) 
    {
      prosystem_cycles += cycles;
      if(riot_timing) riot_UpdateTimer( cycles >> 2 );
    }
 
    sally_Execute(CYCLES_PER_SCANLINE);
    tia_Process(2);
    if(cartridge_pokey) 
    {
        pokey_Process(2);      
        pokey_Scanline();
    }
  }  
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
