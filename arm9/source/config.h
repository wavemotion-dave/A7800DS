// =====================================================================================
// Copyright (c) 2022-2024 Dave Bernazzani (wavemotion-dave)
//
// Copying and distribution of this emulator, it's source code and associated 
// readme files, with or without modification, are permitted in any medium without 
// royalty provided this copyright notice is used and wavemotion-dave (Phoenix-Edition),
// Alekmaul (original port) and Greg Stanton (ProSystem Emulator) are thanked profusely.
//
// A7800DS emulator is offered as-is, without any warranty.
//
// The original GPL license:
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
// =====================================================================================
#ifndef __CONFIG_H
#define __CONFIG_H

#include <nds.h>
#include "Database.h"
#include "Cartridge.h"

// ---------------------------
// Config handling...
// ---------------------------
#define CONFIG_VER  0x000A

#define MAX_CONFIGS 700

struct AllConfig_t
{
    u16              config_ver;
    u8               global_buttonAB;
    u8               global_showBios;
    u8               global_spare1;
    u8               global_spare2;
    u8               global_spare3;
    u8               global_spare4;
    u8               global_spare5;
    u8               global_spare6;
    u8               global_spare7;
    u8               global_spare10;
    u8               global_spare11;
    u8               global_spare12;
    u8               global_spare13;
    u8               global_spare14;
    u8               global_unused[512];
    Database_Entry   cart[MAX_CONFIGS];
    u32              crc32;
};

extern struct AllConfig_t allConfigs;

void LoadConfig(void);
void ShowConfig(void);
void SaveConfig(bool bShow);

#endif
