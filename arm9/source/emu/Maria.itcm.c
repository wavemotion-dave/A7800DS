// ----------------------------------------------------------------------------
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
// Maria.c
// ----------------------------------------------------------------------------
#include "Maria.h"

#include "ProSystem.h"

union ColorUnion
{
    int32 color32;
    struct {
        byte color0;
        byte color1;
        byte color2;
        byte color3;
    } by;
    struct {
        word color0;
        word color1;
    } wo;
};
union ColorUnion colors;

#define MARIA_LINERAM_SIZE 160

rect maria_displayArea = {0, 16, 319, 258};
rect maria_visibleArea = {0, 26, 319, 248};

word* maria_surface = 0;
word  maria_scanline = 1;

static byte maria_lineRAM[MARIA_LINERAM_SIZE+4];
static uint maria_cycles;
static pair maria_dpp;
static pair maria_dp;
static pair maria_pp;
static byte maria_horizontal;
static byte maria_palette;
static char maria_offset;
static byte maria_h08;
static byte maria_h16;
static byte maria_wmode;

static inline void _maria_ClearCells(void)
{
    if (memory_ram[CTRL] & 4)
    {
      *((u16 *)&maria_lineRAM[maria_horizontal]) = 0;
    }
    maria_horizontal += 2;
}

static inline void _maria_ClearCells4(void)
{
    if (memory_ram[CTRL] & 4)
    {
      *((u32 *)&maria_lineRAM[maria_horizontal]) = 0;
    }
    maria_horizontal += 4;
}

// ----------------------------------------------------------------------------
// StoreCell
// ----------------------------------------------------------------------------
static inline void _maria_StoreCells4(byte data) 
{
  if(maria_horizontal < MARIA_LINERAM_SIZE) 
  {
    byte kmode = memory_ram[CTRL] & 4;
    byte *ptr = &(maria_lineRAM[maria_horizontal+3]);
    if (data & 0x03) *ptr-- = maria_palette | (data & 0x03); else if (kmode) *ptr-- = 0; else ptr--;
    data = data >> 2;
    if (data & 0x03) *ptr-- = maria_palette | (data & 0x03); else if (kmode) *ptr-- = 0; else ptr--;
    data = data >> 2;
    if (data & 0x03) *ptr-- = maria_palette | (data & 0x03); else if (kmode) *ptr-- = 0; else ptr--;
    data = data >> 2;
    if (data) *ptr = maria_palette | (data); else if (kmode) *ptr = 0;
  }
  maria_horizontal += 4;
}

static inline void maria_StoreCellWide(byte data) 
{
  if(maria_horizontal < MARIA_LINERAM_SIZE) 
  {
      maria_lineRAM[maria_horizontal++] = (maria_palette & 16) | data;
  }
  else
  {
    maria_horizontal++;maria_horizontal++;        
  }
}

// ----------------------------------------------------------------------------
// StoreCell
// ----------------------------------------------------------------------------
static inline void maria_ClearCellWide(void) 
{
  if(maria_horizontal < MARIA_LINERAM_SIZE) 
  {
      if(memory_ram[CTRL] & 4) 
      {
        maria_lineRAM[maria_horizontal++] = 0;
        maria_lineRAM[maria_horizontal++] = 0;
      }
      else
      {
        maria_horizontal++;maria_horizontal++;        
      }
  }  
  else
  {
    maria_horizontal++;maria_horizontal++;        
  }
}

// ----------------------------------------------------------------------------
// StoreCell
// ----------------------------------------------------------------------------
static inline void maria_StoreCell(byte high, byte low) 
{
  if(maria_horizontal < MARIA_LINERAM_SIZE) 
  {
    byte data = high | low;
    if(data) 
    {
      maria_lineRAM[maria_horizontal] = (maria_palette & 16) | data;
    }
    else 
    { 
      if(memory_ram[CTRL] & 4) 
      {
        maria_lineRAM[maria_horizontal] = 0;
      }
    }
  }
  maria_horizontal++;
}

// ----------------------------------------------------------------------------
// IsHolyDMA
// ----------------------------------------------------------------------------
static inline bool maria_IsHolyDMA( ) 
{
  if(maria_pp.w > 32767) 
  {
    if(maria_h16 && (maria_pp.w & 4096)) {
      return true;
    }
    if(maria_h08 && (maria_pp.w & 2048)) {
      return true;
    }
  }
  return false;
}

// ----------------------------------------------------------------------------
// GetColor
// ----------------------------------------------------------------------------
static inline byte maria_GetColor(byte data) 
{
  return (data & 3) ? memory_ram[BACKGRND + data] : memory_ram[BACKGRND];
}


// ----------------------------------------------------------------------------
// StoreGraphic
// ----------------------------------------------------------------------------
static inline void maria_StoreGraphic( ) 
{
  byte data = memory_ram[maria_pp.w];
  if(maria_wmode) 
  {
    if(maria_IsHolyDMA() || !data)
    {
      maria_ClearCellWide();
    }
    else
    {
      maria_StoreCellWide((data & 0x0C) | ((data & 0xC0) >> 6));
      maria_StoreCellWide(((data & 0x30) >> 4) | ((data & 0x03) << 2));
    }
  }
  else 
  {
    if(maria_IsHolyDMA() || !data) 
    {
      _maria_ClearCells4();
    }
    else 
    {
      _maria_StoreCells4(data);
    }
  }
  maria_pp.w++;
}

// ----------------------------------------------------------------------------
// WriteLineRAM
// ----------------------------------------------------------------------------
static inline void maria_WriteLineRAM(word* buffer) 
{
  extern uint32 bg32;
  uint index;
  unsigned int *pix=(unsigned int *) buffer;
  uint32 *ptr = (uint32 *)&maria_lineRAM[0];
  byte rmode = memory_ram[CTRL] & 3;
    
  if(rmode == 0) 
  {
    for(index = 0; index < MARIA_LINERAM_SIZE/4; index++) 
    {
      colors.color32 = *ptr++;
      if ((colors.wo.color0) == 0)
      {
          *pix++ = bg32;
      } 
      else
      {
          word color, color1;
          color = maria_GetColor(colors.by.color0);
          color1 = maria_GetColor(colors.by.color1);
          *pix++ = color | (color<<8) | (color1<<16) | (color1<<24);
      }
      if ((colors.wo.color1) == 0)
      {
          *pix++ = bg32;
      } 
      else
      {
          word color, color1;
          color = maria_GetColor(colors.by.color2);
          color1 = maria_GetColor(colors.by.color3);
          *pix++ = color | (color<<8) | (color1<<16) | (color1<<24);      
      }
    }
  }
  else if(rmode == 2) 
  {    
    for(index = 0; index < MARIA_LINERAM_SIZE/4; index++) 
    {
      colors.color32 = *ptr++;
      if (colors.color32 == 0)
      {
         *pix++ = bg32;   
         *pix++ = bg32;
      }
      else
      {
          *pix++ = maria_GetColor((colors.by.color0  & 16) | ((colors.by.color0  & 8) >> 3) | ((colors.by.color0  & 2))) |
                  (maria_GetColor((colors.by.color0  & 16) | ((colors.by.color0  & 4) >> 2) | ((colors.by.color0  & 1) << 1))<<8) |
                  (maria_GetColor((colors.by.color1 & 16) | ((colors.by.color1 & 8) >> 3) | ((colors.by.color1 & 2)))<<16) |
                  (maria_GetColor((colors.by.color1 & 16) | ((colors.by.color1 & 4) >> 2) | ((colors.by.color1 & 1) << 1))<<24);

          *pix++ = maria_GetColor((colors.by.color2 & 16) | ((colors.by.color2 & 8) >> 3) | ((colors.by.color2 & 2))) |
                  (maria_GetColor((colors.by.color2 & 16) | ((colors.by.color2 & 4) >> 2) | ((colors.by.color2 & 1) << 1))<<8) |
                  (maria_GetColor((colors.by.color3 & 16) | ((colors.by.color3 & 8) >> 3) | ((colors.by.color3 & 2)))<<16) |
                  (maria_GetColor((colors.by.color3 & 16) | ((colors.by.color3 & 4) >> 2) | ((colors.by.color3 & 1) << 1))<<24);
      }
    }
  }
  else if(rmode == 3) 
  {
    for(index = 0; index < MARIA_LINERAM_SIZE/4; index++) 
    {
      colors.color32 = *ptr++;

      if (colors.color32 == 0)
      {
         *pix++ = bg32;   
         *pix++ = bg32;
      }
      else
      {
          *pix++ = maria_GetColor((colors.by.color0 & 30)) |
                  (maria_GetColor((colors.by.color0 & 28) | ((colors.by.color0 & 1) << 1)) <<8)  |
                  (maria_GetColor((colors.by.color1 & 30))<<16) |
                  (maria_GetColor((colors.by.color1 & 28) | ((colors.by.color1 & 1) << 1)) <<24);
          *pix++ = maria_GetColor((colors.by.color2 & 30)) |
                  (maria_GetColor((colors.by.color2 & 28) | ((colors.by.color2 & 1) << 1)) <<8) |
                  (maria_GetColor((colors.by.color3 & 30))<<16) |
                  (maria_GetColor((colors.by.color3 & 28) | ((colors.by.color3 & 1) << 1)) <<24);
      }
    }
  }
}

// ----------------------------------------------------------------------------
// StoreLineRAM
// ----------------------------------------------------------------------------
static inline void maria_StoreLineRAM( ) 
{
  uint index;
  memset(maria_lineRAM, 0, MARIA_LINERAM_SIZE);
  
  byte mode = memory_ram[maria_dp.w + 1];
  while(mode & 0x5f) 
  {
    byte width;
    byte indirect = 0;
 
    maria_pp.b.l = memory_ram[maria_dp.w];
    maria_pp.b.h = memory_ram[maria_dp.w + 2];
    
    if(mode & 31) 
    { 
      maria_cycles += 8; // Maria cycles (Header 4 byte)
      maria_palette = (memory_ram[maria_dp.w + 1] & 224) >> 3;
      maria_horizontal = memory_ram[maria_dp.w + 3];
      width = memory_ram[maria_dp.w + 1] & 31;
      width = ((~width) & 31) + 1;
      maria_dp.w += 4;
    }
    else {
        
      maria_cycles += 12; // Maria cycles (Header 5 byte)
      maria_palette = (memory_ram[maria_dp.w + 3] & 224) >> 3;
      maria_horizontal = memory_ram[maria_dp.w + 4];
      indirect = memory_ram[maria_dp.w + 1] & 32;
      maria_wmode = memory_ram[maria_dp.w + 1] & 128;
      width = memory_ram[maria_dp.w + 3] & 31;
      width = (width == 0)? 32: ((~width) & 31) + 1;
      maria_dp.w += 5;
    }

    if(!indirect) {
      maria_pp.b.h += maria_offset;
      for(index = 0; index < width; index++) {
        maria_cycles += 3; // Maria cycles (Direct graphic read)
        maria_StoreGraphic( );
      }
    }
    else {
      byte cwidth = memory_ram[CTRL] & 16;
      pair basePP = maria_pp;
      for(index = 0; index < width; index++) {
        maria_cycles += 3; // Maria cycles (Indirect)
        maria_pp.b.l = memory_ram[basePP.w++];
        maria_pp.b.h = memory_ram[CHARBASE] + maria_offset;
        
        maria_cycles += 3; // Maria cycles (Indirect, 1 byte)
        maria_StoreGraphic( );
        if(cwidth) {
          maria_cycles += 3; // Maria cycles (Indirect, 2 bytes)
          maria_StoreGraphic( );
        }
      }
    }
    mode = memory_ram[maria_dp.w + 1];
  }
}

// ----------------------------------------------------------------------------
// Reset
// ----------------------------------------------------------------------------
void maria_Reset( ) {
  maria_surface = bufVideo;
  maria_scanline = 1;
  memset(maria_surface, 0, MARIA_SURFACE_SIZE);
  
   // These values need to be reset to allow switching between carts. 
   // This appears to be a bug in the ProSystem emulator.
   //
   maria_cycles = 0;
   maria_dpp.w = 0;
   maria_dp.w = 0;
   maria_pp.w = 0;
   maria_horizontal = 0;
   maria_palette = 0;
   maria_offset = 0;
   maria_h08 = 0;
   maria_h16 = 0;
   maria_wmode = 0;
}

// ----------------------------------------------------------------------------
// RenderScanline
// ----------------------------------------------------------------------------
word *framePtr = (word *)0;
ITCM_CODE uint maria_RenderScanline( ) 
{
  extern int gTotalAtariFrames;
  maria_cycles = 0;

  if(((memory_ram[CTRL] & 0x60) == 0x40))
  {
    maria_cycles += 5; // Maria cycles (DMA Startup)
    if(maria_scanline == maria_displayArea.top) 
    {
      maria_cycles += 10; // Maria cycles (End of VBLANK)
      maria_dpp.b.l = memory_ram[DPPL];
      maria_dpp.b.h = memory_ram[DPPH];
      maria_h08 = memory_ram[maria_dpp.w] & 32;
      maria_h16 = memory_ram[maria_dpp.w] & 64;
      maria_offset = memory_ram[maria_dpp.w] & 15;
      maria_dp.b.l = memory_ram[maria_dpp.w + 2];
      maria_dp.b.h = memory_ram[maria_dpp.w + 1];
      if(memory_ram[maria_dpp.w] & 128) 
      {
        maria_cycles += 20; // Maria cycles (NMI)  /*29, 16, 20*/
        sally_ExecuteNMI( );
      }
    }
    else
    {
      // This is where we render the video memory... 
        if (gTotalAtariFrames & 1)
        {
            maria_WriteLineRAM(framePtr);
            framePtr += 256;
        }
    }
    
    if(maria_scanline != maria_displayArea.bottom) 
    {
      maria_dp.b.l = memory_ram[maria_dpp.w + 2];
      maria_dp.b.h = memory_ram[maria_dpp.w + 1];
      maria_StoreLineRAM( );
      maria_offset--;
      if(maria_offset < 0) 
      {
        maria_cycles += 10; // Maria cycles (Last line of zone) ( /*20*/ 
        maria_dpp.w += 3;
        maria_h08 = memory_ram[maria_dpp.w] & 32;
        maria_h16 = memory_ram[maria_dpp.w] & 64;
        maria_offset = memory_ram[maria_dpp.w] & 15;
        if(memory_ram[maria_dpp.w] & 128) 
        {
          maria_cycles += 20; // Maria cycles (NMI) /*29, 16, 20*/
          //ALEK Fst6502_Cause_Interrupt(IRQ_NMI);
          sally_ExecuteNMI( );
        }
      }
      else
      {
         maria_cycles += 4; // Maria cycles (Other lines of zone)
      }
    }
  }
  return maria_cycles;
}

// ----------------------------------------------------------------------------
// Clear
// ----------------------------------------------------------------------------
void maria_Clear( ) 
{
    maria_surface = bufVideo;
    memset(maria_surface, 0x00, MARIA_SURFACE_SIZE);
}
