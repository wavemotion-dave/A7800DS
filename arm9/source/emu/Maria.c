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
#include "Database.h"

extern uint bRenderFrame;

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

#define MARIA_LINERAM_SIZE 160
static byte maria_lineRAM[256]  __attribute__((section(".dtcm")));

word* maria_surface             __attribute__((section(".dtcm"))) = 0;
uint  maria_scanline            __attribute__((section(".dtcm"))) = 1;
u8 write_mask_low               __attribute__((section(".dtcm")));
u8 write_mask_high              __attribute__((section(".dtcm")));
u8 bg8                          __attribute__((section(".dtcm")));

uint maria_cycles               __attribute__((section(".dtcm")));
static lpair maria_dpp          __attribute__((section(".dtcm")));
static lpair maria_dp           __attribute__((section(".dtcm")));
static lpair maria_pp           __attribute__((section(".dtcm")));
static byte maria_horizontal    __attribute__((section(".dtcm")));
static byte maria_palette       __attribute__((section(".dtcm")));
static int maria_offset         __attribute__((section(".dtcm"))); 
static byte maria_h08           __attribute__((section(".dtcm")));
static byte maria_h16           __attribute__((section(".dtcm")));
static uint maria_h8_h16        __attribute__((section(".dtcm")));
static u32 maria_wmode          __attribute__((section(".dtcm")));

word *framePtr                  __attribute__((section(".dtcm"))) = (word *)0;

u32 color_lookup_160AB[256][256];
byte color_lookup_320AC[256]    __attribute__((section(".dtcm")));
u32  maria_charbase             __attribute__((section(".dtcm")));
u16  banksets_mask              __attribute__((section(".dtcm"))) = 0x0000;

void mariabank_RenderScanlineTOP(void);
void mariabank_RenderScanline(void);

extern u32 bg32;

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
   maria_h8_h16 = 0x0000;
   maria_wmode = 0;
   bg32 = 0x00000000;
   bg8=0x00;
   maria_charbase = 0;
   banksets_mask = 0x0000;
   if ((myCartInfo.cardtype == CARTRIDGE_TYPE_BANKSETS) || (myCartInfo.cardtype == CARTRIDGE_TYPE_BANKSETS_RAM)) banksets_mask = 0x8000;
   if ((myCartInfo.cardtype == CARTRIDGE_TYPE_BANKSETS_HALTRAM)) banksets_mask = 0xC000;
    
   // ----------------------------------------------------------------------------------
   // Build the 160 A/B color lookup table for a few frames of increased performance
   // ----------------------------------------------------------------------------------
   for (uint color=0; color<256; color++)
   {
       for (uint color1=0; color1<256; color1++)
       {
          color_lookup_160AB[color][color1] = color | (color<<8) | (color1<<16) | (color1<<24);
       }
       
       color_lookup_320AC[color] = (color & 28) | ((color & 1) << 1);
   }
}


// ----------------------------------------------------------------------------
// Clear
// ----------------------------------------------------------------------------
void maria_Clear( ) 
{
    maria_surface = bufVideo;
    memset(maria_surface, 0x00, MARIA_SURFACE_SIZE);
}

// ----------------------------------------------------------------------------
// ClearCells - 4 bytes at a time
// ----------------------------------------------------------------------------
static inline void _maria_ClearCells4(void)
{
  if ((maria_horizontal) < MARIA_LINERAM_SIZE)
  {
      if (memory_ram[CTRL] & 4) 
      {
          *((u32 *)&maria_lineRAM[maria_horizontal]) = 0;
      }
  }
  maria_horizontal += 4;
}

// ----------------------------------------------------------------------------
// StoreCells - 4 bytes at a time
// ----------------------------------------------------------------------------
static inline void _maria_StoreCells4(byte data) 
{
  if((maria_horizontal) < MARIA_LINERAM_SIZE) 
  {
    byte *ptr = &(maria_lineRAM[maria_horizontal]);
#ifdef KANGAROO_MODE_SUPPORTED
    if (memory_ram[CTRL] & 4)
    {
        if (data & 0xC0) *ptr++ = maria_palette | ((data       ) >> 6); else *ptr++ = 0;
        if (data & 0x30) *ptr++ = maria_palette | ((data & 0x30) >> 4); else *ptr++ = 0;
        if (data & 0x0C) *ptr++ = maria_palette | ((data & 0x0C) >> 2); else *ptr++ = 0;
        if (data & 0x03) *ptr   = maria_palette | (data & 0x03);        else *ptr++ = 0;
    }
    else
#endif        
    {
        if (data & 0xC0) *ptr++ = maria_palette | ((data       ) >> 6); else ptr++;
        if (data & 0x30) *ptr++ = maria_palette | ((data & 0x30) >> 4); else ptr++;
        if (data & 0x0C) *ptr++ = maria_palette | ((data & 0x0C) >> 2); else ptr++;
        if (data & 0x03) *ptr   = maria_palette | (data & 0x03); 
    }
  }
  maria_horizontal += 4;
}

// ----------------------------------------------------------------------------
// StoreCell - write mode
// ----------------------------------------------------------------------------
static inline void maria_StoreCellWriteMode(byte data) 
{
  if(maria_horizontal < MARIA_LINERAM_SIZE) 
  {
      if (data)
      {
          byte *ptr = (byte *)&maria_lineRAM[maria_horizontal];
          if (data & write_mask_high)  // high
          {
              *ptr = (maria_palette & 0x10) | (data >> 4);
          }
          if (data & write_mask_low)  // low
          {
            ptr++;
            *ptr = (maria_palette & 0x10) | (data & 0x0F);
          }
      }
#ifdef KANGAROO_MODE_SUPPORTED
      else
      {
          if ((memory_ram[CTRL] & 4))
          {
              u16 *ptr = (u16 *)&maria_lineRAM[maria_horizontal];
              *ptr++ = 0x0000;
          }  
      }
#endif      
  }
  maria_horizontal += 2;
}


// ----------------------------------------------------------------------------
// IsHoleyDMA
// ----------------------------------------------------------------------------
static inline bool maria_IsHoleyDMA( ) 
{
    if(maria_pp.w & 0x8000)
    {
        if (maria_pp.w & maria_h8_h16) return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
// GetColor
// ----------------------------------------------------------------------------
static inline byte maria_GetColor(byte data) 
{
  return (data & 3) ? memory_ram[BACKGRND | data] : bg8;
}


static u8 write_mode_lookup[256] __attribute__((section(".dtcm"))) =
{
    0x00, 0x04, 0x08, 0x0C, 0x40, 0x44, 0x48, 0x4C, 0x80, 0x84, 0x88, 0x8C, 0xC0, 0xC4, 0xC8, 0xCC, 
    0x01, 0x05, 0x09, 0x0D, 0x41, 0x45, 0x49, 0x4D, 0x81, 0x85, 0x89, 0x8D, 0xC1, 0xC5, 0xC9, 0xCD, 
    0x02, 0x06, 0x0A, 0x0E, 0x42, 0x46, 0x4A, 0x4E, 0x82, 0x86, 0x8A, 0x8E, 0xC2, 0xC6, 0xCA, 0xCE, 
    0x03, 0x07, 0x0B, 0x0F, 0x43, 0x47, 0x4B, 0x4F, 0x83, 0x87, 0x8B, 0x8F, 0xC3, 0xC7, 0xCB, 0xCF, 
    0x10, 0x14, 0x18, 0x1C, 0x50, 0x54, 0x58, 0x5C, 0x90, 0x94, 0x98, 0x9C, 0xD0, 0xD4, 0xD8, 0xDC, 
    0x11, 0x15, 0x19, 0x1D, 0x51, 0x55, 0x59, 0x5D, 0x91, 0x95, 0x99, 0x9D, 0xD1, 0xD5, 0xD9, 0xDD, 
    0x12, 0x16, 0x1A, 0x1E, 0x52, 0x56, 0x5A, 0x5E, 0x92, 0x96, 0x9A, 0x9E, 0xD2, 0xD6, 0xDA, 0xDE, 
    0x13, 0x17, 0x1B, 0x1F, 0x53, 0x57, 0x5B, 0x5F, 0x93, 0x97, 0x9B, 0x9F, 0xD3, 0xD7, 0xDB, 0xDF, 
    0x20, 0x24, 0x28, 0x2C, 0x60, 0x64, 0x68, 0x6C, 0xA0, 0xA4, 0xA8, 0xAC, 0xE0, 0xE4, 0xE8, 0xEC, 
    0x21, 0x25, 0x29, 0x2D, 0x61, 0x65, 0x69, 0x6D, 0xA1, 0xA5, 0xA9, 0xAD, 0xE1, 0xE5, 0xE9, 0xED, 
    0x22, 0x26, 0x2A, 0x2E, 0x62, 0x66, 0x6A, 0x6E, 0xA2, 0xA6, 0xAA, 0xAE, 0xE2, 0xE6, 0xEA, 0xEE, 
    0x23, 0x27, 0x2B, 0x2F, 0x63, 0x67, 0x6B, 0x6F, 0xA3, 0xA7, 0xAB, 0xAF, 0xE3, 0xE7, 0xEB, 0xEF, 
    0x30, 0x34, 0x38, 0x3C, 0x70, 0x74, 0x78, 0x7C, 0xB0, 0xB4, 0xB8, 0xBC, 0xF0, 0xF4, 0xF8, 0xFC, 
    0x31, 0x35, 0x39, 0x3D, 0x71, 0x75, 0x79, 0x7D, 0xB1, 0xB5, 0xB9, 0xBD, 0xF1, 0xF5, 0xF9, 0xFD, 
    0x32, 0x36, 0x3A, 0x3E, 0x72, 0x76, 0x7A, 0x7E, 0xB2, 0xB6, 0xBA, 0xBE, 0xF2, 0xF6, 0xFA, 0xFE, 
    0x33, 0x37, 0x3B, 0x3F, 0x73, 0x77, 0x7B, 0x7F, 0xB3, 0xB7, 0xBB, 0xBF, 0xF3, 0xF7, 0xFB, 0xFF
};


static u8 write_mode_lookup_mode2A[] __attribute__((section(".dtcm"))) =
{
    0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x02, 0x02, 0x01, 0x01, 0x03, 0x03, 0x01, 0x01, 0x03, 0x03, 
    0x10, 0x10, 0x12, 0x12, 0x10, 0x10, 0x12, 0x12, 0x11, 0x11, 0x13, 0x13, 0x11, 0x11, 0x13, 0x13, 
    0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x02, 0x02, 0x01, 0x01, 0x03, 0x03, 0x01, 0x01, 0x03, 0x03, 
    0x10, 0x10, 0x12, 0x12, 0x10, 0x10, 0x12, 0x12, 0x11, 0x11, 0x13, 0x13, 0x11, 0x11, 0x13, 0x13, 
    0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x02, 0x02, 0x01, 0x01, 0x03, 0x03, 0x01, 0x01, 0x03, 0x03, 
    0x10, 0x10, 0x12, 0x12, 0x10, 0x10, 0x12, 0x12, 0x11, 0x11, 0x13, 0x13, 0x11, 0x11, 0x13, 0x13, 
    0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x02, 0x02, 0x01, 0x01, 0x03, 0x03, 0x01, 0x01, 0x03, 0x03, 
    0x10, 0x10, 0x12, 0x12, 0x10, 0x10, 0x12, 0x12, 0x11, 0x11, 0x13, 0x13, 0x11, 0x11, 0x13, 0x13, 
    0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x02, 0x02, 0x01, 0x01, 0x03, 0x03, 0x01, 0x01, 0x03, 0x03, 
    0x10, 0x10, 0x12, 0x12, 0x10, 0x10, 0x12, 0x12, 0x11, 0x11, 0x13, 0x13, 0x11, 0x11, 0x13, 0x13, 
    0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x02, 0x02, 0x01, 0x01, 0x03, 0x03, 0x01, 0x01, 0x03, 0x03, 
    0x10, 0x10, 0x12, 0x12, 0x10, 0x10, 0x12, 0x12, 0x11, 0x11, 0x13, 0x13, 0x11, 0x11, 0x13, 0x13, 
    0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x02, 0x02, 0x01, 0x01, 0x03, 0x03, 0x01, 0x01, 0x03, 0x03, 
    0x10, 0x10, 0x12, 0x12, 0x10, 0x10, 0x12, 0x12, 0x11, 0x11, 0x13, 0x13, 0x11, 0x11, 0x13, 0x13, 
    0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x02, 0x02, 0x01, 0x01, 0x03, 0x03, 0x01, 0x01, 0x03, 0x03, 
    0x10, 0x10, 0x12, 0x12, 0x10, 0x10, 0x12, 0x12, 0x11, 0x11, 0x13, 0x13, 0x11, 0x11, 0x13, 0x13
};


static u8 write_mode_lookup_mode2B[] __attribute__((section(".dtcm"))) =
{
    0x00, 0x02, 0x00, 0x02, 0x01, 0x03, 0x01, 0x03, 0x00, 0x02, 0x00, 0x02, 0x01, 0x03, 0x01, 0x03, 
    0x10, 0x12, 0x10, 0x12, 0x11, 0x13, 0x11, 0x13, 0x10, 0x12, 0x10, 0x12, 0x11, 0x13, 0x11, 0x13, 
    0x00, 0x02, 0x00, 0x02, 0x01, 0x03, 0x01, 0x03, 0x00, 0x02, 0x00, 0x02, 0x01, 0x03, 0x01, 0x03, 
    0x10, 0x12, 0x10, 0x12, 0x11, 0x13, 0x11, 0x13, 0x10, 0x12, 0x10, 0x12, 0x11, 0x13, 0x11, 0x13, 
    0x00, 0x02, 0x00, 0x02, 0x01, 0x03, 0x01, 0x03, 0x00, 0x02, 0x00, 0x02, 0x01, 0x03, 0x01, 0x03, 
    0x10, 0x12, 0x10, 0x12, 0x11, 0x13, 0x11, 0x13, 0x10, 0x12, 0x10, 0x12, 0x11, 0x13, 0x11, 0x13, 
    0x00, 0x02, 0x00, 0x02, 0x01, 0x03, 0x01, 0x03, 0x00, 0x02, 0x00, 0x02, 0x01, 0x03, 0x01, 0x03, 
    0x10, 0x12, 0x10, 0x12, 0x11, 0x13, 0x11, 0x13, 0x10, 0x12, 0x10, 0x12, 0x11, 0x13, 0x11, 0x13, 
    0x00, 0x02, 0x00, 0x02, 0x01, 0x03, 0x01, 0x03, 0x00, 0x02, 0x00, 0x02, 0x01, 0x03, 0x01, 0x03, 
    0x10, 0x12, 0x10, 0x12, 0x11, 0x13, 0x11, 0x13, 0x10, 0x12, 0x10, 0x12, 0x11, 0x13, 0x11, 0x13, 
    0x00, 0x02, 0x00, 0x02, 0x01, 0x03, 0x01, 0x03, 0x00, 0x02, 0x00, 0x02, 0x01, 0x03, 0x01, 0x03, 
    0x10, 0x12, 0x10, 0x12, 0x11, 0x13, 0x11, 0x13, 0x10, 0x12, 0x10, 0x12, 0x11, 0x13, 0x11, 0x13, 
    0x00, 0x02, 0x00, 0x02, 0x01, 0x03, 0x01, 0x03, 0x00, 0x02, 0x00, 0x02, 0x01, 0x03, 0x01, 0x03, 
    0x10, 0x12, 0x10, 0x12, 0x11, 0x13, 0x11, 0x13, 0x10, 0x12, 0x10, 0x12, 0x11, 0x13, 0x11, 0x13, 
    0x00, 0x02, 0x00, 0x02, 0x01, 0x03, 0x01, 0x03, 0x00, 0x02, 0x00, 0x02, 0x01, 0x03, 0x01, 0x03, 
    0x10, 0x12, 0x10, 0x12, 0x11, 0x13, 0x11, 0x13, 0x10, 0x12, 0x10, 0x12, 0x11, 0x13, 0x11, 0x13
};


// ----------------------------------------------------------------------------
// WriteLineRAM
// ----------------------------------------------------------------------------
static inline void maria_WriteLineRAM(word* buffer) 
{
  union ColorUnion colors; 
  unsigned int *pix=(unsigned int *) buffer;
  uint32 *ptr = (uint32 *)&maria_lineRAM[0];
  byte rmode = memory_ram[CTRL] & 3;
    
  if(rmode == 0) // 160A/B
  {
    for(uint index = 0; index < MARIA_LINERAM_SIZE/4; index++) 
    {
      colors.color32 = *ptr++;
        
      if (colors.color32 == 0)
      {
          *pix++ = bg32;
          *pix++ = bg32;
      }
      else
      {
          if ((colors.wo.color0 == 0))
          {
              *pix++ = bg32;
          }
          else
          {
              *pix++ = color_lookup_160AB[maria_GetColor(colors.by.color0)][maria_GetColor(colors.by.color1)];
          }
          
          if ((colors.wo.color1 == 0))
          {
              *pix++ = bg32;
          }
          else
          {
              *pix++ = color_lookup_160AB[maria_GetColor(colors.by.color2)][maria_GetColor(colors.by.color3)];
          }
      }
    }
  }
  else if(rmode == 2)  // 320B/D
  {    
    for(uint index = 0; index < MARIA_LINERAM_SIZE/4; index++) 
    {
      colors.color32 = *ptr++;
      if (colors.color32 == 0)
      {
         *pix++ = bg32;   
         *pix++ = bg32;
      }
      else
      {
          if ((colors.wo.color0 == 0))
          {
              *pix++ = bg32;
          }
          else
          {
              *pix++ = maria_GetColor(write_mode_lookup_mode2A[colors.by.color0])       |
                       maria_GetColor(write_mode_lookup_mode2B[colors.by.color0]) << 8  |
                       maria_GetColor(write_mode_lookup_mode2A[colors.by.color1]) << 16 |
                       maria_GetColor(write_mode_lookup_mode2B[colors.by.color1]) << 24;
          }
              
          if ((colors.wo.color1 == 0))
          {
              *pix++ = bg32;
          }
          else
          {
              *pix++ = maria_GetColor(write_mode_lookup_mode2A[colors.by.color2])       |
                       maria_GetColor(write_mode_lookup_mode2B[colors.by.color2]) << 8  |
                       maria_GetColor(write_mode_lookup_mode2A[colors.by.color3]) << 16 |
                       maria_GetColor(write_mode_lookup_mode2B[colors.by.color3]) << 24;
          }
      }
    }
  }
  else if(rmode == 3) // 320A/C
  {
    for(uint index = 0; index < MARIA_LINERAM_SIZE/4; index++) 
    {
      colors.color32 = *ptr++;

      if (colors.color32 == 0)
      {
         *pix++ = bg32;   
         *pix++ = bg32;
      }
      else
      {
          *pix++ = maria_GetColor((colors.by.color0 & 30)) | (maria_GetColor(color_lookup_320AC[colors.by.color0]) <<8)  | (maria_GetColor((colors.by.color1 & 30))<<16) | (maria_GetColor(color_lookup_320AC[colors.by.color1]) <<24);
          *pix++ = maria_GetColor((colors.by.color2 & 30)) | (maria_GetColor(color_lookup_320AC[colors.by.color2]) <<8)  | (maria_GetColor((colors.by.color3 & 30))<<16) | (maria_GetColor(color_lookup_320AC[colors.by.color3]) <<24);
      }
    }
  }
}


// ==============================================================================
// Functions below this point read into the 7800 Cart Address Space... and must 
// be handled differently for the new 'banksets' scheme - bankset versions of
// these functions can be found further down the source code...
// ==============================================================================

// ----------------------------------------------------------------------------
// StoreGraphic
// ----------------------------------------------------------------------------
static inline void maria_StoreGraphic( ) 
{
  byte data = memory_ram[maria_pp.w];
  if(maria_wmode) 
  {
    if(maria_IsHoleyDMA())
    {
      maria_horizontal += 2;
    }
    else
    {
      maria_StoreCellWriteMode(write_mode_lookup[data]);
    }
  }
  else 
  {
#ifdef KANGAROO_MODE_SUPPORTED      
    if(maria_IsHoleyDMA()) 
    {
        maria_horizontal += 4;
    }
    else if (!data)
    {
       _maria_ClearCells4();   
    }
#else
    if(maria_IsHoleyDMA() || !data) 
    {
        maria_horizontal += 4;
    }
#endif      
    else         
    {
      _maria_StoreCells4(data);
    }
  }
  maria_pp.w++;
}


// ----------------------------------------------------------------------------
// StoreLineRAM
// ----------------------------------------------------------------------------
ITCM_CODE static void maria_StoreLineRAM( ) 
{
  uint index;

  if (bRenderFrame)  // Skip every other frame...
  {
    u32 *ptr=(u32*)maria_lineRAM;
    *ptr++ = 0;*ptr++ = 0;*ptr++ = 0;*ptr++ = 0;
    *ptr++ = 0;*ptr++ = 0;*ptr++ = 0;*ptr++ = 0;
    *ptr++ = 0;*ptr++ = 0;*ptr++ = 0;*ptr++ = 0;
    *ptr++ = 0;*ptr++ = 0;*ptr++ = 0;*ptr++ = 0;
    *ptr++ = 0;*ptr++ = 0;*ptr++ = 0;*ptr++ = 0;
    *ptr++ = 0;*ptr++ = 0;*ptr++ = 0;*ptr++ = 0;
    *ptr++ = 0;*ptr++ = 0;*ptr++ = 0;*ptr++ = 0;
    *ptr++ = 0;*ptr++ = 0;*ptr++ = 0;*ptr++ = 0;
    *ptr++ = 0;*ptr++ = 0;*ptr++ = 0;*ptr++ = 0;
    *ptr++ = 0;*ptr++ = 0;*ptr++ = 0;*ptr++ = 0; *ptr = 0;
      
    write_mask_low = ((memory_ram[CTRL] & 3)) ? 0x0F : 0x03;
    write_mask_high = ((memory_ram[CTRL] & 3)) ? 0xF0 : 0x30;      
  }    

  maria_pp.b.l = memory_ram[maria_dp.w++];
  uint mode = memory_ram[maria_dp.w++];
  maria_pp.b.h = memory_ram[maria_dp.w++];
  while(mode & 0x5f) 
  {
    uint width;
    uint indirect = 0;
 
    if(mode & 31) 
    { 
      maria_cycles += 8; // Maria cycles (Header 4 byte)
      maria_palette = (mode & 0xE0) >> 3;
      maria_horizontal = memory_ram[maria_dp.w++];
      width = (~mode & 31) + 1;
    }
    else 
    {
      maria_cycles += 10; // Maria cycles (Header 5 byte)
      maria_palette = (memory_ram[maria_dp.w] & 0xE0) >> 3;
      width = memory_ram[maria_dp.w++] & 31;
      width = (width == 0) ? 32: ((~width) & 31) + 1;
      maria_horizontal = memory_ram[maria_dp.w++];
      indirect = mode & 32;
      maria_wmode = mode & 128;
    }

    if(!indirect) 
    {
      if (bRenderFrame)
      {
          maria_pp.b.h += maria_offset;
          for(index = 0; index < width; index++) 
          {
             maria_StoreGraphic();
          }
          maria_pp.w &= 0xFFFF;   // Pole Position II and Failsafe both require that we wrap this...      
      }
      maria_cycles += (3*width); // Maria cycles (Direct graphic read)
    }
    else 
    {
      uint cwidth = (memory_ram[CTRL] & 16);
      if (bRenderFrame)
      {
          lpair basePP = maria_pp;
          for(index = 0; index < width; index++) 
          {
            maria_pp.w = ((maria_charbase + maria_offset) << 8) | memory_ram[basePP.w++];
            maria_StoreGraphic( );              //maria_cycles += 6; // Maria cycles (Indirect, 1 byte)
            if(cwidth) maria_StoreGraphic( );   //maria_cycles += 3; // Maria cycles (Indirect, 2 bytes)
          }
          maria_pp.w &= 0xFFFF;   // Pole Position II and Failsafe both require that we wrap this...      
      }
      maria_cycles += ((cwidth ? 9:6)*width);             // Maria cycles for Indirect 1 byte (6 cycles) or 2 bytes (9 cycles)
    }
      
    maria_dp.w &= 0xFFFF;   // Super Pac-Man requires that we wrap this...
    
    maria_pp.b.l = memory_ram[maria_dp.w++];
    mode = memory_ram[maria_dp.w++];
    maria_pp.b.h = memory_ram[maria_dp.w++];      
  }
}

// ----------------------------------------------------------------------------
// RenderScanline
// ----------------------------------------------------------------------------
ITCM_CODE void maria_RenderScanlineTOP(void) 
{
  maria_cycles = 0;
    
  //
  // Displays the background color when Maria is disabled (if applicable)
  //
  if ((memory_ram[CTRL] & 96) != 64)
  {
      u32 *bgstart = (u32*)framePtr;
      for(uint index = 0; index < MARIA_LINERAM_SIZE/4; index++ ) 
      {
        *bgstart++ = bg32;
      }
      framePtr += 256;
  }
  else
  {
      if (banksets_mask) return mariabank_RenderScanlineTOP();
      
      maria_dpp.b.l = memory_ram[DPPL];
      maria_dpp.b.h = memory_ram[DPPH];
      maria_h08 = memory_ram[maria_dpp.w] & 32;
      maria_h16 = memory_ram[maria_dpp.w] & 64;
      maria_h8_h16 = (maria_h08 ? 2048:0) | (maria_h16 ? 4096:0);
      maria_offset = memory_ram[maria_dpp.w] & 15;
      maria_dp.b.h = memory_ram[maria_dpp.w + 1];
      maria_dp.b.l = memory_ram[maria_dpp.w + 2];
      if(memory_ram[maria_dpp.w] & 128) 
      {
        sally_ExecuteNMI( );
        maria_dp.b.h = memory_ram[maria_dpp.w + 1];
        maria_dp.b.l = memory_ram[maria_dpp.w + 2];
      }
    
      maria_StoreLineRAM( );
      
      if(--maria_offset < 0) 
      {
        maria_cycles += 15; // Maria cycles (Startup+Shutdown Last line of zone) - Techncially 24 but we know we're over estimating so....
        maria_dpp.w += 3;
        maria_h08 = memory_ram[maria_dpp.w] & 32;
        maria_h16 = memory_ram[maria_dpp.w] & 64;
        maria_h8_h16 = (maria_h08 ? 2048:0) | (maria_h16 ? 4096:0);
        maria_offset = memory_ram[maria_dpp.w] & 15;
        if(memory_ram[maria_dpp.w] & 128) 
        {
          sally_ExecuteNMI( );
        }
      } else maria_cycles += 10; // Maria cycles (Startup+Shutdown Other lines of zone) - Technically 16 but we know we're over estimating so....
  }
}

ITCM_CODE void maria_RenderScanline(void) 
{
  maria_cycles = 0;
    
  //
  // Displays the background color when Maria is disabled (if applicable)
  //
  if ((memory_ram[CTRL] & 96) != 64)
  {
      u32 *bgstart = (u32*)framePtr;
      for(uint index = 0; index < MARIA_LINERAM_SIZE/4; index++ ) 
      {
        *bgstart++ = bg32;
      }
  }
  else
  {
    if (banksets_mask) return mariabank_RenderScanline();

    // This is where we render the video memory... 
    if (bRenderFrame)  // Skip every other frame...
    {
        maria_WriteLineRAM(framePtr);
        framePtr += 256;
    }
    
    maria_dp.b.h = memory_ram[maria_dpp.w + 1];
    maria_dp.b.l = memory_ram[maria_dpp.w + 2];
    
    maria_StoreLineRAM( );
      
    if(--maria_offset < 0) 
    {
      maria_cycles += 15; // Maria cycles (Startup+Shutdown Last line of zone) - Techncially 24 but we know we're over estimating so....
      maria_dpp.w += 3;
      maria_h08 = memory_ram[maria_dpp.w] & 32;
      maria_h16 = memory_ram[maria_dpp.w] & 64;
      maria_h8_h16 = (maria_h08 ? 2048:0) | (maria_h16 ? 4096:0);
      maria_offset = memory_ram[maria_dpp.w] & 15;
      if(memory_ram[maria_dpp.w] & 128) 
      {
        sally_ExecuteNMI( );
      }
    }
    else maria_cycles += 10; // Maria cycles (Startup+Shutdown Other lines of zone) - Techncially 16 but we know we're over estimating so....
  }
}



// =======================================================================================
// BANKSET Handling
// We subsitutue out the maria_XXXX() calls with mariabank_XXXX() calls that will fetch
// non-system memory from the Maria portion of the banksets memory.
// =======================================================================================

extern byte banksets_memory[];
inline byte bankset_memory_read(word address)
{
    // -----------------------------------------------------------------------
    // banksets_mask will be either 0x8000 (if we are only swapping in ROM) 
    // or 0xC000 (if we are swapping in ROM and RAM). The lower memory is
    // always system memory and we get that from memory_ram[]
    // -----------------------------------------------------------------------
    if (address & banksets_mask) return banksets_memory[address];
    else return memory_ram[address];
}

// ----------------------------------------------------------------------------
// StoreGraphic
// ----------------------------------------------------------------------------
static inline void mariabank_StoreGraphic( ) 
{
  byte data = bankset_memory_read(maria_pp.w);
  if(maria_wmode) 
  {
    if(maria_IsHoleyDMA())
    {
      maria_horizontal += 2;
    }
    else
    {
      maria_StoreCellWriteMode(write_mode_lookup[data]);
    }
  }
  else 
  {
#ifdef KANGAROO_MODE_SUPPORTED      
    if(maria_IsHoleyDMA()) 
    {
        maria_horizontal += 4;
    }
    else if (!data)
    {
       _maria_ClearCells4();   
    }
#else
    if(maria_IsHoleyDMA() || !data) 
    {
        maria_horizontal += 4;
    }
#endif      
    else         
    {
      _maria_StoreCells4(data);
    }
  }
  maria_pp.w++;
}


// ----------------------------------------------------------------------------
// StoreLineRAM
// ----------------------------------------------------------------------------
ITCM_CODE static void mariabank_StoreLineRAM( ) 
{
  uint index;

  if (bRenderFrame)  // Skip every other frame...
  {
    u32 *ptr=(u32*)maria_lineRAM;
    *ptr++ = 0;*ptr++ = 0;*ptr++ = 0;*ptr++ = 0;
    *ptr++ = 0;*ptr++ = 0;*ptr++ = 0;*ptr++ = 0;
    *ptr++ = 0;*ptr++ = 0;*ptr++ = 0;*ptr++ = 0;
    *ptr++ = 0;*ptr++ = 0;*ptr++ = 0;*ptr++ = 0;
    *ptr++ = 0;*ptr++ = 0;*ptr++ = 0;*ptr++ = 0;
    *ptr++ = 0;*ptr++ = 0;*ptr++ = 0;*ptr++ = 0;
    *ptr++ = 0;*ptr++ = 0;*ptr++ = 0;*ptr++ = 0;
    *ptr++ = 0;*ptr++ = 0;*ptr++ = 0;*ptr++ = 0;
    *ptr++ = 0;*ptr++ = 0;*ptr++ = 0;*ptr++ = 0;
    *ptr++ = 0;*ptr++ = 0;*ptr++ = 0;*ptr++ = 0; *ptr = 0;
      
    write_mask_low = ((memory_ram[CTRL] & 3)) ? 0x0F : 0x03;
    write_mask_high = ((memory_ram[CTRL] & 3)) ? 0xF0 : 0x30;      
  }    

  maria_pp.b.l = bankset_memory_read(maria_dp.w++);
  uint mode = bankset_memory_read(maria_dp.w++);
  maria_pp.b.h = bankset_memory_read(maria_dp.w++);
  while(mode & 0x5f) 
  {
    uint width;
    uint indirect = 0;
 
    if(mode & 31) 
    { 
      maria_cycles += 8; // Maria cycles (Header 4 byte)
      maria_palette = (mode & 0xE0) >> 3;
      maria_horizontal = bankset_memory_read(maria_dp.w++);
      width = (~mode & 31) + 1;
    }
    else 
    {
      maria_cycles += 10; // Maria cycles (Header 5 byte)
      maria_palette = (bankset_memory_read(maria_dp.w) & 0xE0) >> 3;
      width = bankset_memory_read(maria_dp.w++) & 31;
      width = (width == 0) ? 32: ((~width) & 31) + 1;
      maria_horizontal = bankset_memory_read(maria_dp.w++);
      indirect = mode & 32;
      maria_wmode = mode & 128;
    }

    if(!indirect) 
    {
      if (bRenderFrame)
      {
          maria_pp.b.h += maria_offset;
          for(index = 0; index < width; index++) 
          {
             mariabank_StoreGraphic();
          }
          maria_pp.w &= 0xFFFF;   // Pole Position II and Failsafe both require that we wrap this...      
      }
      maria_cycles += (3*width); // Maria cycles (Direct graphic read)
    }
    else 
    {
      uint cwidth = (memory_ram[CTRL] & 16);
      if (bRenderFrame)
      {
          lpair basePP = maria_pp;
          for(index = 0; index < width; index++) 
          {
            maria_pp.w = ((maria_charbase + maria_offset) << 8) | bankset_memory_read(basePP.w++);
            mariabank_StoreGraphic( );              //maria_cycles += 6; // Maria cycles (Indirect, 1 byte)
            if(cwidth) mariabank_StoreGraphic( );   //maria_cycles += 3; // Maria cycles (Indirect, 2 bytes)
          }
          maria_pp.w &= 0xFFFF;   // Pole Position II and Failsafe both require that we wrap this...      
      }
      maria_cycles += ((cwidth ? 9:6)*width);             // Maria cycles for Indirect 1 byte (6 cycles) or 2 bytes (9 cycles)
    }
      
    maria_dp.w &= 0xFFFF;   // Super Pac-Man requires that we wrap this...
    
    maria_pp.b.l = bankset_memory_read(maria_dp.w++);
    mode = bankset_memory_read(maria_dp.w++);
    maria_pp.b.h = bankset_memory_read(maria_dp.w++);
  }
}


ITCM_CODE void mariabank_RenderScanlineTOP(void) 
{
    maria_dpp.b.l = bankset_memory_read(DPPL);
    maria_dpp.b.h = bankset_memory_read(DPPH);
    maria_h08 = bankset_memory_read(maria_dpp.w) & 32;
    maria_h16 = bankset_memory_read(maria_dpp.w) & 64;
    maria_h8_h16 = (maria_h08 ? 2048:0) | (maria_h16 ? 4096:0);
    maria_offset = bankset_memory_read(maria_dpp.w) & 15;
    maria_dp.b.h = bankset_memory_read(maria_dpp.w + 1);
    maria_dp.b.l = bankset_memory_read(maria_dpp.w + 2);
    if(bankset_memory_read(maria_dpp.w) & 128) 
    {
        sally_ExecuteNMI( );
        maria_dp.b.h = bankset_memory_read(maria_dpp.w + 1);
        maria_dp.b.l = bankset_memory_read(maria_dpp.w + 2);
    }

    mariabank_StoreLineRAM( );
    
    if(--maria_offset < 0) 
    {
        maria_cycles += 15; // Maria cycles (Startup+Shutdown Last line of zone) - Techncially 24 but we know we're over estimating so....
        maria_dpp.w += 3;
        maria_h08 = bankset_memory_read(maria_dpp.w) & 32;
        maria_h16 = bankset_memory_read(maria_dpp.w) & 64;
        maria_h8_h16 = (maria_h08 ? 2048:0) | (maria_h16 ? 4096:0);
        maria_offset = bankset_memory_read(maria_dpp.w) & 15;
        if(bankset_memory_read(maria_dpp.w) & 128) 
        {
          sally_ExecuteNMI( );
        }
    }
    else maria_cycles += 10; // Maria cycles (Startup+Shutdown Other lines of zone) - Techncially 16 but we know we're over estimating so....
}

ITCM_CODE void mariabank_RenderScanline(void) 
{
    // This is where we render the video memory... 
    if (bRenderFrame)  // Skip every other frame...
    {
        maria_WriteLineRAM(framePtr);
        framePtr += 256;
    }
    
    maria_dp.b.h = bankset_memory_read(maria_dpp.w + 1);
    maria_dp.b.l = bankset_memory_read(maria_dpp.w + 2);
    
    mariabank_StoreLineRAM( );
    
    if(--maria_offset < 0) 
    {
      maria_cycles += 15; // Maria cycles (Startup+Shutdown Last line of zone) - Techncially 24 but we know we're over estimating so....
      maria_dpp.w += 3;
      maria_h08 = bankset_memory_read(maria_dpp.w) & 32;
      maria_h16 = bankset_memory_read(maria_dpp.w) & 64;
      maria_h8_h16 = (maria_h08 ? 2048:0) | (maria_h16 ? 4096:0);
      maria_offset = bankset_memory_read(maria_dpp.w) & 15;
      if(bankset_memory_read(maria_dpp.w) & 128) 
      {
        sally_ExecuteNMI( );
      }
    }
    else maria_cycles += 10; // Maria cycles (Startup+Shutdown Other lines of zone) - Techncially 16 but we know we're over estimating so....
}

