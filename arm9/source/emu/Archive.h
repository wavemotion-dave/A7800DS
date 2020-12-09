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
// Archive.h
// ----------------------------------------------------------------------------
#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <string.h>
#include "Logger.h"
#include "Cartridge.h"
//#include "zip.h"
//#include "unzip.h"

#include "shared.h"

extern uint archive_GetUncompressedFileSize(char* filename);
extern bool archive_Uncompress(char* filename, byte* data, uint size);
extern bool archive_Compress(char* zipFilename, char* filename, const byte* data, uint size);

#endif
