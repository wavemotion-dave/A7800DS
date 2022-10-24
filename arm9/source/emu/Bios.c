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
// Bios.cpp
// ----------------------------------------------------------------------------
#include "Bios.h"
#define BIOS_SOURCE "Bios.cpp"

bool bios_enabled = false;
char bios_filename[128];

static byte* bios_data = NULL;
static word bios_size = 0;

// ----------------------------------------------------------------------------
// Load
// ----------------------------------------------------------------------------
bool bios_Load(char* filename) {
  if(strlen(filename) == 0) {
    return false;
  }
  
  bios_Release( );

  bios_size = 0;
  if(bios_size == 0) {
    FILE* file = fopen(filename, "rb");
    if(file == NULL) {
      return false;
    } 
    fseek(file, 0, SEEK_END);
    bios_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    bios_data = (byte *) malloc(bios_size);
    fread(bios_data, 1, bios_size, file);
    fclose(file);
  }

  strcpy(bios_filename,filename);
  return true; 
}

// ----------------------------------------------------------------------------
// IsLoaded
// ----------------------------------------------------------------------------
bool bios_IsLoaded( ) {
  return (bios_data != NULL)? true: false;
}

// ----------------------------------------------------------------------------
// Release
// ----------------------------------------------------------------------------
void bios_Release( ) {
  if(bios_data) {
    free(bios_data);
    bios_size = 0;
    bios_data = NULL;
  }
}

// ----------------------------------------------------------------------------
// Store
// ----------------------------------------------------------------------------
void bios_Store( ) {
  if(bios_data != NULL && bios_enabled) {
    memory_WriteROM(65536 - bios_size, bios_size, bios_data);
  }
}
