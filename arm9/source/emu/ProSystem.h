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
// ProSystem.h
// ----------------------------------------------------------------------------
#ifndef PRO_SYSTEM_H
#define PRO_SYSTEM_H

#include <string.h>
#include <stdio.h>
#include "Equates.h"
#include "Cartridge.h"
#include "HighScore.h"
#include "Maria.h"
#include "Memory.h"
#include "Region.h"
#include "Riot.h"
#include "Sally.h"
#include "Tia.h"
#include "Pokey.h"

#include "shared.h"

//#define KANGAROO_MODE_SUPPORTED 1   // Uncomment this for KANGAROO support. Slightly slower and virtually no game uses it... but more accurate
//#define RAM_MIRRORS_ENABLED     1   // Uncomment if you want to have RAM mirrors handled (slower and no games need it as of this writing)

extern int debug[];
extern u8 isDS_LITE;

// Difficulty switches... 
#define DIFF_A      0
#define DIFF_B      1

extern void prosystem_Reset( );
extern void prosystem_ExecuteFrame(const byte* input);
extern void prosystem_Close( );
extern byte prosystem_frame;
extern uint prosystem_cycles;

#endif
