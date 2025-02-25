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


#define CYCLES_BEFORE_DMA    28     // Number of cycles before DMA kicks in (really 7 CPU cycles)
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
  // Note: this is not accurate. It should be 263 scanlines total but it doesn't work for all
  // games at 263 so we've gone with 262 for maximum compatibility. This is likely due to some
  // emulation cycle counting inaccuracies. At 263, some games run too fast (Asteroids, Deluxe) 
  // and some crash (Robotron) and some issues with Pole Position II and the joystick selection
  // of a course to play. I've also seen graphical glitches in Crossbow when loaded via the 
  // BIOS and all kinds of graphical oddities in Xenophobe. Be very careful if you change this...
  // be sure you understand the consequences (and this developer doesn't... so be warned!).
  // -------------------------------------------------------------------------------------------    
  for (maria_scanline = 1; maria_scanline <= 21; maria_scanline++) 
  {
    prosystem_cycles = 0;
      
    if (maria_scanline == 21)   // Maria can start to do her thing... We've had 20 VBLANK scanlines
    {
      memory_ram[MSTAT] = 0;    // Out of the vertical blank
      framePtr = (word*)(maria_surface);
      sally_Execute(CYCLES_BEFORE_DMA);
        
      maria_RenderScanlineTOP( );
      
      // Cycle Stealing happens here...
      prosystem_cycles += ((maria_cycles+3) >> 2) << 2; // Always a multiple of 4
      if(riot_and_wsync&2) riot_UpdateTimer( maria_cycles >> 2 );
    }
    else
    {    
        sally_Execute(CYCLES_BEFORE_DMA);
    }
    
    sally_Execute(CYCLES_PER_SCANLINE);
      
    if(myCartInfo.pokeyType) // If pokey enabled, we process 1 pokey sample and 1 TIA sample. Good enough.
    {
        pokey_Process();
        pokey_Scanline();
    } else tia_Process(); // If all we have to deal with is the TIA, we can do so at 31KHz
  }   
    
  // ------------------------------------------------------------
  // Now handle the Main display area...
  // ------------------------------------------------------------
  for (; maria_scanline < 263; maria_scanline++) 
  {
    prosystem_cycles = 0;
      
    if (maria_scanline >= 30) 
    {
       // ------------------------------------------------------------------------
       // We can start to render the scanlines if we are not skipping this frame.
       // ------------------------------------------------------------------------
       bRenderFrame = gTotalAtariFrames & frameSkipMask;
    } 
    else if (maria_scanline == 251)
    {
       bRenderFrame = 0;    // We can stop rendering frames... DS can't show it anyway.
    }
      
    sally_Execute(CYCLES_BEFORE_DMA);

    maria_RenderScanline( );
    
    // Cycle Stealing happens here...
    prosystem_cycles += ((maria_cycles+3) >> 2) << 2; // Always a multiple of 4
    if(riot_and_wsync&2) riot_UpdateTimer( maria_cycles >> 2 );
 
    sally_Execute(CYCLES_PER_SCANLINE);
      
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
