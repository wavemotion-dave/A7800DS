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
// Maria.h
// ----------------------------------------------------------------------------
#ifndef MARIA_H
#define MARIA_H

#define MARIA_SURFACE_SIZE (128*1024)   // DS screen buffer size

#include "Equates.h"
#include "Pair.h"
#include "Memory.h"
#include "Sally.h"

#include "shared.h"

extern void maria_Reset( );
extern ITCM_CODE void maria_RenderScanline(void);
extern ITCM_CODE void maria_RenderScanlineTOP(void);
extern void maria_Clear( );
extern word* maria_surface;
extern uint maria_scanline;

#endif
