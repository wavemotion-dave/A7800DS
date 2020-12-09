#ifndef SHARED_H
#define SHARED_H

#include <nds.h>

//#define SYSVID_WIDTH  320
//#define SYSVID_HEIGHT 223

//typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int uint;

extern unsigned short *bufVideo;   // Video buffer

extern char gameName[256];
extern unsigned int gameCRC;
extern uint video_height;
extern word atari_pal16[256];

extern void system_loadcfg(char *cfg_name);
extern void system_savecfg(char *cfg_name);

#endif
