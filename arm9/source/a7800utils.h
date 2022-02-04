#ifndef _A7800UTILS_H
#define _A7800UTILS_H

#define A7800_MENUINIT 0x01
#define A7800_MENUSHOW 0x02
#define A7800_PLAYINIT 0x03 
#define A7800_PLAYGAME 0x04 
#define A7800_QUITSTDS 0x05

extern short int etatEmu;

typedef enum {
  EMUARM7_INIT_SND = 0x123C,
  EMUARM7_STOP_SND = 0x123D,
  EMUARM7_PLAY_SND = 0x123E,
} FifoMesType;

typedef struct FICtoLoad {
  char filename[255];
  bool directory;
  unsigned int uCrc;
} FICA7800;

typedef struct {
  unsigned int sndLevel;
  unsigned int m_ScreenRatio; // 0 = original show, 1 = full screen
  unsigned short DS_Pad[16]; // each key mapping
  unsigned int m_DisplayFPS;
} gamecfg;

#define ds_GetTicks() (TIMER0_DATA)

extern gamecfg GameConf;

extern uint video_height;                  // Actual video height
extern unsigned short *bufVideo;   // Video buffer


extern void FadeToColor(unsigned char ucSens, unsigned short ucBG, unsigned char ucScr, unsigned char valEnd, unsigned char uWait);

extern unsigned long crc32 (unsigned int crc, const unsigned char *buf, unsigned int len);

extern void vblankIntr();

extern void dsInitScreenMain(void);
extern void dsInitTimer(void);
extern void dsShowScreenEmu(void);
extern void dsShowScreenMain(void);
extern void dsFreeEmu(void);
extern void VsoundHandler(void);
extern void dsLoadGame(char *filename);
extern unsigned int dsReadPad(void);
extern bool dsWaitOnQuit(void);
extern void dsDisplayFiles(unsigned int NoDebGame,u32 ucSel);
extern unsigned int dsWaitForRom(void);
extern unsigned int dsWaitOnMenu(unsigned int actState);
extern void dsPrintValue(int x, int y, unsigned int isSelect, char *pchStr);
extern void dsInstallSoundEmuFIFO(void);
extern void dsMainLoop(void);
extern int a78Filescmp (const void *c1, const void *c2);
extern void proFindFiles(void);

#endif
