#include <nds.h>
#include <nds/fifomessages.h>

#include <fat.h>
#include <dirent.h>
#include <unistd.h>

#include "main.h"
#include "a7800utils.h"
#include "emu/Bios.h"
#include "emu/Database.h"
#include "emu/ProSystem.h"
#include "emu/Sound.h"

#include "clickNoQuit_wav.h"
#include "bgBottom.h"
#include "bgTop.h"
#include "bgFileSel.h"

u8 isDS_LITE                            __attribute__((section(".dtcm"))) = 0;

static unsigned char lastPokeySample    __attribute__((section(".dtcm"))) = 0;
static unsigned char lastTiaSample      __attribute__((section(".dtcm"))) = 0;
static unsigned char lastSample         __attribute__((section(".dtcm"))) = 0;
u16 gTotalAtariFrames                   __attribute__((section(".dtcm"))) = 0;
int atari_frames                        __attribute__((section(".dtcm"))) = 0;
u8 bRefreshXY                           __attribute__((section(".dtcm"))) = false;

unsigned char keyboard_data[20]         __attribute__((section(".dtcm")));

FICA7800 proromlist[1024];  
unsigned int countpro=0, countfiles=0, ucFicAct=0;

int bg0;
int bg1;
int bg0b,bg1b;
int bg2;
int bg3;             // BG pointers 
int bg0s, bg1s, bg2s, bg3s;         // sub BG pointers 

u16 full_speed = 0;
int etatEmu;
u16 fpsDisplay=0;

#define MAX_DEBUG 5
int debug[MAX_DEBUG]={0};
//#define DEBUG_DUMP
//#define CART_INFO

#define SOUND_FREQ  (31440/2)           // Be careful if you change this - this matches the frequency of the POKEY update and if we are TIA-only, we will double it.

uint video_height;                       // Actual video height
u16 *bufVideo;                           // Video flipping buffer
gamecfg GameConf;                        // Game Config svg

short cxBG, cyBG, xdxBG,ydyBG;

unsigned char *filebuffer;

#define WAITVBL swiWaitForVBlank(); swiWaitForVBlank(); swiWaitForVBlank(); swiWaitForVBlank(); swiWaitForVBlank();

static void DumpDebugData(void)
{
#ifdef DEBUG_DUMP
    char dbgbuf[32];
    for (int i=0; i<MAX_DEBUG; i++)
    {
        int idx=0;
        int val = debug[i];
        if (val < 0)
        {
            dbgbuf[idx++] = '-';
            val = val * -1;
        }
        else
        {
            dbgbuf[idx++] = '0' + (int)val/10000000;
        }
        val = val % 10000000;
        dbgbuf[idx++] = '0' + (int)val/1000000;
        val = val % 1000000;
        dbgbuf[idx++] = '0' + (int)val/100000;
        val = val % 100000;
        dbgbuf[idx++] = '0' + (int)val/10000;
        val = val % 10000;
        dbgbuf[idx++] = '0' + (int)val/1000;
        val= val % 1000;
        dbgbuf[idx++] = '0' + (int)val/100;
        val = val % 100;
        dbgbuf[idx++] = '0' + (int)val/10;
        dbgbuf[idx++] = '0' + (int)val%10;
        dbgbuf[idx++] = 0;
        dsPrintValue(0,3+i,0, dbgbuf);
    }
#endif
}

u16 myTiaBufIdx __attribute__((section(".dtcm"))) = 0;
u8* snd_ptr __attribute__((section(".dtcm"))) = (u8*)((u32)sound_buffer + 0xA000000);
u8* snd_sta __attribute__((section(".dtcm"))) = (u8*)((u32)sound_buffer + 0xA000000);
u8* snd_end __attribute__((section(".dtcm"))) = (u8*)((u32)sound_buffer + 0xA000000 + SNDLENGTH);
ITCM_CODE void VsoundHandler(void) 
{
  extern unsigned char tia_buffer[];
  extern u16 tiaBufIdx;

  for (u8 i=0; i<4; i++)
  {
      // If there is a fresh sample... 
      if (myTiaBufIdx != tiaBufIdx)
      {
          lastSample = tia_buffer[myTiaBufIdx];
          myTiaBufIdx = (myTiaBufIdx+1) & (SNDLENGTH-1);
      }
      *snd_ptr++ = lastSample;
      if (snd_ptr == snd_end)
      {
          snd_ptr = snd_sta;
      }
  }
}

ITCM_CODE void VsoundHandler_Lite(void) 
{
  extern unsigned char tia_buffer[];
  extern u16 tiaBufIdx;

  for (u8 i=0; i<2; i++)
  {
      // If there is a fresh sample... 
      if (myTiaBufIdx != tiaBufIdx)
      {
          lastSample = tia_buffer[myTiaBufIdx];
          myTiaBufIdx = (myTiaBufIdx+1) & (SNDLENGTH-1);
      }
      *snd_ptr++ = lastSample;
      if (snd_ptr == snd_end)
      {
          snd_ptr = snd_sta;
      }
  }
}

u16 myPokeyBufIdx __attribute__((section(".dtcm"))) = 0;
ITCM_CODE void VsoundHandler_Pokey(void)
 {
  extern unsigned char pokey_buffer[];
  extern u16 pokeyBufIdx;

  for (u8 i=0; i<2; i++)
  {
      // If there is a fresh Pokey sample... 
      if (myPokeyBufIdx != pokeyBufIdx)
      {
          lastPokeySample = pokey_buffer[myPokeyBufIdx];
          myPokeyBufIdx = (myPokeyBufIdx+1) & (SNDLENGTH-1);
      }
      *snd_ptr++ = lastPokeySample;
      if (snd_ptr == snd_end)
      {
          snd_ptr = snd_sta;
      }
  }
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


#define tchepres(a) \
   keyboard_data[GameConf.DS_Pad[a]] = 1;

static const u8 jitter[] = 
{
  0x48, 0x22, 
  0x40, 0x44
};
void vblankIntr() 
{
  static u8 xx=0;

  if (bRefreshXY)
  {
    cxBG = (cartridge_xOffset << 8); 
    cyBG = (cartridge_yOffset << 8);
    xdxBG = ((320 / cartridge_xScale) << 8) | (320 % cartridge_xScale) ;
    ydyBG = ((video_height / cartridge_yScale) << 8) | (video_height % cartridge_yScale);

    REG_BG2X = cxBG; 
    REG_BG2Y = cyBG; 
    REG_BG3X = cxBG; 
    REG_BG3Y = cyBG; 

    REG_BG2PA = xdxBG; 
    REG_BG2PD = ydyBG; 
    REG_BG3PA = xdxBG; 
    REG_BG3PD = ydyBG; 

    bRefreshXY = false;
  }
  if (xx++ & 1)
  {
    REG_BG2X = cxBG+jitter[0]; 
    REG_BG2Y = cyBG+jitter[1]; 
  }
  else
  {
    REG_BG2X = cxBG+jitter[2]; 
    REG_BG2Y = cyBG+jitter[3]; 
  }
}

void dsInitScreenMain(void) 
{
    SetYtrigger(190); //trigger 2 lines before vsync
    irqSet(IRQ_VBLANK, vblankIntr);
    irqEnable(IRQ_VBLANK);
    
    if (isDSiMode()) isDS_LITE = false; 
    else isDS_LITE = true;    

    vramSetBankB(VRAM_B_MAIN_BG_0x06020000 ); // Need to do this early so we can steal a bit of this RAM for bank swap...
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
    vramSetBankB(VRAM_B_MAIN_BG_0x06020000 );
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
    cxBG = (cartridge_xOffset << 8); 
    cyBG = (cartridge_yOffset << 8);
    xdxBG = ((320 / cartridge_xScale) << 8) | (320 % cartridge_xScale) ;
    ydyBG = ((video_height / cartridge_yScale) << 8) | (video_height % cartridge_yScale);

    REG_BG2PB = 0;
    REG_BG2PC = 0;
    REG_BG3PB = 0;
    REG_BG3PC = 0;

    REG_BG2X = cxBG; 
    REG_BG2Y = cyBG; 
    REG_BG3X = cxBG; 
    REG_BG3Y = cyBG; 

    REG_BG2PA = xdxBG; 
    REG_BG2PD = ydyBG; 
    REG_BG3PA = xdxBG; 
    REG_BG3PD = ydyBG;  
}

void dsShowScreenMain(void) 
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
    // Stop timer of sound
    TIMER2_CR=0; irqDisable(IRQ_TIMER2); 
}

void dsLoadGame(char *filename) 
{
  unsigned int index;
  
  // Free buffer if needed
  TIMER2_CR=0; irqDisable(IRQ_TIMER2); 
	if (filebuffer != 0)
		free(filebuffer);
  
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

    if (bios_Load("7800.rom")) 
    {
      bios_enabled = true;
    }
  
    dsShowScreenEmu();
    prosystem_Reset();
      
    lastPokeySample = 0;
    lastTiaSample = 0;
    lastSample = 0;
      
#ifdef CART_INFO
    char cart_info_buf[64];
    dsPrintValue(0,22,0, (char*)cartridge_digest);
    snprintf(cart_info_buf, 63, "CT=%d PK=%d RE=%d SY=%d ST=%d HS=%d ", cartridge_type, cartridge_pokey, cartridge_region, cartridge_uses_wsync, cartridge_steals_cycles, cartridge_hsc_enabled);
    cart_info_buf[32] = 0;
    dsPrintValue(0,21,0,cart_info_buf);
    dsPrintValue(0,19,0,cartridge_filename);      
#endif          
      
    if (cartridge_region != NTSC)
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
    keyboard_data[15] = cartridge_diff1;
    keyboard_data[16] = cartridge_diff2;
    GameConf.DS_Pad[ 0] = 3;   GameConf.DS_Pad[ 1] = 2;
    GameConf.DS_Pad[ 2] = 1;   GameConf.DS_Pad[ 3] = 0;
    GameConf.DS_Pad[ 4] = 4;   GameConf.DS_Pad[ 5] = 5;
    GameConf.DS_Pad[ 6] = 12;  GameConf.DS_Pad[ 7] = 12;
    GameConf.DS_Pad[ 8] = 15;  GameConf.DS_Pad[ 9] = 16;
    GameConf.DS_Pad[10] = 14;  GameConf.DS_Pad[11] = 13;
    GameConf.DS_Pad[12] = 6;   GameConf.DS_Pad[13] = 8;
    GameConf.DS_Pad[14] = 7;   GameConf.DS_Pad[15] = 9;

    dsInstallSoundEmuFIFO();
      
    atari_frames = 0;
    TIMER0_CR=0;
    TIMER0_DATA=0;
    TIMER0_CR=TIMER_ENABLE|TIMER_DIV_1024;
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

bool dsWaitOnQuit(void) {
  bool bRet=false, bDone=false;
  unsigned int keys_pressed;
  unsigned int posdeb=0;
  char szName[32];
  
  decompress(bgFileSelTiles, bgGetGfxPtr(bg0b), LZ77Vram);
  decompress(bgFileSelMap, (void*) bgGetMapPtr(bg0b), LZ77Vram);
  dmaCopy((void *) bgFileSelPal,(u16*) BG_PALETTE_SUB,256*2);
  unsigned short dmaVal = *(bgGetMapPtr(bg1b) +31*32);
  dmaFillWords(dmaVal | (dmaVal<<16),(void*) bgGetMapPtr(bg1b),32*24*2);
  
  strcpy(szName,"Quit  a7800DS ?");
  dsPrintValue(16-strlen(szName)/2,3,0,szName);
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

void dsDisplayFiles(unsigned int NoDebGame,u32 ucSel) 
{
  unsigned int ucBcl,ucGame;
  u8 maxLen;
  char szName[256];
  char szName2[256];
  
  // Display all games if possible
  unsigned short dmaVal = *(bgGetMapPtr(bg1b) +31*32);
  dmaFillWords(dmaVal | (dmaVal<<16),(void*) (bgGetMapPtr(bg1b)),32*24*2);
  countfiles ? sprintf(szName,"%04d/%04d GAMES",(int)(1+ucSel+NoDebGame),(int)countfiles) : sprintf(szName,"%04d/%04d FOLDERS",(int)(1+ucSel+NoDebGame),(int)countpro);
  dsPrintValue(16-strlen(szName)/2,3,0,szName);
  dsPrintValue(31,5,0,(char *) (NoDebGame>0 ? "<" : " "));
  dsPrintValue(31,22,0,(char *) (NoDebGame+14<countpro ? ">" : " "));
  sprintf(szName,"%s","A=SELECT, X=NO SOUND, B=BACK");
  dsPrintValue(16-strlen(szName)/2,23,0,szName);
  for (ucBcl=0;ucBcl<17; ucBcl++) 
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
  bool bDone=false, bRet=false;
  u32 ucHaut=0x00, ucBas=0x00,ucSHaut=0x00, ucSBas=0x00,romSelected= 0, firstRomDisplay=0,nbRomPerPage, uNbRSPage, uLenFic=0;
  s32 ucFlip=0, ucFlop=0;
  char szName[64];

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

    if (keysCurrent() & (KEY_A | KEY_X)) 
    {
      if (!proromlist[ucFicAct].directory) 
      {
        if (keysCurrent() & KEY_X) is_mute = true; else is_mute=false;
        bRet=true;
        bDone=true;
      }
      else 
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
  int iTx,iTy;
  
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
      if ((iTx>79) && (iTx<180) && (iTy>10) && (iTy<65)) {     // 80,32 -> 179,61 cartridge slot
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

//---------------------------------------------------------------------------------
void dsInstallSoundEmuFIFO(void) 
{
    memset(sound_buffer, 0x00, SNDLENGTH);
    irqDisable(IRQ_TIMER2);  
    
    if (is_mute) 
    {
        return;    // We've been asked to not install sound... 
    }

    FifoMessage msg;
    msg.SoundPlay.data = &sound_buffer;
    msg.SoundPlay.freq = (cartridge_pokey ? SOUND_FREQ+21:(SOUND_FREQ*(isDS_LITE ? 1:2))+21);
    msg.SoundPlay.volume = 127;
    msg.SoundPlay.pan = 64;
    msg.SoundPlay.loop = 1;
    msg.SoundPlay.format = ((1)<<4) | SoundFormat_8Bit;
    msg.SoundPlay.loopPoint = 0;
    msg.SoundPlay.dataSize = SNDLENGTH >> 2;
    msg.type = EMUARM7_PLAY_SND;
    fifoSendDatamsg(FIFO_USER_01, sizeof(msg), (u8*)&msg);
    
    if (isDS_LITE)
    {
        snd_ptr = (u8*)((u32)sound_buffer + 0x00400000);
        snd_sta = (u8*)((u32)sound_buffer + 0x00400000);
        snd_end = (u8*)((u32)sound_buffer + 0x00400000 + SNDLENGTH);
    }
    else
    {
        snd_ptr = (u8*)((u32)sound_buffer + 0xA000000);
        snd_sta = (u8*)((u32)sound_buffer + 0xA000000);
        snd_end = (u8*)((u32)sound_buffer + 0xA000000 + SNDLENGTH);
    }
    
    TIMER2_DATA = TIMER_FREQ(SOUND_FREQ/2);
    TIMER2_CR = TIMER_DIV_1 | TIMER_IRQ_REQ | TIMER_ENABLE;	     
    if (cartridge_pokey)
        irqSet(IRQ_TIMER2, VsoundHandler_Pokey);  
    else
    {
        if (isDS_LITE)
            irqSet(IRQ_TIMER2, VsoundHandler_Lite);
        else
            irqSet(IRQ_TIMER2, VsoundHandler);
    }
    irqEnable(IRQ_TIMER2);  
}


void dsMainLoop(void) 
{
  static int lcd_swap_counter=0;
  static int special_hsc_entry=0;    
  static int last_keys_pressed = 999;
  char fpsbuf[32];
  unsigned int keys_pressed,keys_touch=0, romSel;
  int iTx,iTy;
  static int dampen = 0;
  static int scale_screen_dampen=0;
  
    // Timers are fed with 33.513982 MHz clock.
    // With DIV_1024 the clock is 32,728.5 ticks per sec...
    TIMER0_DATA=0;
    TIMER0_CR=TIMER_ENABLE|TIMER_DIV_1024;
    TIMER1_DATA=0;
    TIMER1_CR=TIMER_ENABLE | TIMER_DIV_1024;  
  
  while(etatEmu != A7800_QUITSTDS) {
    switch (etatEmu) {
      case A7800_MENUINIT:
        dsShowScreenMain();
        etatEmu = A7800_MENUSHOW;
        break;
        
      case A7800_MENUSHOW:
        etatEmu =  dsWaitOnMenu(A7800_MENUSHOW);
        break;
        
      case A7800_PLAYINIT:
        dsShowScreenEmu();
        etatEmu = A7800_PLAYGAME;
        break;
        
      case A7800_PLAYGAME:
        // 32,728.5 ticks = 1 second
        // 1 frame = 1/50 or 1/60 (0.02 or 0.016)
        // 655 -> 50 fps and 546 -> 60 fps
        if (!full_speed)
        {
            while(TIMER0_DATA < (546*atari_frames))
                ;
        }

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
        scanKeys();
        keys_pressed = keysCurrent();

        if (dampen == 0)
        {
          // if touch screen pressed
          if (keys_pressed & KEY_TOUCH) 
          {
              touchPosition touch;
              touchRead(&touch);
              iTx = touch.px;
              iTy = touch.py;
              if ((iTx>8) && (iTx<55) && (iTy>154) && (iTy<171))  { // 32,160  -> 64,168   POWER
                irqDisable(IRQ_TIMER2); fifoSendValue32(FIFO_USER_01,(1<<16) | (0) | SOUND_SET_VOLUME);
                soundPlaySample(clickNoQuit_wav, SoundFormat_16Bit, clickNoQuit_wav_size, 22050, 127, 64, false, 0);
                if (dsWaitOnQuit()) etatEmu=A7800_QUITSTDS;
                else { 
                    irqEnable(IRQ_TIMER2); 
                }
                fifoSendValue32(FIFO_USER_01,(1<<16) | (127) | SOUND_SET_VOLUME);
              }
              else if ((iTx>240) && (iTx<256) && (iTy>0) && (iTy<20))  { // Full Speed Toggle ... upper corner...
                 full_speed = 1-full_speed; 
                 if (full_speed) dsPrintValue(30,0,0,"FS"); else dsPrintValue(30,0,0,"  ");
                 dampen=60;
              }
              else if ((iTx>63) && (iTx<105) && (iTy>154) && (iTy<171))  { // 72,160  -> 105,168   PAUSE
                if (keys_touch == 0) soundPlaySample(clickNoQuit_wav, SoundFormat_16Bit, clickNoQuit_wav_size, 22050, 127, 64, false, 0);
                tchepres(10);
              }
              else if ((iTx>152) && (iTx<198) && (iTy>154) && (iTy<171))  { // 142,160  -> 175,168   SELECT
                if (keys_touch == 0) soundPlaySample(clickNoQuit_wav, SoundFormat_16Bit, clickNoQuit_wav_size, 22050, 127, 64, false, 0);
                tchepres(11);
              }
              else if ((iTx>208) && (iTx<251) && (iTy>154) && (iTy<171))  { // 191,160  -> 224,168   RESET
                if (keys_touch == 0) soundPlaySample(clickNoQuit_wav, SoundFormat_16Bit, clickNoQuit_wav_size, 22050, 127, 64, false, 0);
                tchepres(6);
              }
              else if ((iTx>90) && (iTx<110) && (iTy>90) && (iTy<110))  { // Atari Logo - Activate HSC Maintenence Mode (only on High Score screen)
                special_hsc_entry=70; 
              }
              else if ((iTx>115) && (iTx<144) && (iTy>154) && (iTy<171))  { // Snap HSC Sram
                dsPrintValue(13,0,0, "SAVING");
                soundPlaySample(clickNoQuit_wav, SoundFormat_16Bit, clickNoQuit_wav_size, 22050, 127, 64, false, 0);
                WAITVBL;WAITVBL;
                cartridge_SaveHighScoreSram();
                dsPrintValue(13,0,0, "      ");
                dampen=60;
                continue;
              }
              else if ((iTx>79) && (iTx<180) && (iTy>31) && (iTy<62)) {     // 80,32 -> 179,61 cartridge slot
                irqDisable(IRQ_TIMER2); fifoSendValue32(FIFO_USER_01,(1<<16) | (0) | SOUND_SET_VOLUME);
                // Find files in current directory and show it 
                proFindFiles();
                romSel=dsWaitForRom();
                if (romSel) { etatEmu=A7800_PLAYINIT; dsLoadGame(proromlist[ucFicAct].filename); if (full_speed) dsPrintValue(30,0,0,"FS"); else dsPrintValue(30,0,0,"  ");}
                else { 
                    irqEnable(IRQ_TIMER2); 
                }
                fifoSendValue32(FIFO_USER_01,(1<<16) | (127) | SOUND_SET_VOLUME);
              }
              
              keys_touch=1;
          } else keys_touch=0;
          
          if (keys_pressed != last_keys_pressed)
          {
              last_keys_pressed = keys_pressed;
              if ( (keys_pressed & KEY_SELECT) ) { tchepres(11); } // BUTTON SELECT
              if (cartridge_controller[0] != TWIN)
              {
                if ( (keys_pressed & KEY_START) ) {tchepres(10);} // BUTTON PAUSE
                if ( (keys_pressed & KEY_X) )  { fpsDisplay = 1-fpsDisplay; gTotalAtariFrames=0; if (!fpsDisplay) dsPrintValue(0,0,0,"   ");}
              }
              if (cartridge_controller[0] == SOTA)
              {
                  if ( (keys_pressed & KEY_R) )  { cartridge_xOffset +=28; bRefreshXY = true; }
                  if ( (keys_pressed & KEY_L) )  { cartridge_xOffset -=28; bRefreshXY = true; }  
              }
          }
          dampen = 6;
        } else dampen--;
        
        // manage a7800 pad 
        if ( (keys_pressed & KEY_UP) ) { tchepres(0); } // UP
        if ( (keys_pressed & KEY_DOWN) ) { tchepres(1); } // DOWN
        if ( (keys_pressed & KEY_RIGHT) ) { tchepres(3); } // RIGHT
        if ( (keys_pressed & KEY_LEFT) ) { tchepres(2); } // LEFT
            
        if (cartridge_controller[0] == TWIN)
        {
            if ( (keys_pressed & KEY_A) ) { tchepres(12); }  // Left Joystick Right
            if ( (keys_pressed & KEY_B) ) { tchepres(13); }  // Left Joystick Down
            if ( (keys_pressed & KEY_X) ) { tchepres(15); }  // Left Joystick Up
            if ( (keys_pressed & KEY_Y) ) { tchepres(14); }  // Left Joystick Left
            if ( (keys_pressed & KEY_START) ) { tchepres(4); }  // Fire Button (mainly to enter high scores and start game)            
        }
        else
        {
            if ( (keys_pressed & KEY_A) ) { tchepres(4); }  // BUTTON #1
            if ( (keys_pressed & KEY_B) ) { tchepres(5); }  // BUTTON #2
            if ( (keys_pressed & KEY_Y) ) { tchepres(4); }  // BUTTON #1
        }
            
        if ((keys_pressed & KEY_R) || (keys_pressed & KEY_L))
        {
          if ((keys_pressed & KEY_R) && (keys_pressed & KEY_L))
          {
              if (++lcd_swap_counter == 30)
              {
                  if (keys_pressed & KEY_A)   lcdSwap();
              }
          } 
          if (scale_screen_dampen > 5)
          {
              if ((keys_pressed & KEY_R) && (keys_pressed & KEY_UP))   { cartridge_yOffset++; bRefreshXY = true; }
              if ((keys_pressed & KEY_R) && (keys_pressed & KEY_DOWN)) { cartridge_yOffset--; bRefreshXY = true; }
              if ((keys_pressed & KEY_R) && (keys_pressed & KEY_LEFT))  { cartridge_xOffset++; bRefreshXY = true; }
              if ((keys_pressed & KEY_R) && (keys_pressed & KEY_RIGHT)) { cartridge_xOffset--; bRefreshXY = true; }

              if ((keys_pressed & KEY_L) && (keys_pressed & KEY_UP))   if (cartridge_yScale <= 256) { cartridge_yScale++; bRefreshXY = true; }
              if ((keys_pressed & KEY_L) && (keys_pressed & KEY_DOWN)) if (cartridge_yScale > 192) { cartridge_yScale--; bRefreshXY = true; }
              if ((keys_pressed & KEY_L) && (keys_pressed & KEY_RIGHT))  if (cartridge_xScale < 320) { cartridge_xScale++; bRefreshXY = true; }
              if ((keys_pressed & KEY_L) && (keys_pressed & KEY_LEFT)) if (cartridge_xScale > 192)  { cartridge_xScale--; bRefreshXY = true; }                  
              scale_screen_dampen=0;
          } else scale_screen_dampen++;
        }  else lcd_swap_counter=0;

        // -------------------------------------------------------------
        // Stuff to do once/second such as FPS display and Debug Data
        // -------------------------------------------------------------
        if (TIMER1_DATA >= 32728)   // 1000MS (1 sec)
        {
            TIMER1_CR = 0;
            TIMER1_DATA = 0;
            TIMER1_CR=TIMER_ENABLE | TIMER_DIV_1024;
            if (fpsDisplay)
            {
                int fps = gTotalAtariFrames;
                if (fps == 61) fps=60;
                gTotalAtariFrames = 0;
                fpsbuf[0] = '0' + (int)fps/100;
                fps = fps % 100;
                fpsbuf[1] = '0' + (int)fps/10;
                fpsbuf[2] = '0' + (int)fps%10;
                fpsbuf[3] = 0;
                dsPrintValue(0,0,0, fpsbuf);
            }
            DumpDebugData();
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

void proFindFiles(void) {
  DIR *pdir;
  struct dirent *pent;
  char filenametmp[255];
  
  countpro = countfiles = 0;
  
  pdir = opendir(".");

  if (pdir) 
  {
    while (((pent=readdir(pdir))!=NULL)) 
    {
      strcpy(filenametmp,pent->d_name);
      if (pent->d_type == DT_DIR)
      {
        if (!( (filenametmp[0] == '.') && (strlen(filenametmp) == 1))) {
          proromlist[countpro].directory = true;
          strcpy(proromlist[countpro].filename,filenametmp);
          countpro++;
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
