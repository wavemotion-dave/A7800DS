# a7800DS

a7800DS is an Atari ProSystem 7800 console emulator for the DS/DSi.

To use this emulator, you must use compatibles rom with a78/bin format. 
Strongly recommend you use NTSC roms... PAL ones have more scanlines and will render
more slowly and since the sound core is tied to scanlines, the sound will be wrong. 
All the debug on this port to the DS has been done with NTSC roms - seek them out!
Do not ask me about such files, I don't have them. A search with Google will certainly 
help you. The emulator was developed using Trebor's "7800 ProPack" of well-curated games.

Features :
----------
 Most things you should expect from an emulator. Speed is excellent on the DSi and 
 less great on the older DS-LITE. For the DS-LITE you can expect full speed only on
 about half of the 7800 library. The more traditional games (think: Asteroids,
 Astro Blaster, Robotron, Food Fight, Centipede, Pac-Man Collection, etc) should
 all run great. The really big bankswitched games will struggle on the older 
 DS-LITE/PHAT hardware.
  
 Add highscore.rom for 7800 High Score saving. This can be in /roms/bios, /data/bios
 or in the same directory as the emulator.  It's worth the effort!
 
Copyright :
----------
A7800DS is Copyright 2021-2022 by Dave Bernazzani (wavemotion-dave).

Copying and distribution of this emulator, it's source code and associated 
readme files, with or without modification, are permitted in any medium without 
royalty provided this copyright notice is used and wavemotion-dave (Phoenix-Edition),
Alekmaul (original port) and Greg Stanton (ProSystem Emulator) are thanked profusely.
 

Philosophy :
----------
For this particular emulator, I'm not striving for perfect emulation accuracy - the goal 
is to get as many games as close to playable as possible. Nothing else matters if
the game won't run at near full speed with all the gameplay in-tact. The original
baseline emulator (ProSystem) had some issues that were made more exaggerated with the
port to the DS/DSi. Minor screen glitches or minor sound issues are secondary to making
sure the game will actually run.  If you're looking for a highly accurate emulator for 
the 7800 ProSystem, this isn't it - try MAME/MESS or A7800. But if you're looking to enjoy 
some classic 7800 console goodness on your DS/DSi then you've come to the right place!
 
--------------------------------------------------------------------------------
History :
--------------------------------------------------------------------------------
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
  
--------------------------------------------------------------------------------
How to use a7800DS :
--------------------------------------------------------------------------------
Place .NDS on your SD card and launch with Twilight Menu++ or Unlaunch.
If you want to run on a flash cart, it might work... recommend HBMENU to launch.
Strongly recommend you run on a DSi or 3DS to get full speed. DS-LITE/PHAT is slow.

When the emulator starts, click on the cartridge slot to choose a file. Use Up/Down 
to select a file, then use A to load it.

Controls :
 * Direction pad : the joystick ...
 * A             : Fire button 1
 * B             : Fire button 2
 * SELECT        : SELECT Button
 * START         : PAUSE Button
 * X             : FPS Display
 * Y             : Fire Button 1 (for those that prefer not using A)
 * L/R + DPAD    : Used to Shift Offsets and Scale Screen to desired ratio
 
 Use stylus on buttons for other actions on bottom screen.  
 
 High Score Saving works if you have highscore.rom (exact name and case) in your
 roms directory where you load your games... also, you MUST press the HSC button
 if you want to snap the Saved Scores out to the flash card. It's not something 
 I want to do as the game runs... so you must do it... the high scores will also
 auto-save if you quit the emulator or select a new game.
 
--------------------------------------------------------------------------------
Credits:
--------------------------------------------------------------------------------
Thanks Wintermute for devkitpro and libnds (http://www.devkitpro.org).
  Greg Stanton for ProSystem source code (https://home.comcast.net/~gscottstanton/)
   an Atari 7800 emulator.
  zx81 (http://zx81.zx81.free.fr/serendipity_fr/) for PSP A7800 version (that helped
   me a lot to understand ProSystem).
  raz0red (http://www.twitchasylum.com/forum/viewtopic.php?t=519) for WII7800  (that
  helped me a lot to fix some timing problem).

--------------------------------------------------------------------------------
Original Author:
Alekmaul
alekmaul@portabledev.com
http://www.portabledev.com

Updates by wavemotion-dave:  https://github.com/wavemotion-dave/A7800DS
--------------------------------------------------------------------------------

Compatibility Table (on the faster DSi)
```
GAME                    MD5SUM                              HSC   FPS NOTES
========================================================================================================================
Ace Of Aces             0be996d25144966d5541c9eb4919b289    NO    60  Minor graphical glitches. Very playable.
Alien Brigade           877dcc97a775ed55081864b2dbf5f1e2    NO    60  Near perfect
Arkanoid Demo           212ee2a6e66d8bb7fbf26f343cc8dc19    NO    60  Playable though paddles would be nice.
Asteroids               07342c78619ba6ffcc61c10e907e3b50    YES   60  Near perfect
Asteroids Deluxe        a65f79ad4a0bbdecd59d5f7eb3623fd7    YES   60  Near perfect
Astro Blaster           3d38281ed8a8d8c7cd457a18c92c8604    YES   60  Near perfect
Astro Fighter           a51e5df28a0fe8c52e9d28fb5f8e44a6    YES   60  Near perfect
Baby Pac Man            7cdfbe37634e7dcd4dc67db7edbcd3ba    NO    60  Near perfect
Ballblazer              8fc3a695eaea3984912d98ed4a543376    NO    60  Near perfect
Barnyard Blaster        42682415906c21c6af80e4198403ffda    NO    60  Needs Lightgun Support (not implemented)
Basketbrawl             f5f6b69c5eb4b55fc163158d1a6b423e    NO    60  Near perfect
Beef Drop (Final)       78b1061d651ef806becac1dd3fda29a0    YES   60  Near perfect
BonQ (Final)            9fa7743a016c9b7015ee1d386326f88e    YES   60  Near perfect
Centipede               5a09946e57dbe30408a8f253a28d07db    YES   60  Near perfect
Choplifter              93e4387864b014c155d7c17877990d1e    NO    60  Near perfect
Commando                2e8e28f6ad8b9b9267d518d880c73ebb    NO    60  Near perfect
Crack'ed                db691469128d9a4217ec7e315930b646    NO    60  Would be better with lightgun support.
Crossbow                a94e4560b6ad053a1c24e096f1262ebf    NO    60  Would be better with lightgun support.
Dark Chambers           179b76ff729d4849b8f66a502398acae    NO    60  Near perfect
Desert Falcon           95ac811c7d27af0032ba090f28c107bd    NO    60  Near perfect
Dig Dug                 731879ea82fc0ca245e39e036fe293e6    YES   60  Near perfect
Donkey Kong             19f1ee292a23636bd57d408b62de79c7    NO    60  Near perfect
Donkey Kong Jr          5e332fbfc1e0fc74223d2e73271ce650    NO    60  Near perfect
Donkey Kong XM Demo     c3107d3e3e17d67e3a11d47a5946a4f3    NO    60  Near perfect
Double Dragon           543484c00ba233736bcaba2da20eeea9    NO    60  Near perfect
Dragon's Descent        94009ccfdcd4f55d24033ca06269ba6a    NO    60  Near perfect
Drakers Quest I         fab1290f9a4c4f2b4d831c8a57f969f5    NO    60  Near perfect
Drakers Quest II        a9f29004412621f20ad9f5c51cc11486    NO    60  Near perfect
Dungeon Stalker         b3143adbbb7d7d189e918e5b29d55a72    YES   60  Near perfect
E.X.O Alpha 12          faea27017447dd8bae677d91125a8135    YES   60  Near perfect
F-18 Hornet             2251a6a0f3aec84cc0aff66fc9fa91e8    NO    60  A few small graphical glitches but playable.
Failsafe (homebrew)     6287727ab36391a62f728bbdee88675c    YES   60  Near perfect
Fatal Run               d25d5d19188e9f149977c49eb0367cd1    NO    60  Near perfect
Food Fight              cf76b00244105b8e03cdc37677ec1073    YES   60  Near perfect
Frenzy (with Berzerk)   e7d89669a7f92ec2cc99d9663a28671c    YES   60  Near perfect (voice slightly wrong pitch)
Froggie                 6053233cb59c0b4ca633623fd76c4576    YES   60  Near perfect
Galaga                  fb8d803b328b2e442548f7799cfa9a4a    YES   60  Near perfect
Hat Trick               fd9e78e201b6baafddfd3e1fbfe6ba31    NO    60  Near perfect
Ikari Warriors          c3672482ca93f70eafd9134b936c3feb    NO    60  Near perfect
Impossible Mission      baebc9246c087e893dfa489632157180    NO    60  Near perfect
Jinks                   045fd12050b7f2b842d5970f2414e912    NO    60  Near perfect
Joust                   f18b3b897a25ab3885b43b4bd141b396    YES   60  Near perfect                           
Jr. Pac-Man             e54edc299e72d22d0ba05d16f3393e8c    YES   60  Near perfect
Klax                    17b3b764d33eae9b5260f01df7bb9d2f    NO    60  Near perfect
Karateka                c3a5a8692a423d43d9d28dd5b7d109d9    NO    60  Minor glitches.
Kung Fu Master          f57d0af323d4e173fb49ed447f0563d7    NO    60  Near perfect
Mario Bros.             431ca060201ee1f9eb49d44962874049    NO    60  Near perfect
Meteor Shower           c3f6201d6a9388e860328c963a3301cc    YES   60  Near perfect - my favorite homebrew!
Midnight Mutants        bc1e905db1008493a9632aa83ab4682b    NO    60  Near perfect
Moon Cresta             9ff38ea62004201d870caa8bd9463525    YES   60  Near perfect
Ms. Pac-Man             fc0ea52a9fac557251b65ee680d951e5    YES   60  Near perfect
Ninja Golf              220121f771fc4b98cef97dc040e8d378    NO    60  Near perfect
One On One              74569571a208f8b0b1ccfb22d7c914e1    NO    60  Near perfect
Pac-Man Collection      5013b69cb05b21a1194ce48517df7bfc    YES   60  Near perfect
Pac-Man Collection 40th 1330d23ebad9b5ded92ebeacdf305abd    YES   60  Near perfect - Damn this is one fine collection!
Planet Smashers         33aea1e2b6634a1dec8c7006d9afda22    NO    60  Near perfect
Plutos                  86546808dc60961cdb1b20e761c50ab1    NO    60  Near perfect
Pole Position II        584582bb09ee8122e7fc09dc7d1ed813    NO    60  Near perfect
Popeye Demo 2.9         640c7827fe63082efbaffc13f7983744    YES   60  Near perfect.
Spire of the Ancients   19844117863cd38d4e1e4cbc867ae599    NO    60  Near perfect
Rampage                 ac03806cef2558fc795a7d5d8dba7bc0    NO    60  Near perfect
Realsports Baseball     383ed9bd1efb9b6cb3388a777678c928    NO    60  Any voice rendering is slow.
Robotron                66ecaafe1b82ae68ffc96267aaf7a4d7    YES   60  Near perfect
Scramble                a3a85e507d6f718972b1464ce1aaf8a4    YES   60  Near perfect
Scrapyard Dog           980c35ae9625773a450aa7ef51751c04    NO    60  Near perfect
Serpentine              9bd70c06d3386f76f8162881699a777a    YES   60  Near perfect
Space Duel              771cb4609347657f63e6f0eb26036e35    YES   60  Near perfect
Space Invaders          6adf79558a3d7f5beca1bb8d34337417    YES   60  Near perfect
Summer Games            cbb0746192540a13b4c7775c7ce2021f    NO    60  Plays fine - font hard to read on small screen.
Super Skatebordin       59b5793bece1c80f77b55d60fb39cb94    NO    60  Near perfect
Super Circus Atariage   81cee326b99d6831de10a566e338bd25    YES   60  Near perfect 
Time Salvo              a60e4b608505d1fb201703b266f754a7    YES   60  Near perfect 
UniWarS                 79df20ee86a989e669158bcb9d113e8a    YES   60  Near perfect 
Winter Games            3799d72f78dda2ee87b0ef8bf7b91186    NO    60  Plays fine - font hard to read on small screen.
Xenophobe               05fb699db9eef564e2fe45c568746dbc    NO    60  Near perfect
Xevious                 d7dc17379aa25e5ae3c14b9e780c6f6d    YES   60  Near perfect
```
 
