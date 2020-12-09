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

#include "ProSystem.h"
#include "Sound.h"
#define SOUND_SOURCE "Sound.cpp"

uint sound_lenght;

unsigned char sound_buffer[SNDLENGTH];    // Sound buffer

unsigned int targetIndex = 0;

void processSound(register unsigned char *buffer) 
{
    // Handled directly in sound interrupt routine in a7800utils.c
}
