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

uint prosystem_cycles __attribute__((section(".dtcm"))) = 0;
uint32 bg32           __attribute__((section(".dtcm"))) = 0;
uint bRenderFrame     __attribute__((section(".dtcm"))) = 0;


#define HBLANK_BEFORE_DMA   34      // Number of cycles in a HBLANK 
#define CYCLES_PER_SCANLINE 454     // 454 Cycles per Scanline in an NTSC system (really 113.5 CPU cycles)


// ----------------------------------------------------------------------------
// Reset
// ----------------------------------------------------------------------------
void prosystem_Reset( ) 
{
  if(cartridge_IsLoaded( )) 
  {
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

    cartridge_Store( );  // Always call this - it may setup some RAM or other stuff below the BIOS region...
    
    // Load 7800 BIOS if available... otherwise direct load the CART
    if (bios_available && !bSkipBIOS)
    {
         bios_Store();
         bios_show_counter = myCartInfo.biosTimeout;
    }

    prosystem_cycles = sally_ExecuteRES( );
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
  
  // ---------------------------------------------------------------------
  // Handle the VERTICAL BLANK area first... speeds up processing below...
  // ---------------------------------------------------------------------
  memory_ram[MSTAT] = 128;  // Into the Vertical Blank...

  // -------------------------------------------------------------------------------------------  
  // Note: this is not accurate. It should be 20 scanlines of Vertical Blank but experimentally
  // this is working for the emulation across ~200 games both with and without the BIOS enabled.
  // Other combinations have produced some games that run too fast (Asteroids) and some games
  // that crash (Robotron) and some issues with Pole Position II and the joystick selection
  // of a course to play. I've also seen graphical glitches in Crossbow when loaded via the 
  // BIOS. Be very careful if you change this... be sure you understand the consequences (and
  // this developer doesn't... so be warned - don't follow this code for accuracy advice!).
  // -------------------------------------------------------------------------------------------  
  for (maria_scanline = 1; maria_scanline <= 22; maria_scanline++) 
  {
    prosystem_cycles = 0;

    sally_Execute(HBLANK_BEFORE_DMA);
    
    if (maria_scanline == 22) // Maria can start to do her thing... 
    {
      memory_ram[MSTAT] = 0;  // Out of the Vertical Blank
      framePtr = (word*)(maria_surface);
        
      maria_RenderScanlineTOP();
      
      // Cycle Stealing happens here...
      prosystem_cycles += ((maria_cycles+3) >> 2) << 2; // Always a multiple of 4
      if(riot_and_wsync&2) riot_UpdateTimer( maria_cycles >> 2 );
    }
    
    sally_Execute(CYCLES_PER_SCANLINE); // Return value is the cycle deficit from this scanline
      
    if(myCartInfo.pokeyType) // If pokey enabled, we process 1 pokey sample and 1 TIA sample. Good enough.
    {
        pokey_Process();
        pokey_Scanline();
    } else tia_Process(); // If all we have to deal with is the TIA, we can do so at 31KHz
  }   
    
  // -------------------------------------------------------------
  // Now handle the Main display area... All the way to the final 
  // scanline which is 263 (often reported incorrectly as 262).
  // -------------------------------------------------------------
  for (; maria_scanline <= 263; maria_scanline++) 
  {
    prosystem_cycles = 0;
      
    if (maria_scanline & 0xFFE0) // Anything at or above line 32 we can start to render..
    {
       // -------------------------------------------------------------------------
       // We can start to render the scanlines if we are not skipping this frame.
       // For the DSi, we generally don't skip any frames (the mask will be 0xFF).
       // -------------------------------------------------------------------------
       if (maria_scanline & 0x100) bRenderFrame = 0; // Above 256 we can stop... DS can't display this anyway
       else bRenderFrame = gTotalAtariFrames & frameSkipMask;
    } 
     
    sally_Execute(HBLANK_BEFORE_DMA);

    maria_RenderScanline();
    
    // Cycle Stealing happens here...
    prosystem_cycles += ((maria_cycles+3) >> 2) << 2; // Always a multiple of 4
    if(riot_and_wsync&2) riot_UpdateTimer( maria_cycles >> 2 );
 
    sally_Execute(CYCLES_PER_SCANLINE); // Return value is the cycle deficit from this scanline
      
    if(myCartInfo.pokeyType) // If pokey enabled, we process 1 pokey sample and 1 TIA sample. Good enough.
    {
        pokey_Process();
        pokey_Scanline();
    } else tia_Process(); // If all we have to deal with is the TIA, we can do so at 31KHz
  }    
}

// ----------------------------------------------------------------------------
// Close
// ----------------------------------------------------------------------------
void prosystem_Close( ) {
  cartridge_Release( );
  maria_Reset( );
  maria_Clear( );
  memory_Reset( );
  tia_Reset( );
  tia_Clear( );
}
