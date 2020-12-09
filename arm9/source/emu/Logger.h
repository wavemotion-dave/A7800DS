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
// Logger.h
// ----------------------------------------------------------------------------
#ifndef LOGGER_H
#define LOGGER_H
#define LOGGER_LEVEL_DEBUG 0
#define LOGGER_LEVEL_INFO 1
#define LOGGER_LEVEL_ERROR 2

#if 0

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "shared.h"


#ifdef DEBUG
extern bool logger_Initialize( );
extern bool logger_Initialize(char* filename);
extern void logger_LogError(char* message);
extern void logger_LogError(char* message, char* source);
extern void logger_LogInfo(char* message);
extern void logger_LogInfo(char* message, char* source);
extern void logger_LogDebug(char* message);
extern void logger_LogDebug(char* message, char* source);
extern void logger_Release( );
#else
static inline bool logger_Initialize() {}
static inline bool logger_Initialize(char* filename) {}
static inline void logger_LogError(char* message) {}
static inline void logger_LogError(char* message, char* source) {}
static inline void logger_LogInfo(char* message) {}
static inline void logger_LogInfo(char* message, char* source) {}
static inline void logger_LogDebug(char* message) {}
static inline void logger_LogDebug(char* message, char* source) {}
static inline void logger_Release( ) {}
#endif
extern byte logger_level;

#endif

#endif
