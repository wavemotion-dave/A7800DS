// =====================================================================================================
// Stella DSi - Improved Version by Dave Bernazzani (wavemotion)
//
// See readme.txt for a list of everything that has changed in the baseline 1.0 code.
// =====================================================================================================
#ifndef __CONFIG_H
#define __CONFIG_H

#include <nds.h>
#include "Database.h"
#include "Cartridge.h"

// ---------------------------
// Config handling...
// ---------------------------
#define CONFIG_VER  0x0001

#define MAX_CONFIGS 370

struct AllConfig_t
{
    u16              config_ver;
    Database_Entry   cart[MAX_CONFIGS];
    u32              crc32;
};

extern struct AllConfig_t allConfigs;

void LoadConfig(void);
void ShowConfig(void);
void SaveConfig(bool bShow);

#endif
