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
// Riot.h
// ----------------------------------------------------------------------------
#ifndef RIOT_H
#define RIOT_H

#include "Equates.h"
#include "Memory.h"

#include "shared.h"


extern void riot_Reset(void);
extern void riot_SetInput(const byte* input);
extern void riot_SetDRA(byte data);
extern void riot_SetDRB(byte data);
extern void riot_SetTimer(word timer, byte intervals);
extern void riot_UpdateTimer(byte cycles);
extern uint riot_and_wsync;
extern int riot_timer;
extern int riot_intervals;
extern u32 snes_adaptor;
extern void riot_SetDRA(byte data);
extern void riot_SetDRB(byte data);


#endif
