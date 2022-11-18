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
#include <ctype.h>
#include "HighScore.h"
#include "Cartridge.h"
#include "Database.h"

#define HS_SRAM_START           0x1000      // The 7800 RAM memory location of the high score cartridge SRAM
#define HS_SRAM_SIZE            2048        // The size of the high score cartridge SRAM
#define VIRTUAL_SRAM_BLOCKS     8           // We support 8 virtual high score blocks to keep the now crowded HSC filling up
#define HSC_CART_ROM_SIZE       4096        // The highscore.rom is exactly 4K in size and gets loaded at 7800 memory address 0x3000
#define HSC_TITLE_SIZE          33          // Includes 32 chars for title name and 1 char for size

static u32  last_hs_chksum = 0xFFFFEEEF;
static byte high_score_sram[VIRTUAL_SRAM_BLOCKS][HS_SRAM_SIZE];
static byte high_score_rom[HSC_CART_ROM_SIZE];
static byte hs_virtual_slot = 0;

byte high_score_cart_loaded = false;

/*
 * Saves the high score cartridge SRAM
 *
 * return   Whether the save was successful
 */
bool cartridge_SaveHighScoreSram(void) 
{   
    bool status = false;
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
          high_score_sram[hs_virtual_slot][i] = memory_ram[HS_SRAM_START+i];
          chksum += high_score_sram[hs_virtual_slot][i];
      }

      // ------------------------------------------------------------
      // Make sure there is something different/worth saving...
      // ------------------------------------------------------------
      if ((chksum != last_hs_chksum) && (chksum != 0))
      {
          // -----------------------------------------------------------------------------------------------------------
          // Check to make sure the High Score Cart "Magic Numbers" are right... otherwise corrupt and don't save...
          // -----------------------------------------------------------------------------------------------------------
          if ((high_score_sram[hs_virtual_slot][2] == 0x68) && (high_score_sram[hs_virtual_slot][3] == 0x83) && (high_score_sram[hs_virtual_slot][4] == 0xaa) && (high_score_sram[hs_virtual_slot][5] == 0x55) && (high_score_sram[hs_virtual_slot][6] == 0x9c))
          {
              // ------------------------------------------------------------------------------------------------
              // Make sure the other virtual slots are valid... if not, copy the one known 'good' slot to those
              // ------------------------------------------------------------------------------------------------
              for (u8 i=0; i<VIRTUAL_SRAM_BLOCKS; i++)
              {
                  if (i != hs_virtual_slot) // We already know hs_virtual_slot is good
                  {
                      // Check for magic numbers in the other virtual slots...
                      if (!((high_score_sram[i][2] == 0x68) && (high_score_sram[i][3] == 0x83) && (high_score_sram[i][4] == 0xaa) && (high_score_sram[i][5] == 0x55) && (high_score_sram[i][6] == 0x9c)))
                      {
                          // Bad virutal slot memory found... initialize it with the good one
                          memcpy(high_score_sram[i], high_score_sram[hs_virtual_slot], HS_SRAM_SIZE);
                      }
                  }
              }              
              
              // ---------------------------------------------------------------------------------------
              // On the chance that the HS cart 'name' has been altered in the current slot... copy it
              // ---------------------------------------------------------------------------------------
              for (u8 i=0; i<VIRTUAL_SRAM_BLOCKS; i++)
              {
                  if (i != hs_virtual_slot) // We already know hs_virtual_slot is good
                  {
                      // Copy the title from the current slot...
                      memcpy(&high_score_sram[i][8], &high_score_sram[hs_virtual_slot][8], HSC_TITLE_SIZE);
                  }
              }              
              
              
              last_hs_chksum = chksum;
              DIR* dir = opendir("/data");
              if (dir) closedir(dir);      // Directory exists.
              else mkdir("/data", 0777);   // Doesn't exist - make it...
              FILE* file = fopen("/data/A7800DS.sram", "wb+");
              if( file != NULL ) 
              {
                if( fwrite( high_score_sram, 1, HS_SRAM_SIZE*VIRTUAL_SRAM_BLOCKS, file ) != HS_SRAM_SIZE*VIRTUAL_SRAM_BLOCKS ) 
                {
                  status = false;              
                } else status = true;
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
    word retries=3;
    
    // ----------------------------------------------------------------------------------------
    // Based on the first character of the cart name, we will pick a virtual high score slot
    // This is mainly to spread out the increasingly crowded HSC memory which only supports
    // a total of 69 entries (and this includes difficulty settings on a per game basis).
    // ----------------------------------------------------------------------------------------
    if      (toupper(cartridge_filename[0]) <= 'C') hs_virtual_slot = 0;
    else if (toupper(cartridge_filename[0]) <= 'F') hs_virtual_slot = 1;
    else if (toupper(cartridge_filename[0]) <= 'J') hs_virtual_slot = 2;
    else if (toupper(cartridge_filename[0]) <= 'N') hs_virtual_slot = 3;
    else if (toupper(cartridge_filename[0]) <= 'Q') hs_virtual_slot = 4;
    else if (toupper(cartridge_filename[0]) <= 'T') hs_virtual_slot = 5;
    else if (toupper(cartridge_filename[0]) <= 'Z') hs_virtual_slot = 6;
    else                                            hs_virtual_slot = 7;

    // -------------------------------------------------
    // Start with a blank (0xFF) SRAM block in memory
    // -------------------------------------------------
    for( uint i = 0; i < HS_SRAM_SIZE; i++ ) 
    {
        memory_Write( HS_SRAM_START + i, 0xFF);
    }
    
    while (status == false)
    {
        FILE* file = fopen("/data/A7800DS.sram", "rb" );
        if( file == NULL ) 
        {
            memset(high_score_sram, 0xFF, HS_SRAM_SIZE*VIRTUAL_SRAM_BLOCKS);
            status = false;
        }
        else
        {
            word sram_file_size = fread( high_score_sram, 1, HS_SRAM_SIZE*VIRTUAL_SRAM_BLOCKS, file );
            if (sram_file_size == HS_SRAM_SIZE) // Older 2K format... replicate this across the new 8 virtual slots!
            {
                for (u8 i=1; i<VIRTUAL_SRAM_BLOCKS; i++)
                {
                    memcpy(high_score_sram[i], high_score_sram[0], HS_SRAM_SIZE);
                }
                sram_file_size = HS_SRAM_SIZE*VIRTUAL_SRAM_BLOCKS;
            }

            // Make sure the SRAM memory file is exactly the right size...
            if( sram_file_size == HS_SRAM_SIZE*VIRTUAL_SRAM_BLOCKS ) 
            {
                last_hs_chksum = 0;
                for( uint i = 0; i < HS_SRAM_SIZE; i++ ) 
                {
                    memory_Write( HS_SRAM_START + i, high_score_sram[hs_virtual_slot][i] );
                    last_hs_chksum += high_score_sram[hs_virtual_slot][i];
                }
                status = true;
            }
            else // Something is wrong... just blank out the SRAM area and let the HSC initialize it
            {
                memset(high_score_sram, 0xFF, HS_SRAM_SIZE*VIRTUAL_SRAM_BLOCKS);
                status = false;
            }
            fclose(file);
        }        
        if (--retries == 0) break;
    }
    
    return status;
}

/*
 * Loads the high score cartridge
 *
 * return   Whether the load was successful
 */
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
      fread(high_score_rom, 1, HSC_CART_ROM_SIZE, file );
      cartridge_LoadHighScoreSram();
      for( uint i = 0; i < HSC_CART_ROM_SIZE; i++ )
      {
          memory_Write( 0x3000 + i, high_score_rom[i] );
      }
      high_score_cart_loaded = true;
    }
    else 
    {
      high_score_cart_loaded = false;
    }

    return high_score_cart_loaded;
}

