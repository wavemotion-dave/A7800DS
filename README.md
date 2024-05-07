# a7800DS

a7800DS is an Atari ProSystem 7800 console emulator for the DS/DSi.

To use this emulator, you must use compatible roms with a78/bin format. 
Strongly recommend you use NTSC roms... PAL ones have more scanlines and will render
more slowly and since the sound core is tied to scanlines, the sound will be wrong. 
All the debug on this port to the DS has been done with NTSC roms - seek them out!
Do not ask me about such files, I only supply the emulator. A search with Google will certainly 
help you. The emulator was developed using Trebor's "7800 ProPack" of well-curated games.
The emulator will auto-start in /roms/a7800 or /roms/a78 if those directories exist.

Features :
----------
 Most things you should expect from an emulator. Speed is excellent on the DSi and 
 a little less great on the older DS-LITE. For the DS-LITE you can expect full speed on
 a very large chunk of the 7800 library. The more traditional games (think: Asteroids,
 Astro Blaster, Robotron, Food Fight, Centipede, Pac-Man Collection, etc) will 
 all run great. The really big bankswitched games may struggle on the older 
 DS-LITE/PHAT hardware - try the game and see!
 
 The emulator will support ROMs up to 1024K (1MB!) in size + the 128 byte .a78 header.
 All popular bank-switching schemes are supported including an extra 16K of RAM at 4000h.
 Pokey support at 4000h, 800h and 450h - change this in Configuration if it's not auto-detected.
 
 The new Banksets scheme is fully supported - this new banking/memory handling is designed
 for homebrew authors to provide increased ROM density and improved packing and access of
 graphics data vs code.  This allows for games that would have been difficult or impossible 
 without the scheme. See http://7800.8bitdev.org/index.php/Bankset_Bankswitching for more details.
  
 Add highscore.rom for 7800 High Score saving. This can be in /roms/bios, /data/bios
 or in the same directory as the emulator.  It's worth the effort to track down the highscore.rom file!
 
Copyright :
----------
A7800DS is Copyright 2021-2024 by Dave Bernazzani (wavemotion-dave).

This emulator is based heavily upon ProSystem and that emulator was released
in 2005 by Greg Stanton under the GNU General Public License and, as such,
this derived work is released in the same manner. The original license text
that Greg used is included here:

```
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
```


Philosophy :
----------
For this particular emulator, we use ProSystem as the base. That's always been a bit
problematic in terms of accuracy - often resulting in small graphical glitches and
other artifacts on screen. However, in the past year I've worked out many of those
issues and most of the games look, sound and play great now. However, do not expect
perfect emulation - if you're looking for a highly accurate emulator for the 7800
ProSystem, this isn't it - try MAME/MESS or A7800. But if you're looking to enjoy 
some classic 7800 console goodness on your DS/DSi then you've come to the right place!

Known Issues and Limitations:
----------
- Lightgun is not supported.
- Paddles are not supported.
- Games greater than 1MB (1024K + 128b header) are not supported.
- Only one Pokey is supported at 4000h, 800h or 450h (no Dual Pokey)
- XM is not supported (beyond HSC and Pokey)
   
--------------------------------------------------------------------------------
How to use a7800DS :
--------------------------------------------------------------------------------
Place .NDS on your SD card and launch with Twilight Menu++ or Unlaunch.
If you want to run on a flash cart place it as you would any homebrew pretty
much anywhere on your flashcart SD card.

When the emulator starts, click on the cartridge slot to choose a file. Use Up/Down 
to select a file, then use A to load it.

Controls :
 * Direction pad : the joystick ...
 * A             : Fire button 1
 * B             : Fire button 2
 * SELECT        : SELECT Button
 * START         : PAUSE Button
 * X             : Fire button 2
 * Y             : Fire Button 1
 * L/R + DPAD    : Used to Shift Offsets and Scale Screen to desired ratio
 * L + R + X     : Hold for 1 second to swap LCD screens top/bottom
  
 Use stylus on buttons for other actions on bottom screen.  
 
 The new SNES2Atari adaptor is supported as a controller type for the few new games
 that utilize it. This allows all of the DS buttons to map into the game - exactly
 as labeled (D-Pad plus ABXY, Left Shoulder, Right Shoulder and Start, Select).
 
 High Score Saving works if you have highscore.rom (exact name and case) in your
 roms directory where you load your games... also, you MUST press the HSC button
 if you want to snap the Saved Scores out to the flash card. It's not something 
 I want to do as the game runs... so you must do it... the high scores will also
 auto-save if you quit the emulator or select a new game.
  
Configuration :
----------
Generally you would use this to select a bankswitching scheme if the proper type wasn't auto-detected. 
The following schemes are supported:

* NORMAL              Anything 48K or less... fits into memory (0xffff downwards) without switching.
* SUPERCART           Games that are 128+K in size with nothing mapped in at 0x4000
* SUPERCART_LARGE     Games that are 144+K in size with the extra 16K bank 0 fixed at 0x4000
* SUPERCART_RAM       Games that are 128+K in size with extra 16K of RAM at 0x4000
* SUPERCART_ROM       Games that are 128+K in size with bank 6 fixed at 0x4000
* SUPERCART_RAMX2     Games that are 128+K in size with extra 32K of RAM at 0x4000 (bankswitched in 16K chunks)
* FLAT WITH RAM       Games that are 16K or 32K in size and utilize 16K of RAM at 0x4000
* BANKSETS            Games that use the new Banksets handling (2x32, 2x48, 2x52, 2x128, 2x256 and 2x512 supported).
* BANKSETS RAM        Games that use the new Banksets handling with 16K of RAM at 0x4000 (same RAM for Maria and Sally).
* BANKSETS HALT RAM   Games that use the new Banksets handling with 16K of RAM at 0x4000 (Maria and Sally see different 16K RAM).
* ACTIVISION          Mostly for Double Dragon and Rampage by Activision
* ABSOLETE            Only for the F-18 Hornet game by Absolete Entertainment
* FRACTALUS           Only for the Rescue on Fractalus prototype (not a complete game but neat to see). This is EXRAM/A8.

Frame Skipping can be OFF (show all frames), Moderate (Show 3/4 frames) or Agressive (only show 1/2 frames).  The latter is
only really needed for the DS-Lite/Phat where the faster DSi CPU isn't available.

Don't touch the DMA Cycle Adjustment unless you understand them... and most people don't - sometimes including the developer :)

Press START to save off your configuration - if you made changes you should re-load the game to ensure all settings are applied.

Of Mice, Men and Screen Resolutions :
----------
The DS/DSi has a native screen resolution of 256x192. This is not ideal for the Atari 7800 where many of the games are 320 
pixels across and often more than 192 scanlines. The original Atari NTSC spec called for 192 vertical scanlines and a few
of the early games (Asteroids, Ms. Pac-Man, etc.) did stick to that but most later games utilize more vertical scanlines. 
A typical NTSC TV can handle 230+ scanlines fairly well and so many of the Atari 7800 games utilize some number of extra
scanlines to pack as much awesome gameplay as possible onto the screen.

This is a problem for our hero, the DS/DSi. Fortunately the DS has the ability to scale/stretch as needed. But when doing
so, there will be missing scanlines. For example, if the game utilizes 200 scalines but the DS can only show 192, there are 
8 scanlines that must go missing... if scaled down to 192 pixels to fit the screen, these extra scalines might be in the
middle of the playfield which is not great. If you scale the screen up (using Configuration options or the L/R shoulder 
buttons in conjunction with the D-PAD to shift/scale the sceren), then some of the pixels (left or right, up or down) will
be cropped off the screen. This might not be a big deal - sometimes the very top and bottom of a game are just clouds or 
ground that can be safely cropped without any loss in gameplay. For many games, the top 16 pixels are the score / lives
remaining.

Most games have scaling defaults that look good enough. Some games will cut off a few pixel lines at the top and bottom - but
they will still be perfectly playalbe.  However there are some thigns you can (and should!) do to help.

Recent versions of the emulator have a magnifying glass icon that will zoom and center the display to 1:1 of the actual 7800 output.
This will crop some pixels off the sides and top/bottom but is useful to temporarily zoom up to enter things like high scores
where the text may be hard to read when shrunk down.

More importantly, you can utilize the X and Y buttons as a pan down/up. This is massively useful for games that have a score at
the top of the display (Galaxian, Space Invaders, Popeye, bonQ, etc).  Here you can stretch up the screen in the Y direction so
that it is nearly 1:1 and shift the score off the top of the screen. Then, during gameplay, you can tap the X button to temporarily
pan the display down so the score comes into view briefly... and it will magically shift back up after a half second. You can do
this while you are playing - usually when you lose a life you can tap the X button to quickly glace at your score. This gives 
more useable scanlines for actual gameplay. Think of this like you're at the arcade and you have to glance up to see your 
score when focused on the field of play. It takes a little getting used to but this mechanism really helps map the more complicated
game graphics onto the small sceren. Of course youc an always scale the screen down to it's totally visible - but there will be
some loss of scanline information. Experiment and determine what works best for you.

And remember - once you get your screen settings the way you want, be sure to go into the GEAR icon and hit START to save out
your current configuration (which includes your screen offset/scaling tweaks on a per-game basis).

--------------------------------------------------------------------------------
Credits:
--------------------------------------------------------------------------------
 * Thanks Wintermute for devkitpro and libnds (http://www.devkitpro.org).
 * Greg Stanton for ProSystem source code (https://home.comcast.net/~gscottstanton/) an Atari 7800 emulator.
 * zx81 (http://zx81.zx81.free.fr/serendipity_fr/) for PSP A7800 version (that helped to understand ProSystem).
 * raz0red (http://www.twitchasylum.com/forum/viewtopic.php?t=519) for WII7800  (that helped me to fix some timing problems).
 * The folks at AtariAge who helped weed out many of the old ProSystem Maria rendering and timing issues.
 * The MAXMOD audio library is Copyright (c) 2008, Mukunda Johnson (mukunda@maxmod.org). See https://github.com/devkitPro/maxmod

--------------------------------------------------------------------------------
Original Author:
Alekmaul
alekmaul@portabledev.com
http://www.portabledev.com

Updates by wavemotion-dave:  https://github.com/wavemotion-dave/A7800DS
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
History :
--------------------------------------------------------------------------------
V4.6 : 06-May-2024 by wavemotion-dave
  * Each game that utilizes a High Score Cart (HSC) gets its own 2K .hsc file 
  * Improved High Score Cart (HSC) emulation - improved initialization of the SRAM contents.
  * Sanity checks added so that carts marked as 'NORMAL' (or selected as such) but are larger than 48K will not corrupt memory.
  * New magnifying glass icon to ZOOM (and center) the display 1:1 with real 7800 output. This will crop on a DS/DSi since it only has 256x192 but very useful to toggle the 1:1 zoom for High Score Entries, etc.
  * Support for .a78 V4 headers (will fall back to V3 if not available)

V4.5 : 18-Nov-2022 by wavemotion-dave
  * SNES2Atari adaptor supported.
  * Improved display output to smooth over the fonts a bit.
  * Small tweaks to internal database to ensure everything looks as good as possible.

V4.4 : 14-Nov-2022 by wavemotion-dave
  * New palette options from the Trebor 'Pro Pack' of colors. COOL, WARM (default), and HOT allow you to shift the color temperature slightly on a per-game basis.
  * Improved sound channel mixing so as not to halve the volume when mixing POKEY + TIA.

V4.3 : 11-Nov-2022 by wavemotion-dave
  * Full support for the new Banksets scheme including the upcoming Attack of the Petscii Robots game!
  * Added the stable "illegal" opcode support for the 6502 CPU to ensure all games run properly.
  * New Maria cycle handling is more accurate than it's been in any previous version. More games run closer to perfect.
  * Improved rendering and a bit more optmization to make almost every game playable on the older DS-Lite. 

V4.2 : 06-Nov-2022 by wavemotion-dave
  * Added support for bankswitched RAM (32K of RAM swiched in 16K chunks). This makes the Ex version of 1942 playable.
  * Added alternate way of handling bankswitched RAM the same way as the DragonFly cart or SN board: via writes to 0xFFFF.
  * Fix graphical glitch for Ballblazer (just hiding it off screen).
  * Numerous small updates to the internal cart database to ensure game run with the proper settings (mostly High Scores)
  * New 8x density on the High Score Cart - transparent to the user but provides more slots so the HSC won't fill up.
  * Flat 32K plus RAM cart type supported.
  * Pokey @800 supported for upcoming homebrews.

V4.1 : 02-Nov-2022 by wavemotion-dave
  * Overhaul Maria cycle stealing - all games now use the proper cycle stealing with much closer to accurate timing. This fixes games like One-on-One and Kung Fu Master.
  * Other minor cleanups and tweaks as time permitted.

V4.0 : 01-Nov-2022 by wavemotion-dave
  * Overhaul the audio system to use the MAXMOD streaming library to eliminate sound 'zingers'
  * Other minor cleanup and tweaks as time permitted.

V3.9 : 29-Oct-2022 by wavemotion-dave
  * More optmization and more games playable on the DS-Lite.
  * Highscore A7800.SRAM file moved to /data (move yours manually) - this allows the same high score file even if your roms are in different directories.
  * Fix for One-on-One.
  * Fix for voices in Jinx.
  * Cleanup of code - removed unused functions and vars. Switched to a memory-lite sprintf().

V3.8 : 26-Oct-2022 by wavemotion-dave
  * Massive optmization of the Maria rendering to help the DS-Lite run more games. 
  * Fix for Rampage
  * Fix for Rescue on Fractalus
  * Fix for F-18 Hornet
  * Fix for Double Dragon
  * Fix for Basketbrawl
  * Better A78 header parsing for improved game detection
  * New Moderate Frameskip to show 3/4 frames (not as aggressive as the old 1/2 frameskip)
  * Support for 1024K (+128 byte header) games. Only a few tech demos available so far.
  * Removed most of the old hacks for DS-Lite as the speed is good enough to render those games properly.

V3.7 : 23-Oct-2022 by wavemotion-dave
  * Optmization across the board for faster and smoother performance.
  * Gained enough speed that we have restored full HQ sound for the DS-Lite.
  * bonQ fixed graphical glitches.
  * Keystone Koppers fixed graphical glitches.
  * Latest homebrews added to the internal database.
  * New configuration menu so you can tweak settings - new homebrews won't need a new A7800DS.
  * Better A78 v3 header support so more games run right.

V3.6 : 04-Jun-2022 by wavemotion-dave
  * Fix for XM detection so the newest Pac-Man Collection (PMC-XM) works!
  * Minor database cleanups so all the more recent games run.
  
V3.5 : 12-Feb-2022 by wavemotion-dave
  * Across the board cleanup of code. Copyright notice added.
  * Fixed High-Score save on Time Salvo
  * Slight memory optmization for reduced memory footprint.
  
V3.4 : 6-Feb-2022 by wavemotion-dave
  * Frameskip rendering improved by more than 10% - making more DS-LITE games playable!
  * Minor improvements to touch-screen key handling and other code cleanups.
  
V3.3 : 4-Feb-2022 by wavemotion-dave
  * Squeezed out another frame of performance.
  * Super Skateboardin' graphics fixed.
  * DSi eliminates frameskip on 80% of the library.
  * Updated 50 entries in the internal database - more new homebrews run.
  
V3.2 : 1-Feb-2022 by wavemotion-dave
  * A 5-8% speedup across the board through a number of small optimizations in core areas.
  
V3.1 : 31-Jan-2022 by wavemotion-dave
  * Now using more VRAM for bank swapping and partial DMA transfer to speed up large games.
  * Reverted part of the sound core to improve sound quality.
  * File selection cleanups to scroll less fast and be generally easier to see.
  
V3.0 : 06-Nov-2021 by wavemotion-dave
  * Reworked sound output core so it's now zinger-free!
  * Refresh of bottom screen - improved font and button debounce.
  
V2.9 : 03-Nov-2021 by wavemotion-dave
  * Fixed loading of large SUPER CART roms.

V2.8 : 02-Nov-2021 by wavemotion-dave
  * A bit of speed - enough to eliminate the old DS-LITE version. 
  * A few new homebrews added to the internal database. 
  * Optmized sound buffers for (very) slightly better performance.
  * highscore.rom can now bin in /roms/bios or /data/bios 
  * Cleanup code as time permitted.
  
V2.7 : 02-Apr-2021 by wavemotion-dave
  * New support for the latest homebrews: Galaxian final
  * Added ability to swap screens using L+R+A

V2.6 : 02-Apr-2021 by wavemotion-dave
  * Faster directory/file listing.
  * Support for the latest homebrews: Galaxian and Popeye
  * Improved scaling and offset handling using L/R + Arrow Keys to match other emulators.

V2.5 : 15-Jan-2021 by wavemotion-dave
  * Added .A78 header naming search and as a backup filename search to help
    properly identify the game being loaded so that the right settings can be applied.

V2.4 : 8-Jan-2021 by wavemotion-dave
  * Improvement in bank switching allowing most games to run 60FPS.
  
V2.3 : 6-Jan-2021 by wavemotion-dave
  * Fixed large cart support so 512kb games run fine. 
  * Fixed voice in Frenzy/Berzerk.
  * Fixed graphical glitches in Alien Brigade.
  * Optmized bank switching so more big games run smoothly.
  * Added default difficulty switches for the few games that need them.
  * Lots of cleanups as time permitted.

V2.2 : 5-Jan-2021 by wavemotion-dave
  * More memory tweaks - faster processing of memory for another 5% speedup.

V2.1 : 3-Jan-2021 by wavemotion-dave
  * Now using DTCM and VRAM for some key memory areas to speed up the emulator almost 10%
  
V2.0 : 1-Jan-2021 by wavemotion-dave
  * Pokey and TIA sound core reworked to provide sound that is worth listening to.
  * Please run on a DSi, DSi-XL/LL or 3DS. You won't be happy with the performance on a DS-LITE/PHAT.
  
V1.9 : 31-Dec-2020 by wavemotion-dave
  * Robotron now works with with Twin-Sticks! 
  * Minor cleanup and polish to end the year...
  
V1.8 : 23-Dec-2020 by wavemotion-dave
  * Major improvements to CPU core to get about 10% speed boost across the board.
    More games playable at full speed on DSi and above. Special DS-LITE build 
    included that supports only the smallest and most basic games (but at near 
    full speed).

V1.7a : 21-Dec-2020 by wavemotion-dave
  * Full Pokey Support at both 450 and 4000. XM RAM mapping at 4000 so games
  like Serpentine will run properly now. Undocumented opcodes for the Sally 
  processor now implemented so games like Popeye will run fine. 

V1.7 : 20-Dec-2020 by wavemotion-dave
  * After much thought and debug, I've ditched Kangaroo mode. As near as I can 
    tell it's used on only a couple of games and not for gameplay - and it costs
    more DS-CPU time than it's worth. There is no harm other than a potential 
    graphical glitch which I've not noticed and that CPU time is precious. 

V1.6b : 19-Dec-2020 by wavemotion-dave
  * Fixed bug causing black rectangles on some games - thanks to Wii-7800!

V1.6 : 17-Dec-2020 by wavemotion-dave
  * Minor cleanup. Rebranding to PHOENIX EDITION. 
  
V1.5 : 15-Dec-2020 by wavemotion-dave
  * Overhaul of scaling and X/Y offsets so that more games look pixel perfect.
  * More homebrews run... compatibility table added to the back end of this doc.
  * More robust handling of High Score Cart and saving of files.

V1.4 : 13-Dec-2020 by wavemotion-dave
  * Implemented Hiscore Saving per the HSC cart. Put the 4k highscore.rom (don't ask)
    where your ROMs live. If found, it will enable saving of high scores
    on all 9 original games (Asteroids, Joust, Centipede, Xevious, Dig Dug, Galaga,
    Food Fight, Ms Pac-Man and Robotron) as well as most of the newer homebrews.

V1.3 : 12-Dec-2020 by wavemotion-dave
  * Backported some fixes and emulation timing improvments from Prosystem 1.3g
    plus the Wii version.  Renders a few more games playable.
  * Fixed region for Froggie and Beef Drop - they now look right and play 
    at full speed.
  * Minor other improvements as time permitted. 

V1.2 : 10-Dec-2020 by wavemotion-dave
  * More speed improvements. Fixed some 320 pixel games. Improved sound.

V1.1 : 09-Dec-2020 by wavemotion-dave
  * Brought back to life... cleanup and about a 50% speed improvement, better
    screen rendering/scaling and slightly improved UI features.

V1.0 : 24/05/2011
  * Initial release based on my a320 version (which is based on Prosystem 1.0.3)
  * Compiled with last version of Devkitpro/libnds, so DSi compatible \o/
