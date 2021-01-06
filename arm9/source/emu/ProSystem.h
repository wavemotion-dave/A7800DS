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
#include "Bios.h"
#include "Cartridge.h"
#include "HighScore.h"
#include "Maria.h"
#include "Memory.h"
#include "Region.h"
#include "Riot.h"
#include "Sally.h"
#include "Archive.h"
#include "Tia.h"
#include "Pokey.h"

#include "shared.h"

//#define KANGAROO_MODE_SUPPORTED YES   // Uncomment this for KANGAROO support. Slightly slower and virtually no game uses it... but more accurate
//#define DS_LITE 1                     // Uncomment this for a very stripped-down version that has very low compaitibility but will run fast enough on the DS_LITE

extern int debug[];

// The number of cycles per scan line
#define CYCLES_PER_SCANLINE 454

// Difficulty switches... 
#define DIFF_A      0
#define DIFF_B      1

extern void prosystem_Reset( );
extern void prosystem_ExecuteFrame(const byte* input);
extern void prosystem_Pause(bool pause);
extern void prosystem_Close( );
extern bool prosystem_active;
extern bool prosystem_paused;
extern word prosystem_frequency;
extern byte prosystem_frame;
extern word prosystem_scanlines;
extern uint prosystem_cycles;

#endif
