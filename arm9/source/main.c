#include <stdio.h>
#include <fat.h>
#include <nds.h>

#include "main.h"
#include "intro.h"
#include "a7800utils.h"

// Program entry point
int main(int argc, char **argv) 
{
  // Init sound
  consoleDemoInit();
  soundEnable();
  lcdMainOnTop();

  // Init Fat
	if (!fatInitDefault()) 
    {
		iprintf("Unable to initialize libfat!\n");
		return -1;
	}

  // Init Timer
  dsInitTimer();
  dsInstallSoundEmuFIFO();
  
  // Intro and main screen
  intro_logo();  
  dsInitScreenMain();
  etatEmu = A7800_MENUINIT;
  
    //load rom file via args if a rom path is supplied
  if(argc > 1) 
  {
    dsShowScreenMain();
    dsLoadGame(argv[1]);
    etatEmu = A7800_PLAYINIT;
  }
  
  // Main loop of emulation
  dsMainLoop();
  
  // Free memory to be correct 
  dsFreeEmu();
  
  return(0);
}
