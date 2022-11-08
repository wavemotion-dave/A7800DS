// =====================================================================================================
// Stella DSi - Improved Version by Dave Bernazzani (wavemotion)
//
// See readme.txt for a list of everything that has changed in the baseline 1.0 code.
// =====================================================================================================

#include <nds.h>
#include <stdio.h>
#include <fat.h>
#include <dirent.h>
#include <unistd.h>
#include "config.h"
#include "bgBottom.h"
#include "bgFileSel.h"
#include "printf.h"

struct AllConfig_t allConfigs;

extern int bg0, bg0b, bg1b;

static int display_options_list(bool bFullDisplay);

#define CONFIG_INSTRUCTION_STR "      B=EXIT   START=SAVE      "
#define WAITVBL swiWaitForVBlank(); swiWaitForVBlank(); swiWaitForVBlank(); swiWaitForVBlank(); swiWaitForVBlank();
extern void dsPrintValue(int x, int y, unsigned int isSelect, char *pchStr);
extern void dsShowScreenMain(bool);

// ---------------------------------------------------------------------------
// Write out the A7800DS.DAT configuration file to capture the settings for
// each game.  We can store more than 350 game settings!
// ---------------------------------------------------------------------------
void SaveConfig(bool bShow)
{
    FILE *fp;
    int slot = 0;
    
    if (bShow) dsPrintValue(0,23,0, (char*)"     SAVING CONFIGURATION       ");

    // Set the global configuration version number...
    allConfigs.config_ver = CONFIG_VER;

    // Find the slot we should save into...
    for (slot=0; slot<MAX_CONFIGS; slot++)
    {
        if (strcmp(allConfigs.cart[slot].digest, myCartInfo.digest) == 0)  // Got a match?!
        {
            break;                           
        }
        if (strcmp(allConfigs.cart[slot].digest, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx") == 0) // Didn't find it... use a blank slot...
        {
            break;                           
        }
    }

    allConfigs.cart[slot] = myCartInfo;
    
    // -------------------------------------------------------------------------------------
    // Compute the CRC32 of everything and we can check this as integrity in the future...
    // -------------------------------------------------------------------------------------
    u8 *ptr=(u8*)&allConfigs;
    allConfigs.crc32 = 0x00000000;
    for (u32 i=0; i < sizeof(allConfigs) - 4; i++)
    {
        allConfigs.crc32 += *ptr++;
    }    

    DIR* dir = opendir("/data");
    if (dir)
    {
        closedir(dir);  // Directory exists.
    }
    else
    {
        mkdir("/data", 0777);   // Doesn't exist - make it...
    }
    fp = fopen("/data/A7800DS.DAT", "wb+");
    if (fp != NULL)
    {
        fwrite(&allConfigs, sizeof(allConfigs), 1, fp);
        fclose(fp);
    } else dsPrintValue(2,20,0, (char*)"  ERROR SAVING CONFIG FILE  ");

    if (bShow) 
    {
        WAITVBL;WAITVBL;WAITVBL;WAITVBL;WAITVBL;
        dsPrintValue(0,23, 0, (char *)CONFIG_INSTRUCTION_STR);
    }
}


// -------------------------------------------------------------------------------------------------
// After settings hae changed, we call this to apply the new options to the game being played.
// This is also called when loading a game and after the configuration if read from StelaDS.DAT
// -------------------------------------------------------------------------------------------------
static void ApplyOptions(void)
{
    extern u8 bRefreshXY, frameSkipMask;
    extern unsigned char keyboard_data[];
        
    bRefreshXY = true;
    
    keyboard_data[15] = myCartInfo.diff1;
    keyboard_data[16] = myCartInfo.diff2;
    
    frameSkipMask = 0xFF;   // Assume frameskip disabled until proven otherwise directly below
    if (myCartInfo.frameSkip == FRAMESKIP_MEDIUM)     frameSkipMask = 0x03;
    if (myCartInfo.frameSkip == FRAMESKIP_AGGRESSIVE) frameSkipMask = 0x01;
}


static void SetDefaultGameConfig(void)
{
    // Init the entire database
    for (int slot=0; slot<MAX_CONFIGS; slot++)
    {
        strcpy(allConfigs.cart[slot].digest, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
        // TBD - do more.
    }
}

// -------------------------------------------------------------------------
// Find the A7800DS.DAT file and load it... if it doesn't exist, then
// default values will be used for the entire configuration database...
// -------------------------------------------------------------------------
void LoadConfig(void)
{
    bool bInitDatabase = true;
    FILE *fp;

    fp = fopen("/data/A7800DS.DAT", "rb");
    if (fp != NULL)
    {
        fread(&allConfigs, sizeof(allConfigs), 1, fp);
        fclose(fp);
        
        if (allConfigs.config_ver == CONFIG_VER)
        {
            bInitDatabase = false;
        }
    }
    
    if (bInitDatabase)
    {
        dsPrintValue(0,1,0, (char*)"PLEASE WAIT...");
        memset(&allConfigs, 0x00, sizeof(allConfigs));
        allConfigs.config_ver = CONFIG_VER;
        SetDefaultGameConfig();
        SaveConfig(FALSE);
        dsPrintValue(0,1,0, (char*)"              ");
    }
    
    ApplyOptions();
}


// ------------------------------------------------------------------------------
// Options are handled here... we have a number of things the user can tweak
// and these options are applied immediately. The user can also save off 
// their option choices for the currently running game into the A7800DS.DAT
// configuration database. When games are loaded back up, A7800DS.DAT is read
// to see if we have a match and the user settings can be restored for the game.
// ------------------------------------------------------------------------------
struct options_t
{
    const char  *label;
    u8 isNumeric;            // 0=String List, 1=Positive 8-bit, 2=Negative 8-bit
    const char  *option[15];
    u8 *option_val;
    u8 option_max;
};

const struct options_t Game_Option_Table[] =
{
    {"CART TYPE",   0, {"NORMAL/FLAT", "SUPERCART", "SUPER LARGE", "SUPER RAM", "SUPER ROM", "SUPER RAM X2", "ABSOLUTE", "ACTIVISION", "FRACTALUS", "FLAT WITH RAM", "BANKSETS", "BANKSETS RAM", "BANKSETS HALT"},&myCartInfo.cardtype, 13},
    {"HIGHSCORE",   0, {"DISABLED", "ENABLED"},                                                                                                                &myCartInfo.hsc,                 2},
    {"FRAMESKIP",   0, {"DISABLED", "MEDIUM 3/4", "HIGH 1/2"},                                                                                                 &myCartInfo.frameSkip,           3},
    {"POKEY",       0, {"NONE", "AT 4000", "AT 450", "AT 800"},                                                                                                &myCartInfo.pokeyType,           4},
    {"DMA ADJUST",  2, {"-25","+50"},                                                                                                                     (u8*)&myCartInfo.dma_adjust,          2},    
    {"LEFT DIFF",   0, {"A", "B"}   ,                                                                                                                          &myCartInfo.diff1,               2},
    {"RIGHT DIFF",  0, {"A", "B"}   ,                                                                                                                          &myCartInfo.diff2,               2},
    {"LEFT JOY",    0, {"NONE", "JOYSTICK", "LIGHTGUN", "PADDLES", "TWIN STICKS", "SOTA"},                                                                     &myCartInfo.cardctrl1,           6},
    {"RIGHT JOY",   0, {"NONE", "JOYSTICK", "LIGHTGUN", "PADDLES", "TWIN STICKS", "SOTA"},                                                                     &myCartInfo.cardctrl2,           6},    
    {"X OFFSET",    2, {"-50",   "+50"},                                                                                                                  (u8*)&myCartInfo.xOffset,             2},
    {"Y OFFSET",    2, {"-50",   "+50"},                                                                                                                  (u8*)&myCartInfo.yOffset,             2},
    {"X SCALE",     2, {"+200",  "+320"},                                                                                                                 (u8*)&myCartInfo.xScale,              2},
    {"Y SCALE",     2, {"+180",  "+256"},                                                                                                                 (u8*)&myCartInfo.yScale,              2},
    
    {NULL,          0, {"",      ""},                                                                                                                          NULL,                            1},
};

void display_line(u8 idx, u8 highlight)
{
    char strBuf[35];
    if (Game_Option_Table[idx].isNumeric == 1)  // Unsigned 8 bit
    {
        sprintf(strBuf, " %-11s : %-15d", Game_Option_Table[idx].label, *(Game_Option_Table[idx].option_val));
    }
    else if (Game_Option_Table[idx].isNumeric == 2) // Signed 16 bit
    {
        sprintf(strBuf, " %-11s : %-15d", Game_Option_Table[idx].label, *((s16*)Game_Option_Table[idx].option_val));
    }
    else    // Array of strings
    {
        sprintf(strBuf, " %-11s : %-15s", Game_Option_Table[idx].label, Game_Option_Table[idx].option[*(Game_Option_Table[idx].option_val)]);
    }
    dsPrintValue(1,5+idx, highlight, strBuf);
}

// ------------------------------------------------------------------
// Display the current list of options...
// ------------------------------------------------------------------
static int display_options_list(bool bFullDisplay)
{
    int len=0;
    
    if (bFullDisplay)
    {
        while (true)
        {
            display_line(len, (len==0 ? 1:0));
            len++;
            if (Game_Option_Table[len].label == NULL) break;
        }

        // Blank out rest of the screen... option menus are of different lengths...
        for (int i=len; i<20; i++) 
        {
            dsPrintValue(1,6+i, 0, (char *)"                               ");
        }
    }

    dsPrintValue(0,23, 0, (char *)CONFIG_INSTRUCTION_STR);
    return len;    
}


// -----------------------------------------------------------------------------
// Allows the user to move the cursor up and down through the various table 
// enties  above to select options for the game they wish to play. 
// -----------------------------------------------------------------------------
void ShowConfig(void)
{
    int optionHighlighted;
    int idx;
    bool bDone=false;
    int keys_pressed;
    int last_keys_pressed = 999;

    // Show the Options background...
    decompress(bgFileSelTiles, bgGetGfxPtr(bg0b), LZ77Vram);
    decompress(bgFileSelMap, (void*) bgGetMapPtr(bg0b), LZ77Vram);
    dmaCopy((void *) bgFileSelPal,(u16*) BG_PALETTE_SUB,256*2);
    unsigned short dmaVal = *(bgGetMapPtr(bg1b) +31*32);
    dmaFillWords(dmaVal | (dmaVal<<16),(void*) bgGetMapPtr(bg1b),32*24*2);
    swiWaitForVBlank();
    

    idx=display_options_list(true);
    optionHighlighted = 0;
    while (!bDone)
    {
        keys_pressed = keysCurrent();
        if (keys_pressed != last_keys_pressed)
        {
            last_keys_pressed = keys_pressed;
            if (keysCurrent() & KEY_UP) // Previous option
            {
                display_line(optionHighlighted, 0);
                if (optionHighlighted > 0) optionHighlighted--; else optionHighlighted=(idx-1);
                display_line(optionHighlighted, 1);
            }
            if (keysCurrent() & KEY_DOWN) // Next option
            {
                display_line(optionHighlighted, 0);
                if (optionHighlighted < (idx-1)) optionHighlighted++;  else optionHighlighted=0;
                display_line(optionHighlighted, 1);
            }

            if (keysCurrent() & KEY_RIGHT)  // Toggle option clockwise
            {
                if (Game_Option_Table[optionHighlighted].isNumeric == 1)
                {
                    if (*(Game_Option_Table[optionHighlighted].option_val) < atoi(Game_Option_Table[optionHighlighted].option[1]))
                    {
                        *(Game_Option_Table[optionHighlighted].option_val) += 1;
                    }
                }
                else if (Game_Option_Table[optionHighlighted].isNumeric == 2)
                {
                    if (*((s16*)Game_Option_Table[optionHighlighted].option_val) < atoi(Game_Option_Table[optionHighlighted].option[1]))
                    {
                        *((s16*)Game_Option_Table[optionHighlighted].option_val) += 1;
                    }
                }
                else
                {
                    *(Game_Option_Table[optionHighlighted].option_val) = (*(Game_Option_Table[optionHighlighted].option_val) + 1) % Game_Option_Table[optionHighlighted].option_max;
                }
                display_line(optionHighlighted, 1);
                ApplyOptions();                
            }
            if (keysCurrent() & KEY_LEFT)  // Toggle option counterclockwise
            {
                if (Game_Option_Table[optionHighlighted].isNumeric == 1)
                {
                    if (*(Game_Option_Table[optionHighlighted].option_val) > atoi(Game_Option_Table[optionHighlighted].option[0]))
                    {
                        *(Game_Option_Table[optionHighlighted].option_val) -= 1;
                    }
                }
                else if (Game_Option_Table[optionHighlighted].isNumeric == 2)
                {
                    if (*((s16*)Game_Option_Table[optionHighlighted].option_val) > atoi(Game_Option_Table[optionHighlighted].option[0]))
                    {
                        *((s16*)Game_Option_Table[optionHighlighted].option_val) -= 1;
                    }
                }
                else
                {
                    if ((*(Game_Option_Table[optionHighlighted].option_val)) == 0)
                        *(Game_Option_Table[optionHighlighted].option_val) = Game_Option_Table[optionHighlighted].option_max -1;
                    else
                        *(Game_Option_Table[optionHighlighted].option_val) = (*(Game_Option_Table[optionHighlighted].option_val) - 1) % Game_Option_Table[optionHighlighted].option_max;
                }
                display_line(optionHighlighted, 1);
                ApplyOptions();                
            }
            if (keysCurrent() & KEY_START)  // Save Options
            {
                SaveConfig(TRUE);
            }
            if (keysCurrent() & KEY_SELECT)  // Restore Defaults
            {
                //extern void CartSetDefaultFromInternalDatabase(void);
                //CartSetDefaultFromInternalDatabase();
                display_options_list(true);
                optionHighlighted = 0;
            }
            if ((keysCurrent() & KEY_B) || (keysCurrent() & KEY_A))  // Exit options
            {
                break;
            }
        }
        swiWaitForVBlank();
    }

    // Restore original bottom graphic
    dsShowScreenMain(false);
    
    // Give a third of a second time delay...
    for (int i=0; i<20; i++)
    {
        swiWaitForVBlank();
    }

    return;
}


// End of Line

