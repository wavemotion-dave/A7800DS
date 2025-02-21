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
// Database.h
// ----------------------------------------------------------------------------
#ifndef DATABASE_H
#define DATABASE_H

#include "Cartridge.h"

typedef struct {
  char  digest[33];
  char  header_name[33];
  u8    cardtype;
  u8    pokeyType;
  u8    cardctrl1;
  u8    cardctrl2;
  u8    diff1;
  u8    diff2;
  u8    region;
  u8    hsc;
  s16   dma_adjust;
  s16   xOffset;
  s16   yOffset;
  s16   xScale;
  s16   yScale;
  u8    frameSkip;
  u8    hasHeader;
  u8    palette;
  u8    xJiggle;
  u8    yJiggle;
  u8    xButton;
  u8    spare1;
  u8    spare2;
  u8    spare3;
  u8    spare4;
  s16   spare5;
} Database_Entry;


extern Database_Entry myCartInfo;

#define KEY_MAP_DEFAULT     0
#define KEY_MAP_PANUP       1
#define KEY_MAP_PANDN       2
#define KEY_MAP_JOYUP       3
#define KEY_MAP_JOYDN       4
#define KEY_MAP_JOYLEFT     5
#define KEY_MAP_JOYRIGHT    6
#define KEY_MAP_JOYB1       7
#define KEY_MAP_JOYB2       8
#define KEY_MAP_PAUSE       9


extern void database_Initialize( );
extern bool database_Load(byte *digest);
extern bool database_enabled;

#endif
