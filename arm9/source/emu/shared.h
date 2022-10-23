#ifndef SHARED_H
#define SHARED_H

#include <nds.h>

//typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int uint;

extern unsigned short *bufVideo;   // Video buffer

extern char gameName[256];
extern unsigned int gameCRC;
extern uint video_height;
extern word atari_pal16[256];

#endif
