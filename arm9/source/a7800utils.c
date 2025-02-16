// =====================================================================================
// Copyright (c) 2022-2025 Dave Bernazzani (wavemotion-dave)
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
#include <nds.h>
#include <nds/fifomessages.h>
#include <maxmod9.h>

#include <fat.h>
#include <dirent.h>
#include <unistd.h>

#include "main.h"
#include "config.h"
#include "a7800utils.h"
#include "emu/Database.h"
#include "emu/ProSystem.h"

#include "clickNoQuit_wav.h"
#include "bgBottom.h"
#include "bgTop.h"
#include "bgFileSel.h"
#include "printf.h"
#include "soundbank.h"
#include "soundbank_bin.h"

u8 isDS_LITE                            __attribute__((section(".dtcm"))) = 0;
u8 frameSkipMask                        __attribute__((section(".dtcm"))) = 1;
u16 lastSample                          __attribute__((section(".dtcm"))) = 0;
u16 gTotalAtariFrames                   __attribute__((section(".dtcm"))) = 0;
int atari_frames                        __attribute__((section(".dtcm"))) = 0;
u8 bRefreshXY                           __attribute__((section(".dtcm"))) = false;
u16 dampen                              __attribute__((section(".dtcm"))) = 0;
unsigned char keyboard_data[20]         __attribute__((section(".dtcm"))) ALIGN(32);
short int emu_state                     __attribute__((section(".dtcm"))) = 0;
u8  full_speed                           __attribute__((section(".dtcm"))) = 0;
u8  fpsDisplay                          __attribute__((section(".dtcm"))) = 0;
u8  bEmulatorRun                        __attribute__((section(".dtcm"))) = 1;
u8  bNoDatabase                         __attribute__((section(".dtcm"))) = 0;
u32 snes_adaptor                        __attribute__((section(".dtcm"))) = 0x0000FFFF;

extern u32 tiaBufIdx;
char fpsbuf[34];
char dbgbuf[40];
u8   bJustSavedHSC = 0;
u8   spamHSC = 0;

// -----------------------------------------------------------------
// Some vars for listing filenames of ROMs... 1K of ROMs is plenty
// -----------------------------------------------------------------
FICA7800 proromlist[1024];  
unsigned int countpro=0, countfiles=0, ucFicAct=0;

int bg0;
int bg1;
int bg0b,bg1b;
int bg2;
int bg3;             // BG pointers 
int bg0s, bg1s;      // sub BG pointers 

#define MAX_DEBUG 6
int debug[MAX_DEBUG]={0};
u8 DEBUG_DUMP = 0;

uint video_height;                       // Actual video height
u16 *bufVideo;                           // Video flipping buffer
gamecfg GameConf;                        // Game Config svg

short cxBG __attribute__((section(".dtcm")));
short cyBG __attribute__((section(".dtcm")));
short xdxBG  __attribute__((section(".dtcm")));
short ydyBG  __attribute__((section(".dtcm")));

u32 myTiaBufIdx     __attribute__((section(".dtcm"))) = 0;
u8 soundEmuPause = 1;

#define WAITVBL swiWaitForVBlank(); swiWaitForVBlank(); swiWaitForVBlank(); swiWaitForVBlank(); swiWaitForVBlank();

static void DumpDebugData(void)
{
    if (DEBUG_DUMP)
    {
        for (int i=0; i<MAX_DEBUG; i++)
        {
            sprintf(fpsbuf, "D%-2d: %08ld %08XH", i, debug[i], debug[i]);
            dsPrintValue(0,2+i,0, fpsbuf);
        }
        
        extern byte last_illegal_opcode;
        if (last_illegal_opcode > 0)
        {
            sprintf(fpsbuf, "ILLEGAL OP=%02X", last_illegal_opcode);
            dsPrintValue(10,23,0, fpsbuf);
        }
            
    }
}

// ------------------------------------------------------------
// Utility function to pause the sound... 
// ------------------------------------------------------------
void SoundPause(void)
{
    soundEmuPause = 1;
}

// ------------------------------------------------------------
// Utility function to un pause the sound... 
// ------------------------------------------------------------
void SoundUnPause(void)
{
    myTiaBufIdx = 0;
    tiaBufIdx = 0;
    memset(tia_buffer, 0x00, SNDLENGTH);
    TIMER0_CR=0;
    TIMER0_DATA=0;
    TIMER0_CR=TIMER_ENABLE|TIMER_DIV_1024;
    atari_frames = 0;
    WAITVBL;WAITVBL;
    soundEmuPause = 0;
}



// Color fading effect
void FadeToColor(unsigned char ucSens, unsigned short ucBG, unsigned char ucScr, unsigned char valEnd, unsigned char uWait) {
  unsigned short ucFade;
  unsigned char ucBcl;

  // Fade-out vers le noir
  if (ucScr & 0x01) REG_BLDCNT=ucBG;
  if (ucScr & 0x02) REG_BLDCNT_SUB=ucBG;
  if (ucSens == 1) {
    for(ucFade=0;ucFade<valEnd;ucFade++) {
      if (ucScr & 0x01) REG_BLDY=ucFade;
      if (ucScr & 0x02) REG_BLDY_SUB=ucFade;
      for (ucBcl=0;ucBcl<uWait;ucBcl++) {
        swiWaitForVBlank();
      }
    }
  }
  else {
    for(ucFade=16;ucFade>valEnd;ucFade--) {
      if (ucScr & 0x01) REG_BLDY=ucFade;
      if (ucScr & 0x02) REG_BLDY_SUB=ucFade;
      for (ucBcl=0;ucBcl<uWait;ucBcl++) {
        swiWaitForVBlank();
      }
    }
  }
}

u8 tchepres_delay = 0;
u8 tchepres_value = 0;

#define tchepres(a) \
   keyboard_data[GameConf.DS_Pad[a]] = 1;

int16_t temp_shift __attribute__((section(".dtcm"))) = 0;
uint8_t shiftTime = 0;
uint8_t shift_dampen = 0;

ITCM_CODE void vblankIntr() 
{
  if (bRefreshXY || temp_shift)
  {
    cxBG = (myCartInfo.xOffset << 8); 
    cyBG = (myCartInfo.yOffset + temp_shift) << 8;
    xdxBG = ((320 / myCartInfo.xScale) << 8) | (320 % myCartInfo.xScale) ;
    ydyBG = ((video_height / myCartInfo.yScale) << 8) | (video_height % myCartInfo.yScale);

    REG_BG2X = cxBG; 
    REG_BG2Y = cyBG; 
    REG_BG3X = cxBG+myCartInfo.xJiggle; 
    REG_BG3Y = cyBG+myCartInfo.yJiggle; 

    REG_BG2PA = xdxBG; 
    REG_BG2PD = ydyBG; 
    REG_BG3PA = xdxBG; 
    REG_BG3PD = ydyBG; 

    bRefreshXY = false;
    
    if (shift_dampen)
    {
        shift_dampen--;
    }
    else if (temp_shift)
    {
        ++shiftTime;
        if (shiftTime > 40) 
        {
            if (temp_shift < 0) temp_shift++; else temp_shift--;
            if (temp_shift == 0)
            {
                shiftTime = 0;
                bRefreshXY = 1; // Force the next vBlank interrupt to put screen right
            }
        }
    }    
  }
}

void dsInitScreenMain(void) 
{
    SetYtrigger(190); //trigger 2 lines before vsync
    irqSet(IRQ_VBLANK, vblankIntr);
    irqEnable(IRQ_VBLANK);
    
    if (isDSiMode()) isDS_LITE = false; 
    else isDS_LITE = true;    

    vramSetBankB(VRAM_B_LCD );                // Not using this for video but 128K of faster RAM always useful! Mapped at 0x06820000 - 64K for extra Bankswitched RAM
    vramSetBankD(VRAM_D_LCD );                // Not using this for video but 128K of faster RAM always useful! Mapped at 0x06860000 - Used for Cart Bankswitch
    vramSetBankE(VRAM_E_LCD );                // Not using this for video but 64K of faster RAM always useful!  Mapped at 0x06880000 - Used for Cart Bankswitch
    vramSetBankF(VRAM_F_LCD );                // Not using this for video but 16K of faster RAM always useful!  Mapped at 0x06890000 -   ..
    vramSetBankG(VRAM_G_LCD );                // Not using this for video but 16K of faster RAM always useful!  Mapped at 0x06894000 -   ..
    vramSetBankH(VRAM_H_LCD );                // Not using this for video but 32K of faster RAM always useful!  Mapped at 0x06898000 -   ..
    vramSetBankI(VRAM_I_LCD );                // Not using this for video but 16K of faster RAM always useful!  Mapped at 0x068A0000 -   ..    
}

void dsInitTimer(void) 
{
    TIMER0_DATA=0;
    TIMER0_CR=TIMER_ENABLE|TIMER_DIV_1024; 
}

void dsShowScreenEmu(void) 
{
    // Change vram
    videoSetMode(MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE);
    vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
    vramSetBankB(VRAM_B_LCD );                // Not using this for video but 128K of faster RAM always useful! Mapped at 0x06820000 - 64K used for Cart Bankswitch and 64K for extra RAM
    vramSetBankD(VRAM_D_LCD );                // Not using this for video but 128K of faster RAM always useful! Mapped at 0x06860000 - Used for Cart Bankswitch
    vramSetBankE(VRAM_E_LCD );                // Not using this for video but 64K of faster RAM always useful!  Mapped at 0x06880000 - Used for Cart Bankswitch
    vramSetBankF(VRAM_F_LCD );                // Not using this for video but 16K of faster RAM always useful!  Mapped at 0x06890000 -   ..
    vramSetBankG(VRAM_G_LCD );                // Not using this for video but 16K of faster RAM always useful!  Mapped at 0x06894000 -   ..
    vramSetBankH(VRAM_H_LCD );                // Not using this for video but 32K of faster RAM always useful!  Mapped at 0x06898000 -   ..
    vramSetBankI(VRAM_I_LCD );                // Not using this for video but 16K of faster RAM always useful!  Mapped at 0x068A0000 -   ..
    bg0 = bgInit(3, BgType_Bmp8, BgSize_B8_512x512, 0,0);
    bg1 = bgInit(2, BgType_Bmp8, BgSize_B8_512x512, 0,0);

    REG_BLDCNT = BLEND_ALPHA | BLEND_SRC_BG2 | BLEND_DST_BG3;
    REG_BLDALPHA = (8 << 8) | 8; // 50% / 50% 
    
    // Setup video scaling...
    bufVideo = BG_GFX;   
    cxBG = (myCartInfo.xOffset << 8); 
    cyBG = (myCartInfo.yOffset << 8);
    xdxBG = ((320 / myCartInfo.xScale) << 8) | (320 % myCartInfo.xScale) ;
    ydyBG = ((video_height / myCartInfo.yScale) << 8) | (video_height % myCartInfo.yScale);

    REG_BG2PB = 0;
    REG_BG2PC = 0;
    REG_BG3PB = 0;
    REG_BG3PC = 0;

    REG_BG2X = cxBG+myCartInfo.xJiggle; 
    REG_BG2Y = cyBG+myCartInfo.yJiggle; 
    REG_BG3X = cxBG; 
    REG_BG3Y = cyBG; 

    REG_BG2PA = xdxBG; 
    REG_BG2PD = ydyBG; 
    REG_BG3PA = xdxBG; 
    REG_BG3PD = ydyBG;  
}

void dsShowScreenMain(bool full) 
{
    if (full)
    {
        // Init BG mode for 16 bits colors
        videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE );
        videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE);
        vramSetBankA(VRAM_A_MAIN_BG); vramSetBankC(VRAM_C_SUB_BG);
        bg0 = bgInit(0, BgType_Text8bpp, BgSize_T_256x256, 31,0);
        bg0b = bgInitSub(0, BgType_Text8bpp, BgSize_T_256x256, 31,0);
        bg1b = bgInitSub(1, BgType_Text8bpp, BgSize_T_256x256, 30,0);
        bgSetPriority(bg0b,1);bgSetPriority(bg1b,0);

        decompress(bgTopTiles, bgGetGfxPtr(bg0), LZ77Vram);
        decompress(bgTopMap, (void*) bgGetMapPtr(bg0), LZ77Vram);
        dmaCopy((void *) bgTopPal,(u16*) BG_PALETTE,256*2);
    }
    
    decompress(bgBottomTiles, bgGetGfxPtr(bg0b), LZ77Vram);
    decompress(bgBottomMap, (void*) bgGetMapPtr(bg0b), LZ77Vram);
    dmaCopy((void *) bgBottomPal,(u16*) BG_PALETTE_SUB,256*2);
    unsigned short dmaVal = *(bgGetMapPtr(bg1b) +31*32);
    dmaFillWords(dmaVal | (dmaVal<<16),(void*) bgGetMapPtr(bg1b),32*24*2);

    REG_BLDCNT=0; REG_BLDCNT_SUB=0; REG_BLDY=0; REG_BLDY_SUB=0;

    swiWaitForVBlank();
}

void dsFreeEmu(void) 
{
    SoundPause();
}

void dsLoadGame(char *filename) 
{
  u16 index;
  
  SoundPause();  
    
  // Clear out debug info...
  for (int i=0; i<MAX_DEBUG; i++)
  {
      debug[i] = 0;
  }
    
  // load card game if ok
  if (cartridge_Load(filename) != 0) 
  {	
    // Initialize the virtual console emulation 
    database_Load(cartridge_digest);

    // ------------------------------------------------------------------------------------------------
    // Enable or disbale frameskip... 0xFF means display every frame and 0x01 means every-other frame
    // ------------------------------------------------------------------------------------------------
    frameSkipMask = 0xFF;   // Assume frameskip disabled until proven otherwise directly below
    if (myCartInfo.frameSkip == FRAMESKIP_MEDIUM)     frameSkipMask = 0x03;
    if (myCartInfo.frameSkip == FRAMESKIP_AGGRESSIVE) frameSkipMask = 0x01;
      
    dsShowScreenEmu();
    prosystem_Reset();
      
    lastSample = 0;
    bJustSavedHSC = 0;
    spamHSC = 0;
          
    if (DEBUG_DUMP)
    {
        extern char header[];
        extern word cardtype;
        sprintf(dbgbuf, "V%c CTV3: %04X  V4MT: %04X AUD:%02X", '0'+header[0], cardtype, (((u16)header[64]<<8)|header[65])&0xFFFF, header[67]);
        dsPrintValue(0,21,0, (char*)dbgbuf);
        for (u8 i=0; i<31; i++) dbgbuf[i] = header[17+i];
        dbgbuf[31] = 0;        
        dsPrintValue(0,22,0, (char*)dbgbuf);
        dsPrintValue(0,23,0, (char*)cartridge_digest);
    }
      
    if (myCartInfo.region != NTSC)
    {
        dsPrintValue(0,22,0,"PAL ROM NOT SUPPORTED. SEEK NTSC");
    }
      
    // Init palette
    for(index = 0; index < 256; index++) {
      word r = palette_data[(index * 3) + 0];
      word g = palette_data[(index * 3) + 1];
      word b = palette_data[(index * 3) + 2];
      BG_PALETTE[index] = RGB8(r, g, b);
    }
    
    // Init vars
    memset(keyboard_data,0,sizeof(keyboard_data));

    // Left difficulty switch defaults to DIFF_A
    // Right difficulty swtich defaults to DIFF_A
    keyboard_data[15] = myCartInfo.diff1;
    keyboard_data[16] = myCartInfo.diff2;
    GameConf.DS_Pad[ 0] = 3;   GameConf.DS_Pad[ 1] = 2;
    GameConf.DS_Pad[ 2] = 1;   GameConf.DS_Pad[ 3] = 0;
    GameConf.DS_Pad[ 4] = 4;   GameConf.DS_Pad[ 5] = 5;
    GameConf.DS_Pad[ 6] = 12;  GameConf.DS_Pad[ 7] = 12;
    GameConf.DS_Pad[ 8] = 15;  GameConf.DS_Pad[ 9] = 16;
    GameConf.DS_Pad[10] = 14;  GameConf.DS_Pad[11] = 13;
    GameConf.DS_Pad[12] = 6;   GameConf.DS_Pad[13] = 8;
    GameConf.DS_Pad[14] = 7;   GameConf.DS_Pad[15] = 9;

    atari_frames = 0;
    TIMER0_CR=0;
    TIMER0_DATA=0;
    TIMER0_CR=TIMER_ENABLE|TIMER_DIV_1024;
    
    tchepres_delay = 0;
    tchepres_value = 0;
      
    SoundUnPause();
  }
}

unsigned int dsReadPad(void) {
	unsigned int keys_pressed, ret_keys_pressed;

	do {
		keys_pressed = keysCurrent();
	} while ((keys_pressed & (KEY_LEFT | KEY_RIGHT | KEY_DOWN | KEY_UP | KEY_A | KEY_B | KEY_L | KEY_R))==0);
	ret_keys_pressed = keys_pressed;

	do {
		keys_pressed = keysCurrent();
	} while ((keys_pressed & (KEY_LEFT | KEY_RIGHT | KEY_DOWN | KEY_UP | KEY_A | KEY_B | KEY_L | KEY_R))!=0);

	return ret_keys_pressed;
}

char szName[256];
char szName2[256];

bool dsWaitOnQuit(void) {
  bool bRet=false, bDone=false;
  unsigned int keys_pressed;
  unsigned int posdeb=0;
  
  decompress(bgFileSelTiles, bgGetGfxPtr(bg0b), LZ77Vram);
  decompress(bgFileSelMap, (void*) bgGetMapPtr(bg0b), LZ77Vram);
  dmaCopy((void *) bgFileSelPal,(u16*) BG_PALETTE_SUB,256*2);
  unsigned short dmaVal = *(bgGetMapPtr(bg1b) +31*32);
  dmaFillWords(dmaVal | (dmaVal<<16),(void*) bgGetMapPtr(bg1b),32*24*2);
  
  strcpy(szName,"Quit  a7800DS ?");
  dsPrintValue(16-strlen(szName)/2,5,0,szName);
  sprintf(szName,"%s","A TO CONFIRM, B TO GO BACK");
  dsPrintValue(16-strlen(szName)/2,23,0,szName);
      
  while(!bDone) {
    strcpy(szName,"          YES          ");
    dsPrintValue(5,10+0,(posdeb == 0 ? 1 :  0),szName);
    strcpy(szName,"          NO           ");
    dsPrintValue(5,14+1,(posdeb == 2 ? 1 :  0),szName);
    swiWaitForVBlank();
  
    // Check pad
    keys_pressed=dsReadPad();
    if (keys_pressed & KEY_UP) {
      if (posdeb) posdeb-=2;
    }
    if (keys_pressed & KEY_DOWN) {
      if (posdeb<1) posdeb+=2;
    }
    if (keys_pressed & KEY_A) {
      bRet = (posdeb ? false : true);
      bDone = true;
    }
    if (keys_pressed & KEY_B) {
      bDone = true;
    }
  }

  decompress(bgBottomTiles, bgGetGfxPtr(bg0b), LZ77Vram);
  decompress(bgBottomMap, (void*) bgGetMapPtr(bg0b), LZ77Vram);
  dmaCopy((void *) bgBottomPal,(u16*) BG_PALETTE_SUB,256*2);
  dmaVal = *(bgGetMapPtr(bg1b) +31*32);
  dmaFillWords(dmaVal | (dmaVal<<16),(void*) bgGetMapPtr(bg1b),32*24*2);  

  return bRet;
}

void _putchar(char character) {};   // Not used but needed to link printf()

void dsDisplayFiles(unsigned int NoDebGame,u32 ucSel) 
{
  u16 ucGame;
  u8 maxLen;
  
  // Display all games if possible
  unsigned short dmaVal = *(bgGetMapPtr(bg1b) +31*32);
  dmaFillWords(dmaVal | (dmaVal<<16),(void*) (bgGetMapPtr(bg1b)),32*24*2);
  countfiles ? sprintf(szName,"%04d/%04d GAMES",(int)(1+ucSel+NoDebGame),(int)countfiles) : sprintf(szName,"%04d/%04d FOLDERS",(int)(1+ucSel+NoDebGame),(int)countpro);
  dsPrintValue(16-strlen(szName)/2,3,0,szName);
  dsPrintValue(31,5,0,(char *) (NoDebGame>0 ? "<" : " "));
  dsPrintValue(31,22,0,(char *) (NoDebGame+14<countpro ? ">" : " "));
  sprintf(szName,"%s","A=SELECT, Y=HALT EMU, B=BACK");
  dsPrintValue(16-strlen(szName)/2,23,0,szName);
  for (u8 ucBcl=0;ucBcl<17; ucBcl++) 
  {
    ucGame= ucBcl+NoDebGame;
    if (ucGame < countpro) {
      maxLen=strlen(proromlist[ucGame].filename);
      strcpy(szName,proromlist[ucGame].filename);
      if (maxLen>29) szName[29]='\0';
      if (proromlist[ucGame].directory) 
      {
        sprintf(szName2,"%-29s",szName);
        dsPrintValue(0,5+ucBcl,(ucSel == ucBcl ? 1 :  0),szName2);
      }
      else {
        sprintf(szName2,"%-29s",strupr(szName));
        dsPrintValue(1,5+ucBcl,(ucSel == ucBcl ? 1 : 0),szName2);
      }
    }
  }
}

unsigned int dsWaitForRom(void) 
{
  u8 bDone=false, bRet=false;
  u16 ucHaut=0x00, ucBas=0x00,ucSHaut=0x00, ucSBas=0x00,romSelected= 0, firstRomDisplay=0,nbRomPerPage, uNbRSPage, uLenFic=0;
  s16 ucFlip=0, ucFlop=0;

  decompress(bgFileSelTiles, bgGetGfxPtr(bg0b), LZ77Vram);
  decompress(bgFileSelMap, (void*) bgGetMapPtr(bg0b), LZ77Vram);
  dmaCopy((void *) bgFileSelPal,(u16*) BG_PALETTE_SUB,256*2);
  unsigned short dmaVal = *(bgGetMapPtr(bg1b) +31*32);
  dmaFillWords(dmaVal | (dmaVal<<16),(void*) bgGetMapPtr(bg1b),32*24*2);
  
  nbRomPerPage = (countpro>=17 ? 17 : countpro);
  uNbRSPage = (countpro>=5 ? 5 : countpro);
  if (ucFicAct>countpro-nbRomPerPage) {
    firstRomDisplay=countpro-nbRomPerPage;
    romSelected=ucFicAct-countpro+nbRomPerPage;
  }
  else {
    firstRomDisplay=ucFicAct;
    romSelected=0;
  }
  dsDisplayFiles(firstRomDisplay,romSelected);
  while (!bDone) {
    if (keysCurrent() & KEY_UP) {
      ucFlip = -50;
      ucFlop = 0;
      uLenFic = 0;  
      if (!ucHaut) {
        ucFicAct = (ucFicAct>0 ? ucFicAct-1 : countpro-1);
        if (romSelected>uNbRSPage) { romSelected -= 1; }
        else {
          if (firstRomDisplay>0) { firstRomDisplay -= 1; }
          else {
            if (romSelected>0) { romSelected -= 1; }
            else {
              firstRomDisplay=countpro-nbRomPerPage;
              romSelected=nbRomPerPage-1;
            }
          }
        }
        ucHaut=0x01;
        dsDisplayFiles(firstRomDisplay,romSelected);
      }
      else {
        ucHaut++;
        if (ucHaut>10) ucHaut=0;
      } 
    }
    else {
      ucHaut = 0;
    }  
    if (keysCurrent() & KEY_DOWN) {
      ucFlip = -50;
      ucFlop = 0;
      uLenFic = 0;
      if (!ucBas) {
        ucFicAct = (ucFicAct< countpro-1 ? ucFicAct+1 : 0);
        if (romSelected<uNbRSPage-1) { romSelected += 1; }
        else {
          if (firstRomDisplay<countpro-nbRomPerPage) { firstRomDisplay += 1; }
          else {
            if (romSelected<nbRomPerPage-1) { romSelected += 1; }
            else {
              firstRomDisplay=0;
              romSelected=0;
            }
          }
        }
        ucBas=0x01;
        dsDisplayFiles(firstRomDisplay,romSelected);
      }
      else {
        ucBas++;
        if (ucBas>10) ucBas=0;
      } 
    }
    else {
      ucBas = 0;
    }  
    if((keysCurrent() & KEY_R) || (keysCurrent() & KEY_RIGHT)) {
      if (!ucSBas) {
        ucFicAct = (ucFicAct< countpro-nbRomPerPage ? ucFicAct+nbRomPerPage : countpro-nbRomPerPage);
        if (firstRomDisplay<countpro-nbRomPerPage) { firstRomDisplay += nbRomPerPage; }
        else { firstRomDisplay = countpro-nbRomPerPage; }
        if (ucFicAct == countpro-nbRomPerPage) romSelected = 0;
        ucSBas=0x01;
        dsDisplayFiles(firstRomDisplay,romSelected);
      }
      else {
        ucSBas++;
        if (ucSBas>10) ucSBas=0;
      } 
    }
    else {
      ucSBas = 0;
    }  
    if ((keysCurrent() & KEY_L) || (keysCurrent() & KEY_LEFT)) {
      if (!ucSHaut) {
        ucFicAct = (ucFicAct> nbRomPerPage ? ucFicAct-nbRomPerPage : 0);
        if (firstRomDisplay>nbRomPerPage) { firstRomDisplay -= nbRomPerPage; }
        else { firstRomDisplay = 0; }
        if (ucFicAct == 0) romSelected = 0;
        if (romSelected > ucFicAct) romSelected = ucFicAct;
        ucSHaut=0x01;
        dsDisplayFiles(firstRomDisplay,romSelected);
      }
      else {
        ucSHaut++;
        if (ucSHaut>10) ucSHaut=0;
      } 
    }
    else {
      ucSHaut = 0;
    }  
		if ( keysCurrent() & KEY_B ) {
      bDone=true;
      while (keysCurrent() & KEY_B);
    }

    if (keysCurrent() & (KEY_A | KEY_Y | KEY_SELECT))  // Select ROM
    {
      if (!proromlist[ucFicAct].directory) 
      {
        if (keysCurrent() & KEY_Y)      bEmulatorRun = false; else bEmulatorRun=true;
        if (keysCurrent() & KEY_SELECT) bNoDatabase = true; else bNoDatabase=false;
        if (keysCurrent() & KEY_X)      DEBUG_DUMP = 1; else DEBUG_DUMP=0;
        bRet=true;
        bDone=true;
      }
      else if (keysCurrent() & KEY_A) // Drill down into directory
      {
        chdir(proromlist[ucFicAct].filename);
        proFindFiles();
        ucFicAct = 0;
        nbRomPerPage = (countpro>=16 ? 16 : countpro);
        uNbRSPage = (countpro>=5 ? 5 : countpro);
        if (ucFicAct>countpro-nbRomPerPage) {
          firstRomDisplay=countpro-nbRomPerPage;
          romSelected=ucFicAct-countpro+nbRomPerPage;
        }
        else {
          firstRomDisplay=ucFicAct;
          romSelected=0;
        }
        dsDisplayFiles(firstRomDisplay,romSelected);
        while (keysCurrent() & KEY_A);
      }
    }
    // Scroll la selection courante
    if (strlen(proromlist[ucFicAct].filename) > 29) {
      ucFlip++;
      if (ucFlip >= 20) {
        ucFlip = 0;
        uLenFic++;
        if ((uLenFic+29)>strlen(proromlist[ucFicAct].filename)) {
          ucFlop++;
          if (ucFlop >= 20) {
            uLenFic=0;
            ucFlop = 0;
          }
          else
            uLenFic--;
        }
        strncpy(szName,proromlist[ucFicAct].filename+uLenFic,29);
        szName[29] = '\0';
        dsPrintValue(1,5+romSelected,1,szName);
      }
    }
    swiWaitForVBlank();
  }
  
  decompress(bgBottomTiles, bgGetGfxPtr(bg0b), LZ77Vram);
  decompress(bgBottomMap, (void*) bgGetMapPtr(bg0b), LZ77Vram);
  dmaCopy((void *) bgBottomPal,(u16*) BG_PALETTE_SUB,256*2);
  dmaVal = *(bgGetMapPtr(bg1b) +31*32);
  dmaFillWords(dmaVal | (dmaVal<<16),(void*) bgGetMapPtr(bg1b),32*24*2);
  
  return bRet;
}

unsigned int dsWaitOnMenu(unsigned int actState) {
  unsigned int uState=A7800_PLAYINIT;
  bool bDone=false, romSel;
  short iTx,iTy;
  
  while (bDone==false) 
  {
    // wait for stylus
    scanKeys();
	  if(keysHeld() & KEY_TOUCH) 
    {
      touchPosition touch;
      touchRead(&touch);
      iTx = touch.px;
      iTy = touch.py;
      if ((iTx>31) && (iTx<65) && (iTy>159) && (iTy<169))  { // 32,160  -> 64,168   quit
        soundPlaySample(clickNoQuit_wav, SoundFormat_16Bit, clickNoQuit_wav_size, 22050, 127, 64, false, 0);
        bDone=dsWaitOnQuit();
        if (bDone) uState=A7800_QUITSTDS;
      }
      if ((iTx>69) && (iTx<180) && (iTy>10) && (iTy<65)) {     // 80,32 -> 179,61 cartridge slot
        bDone=true; 
        // Find files in current directory and show it 
        proFindFiles();
        romSel=dsWaitForRom();
        if (romSel) { uState=A7800_PLAYINIT; 
          dsLoadGame(proromlist[ucFicAct].filename); }
        else { uState=actState; }
      }
    }
    swiWaitForVBlank();
  }
  
  return uState;
}

void dsPrintValue(int x, int y, unsigned int isSelect, char *pchStr)
{
  u16 *pusEcran,*pusMap;
  u16 usCharac;
  char *pTrTxt=pchStr;
  char ch;

  pusEcran=(u16*) (bgGetMapPtr(bg1b))+x+(y<<5);
  pusMap=(u16*) (bgGetMapPtr(bg0b)+(2*isSelect+24)*32);

  while((*pTrTxt)!='\0' )
  {
    ch = *pTrTxt;
    if (ch >= 'a' && ch <= 'z') ch -= 32; // Faster than strcpy/strtoupper
    usCharac=0x0000;
    if ((ch) == '|')
      usCharac=*(pusMap);
    else if (((ch)<' ') || ((ch)>'_'))
      usCharac=*(pusMap);
    else if((ch)<'@')
      usCharac=*(pusMap+(ch)-' ');
    else
      usCharac=*(pusMap+32+(ch)-'@');
    *pusEcran++=usCharac;
    pTrTxt++;
  }
}


// --------------------------------------------------------------------------------------------
// MAXMOD streaming setup and handling...
// --------------------------------------------------------------------------------------------
#define sample_rate  31400      // To rough match the TIA driver for the Atari 7800 - we purposely undershoot slightly
#define buffer_size  (256)      // Enough buffer that we don't have to fill it too often but not so big as to create lag

mm_ds_system sys  __attribute__((section(".dtcm")));
mm_stream myStream __attribute__((section(".dtcm")));

// -------------------------------------------------------------------------------------------
// maxmod will call this routine when the buffer is half-empty and requests that
// we fill the sound buffer with more samples. They will request 'len' samples and
// we will fill exactly that many. If the sound is paused, we fill with 'mute' samples.
// -------------------------------------------------------------------------------------------
ITCM_CODE mm_word OurSoundMixer(mm_word len, mm_addr dest, mm_stream_formats format)
{
    if (soundEmuPause)  // If paused, just send same value - no amplitude... no sound
    {
      s32 *p = (s32*)dest;
      for (int i=0; i<len/4; i++)
      {
        *p++ = 0x0000;
      }
    }
    else
    {
      // -----------------------------------------------------------------------------------
      // Len is always a multiple of 2 so we will transfer 16-bits at a time which takes 
      // just as much CPU time as an 8-bit transfer but we get 2x the data throughput...
      // -----------------------------------------------------------------------------------
      s16 *p = (s16*)dest;
      int new_len = (len>>1);
      while (new_len--)
      {
          if (myTiaBufIdx != tiaBufIdx)
          {
            *p++ = tia_buffer[myTiaBufIdx++];
            myTiaBufIdx &= (SNDLENGTH-1);
          }
          else // We're short some samples... this happens due to emualtor timing not being perfect. Just make up the samples.
          {
              *p++ = (myCartInfo.pokeyType ? pokey_ProcessNow() : tia_ProcessNow());
          }
      }
    }
    return  len;
}


// -------------------------------------------------------------------------------------------
// Setup the maxmod audio stream - this will be an 8-bit Mono output.
// -------------------------------------------------------------------------------------------
void setupStream(void) 
{
  //----------------------------------------------------------------
  //  initialize maxmod with our small 3-effect soundbank
  //----------------------------------------------------------------
  mmInitDefaultMem((mm_addr)soundbank_bin);

  mmLoadEffect(SFX_CLICKNOQUIT);
  mmLoadEffect(SFX_KEYCLICK);
  mmLoadEffect(SFX_MUS_INTRO);   

  //----------------------------------------------------------------
  //  open stream
  //----------------------------------------------------------------
  myStream.sampling_rate  = sample_rate;        // sampling rate =
  myStream.buffer_length  = buffer_size;        // buffer length =
  myStream.callback   = OurSoundMixer;          // set callback function
  myStream.format     = MM_STREAM_8BIT_MONO;    // format = mono 8-bit
  myStream.timer      = MM_TIMER2;              // use hardware timer 2
  myStream.manual     = false;                  // use automatic filling
  mmStreamOpen( &myStream );

  //----------------------------------------------------------------
  //  when using 'automatic' filling, your callback will be triggered
  //  every time half of the wave buffer is processed.
  //
  //  so: 
  //  25000 (rate)
  //  ----- = ~21 Hz for a full pass, and ~42hz for half pass
  //  1200  (length)
  //----------------------------------------------------------------
  //  with 'manual' filling, you must call mmStreamUpdate
  //  periodically (and often enough to avoid buffer underruns)
  //----------------------------------------------------------------
}


//---------------------------------------------------------------------------------
void dsInstallSoundEmuFIFO(void) 
{
    setupStream();    
}

__attribute__ ((noinline)) void timer_reset(void)
{
    // Timers are fed with 33.513982 MHz clock.
    // With DIV_1024 the clock is 32,728.5 ticks per sec...
    TIMER0_DATA=0;
    TIMER0_CR=TIMER_ENABLE|TIMER_DIV_1024;
    TIMER1_DATA=0;
    TIMER1_CR=TIMER_ENABLE | TIMER_DIV_1024;  
}

static u8 lcd_swap_counter=0;
static int scale_screen_dampen=0;

__attribute__ ((noinline))  void handle_LR_keys(unsigned int keys_pressed)
{
  if (myCartInfo.cardctrl1 != SNES)
  {
      if ((keys_pressed & KEY_R) && (keys_pressed & KEY_L))
      {
          if (++lcd_swap_counter == 30)
          {
              if (keys_pressed & (KEY_X))   lcdSwap();
          }
      } 
      if (scale_screen_dampen > 5)
      {
          if ((keys_pressed & KEY_R) && (keys_pressed & KEY_UP))    { myCartInfo.yOffset++; bRefreshXY = true; }
          if ((keys_pressed & KEY_R) && (keys_pressed & KEY_DOWN))  { myCartInfo.yOffset--; bRefreshXY = true; }
          if ((keys_pressed & KEY_R) && (keys_pressed & KEY_LEFT))  { myCartInfo.xOffset++; bRefreshXY = true; }
          if ((keys_pressed & KEY_R) && (keys_pressed & KEY_RIGHT)) { myCartInfo.xOffset--; bRefreshXY = true; }

          if ((keys_pressed & KEY_L) && (keys_pressed & KEY_UP))    if (myCartInfo.yScale < 234) { myCartInfo.yScale++; bRefreshXY = true; }
          if ((keys_pressed & KEY_L) && (keys_pressed & KEY_DOWN))  if (myCartInfo.yScale > 192) { myCartInfo.yScale--; bRefreshXY = true; }
          if ((keys_pressed & KEY_L) && (keys_pressed & KEY_RIGHT)) if (myCartInfo.xScale < 320) { myCartInfo.xScale++; bRefreshXY = true; }
          if ((keys_pressed & KEY_L) && (keys_pressed & KEY_LEFT))  if (myCartInfo.xScale > 192) { myCartInfo.xScale--; bRefreshXY = true; }                  
          scale_screen_dampen=0;
      } else scale_screen_dampen++;
  }
}

// Toggle full 320x256
static s16 last_xScale = 0;
static s16 last_yScale = 0;
static s16 last_xOffset = 0;
static s16 last_yOffset = 0;
__attribute__ ((noinline)) void toggle_zoom(void)
{
  if (last_xScale == 0)
  {
      last_xScale  = myCartInfo.xScale; 
      last_yScale  = myCartInfo.yScale; 
      last_xOffset = myCartInfo.xOffset;
      last_yOffset = myCartInfo.yOffset;
      myCartInfo.xScale  = 320;
      myCartInfo.yScale  = 234;
      myCartInfo.xOffset = 32;
      myCartInfo.yOffset = 21;
  }
  else
  {
      myCartInfo.xScale = last_xScale;
      myCartInfo.yScale = last_yScale;
      myCartInfo.xOffset = last_xOffset;
      myCartInfo.yOffset = last_yOffset;
      last_xScale = last_yScale = 0;
      last_xOffset = last_yOffset = 0;
  }
  bRefreshXY = true; 
}

static u8 special_hsc_entry=0;    
u8 keys_touch __attribute__((section(".dtcm"))) = 0;

void handle_touch_screen_input(void)
{
    unsigned int romSel;
    short iTx,iTy;    
    touchPosition touch;
    
    touchRead(&touch);
    iTx = touch.px;
    iTy = touch.py;
    
    if ((iTx>2) && (iTx<67) && (iTy>154) && (iTy<171))  { // POWER
        SoundPause();
        mmEffect(SFX_KEYCLICK);  // Play short key click for feedback...
        if (dsWaitOnQuit()) emu_state=A7800_QUITSTDS;
        else SoundUnPause();
    }
    else if ((iTx>240) && (iTx<256) && (iTy>0) && (iTy<20))  { // Full Speed Toggle ... upper right corner...
       full_speed = 1-full_speed; 
       if (full_speed) dsPrintValue(30,0,0,"FS"); else dsPrintValue(30,0,0,"  ");
       dampen=60;
    }
    else if ((iTx>=0) && (iTx<16) && (iTy>0) && (iTy<20))  { // FPS Toggle ... upper left corner...
        fpsDisplay = 1-fpsDisplay; gTotalAtariFrames=0; if (!fpsDisplay) dsPrintValue(0,0,0,"   ");
        dampen=60;
    }
    else if ((iTx>67) && (iTx<128) && (iTy>154) && (iTy<171))  { // PAUSE
        if (keys_touch == 0) mmEffect(SFX_KEYCLICK);  // Play short key click for feedback...
        tchepres_value = 10;
        tchepres_delay = 5;
        tchepres(10);
    }
    else if ((iTx>128) && (iTx<193) && (iTy>154) && (iTy<171))  { // SELECT
        if (keys_touch == 0) mmEffect(SFX_KEYCLICK);  // Play short key click for feedback...
        tchepres_value = 11;
        tchepres_delay = 5;
        tchepres(11);
    }
    else if ((iTx>193) && (iTx<254) && (iTy>154) && (iTy<171))  { // RESET
        if (keys_touch == 0) mmEffect(SFX_KEYCLICK);  // Play short key click for feedback...
        tchepres_value = 6;
        tchepres_delay = 5;
        tchepres(6);
    }
    else if ((iTx>90) && (iTx<110) && (iTy>90) && (iTy<110))  { // Atari Logo - Activate HSC Maintenence Mode (only on High Score screen)
        special_hsc_entry=70; 
    }
    else if ((iTx>69) && (iTx<180) && (iTy>21) && (iTy<62))   // Cartridge slot
    {     
        SoundPause();
        // Find files in current directory and show it 
        proFindFiles();
        romSel=dsWaitForRom();
        if (romSel) {emu_state=A7800_PLAYINIT; dsLoadGame(proromlist[ucFicAct].filename); if (full_speed) dsPrintValue(30,0,0,"FS"); else dsPrintValue(30,0,0,"  ");}
        else SoundUnPause();
    }
    else if ((iTx>190) && (iTx<230) && (iTy>22) && (iTy<62))   // Gear Icon (Settings)
    {     
        SoundPause();
        ShowConfig();
        SoundUnPause();
    }
    else if ((iTx>10) && (iTx<58) && (iTy>22) && (iTy<62))   // Magnifying Glass (zoom)
    {
        if (!keys_touch) toggle_zoom();
    }
}

// ----------------------------------------------------------------------------------
// This is where the action happens!  The main loop runs continually and clocks
// out the 60 frames per second of the 7800 Prosystem
// ----------------------------------------------------------------------------------
ITCM_CODE void dsMainLoop(void) 
{
  static short int last_keys_pressed = 999;
  unsigned int keys_pressed;

  timer_reset();
  
  while(emu_state != A7800_QUITSTDS) {
    switch (emu_state) {
      case A7800_MENUINIT:
        dsShowScreenMain(true);
        emu_state = A7800_MENUSHOW;
        break;
        
      case A7800_MENUSHOW:
        emu_state = dsWaitOnMenu(A7800_MENUSHOW);
        break;
        
      case A7800_PLAYINIT:
        dsShowScreenEmu();
        emu_state = A7800_PLAYGAME;
        last_xScale = 0;
        break;
        
      case A7800_PLAYGAME:
        // 32,728.5 ticks = 1 second
        // 1 frame = 1/50 or 1/60 (0.02 or 0.016)
        // 655 -> 50 fps and 546 -> 60 fps
        if (!full_speed)
        {
            while(TIMER0_DATA < (546*atari_frames)) // We are only supporting NTSC timing
                ;
        }

        if (bEmulatorRun)
        {
            // Execute one frame
            prosystem_ExecuteFrame(keyboard_data);
            if (++atari_frames == 60)
            {
                TIMER0_CR=0;
                TIMER0_DATA=0;
                TIMER0_CR=TIMER_ENABLE|TIMER_DIV_1024;
                atari_frames=0;
            }        

            // Read keys
            if (special_hsc_entry > 0)
            {
                special_hsc_entry--; 
                tchepres(10);
                if (special_hsc_entry < 10)
                {
                    tchepres(11);
                }
                continue;
            }
            memset(keyboard_data, 0x00, 15); // Not the difficulty switches which are the two bytes after this...
        }
        
        // If we have recently pressed any of the console keys... keep it pressed for a minimum duration    
        if (tchepres_delay)
        {
            tchepres_delay--;
            tchepres(tchepres_value);
        }

        scanKeys();
        keys_pressed = keysCurrent();

        if (dampen == 0)
        {
          // if touch screen pressed
          if (keys_pressed & KEY_TOUCH) 
          {
              handle_touch_screen_input();
              keys_touch=1;
          } else keys_touch=0;
          
          if (keys_pressed != last_keys_pressed)
          {
              last_keys_pressed = keys_pressed;
              if ((myCartInfo.cardctrl1 != SNES))
              {
                  if ( (keys_pressed & KEY_SELECT) ) { tchepres_value = 11; tchepres_delay = 5;tchepres(11); } // BUTTON SELECT
              }
              if ((myCartInfo.cardctrl1 != TWIN) && (myCartInfo.cardctrl1 != SNES))
              {
                if ( (keys_pressed & KEY_START) ) { tchepres_value = 10; tchepres_delay = 5; tchepres(10); } // BUTTON PAUSE
              }
              if (myCartInfo.cardctrl1 == SOTA)
              {
                  if ( (keys_pressed & KEY_R) )  { myCartInfo.xOffset +=28; bRefreshXY = true; }
                  if ( (keys_pressed & KEY_L) )  { myCartInfo.xOffset -=28; bRefreshXY = true; }  
              }
              if (dampen < 6) dampen = 6;
          }
        } else dampen--;
        
        snes_adaptor = 0x0000FFFF;  // Nothing pressed to start
            
        // manage a7800 joystick 
        if ( (keys_pressed & KEY_UP) )    { tchepres(0); snes_adaptor &= 0xFFEF;} // UP
        if ( (keys_pressed & KEY_DOWN) )  { tchepres(1); snes_adaptor &= 0xFFDF;} // DOWN
        if ( (keys_pressed & KEY_LEFT) )  { tchepres(2); snes_adaptor &= 0xFFBF;} // LEFT
        if ( (keys_pressed & KEY_RIGHT) ) { tchepres(3); snes_adaptor &= 0xFF7F;} // RIGHT
            
        if (myCartInfo.cardctrl1 == TWIN)
        {
            if ( (keys_pressed & KEY_A) ) { tchepres(12); }  // Left Joystick Right
            if ( (keys_pressed & KEY_B) ) { tchepres(13); }  // Left Joystick Down
            if ( (keys_pressed & KEY_X) ) { tchepres(15); }  // Left Joystick Up
            if ( (keys_pressed & KEY_Y) ) { tchepres(14); }  // Left Joystick Left
            if ( (keys_pressed & KEY_START) ) { tchepres(4); }  // Fire Button (mainly to enter high scores and start game)            
        }
        else if (myCartInfo.cardctrl1 == SNES)
        {
            if ( (keys_pressed & KEY_A) )       { snes_adaptor &= 0xFEFF;}  // SNES BUTTON A
            if ( (keys_pressed & KEY_B) )       { snes_adaptor &= 0xFFFE;}  // SNES BUTTON B
            if ( (keys_pressed & KEY_Y) )       { snes_adaptor &= 0xFFFD;}  // SNES BUTTON Y
            if ( (keys_pressed & KEY_X) )       { snes_adaptor &= 0xFDFF;}  // SNES BUTTON X
            
            if ( (keys_pressed & KEY_L) )       {snes_adaptor &= 0xFBFF;}   // SNES Left Shoulder
            if ( (keys_pressed & KEY_R) )       {snes_adaptor &= 0xF7FF;}   // SNES Right Shoulder
            if ( (keys_pressed & KEY_START) )   {snes_adaptor &= 0xFFF7;}   // SNES Start
            if ( (keys_pressed & KEY_SELECT) )  {snes_adaptor &= 0xFFFB;}   // SNES Select
        }
        else    // Just normal Proline Joystick handling
        {
            if ( (keys_pressed & KEY_A) ) { tchepres(4); snes_adaptor &= 0xFEFF;}  // BUTTON #1
            if ( (keys_pressed & KEY_B) ) { tchepres(5); snes_adaptor &= 0xFFFE;}  // BUTTON #2
            if ( (keys_pressed & KEY_Y) ) { temp_shift = 16; shift_dampen = 1;}    // Shift Screen Down
            if ( (keys_pressed & KEY_X) ) { temp_shift = -16; shift_dampen = 1;}   // Shift Screen Up
        }
            
        if ((keys_pressed & KEY_R) || (keys_pressed & KEY_L))
        {
            handle_LR_keys(keys_pressed);
        }  else lcd_swap_counter=0;

        // -------------------------------------------------------------
        // Stuff to do once/second such as FPS display and Debug Data
        // -------------------------------------------------------------
        if (TIMER1_DATA >= 32728)   // 1000MS (1 sec)
        {
            if (bHSC_dirty) // Check to see if the High Score area has changed ... if so, snap out the .hsc file
            {
                if (high_score_cart_loaded && myCartInfo.hsc) // Check if HSC is enabled
                {
                    if (++spamHSC <= 8) // Check that we haven't been spamming the HSC saving...
                    {
                        dsPrintValue(11,0,0, "HSC SAVING");
                        cartridge_SaveHighScoreSram();
                        bJustSavedHSC = 1;
                    }
                }
                bHSC_dirty = 0;
            }
            else if (bJustSavedHSC)
            {
                bJustSavedHSC = 0;
                if (spamHSC <= 8) spamHSC = 0;
                dsPrintValue(11,0,0, "          ");
            }

            if (fpsDisplay)
            {
                int fps = gTotalAtariFrames;
                if (fps == 61 && !full_speed) fps=60;
                gTotalAtariFrames = 0;
                static u8 lastFPS=99;
                if (fps != lastFPS)
                {
                    lastFPS = fps;
                    fpsbuf[0] = '0' + (int)fps/100;
                    fps = fps % 100;
                    fpsbuf[1] = '0' + (int)fps/10;
                    fpsbuf[2] = '0' + (int)fps%10;
                    fpsbuf[3] = 0;
                    dsPrintValue(0,0,0, fpsbuf);
                }
            }
            DumpDebugData();

            // And finally reset the frame timer...
            TIMER1_CR = 0;
            TIMER1_DATA = 0;
            TIMER1_CR=TIMER_ENABLE | TIMER_DIV_1024;
        }
        break;
    }
	}
  prosystem_Close();
}

//----------------------------------------------------------------------------------
// Find files (a78 / bin) available
int a78Filescmp (const void *c1, const void *c2) {
  FICA7800 *p1 = (FICA7800 *) c1;
  FICA7800 *p2 = (FICA7800 *) c2;
  
  if (p1->filename[0] == '.' && p2->filename[0] != '.')
      return -1;
  if (p2->filename[0] == '.' && p1->filename[0] != '.')
      return 1;
  if (p1->directory && !(p2->directory))
      return -1;
  if (p2->directory && !(p1->directory))
      return 1;
  return strcasecmp (p1->filename, p2->filename);
}

char filenametmp[255];
void proFindFiles(void) {
  DIR *pdir;
  struct dirent *pent;
  
  countpro = countfiles = 0;
  
  pdir = opendir(".");

  if (pdir) 
  {
    while (((pent=readdir(pdir))!=NULL)) 
    {
      strcpy(filenametmp,pent->d_name);
      if (pent->d_type == DT_DIR)
      {
        if (!( (filenametmp[0] == '.') && (strlen(filenametmp) == 1))) 
        {
            if (strcasecmp(filenametmp, "sav") != 0)
            {
              proromlist[countpro].directory = true;
              strcpy(proromlist[countpro].filename,filenametmp);
              countpro++;
            }
        }
      }
      else {
        if (strlen(filenametmp)>4) {
          if ( (strcasecmp(strrchr(filenametmp, '.'), ".a78") == 0) )  {
            proromlist[countpro].directory = false;
            strcpy(proromlist[countpro].filename,filenametmp);
            countpro++;countfiles++;
          }
          if ( (strcasecmp(strrchr(filenametmp, '.'), ".bin") == 0) )  {
            proromlist[countpro].directory = false;
            strcpy(proromlist[countpro].filename,filenametmp);
            countpro++;countfiles++;
          }
        }
      }
    }
    closedir(pdir);
  }
  if (countpro)
  {
    qsort (proromlist, countpro, sizeof (FICA7800), a78Filescmp);
  }
  else  // Failsafe... always provide a back directory...
  {
    proromlist[countpro].directory = true;
    strcpy(proromlist[countpro].filename,"..");
    countpro = 1;
  }    
}

// End of file
