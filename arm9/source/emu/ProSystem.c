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

#include "ProSystem.h"
#include "Database.h"

extern u8 isDS_LITE;
extern u8 frameSkipMask;

bool prosystem_active __attribute__((section(".dtcm"))) = false;
bool prosystem_paused __attribute__((section(".dtcm"))) = false;
uint prosystem_cycles __attribute__((section(".dtcm"))) = 0;
uint32 bg32           __attribute__((section(".dtcm"))) = 0;
uint bRenderFrame     __attribute__((section(".dtcm"))) = 0;


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
    cartridge_Store( );

    prosystem_cycles = sally_ExecuteRES( );
    prosystem_active = true;
  }
}

// ----------------------------------------------------------------------------
// ExecuteFrame - this is hand-tuned for NTSC output with hard-coded
// NTSC frame values ... this will not work properly if a PAL ROM is used.
// ----------------------------------------------------------------------------
ITCM_CODE void prosystem_ExecuteFrame(const byte* input) 
{
  extern u16 gTotalAtariFrames;
  extern word *framePtr;
  extern uint maria_cycles;

  gTotalAtariFrames++;
  bRenderFrame = 0;

  riot_SetInput(input);
  
  // ------------------------------------------------------------
  // Handle the TOP area first... speeds up processing below...
  // ------------------------------------------------------------
  for (maria_scanline = 1; maria_scanline <= 16; maria_scanline++) 
  {
    prosystem_cycles %= CYCLES_PER_SCANLINE;
      
    if (maria_scanline & 0x10) 
    {
      memory_ram[MSTAT] = 0;
      framePtr = (word*)(maria_surface);
      sally_Execute(34);
      maria_RenderScanlineTOP( );
      if(myCartInfo.steals_cycles == NO_STEALING) 
      {
        prosystem_cycles += maria_cycles;
        if(riot_timing&2) riot_UpdateTimer( maria_cycles >> 2 );
      }
    }
    else
    {    
        sally_Execute(34);
    }
    
    sally_Execute(CYCLES_PER_SCANLINE);
      
    if(myCartInfo.pokeyType) // If pokey enabled, we process 1 pokey sample and 1 TIA sample. Good enough.
    {
        pokey_Process();
        pokey_Scanline();
    } else tia_Process(); // If all we have to deal with is the TIA, we can do so at 31KHz (or half that for DS LITE)
  }
    
  // ------------------------------------------------------------
  // Up to line 26 is "off-screen" so we don't bother to render
  // ------------------------------------------------------------
  for (; maria_scanline < 26; maria_scanline++) 
  {
    prosystem_cycles %= CYCLES_PER_SCANLINE;
      
    sally_Execute(34);

    maria_RenderScanline( );
    
    if(myCartInfo.steals_cycles == NO_STEALING) 
    {
      prosystem_cycles += maria_cycles;
      if(riot_timing&2) riot_UpdateTimer( maria_cycles >> 2 );
    }
 
    sally_Execute(CYCLES_PER_SCANLINE);
      
    if(myCartInfo.pokeyType) // If pokey enabled, we process 1 pokey sample and 1 TIA sample. Good enough.
    {
        pokey_Process();
        pokey_Scanline();
    } else tia_Process(); // If all we have to deal with is the TIA, we can do so at 31KHz (or half that for DS LITE)
  }
    
  // -----------------------------------------------------------------------------------
  // At line 26 we can start to render the scanlines if we are not skipping this frame.
  // -----------------------------------------------------------------------------------
  bRenderFrame = gTotalAtariFrames & frameSkipMask;
    
  // ------------------------------------------------------------
  // Now handle the REST of the display area...
  // ------------------------------------------------------------
  for (; maria_scanline < 258; maria_scanline++) 
  {
    prosystem_cycles %= CYCLES_PER_SCANLINE;
      
    if (maria_scanline == 247) 
    {
       bRenderFrame = 0;
    }
      
    sally_Execute(34);

    maria_RenderScanline( );
    
    if(myCartInfo.steals_cycles == NO_STEALING) 
    {
      prosystem_cycles += maria_cycles;
      if(riot_timing&2) riot_UpdateTimer( maria_cycles >> 2 );
    }
 
    sally_Execute(CYCLES_PER_SCANLINE);
      
    if(myCartInfo.pokeyType) // If pokey enabled, we process 1 pokey sample and 1 TIA sample. Good enough.
    {
        pokey_Process();
        pokey_Scanline();
    } else tia_Process(); // If all we have to deal with is the TIA, we can do so at 31KHz (or half that for DS LITE)
  }    
    
  memory_ram[MSTAT] = 128;
  
  for (; maria_scanline < 262; maria_scanline++) 
  {
    prosystem_cycles %= CYCLES_PER_SCANLINE;
      
    sally_Execute(34);

    sally_Execute(CYCLES_PER_SCANLINE);
    if(myCartInfo.pokeyType) // If pokey enabled, we process 1 pokey sample and 1 TIA sample. Good enough.
    {
        pokey_Process();
        pokey_Scanline();
    } else tia_Process(); // If all we have to deal with is the TIA, we can do so at 31KHz (or half that for DS LITE)
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
