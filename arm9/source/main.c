#include <stdio.h>
#include <fat.h>
#include <nds.h>
#include <maxmod9.h>

#include "main.h"
#include "intro.h"
#include "config.h"
#include "a7800utils.h"
#include "soundbank.h"

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
    
  LoadConfig();
  
    //load rom file via args if a rom path is supplied
  if(argc > 1) 
  {
    dsShowScreenMain(true);
    dsLoadGame(argv[1]);
    etatEmu = A7800_PLAYINIT;
  }
  else
  {
      chdir("/roms");    // Try to start in roms area... doesn't matter if it fails
      chdir("a7800");    // And try to start in the subdir /a7800... doesn't matter if it fails.
      chdir("a78");      // And try to start in the subdir /a78... doesn't matter if it fails.
  }
  
  // Main loop of emulation
  dsMainLoop();
  
  // Free memory to be correct 
  dsFreeEmu();
  
  return(0);
}
