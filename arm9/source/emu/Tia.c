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
// TiaSound is Copyright(c) 1997 by Ron Fries
//                                                                           
// This library is free software; you can redistribute it and/or modify it   
// under the terms of version 2 of the GNU Library General Public License    
// as published by the Free Software Foundation.                             
//                                                                           
// This library is distributed in the hope that it will be useful, but       
// WITHOUT ANY WARRANTY; without even the implied warranty of                
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library 
// General Public License for more details.                                  
// To obtain a copy of the GNU Library General Public License, write to the  
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.   
//                                                                           
// Any permitted reproduction of these routines, in whole or in part, must
// bear this legend.                                                         
// ----------------------------------------------------------------------------
// Tia.cpp
// ----------------------------------------------------------------------------
#include "Tia.h"

#define TIA_POLY4_SIZE 15
#define TIA_POLY5_SIZE 31
#define TIA_POLY9_SIZE 511

u32 tiaBufIdx                   __attribute__((section(".dtcm"))) = 0;
u16 tia_buffer[SNDLENGTH]       __attribute__((section(".dtcm"))) = {0};
byte tia_volume[2]              __attribute__((section(".dtcm"))) = {0};
uint tia_counter[2]             __attribute__((section(".dtcm"))) = {0};
uint tia_counterMax[2]          __attribute__((section(".dtcm"))) = {0};
byte TIA_POLY4[ ]               __attribute__((section(".dtcm"))) = {1,1,0,1,1,1,0,0,0,0,1,0,1,0,0};
byte TIA_POLY5[ ]               __attribute__((section(".dtcm"))) = {0,0,1,0,1,1,0,0,1,1,1,1,1,0,0,0,1,1,0,1,1,1,0,1,0,1,0,0,0,0,1};
static byte TIA_POLY9[ ]        __attribute__((section(".dtcm"))) = {0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,1,1,1,0,0,1,0,1,0,0,1,1,1,1,1,0,0,1,1,0,1,1,0,1,0,1,1,1,0,1,1,0,0,1,0,0,1,1,1,1,0,1,0,0,0,0,1,1,0,1,1,0,0,0,1,0,0,0,1,1,1,1,0,1,0,1,1,0,1,0,1,0,0,0,0,1,1,0,1,0,1,0,0,0,1,0,1,0,0,0,1,1,1,0,0,1,1,0,1,1,0,0,1,1,1,1,1,0,0,1,1,0,0,0,1,1,0,1,0,0,0,1,1,0,0,1,1,1,1,0,0,1,0,0,0,1,1,1,0,0,1,1,0,1,0,1,1,0,1,1,0,1,0,0,1,0,0,1,1,1,1,1,1,0,1,1,1,1,0,1,1,0,0,0,0,1,1,1,1,1,0,0,0,1,0,0,0,0,1,0,0,0,1,0,1,0,1,1,0,0,0,0,1,0,1,1,1,1,0,1,0,0,0,1,1,0,0,0,1,1,1,0,1,1,1,0,1,0,0,0,0,0,0,0,0,1,0,1,0,0,1,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,1,1,0,0,1,0,0,1,0,1,1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,1,1,1,1,0,0,0,1,1,1,0,0,0,1,0,0,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,0,1,1,0,1,0,1,1,1,1,0,0,1,0,1,0,1,1,1,0,0,0,0,0,1,1,0,1,1,0,0,0,1,0,1,0,1,0,0,0,0,1,0,1,1,1,0,0,0,0,1,0,0,1,0,1,0,0,0,1,0,1,1,1,0,0,1,1,1,1,1,1,1,0,0,0,0,0,1,0,0,1,1,0,1,0,0,1,0,0,0,1,0,0,1,0,1,0,0,0,1,1,0,1,0,0,0,0,0,1,1,1,1,0,0,1,0,0,1,0,1,1,1,1,1,1,1,0,1,0,0,1,0,0,0,1,1,0,1,1,1,0,0,0,1,0,1,0,0,1,0,1,0,1,0,1,1,1,0,0,1,0,1,1,0,0,1,1,1,1,1,0,0,0,1,1,0};
static byte TIA_DIV31[ ]        __attribute__((section(".dtcm"))) = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0};
byte tia_audc[2]                __attribute__((section(".dtcm"))) = {0};
byte tia_audf[2]                __attribute__((section(".dtcm"))) = {0};
byte tia_audv[2]                __attribute__((section(".dtcm"))) = {0};
static byte tia_poly4Cntr[2]    __attribute__((section(".dtcm"))) = {0};
static byte tia_poly5Cntr[2]    __attribute__((section(".dtcm"))) = {0};
static u16 tia_poly9Cntr[2]     __attribute__((section(".dtcm"))) = {0};

// ----------------------------------------------------------------------------
// ProcessChannel
// ----------------------------------------------------------------------------
static ITCM_CODE void tia_ProcessChannel0(void)
{
  tia_poly5Cntr[0] = (tia_poly5Cntr[0] + 1) % TIA_POLY5_SIZE;
  if(((tia_audc[0] & 2) == 0) || (((tia_audc[0] & 1) == 0) && TIA_DIV31[tia_poly5Cntr[0]]) || (((tia_audc[0] & 1) == 1) && TIA_POLY5[tia_poly5Cntr[0]])) 
  {
    if(tia_audc[0] & 4) 
    {
      tia_volume[0] = (!tia_volume[0])? tia_audv[0]: 0;
    }
    else if(tia_audc[0] & 8) 
    {
      if(tia_audc[0] == 8) 
      {
        tia_poly9Cntr[0] = (tia_poly9Cntr[0]+1) % TIA_POLY9_SIZE;
        tia_volume[0] = (TIA_POLY9[tia_poly9Cntr[0]])? tia_audv[0]: 0;
      }
      else 
      {
        tia_volume[0] = (TIA_POLY5[tia_poly5Cntr[0]])? tia_audv[0]: 0;
      }
    }
    else 
    {
      tia_poly4Cntr[0] = (tia_poly4Cntr[0] + 1) % TIA_POLY4_SIZE;
      tia_volume[0] = (TIA_POLY4[tia_poly4Cntr[0]])? tia_audv[0]: 0;
    }
  }
}

static ITCM_CODE void tia_ProcessChannel1(void)
{
  tia_poly5Cntr[1] = (tia_poly5Cntr[1] + 1) % TIA_POLY5_SIZE;
  if(((tia_audc[1] & 2) == 0) || (((tia_audc[1] & 1) == 0) && TIA_DIV31[tia_poly5Cntr[1]]) || (((tia_audc[1] & 1) == 1) && TIA_POLY5[tia_poly5Cntr[1]])) 
  {
    if(tia_audc[1] & 4) 
    {
      tia_volume[1] = (!tia_volume[1])? tia_audv[1]: 0;
    }
    else if(tia_audc[1] & 8) 
    {
      if(tia_audc[1] == 8) 
      {
        tia_poly9Cntr[1] = (tia_poly9Cntr[1]+1) % TIA_POLY9_SIZE;
        tia_volume[1] = (TIA_POLY9[tia_poly9Cntr[1]])? tia_audv[1]: 0;
      }
      else 
      {
        tia_volume[1] = (TIA_POLY5[tia_poly5Cntr[1]])? tia_audv[1]: 0;
      }
    }
    else 
    {
      tia_poly4Cntr[1] = (tia_poly4Cntr[1] + 1) % TIA_POLY4_SIZE;
      tia_volume[1] = (TIA_POLY4[tia_poly4Cntr[1]])? tia_audv[1]: 0;
    }
  }
}

// ----------------------------------------------------------------------------
// Reset
// ----------------------------------------------------------------------------
void tia_Reset( ) {
  uint index;
  
  tiaBufIdx = 0;
  for(index = 0; index < 2; index++) {
    tia_volume[index] = 0;
    tia_counterMax[index] = 0;
    tia_counter[index] = 0;
    tia_audc[index] = 0;
    tia_audf[index] = 0;
    tia_audv[index] = 0;
    tia_poly4Cntr[index] = 0;
    tia_poly5Cntr[index] = 0;
    tia_poly9Cntr[index] = 0;
  }
  tia_Clear( );
}

// ----------------------------------------------------------------------------
// Clear
// ----------------------------------------------------------------------------
void tia_Clear( ) {
  uint index;
  for(index = 0; index < SNDLENGTH; index++) {
    tia_buffer[index] = 0;
  }
}



// Same as TIA_Process but designed for Pokey integration...
ITCM_CODE  int TIA_Sample(void)
{
    if(tia_counter[0] > 1) 
    {
      tia_counter[0]--;
    }
    else if(tia_counter[0] == 1) 
    {
      tia_counter[0] = tia_counterMax[0];
      tia_ProcessChannel0();
    }
    if(tia_counter[1] > 1) 
    {
      tia_counter[1]--;
    }
    else if(tia_counter[1] == 1) 
    {
      tia_counter[1] = tia_counterMax[1];
      tia_ProcessChannel1();
    }
    return ((int)tia_volume[0] + (int)tia_volume[1]);
}

// --------------------------------------------------------------------------------------
// Process
// --------------------------------------------------------------------------------------
ITCM_CODE void tia_Process(void) 
{
  u32 samp[2];
  for(u8 index = 0; index < 2; index++) 
  {
    if(tia_counter[0] > 1) 
    {
      tia_counter[0]--;
    }
    else if(tia_counter[0] == 1) 
    {
      tia_counter[0] = tia_counterMax[0];
      tia_ProcessChannel0();
    }
    if(tia_counter[1] > 1) 
    {
      tia_counter[1]--;
    }
    else if(tia_counter[1] == 1) 
    {
      tia_counter[1] = tia_counterMax[1];
      tia_ProcessChannel1();
    }
    samp[index] = ((tia_volume[0] + tia_volume[1]));
  }
  tia_buffer[tiaBufIdx++] = (samp[1] << 8) | (samp[0]);
  tiaBufIdx &= (SNDLENGTH-1);
}


ITCM_CODE u16 tia_ProcessNow(void) 
{
  u32 samp[2];
  for(u8 index = 0; index < 2; index++) 
  {
    if(tia_counter[0] > 1) 
    {
      tia_counter[0]--;
    }
    else if(tia_counter[0] == 1) 
    {
      tia_counter[0] = tia_counterMax[0];
      tia_ProcessChannel0();
    }
    if(tia_counter[1] > 1) 
    {
      tia_counter[1]--;
    }
    else if(tia_counter[1] == 1) 
    {
      tia_counter[1] = tia_counterMax[1];
      tia_ProcessChannel1();
    }
    samp[index] = ((tia_volume[0] + tia_volume[1]));
  }
  return (u16)((samp[1] << 8) | (samp[0]));
}
