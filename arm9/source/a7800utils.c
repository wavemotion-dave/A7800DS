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

FICA7800 proromlist[1024];  
unsigned int countpro=0, countfiles=0, ucFicAct=0;

int bg0;
int bg1;
int bg0b,bg1b;
int bg2;
int bg3;             // BG pointers 
int bg0s, bg1s, bg2s, bg3s;         // sub BG pointers 

int full_speed = 0;
int etatEmu;
int gTotalAtariFrames=0;
int fpsDisplay=0;

#define MAX_DEBUG 10 
int debug[MAX_DEBUG]={0};
//#define DEBUG_DUMP
#define CART_INFO

#ifndef DS_LITE    
#define SOUND_FREQ  22050
#else
#define SOUND_FREQ  11025
#endif

// Difficulty switches... 
#define DIFF_A      0
#define DIFF_B      1

uint video_height;                       // Actual video height
u16 *bufVideo;                           // Video flipping buffer
unsigned int gameCRC;                    // crc checksum of file  
gamecfg GameConf;                        // Game Config svg

short cxBG, cyBG, xdxBG,ydyBG;

word atari_pal16[256] = {0};
unsigned char keyboard_data[20];

unsigned char *filebuffer;

bool bRefreshXY = false;

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

#define DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);
// Table of CRC-32's of all single-byte values (made by make_crc_table)
unsigned int crc_table[256] = {
  0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
  0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
  0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
  0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
  0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
  0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
  0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
  0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
  0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
  0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
  0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
  0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
  0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
  0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
  0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
  0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
  0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
  0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
  0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
  0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
  0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
  0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
  0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
  0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
  0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
  0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
  0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
  0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
  0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
  0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
  0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
  0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
  0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
  0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
  0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
  0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
  0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
  0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
  0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
  0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
  0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
  0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
  0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
  0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
  0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
  0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
  0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
  0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
  0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
  0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
  0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
  0x2d02ef8dL
};

// cal crc32 of a file
unsigned long crc32 (unsigned int crc, const unsigned char *buf, unsigned int len) {
  if (buf == 0) return 0L;
  crc = crc ^ 0xffffffffL;
  while (len >= 8) {
    DO8(buf);
    len -= 8;
  }
  if (len) do {
    DO1(buf);
  } while (--len);
  return crc ^ 0xffffffffL;
}

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
    REG_BG3X = cxBG; 
    REG_BG3Y = cyBG; 
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
    //  Init vbl and hbl func
    //SetYtrigger(192); //trigger 2 lines before vsync
    irqSet(IRQ_VBLANK, vblankIntr);
    irqEnable(IRQ_VBLANK);
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

static word targetIndex = 0;
void VsoundHandler(void) 
{
  static u16 sound_idx=0;
  sound_buffer[sound_idx] = tia_buffer[targetIndex]+128;
  sound_idx = (sound_idx + 1) & 0x0FFF;
  targetIndex=(targetIndex + 1) % 524;  
}

void VsoundHandler_Pokey(void)
 {
  static u16 sound_idx=0;
  sound_buffer[sound_idx] = ((tia_buffer[targetIndex]+pokey_buffer[targetIndex])/2)+128;
  sound_idx = (sound_idx + 1) & 0x0FFF;
  targetIndex=(targetIndex + 1) % 524;  
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

#ifdef CART_INFO
    char cart_info_buf[64];
    dsPrintValue(0,22,0, (char*)cartridge_digest);
    snprintf(cart_info_buf, 63, "CT=%d PK=%d RE=%d SY=%d ST=%d HS=%d ", cartridge_type, cartridge_pokey, cartridge_region, cartridge_uses_wsync, cartridge_steals_cycles, cartridge_hsc_enabled);
    cart_info_buf[32] = 0;
    dsPrintValue(0,21,0,cart_info_buf);
#endif          
      
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
    keyboard_data[15] = DIFF_A;
    keyboard_data[16] = DIFF_A;
    GameConf.DS_Pad[ 0] = 3;   GameConf.DS_Pad[ 1] = 2;
    GameConf.DS_Pad[ 2] = 1;   GameConf.DS_Pad[ 3] = 0;
    GameConf.DS_Pad[ 4] = 4;   GameConf.DS_Pad[ 5] = 5;
    GameConf.DS_Pad[ 6] = 12;  GameConf.DS_Pad[ 7] = 12;
    GameConf.DS_Pad[ 8] = 15;  GameConf.DS_Pad[ 9] = 16;
    GameConf.DS_Pad[10] = 14;  GameConf.DS_Pad[11] = 13;

    TIMER2_DATA = TIMER_FREQ(SOUND_FREQ);                        
    TIMER2_CR = TIMER_DIV_1 | TIMER_IRQ_REQ | TIMER_ENABLE;	     
    if (cartridge_pokey)
        irqSet(IRQ_TIMER2, VsoundHandler_Pokey);  
    else
        irqSet(IRQ_TIMER2, VsoundHandler);
    irqEnable(IRQ_TIMER2);  
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
  sprintf(szName,"%s","A TO SELECT A GAME, B TO GO BACK");
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
  u32 ucHaut=0x00, ucBas=0x00,ucSHaut=0x00, ucSBas=0x00,romSelected= 0, firstRomDisplay=0,nbRomPerPage, uNbRSPage, uLenFic=0,ucFlip=0, ucFlop=0;
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

    if (keysCurrent() & KEY_A) {
      if (!proromlist[ucFicAct].directory) {
        bRet=true;
        bDone=true;
      }
      else {
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
      if (ucFlip >= 8) {
        ucFlip = 0;
        uLenFic++;
        if ((uLenFic+29)>strlen(proromlist[ucFicAct].filename)) {
          ucFlop++;
          if (ucFlop >= 8) {
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
    FifoMessage msg;
    msg.SoundPlay.data = &sound_buffer;
    msg.SoundPlay.freq = SOUND_FREQ;
    msg.SoundPlay.volume = 127;
    msg.SoundPlay.pan = 64;
    msg.SoundPlay.loop = 1;
    msg.SoundPlay.format = ((1)<<4) | SoundFormat_8Bit;
    msg.SoundPlay.loopPoint = 0;
    msg.SoundPlay.dataSize = SNDLENGTH >> 2;
    msg.type = EMUARM7_PLAY_SND;
    fifoSendDatamsg(FIFO_USER_01, sizeof(msg), (u8*)&msg);
}

void dsMainLoop(void) 
{
  static int special_hsc_entry=0;    
  static int last_keys_pressed = 999;
  char fpsbuf[32];
  unsigned int keys_pressed,keys_touch=0, romSel;
  int iTx,iTy;
  static int dampen = 0;
  
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
            while(TIMER0_DATA < 546)
                ;
        }
        TIMER0_CR=0;
        TIMER0_DATA=0;
        TIMER0_CR=TIMER_ENABLE|TIMER_DIV_1024;

        // Execute one frame
        prosystem_ExecuteFrame(keyboard_data);

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
            if (!keys_touch) 
            {
              touchPosition touch;
              keys_touch=1;
              touchRead(&touch);
              iTx = touch.px;
              iTy = touch.py;
              debug[0] = iTx; debug[1]=iTy;
              if ((iTx>31) && (iTx<65) && (iTy>159) && (iTy<169))  { // 32,160  -> 64,168   quit
                irqDisable(IRQ_TIMER2); fifoSendValue32(FIFO_USER_01,(1<<16) | (0) | SOUND_SET_VOLUME);
                soundPlaySample(clickNoQuit_wav, SoundFormat_16Bit, clickNoQuit_wav_size, 22050, 127, 64, false, 0);
                if (dsWaitOnQuit()) etatEmu=A7800_QUITSTDS;
                else { 
                    irqEnable(IRQ_TIMER2); 
                }
                fifoSendValue32(FIFO_USER_01,(1<<16) | (127) | SOUND_SET_VOLUME);
              }
              else if ((iTx>71) && (iTx<106) && (iTy>159) && (iTy<169))  { // 72,160  -> 105,168   pause
                soundPlaySample(clickNoQuit_wav, SoundFormat_16Bit, clickNoQuit_wav_size, 22050, 127, 64, false, 0);
                tchepres(10);
              }
              else if ((iTx>190) && (iTx<225) && (iTy>159) && (iTy<169))  { // 191,160  -> 224,168   reset
                soundPlaySample(clickNoQuit_wav, SoundFormat_16Bit, clickNoQuit_wav_size, 22050, 127, 64, false, 0);
                tchepres(6);
              }
              else if ((iTx>141) && (iTx<176) && (iTy>159) && (iTy<169))  { // 142,160  -> 175,168   select
                soundPlaySample(clickNoQuit_wav, SoundFormat_16Bit, clickNoQuit_wav_size, 22050, 127, 64, false, 0);
                tchepres(11);
              }
              else if ((iTx>90) && (iTx<110) && (iTy>90) && (iTy<110))  { // Atari Logo - Activate HSC Maintenence Mode (only on High Score screen)
                special_hsc_entry=70; 
              }
              else if ((iTx>115) && (iTx<138) && (iTy>159) && (iTy<169))  { // Snap HSC Sram
                dsPrintValue(13,0,0, "SAVING");
                soundPlaySample(clickNoQuit_wav, SoundFormat_16Bit, clickNoQuit_wav_size, 22050, 127, 64, false, 0);
                WAITVBL
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
                if (romSel) { etatEmu=A7800_PLAYINIT; dsLoadGame(proromlist[ucFicAct].filename); }
                else { 
                    irqEnable(IRQ_TIMER2); 
                }
                fifoSendValue32(FIFO_USER_01,(1<<16) | (127) | SOUND_SET_VOLUME);
              }
            } else keys_touch=0;
          }
          
          if (keys_pressed != last_keys_pressed)
          {
              last_keys_pressed = keys_pressed;
              if ( (keys_pressed & KEY_START) ) {tchepres(10);} // BUTTON PAUSE
              if ( (keys_pressed & KEY_SELECT) ) { tchepres(11); } // BUTTON SELECT
              if ( (keys_pressed & KEY_X) )  { fpsDisplay = 1-fpsDisplay; gTotalAtariFrames=0; if (!fpsDisplay) dsPrintValue(0,0,0,"   ");}
              if ( (keys_pressed & KEY_Y) )  { full_speed = 1-full_speed; if (full_speed) dsPrintValue(28,0,0,"FS"); else dsPrintValue(28,0,0,"  ");}  
              if ( (keys_pressed & KEY_R) )  { cartridge_yOffset++; bRefreshXY = true; }
              if ( (keys_pressed & KEY_L) )  { cartridge_yOffset--; bRefreshXY = true; }  
//              if ( (keys_pressed & KEY_R) )  { keyboard_data[15] = (keyboard_data[15] == DIFF_A ? DIFF_B:DIFF_A); dsPrintDifficultySwitches();}
//              if ( (keys_pressed & KEY_L) )  { keyboard_data[16] = (keyboard_data[16] == DIFF_A ? DIFF_B:DIFF_A); dsPrintDifficultySwitches();}
          }
          dampen = 6;
        } else dampen--;
        
        // else manage a7800 pad 
        if ( (keys_pressed & KEY_UP) ) { tchepres(0); } // UP
        if ( (keys_pressed & KEY_DOWN) ) { tchepres(1); } // DOWN
        if ( (keys_pressed & KEY_RIGHT) ) { tchepres(3); } // RIGHT
        if ( (keys_pressed & KEY_LEFT) ) { tchepres(2); } // LEFT

        if ( (keys_pressed & KEY_A) ) { tchepres(4); }  // BUTTON #1
        if ( (keys_pressed & KEY_B) ) { tchepres(5); }  // BUTTON #2

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
  
  return strcmp (p1->filename, p2->filename);
}

void proFindFiles(void) {
	struct stat statbuf;
  DIR *pdir;
  struct dirent *pent;
  char filenametmp[255];
  
  countpro = countfiles = 0;
  
  pdir = opendir(".");

  if (pdir) 
  {

    while (((pent=readdir(pdir))!=NULL)) {
      stat(pent->d_name,&statbuf);

      strcpy(filenametmp,pent->d_name);
      if(S_ISDIR(statbuf.st_mode)) {
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
    qsort (proromlist, countpro, sizeof (FICA7800), a78Filescmp);
}
