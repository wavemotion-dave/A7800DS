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
// HighScore.cpp
// ----------------------------------------------------------------------------
#include <fat.h>
#include <dirent.h>
#include "HighScore.h"
#include "Cartridge.h"
#include "Database.h"

// The memory location of the high score cartridge SRAM
#define HS_SRAM_START 0x1000
// The size of the high score cartridge SRAM
#define HS_SRAM_SIZE 2048

static byte high_score_cart_loaded = false;
static u32  last_chksum = 0xFFFFEEEF;

/*
 * Saves the high score cartridge SRAM
 *
 * return   Whether the save was successful
 */
bool cartridge_SaveHighScoreSram(void) 
{   
    bool status = false;
    byte sram[HS_SRAM_SIZE];
    word retries = 3;
    if(!high_score_cart_loaded || !myCartInfo.hsc)
    {
        // If we didn't load the high score cartridge, or don't have an HSC enabled cart: don't save.
        return false;
    }

  // ------------------------------------------------------------------------------------------------
  // We are being fairly paranoid here mainly because there are games that corrupt the SRAM area
  // of the high score cart... XENOPHOBE is one. The Donkey Kong XM Homebrew is another.  So we
  // want to make sure we aren't saving crap out - this is fairly robust ... so far...
  // ------------------------------------------------------------------------------------------------
    while (status == false)
    {
      u32 chksum = 0;
      for( uint i = 0; i < HS_SRAM_SIZE; i++ ) 
      {
          sram[i] = memory_ram[HS_SRAM_START+i];
          chksum += sram[i];
      }

      // ------------------------------------------------------------
      // Make sure there is something different/worth saving...
      // ------------------------------------------------------------
      if ((chksum != last_chksum) && (chksum != 0))
      {
          // -----------------------------------------------------------------------------------------------------------
          // Check to make sure the High Score Cart "Magic Numbers" are right... otherwise corrupt and don't save...
          // -----------------------------------------------------------------------------------------------------------
          if ((sram[2] == 0x68) && (sram[3] == 0x83) && (sram[4] == 0xaa) && (sram[5] == 0x55) && (sram[6] == 0x9c))
          {
              last_chksum = chksum;
              DIR* dir = opendir("/data");
              if (dir) closedir(dir);      // Directory exists.
              else mkdir("/data", 0777);   // Doesn't exist - make it...
              FILE* file = fopen("/data/A7800DS.sram", "wb+");
              if( file != NULL ) 
              {
                if( fwrite( sram, 1, HS_SRAM_SIZE, file ) != HS_SRAM_SIZE ) 
                {
                  status = false;              
                }
                fflush(file);
                fclose(file);
              }
          }
      }
      if (--retries == 0) break;
    }

    return status;
}

/*
 * Loads the high score cartridge SRAM
 *
 * return   Whether the load was successful
 */
static bool cartridge_LoadHighScoreSram(void)
{    
    bool status = false;
    byte sram[HS_SRAM_SIZE];
    word retries=3;
    
    while (status == false)
    {
        FILE* file = fopen("/data/A7800DS.sram", "rb" );
        if( file == NULL ) 
        {
            status = false;
        }

        if( fread( sram, 1, HS_SRAM_SIZE, file ) == HS_SRAM_SIZE ) 
        {
            fclose( file );
            last_chksum = 0;
            for( uint i = 0; i < HS_SRAM_SIZE; i++ ) 
            {
                memory_Write( HS_SRAM_START + i, sram[i] );
                last_chksum += sram[i];
            }
            status = true;
        }
        fclose(file);
        
        if (--retries == 0) break;
    }
    
    return status;
}

/*
 * Loads the high score cartridge
 *
 * return   Whether the load was successful
 */
#define HSC_CART_ROM_SIZE 4096
byte high_score_buffer[HSC_CART_ROM_SIZE];
bool cartridge_LoadHighScoreCart(void) 
{
    if( !myCartInfo.hsc || myCartInfo.region != 0 ) 
    {
        // Only load the cart if it is enabled and the region is NTSC
        return false;
    }

    FILE* file = fopen("highscore.rom", "rb" );

    // If we don't find it in the current directory, always try /roms/bios and /data/bios
    if (file == NULL)  file = fopen("/roms/bios/highscore.rom", "rb" );
    if (file == NULL)  file = fopen("/data/bios/highscore.rom", "rb" );

    if( file != NULL )
    {
      fread(high_score_buffer, 1, HSC_CART_ROM_SIZE, file );
      cartridge_LoadHighScoreSram();
      for( uint i = 0; i < HSC_CART_ROM_SIZE; i++ )
      {
          memory_Write( 0x3000 + i, high_score_buffer[i] );
      }
      high_score_cart_loaded = true;
    }
    else 
    {
      high_score_cart_loaded = false;
    }

    return high_score_cart_loaded;
}

