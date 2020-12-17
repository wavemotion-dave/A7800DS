a7800DS
--------------------------------------------------------------------------------
a7800DS is an Atari ProSystem 7800 console emulator.
To use this emulator, you must use compatibles rom with a78/bin format. 
Do not ask me about such files, I don't have them. A search with Google will certainly 
help you. 

Features :
----------
 Most things you should expect from an emulator. Speed is good. Sound less so.

--------------------------------------------------------------------------------
History :
--------------------------------------------------------------------------------
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
How tu use a7800DS :
--------------------------------------------------------------------------------
Place .NDS on your SDCARD and launch with Twilight Menu++ or Unlaunch.
If you want to run on a flash cart, it might work... recommend HBMENU to launch.

When the emulator starts, click on the cartridge slot to choose a file. Use Up/Down 
to select a file, then use A to load it.

Controls :
 * Direction pad : the joystick ...
 * A      : Fire button 1
 * B      : Fire button 2
 * SELECT : SELECT Button
 * START  : PAUSE Button
 * X      : FPS Display
 * Y      : Full-Speed 
 * L/R    : Shift screen up/down by 1 pixel
 
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

Compatibility Table

GAME                    MD5SUM                              HSC   FPS NOTES
========================================================================================================================
Ace Of Aces             0be996d25144966d5541c9eb4919b289    NO    60  Minor graphical glitches. Very playable.
Alien Brigade           877dcc97a775ed55081864b2dbf5f1e2    NO    45  Glitchy graphics. Can't really play.
Asteroids               07342c78619ba6ffcc61c10e907e3b50    YES   60  Near perfect
Asteroids Deluxe        a65f79ad4a0bbdecd59d5f7eb3623fd7    YES   60  Near perfect
Astro Blaster           3d38281ed8a8d8c7cd457a18c92c8604    YES   60  Near perfect
Astro Fighter           a51e5df28a0fe8c52e9d28fb5f8e44a6    YES   60  Near perfect
Baby Pac Man            7cdfbe37634e7dcd4dc67db7edbcd3ba    NO    60  Near perfect
Ballblazer              8fc3a695eaea3984912d98ed4a543376    NO    60  Near perfect
Barnyard Blaster        42682415906c21c6af80e4198403ffda    NO    60  Needs Lightgun Support (not implemented)
Basketbrawl             f5f6b69c5eb4b55fc163158d1a6b423e    NO    46  Minor glitches. Too slow.
Beef Drop (Final)       78b1061d651ef806becac1dd3fda29a0    YES   60  Near perfect. Sound could be better.
BonQ (Final)            9fa7743a016c9b7015ee1d386326f88e    YES   60  Graphical glitches on title screen. Play is near perfect.
Centipede               5a09946e57dbe30408a8f253a28d07db    YES   60  Near perfect
Choplifter              93e4387864b014c155d7c17877990d1e    NO    60  Near perfect
Commando                2e8e28f6ad8b9b9267d518d880c73ebb    NO    35  Too slow to be playable.
Crack'ed                db691469128d9a4217ec7e315930b646    NO    60  Would be better with lightgun support.
Crossbow                a94e4560b6ad053a1c24e096f1262ebf    NO    60  Would be better with lightgun support.
Dark Chambers           179b76ff729d4849b8f66a502398acae    NO    60  Near perfect
Desert Falcon           95ac811c7d27af0032ba090f28c107bd    NO    60  Near perfect
Dig Dug                 731879ea82fc0ca245e39e036fe293e6    YES   60  Near perfect
Donkey Kong             19f1ee292a23636bd57d408b62de79c7    NO    60  Near perfect
Donkey Kong Jr          5e332fbfc1e0fc74223d2e73271ce650    NO    60  Near perfect
Double Dragon           543484c00ba233736bcaba2da20eeea9    NO    15  Glitch city. Unplayable.
Dungeon Stalker         b3143adbbb7d7d189e918e5b29d55a72    YES   60  Near perfect
F-18 Hornet             2251a6a0f3aec84cc0aff66fc9fa91e8    NO    60  A few small graphical glitches but playable.
Failsafe (homebrew)     6287727ab36391a62f728bbdee88675c    YES   60  Near perfect
Fatal Run               d25d5d19188e9f149977c49eb0367cd1    NO    45  A bit too slow to be playable.
Food Fight              cf76b00244105b8e03cdc37677ec1073    YES   60  Near perfect
Frenzy (with Berzerk)   e7d89669a7f92ec2cc99d9663a28671c    YES   60  Otto voice sounds terrible and is slow - but playable!
Froggie                 6053233cb59c0b4ca633623fd76c4576    YES   60  Plays perfectly - sound could stand improvement.
Galaga                  fb8d803b328b2e442548f7799cfa9a4a    YES   60  Near perfect
Hat Trick               fd9e78e201b6baafddfd3e1fbfe6ba31    NO    60  Near perfect
Ikari Warriors          c3672482ca93f70eafd9134b936c3feb    NO    40  Too slow to be playable
Impossible Mission      baebc9246c087e893dfa489632157180    NO    60  Near perfect
Jinks                   045fd12050b7f2b842d5970f2414e912    NO    52  A bit of slowdown but playable
Joust                   f18b3b897a25ab3885b43b4bd141b396    YES   60  Near perfect                           
Jr. Pac-Man             e54edc299e72d22d0ba05d16f3393e8c    YES   60  Near perfect
Karateka                c3a5a8692a423d43d9d28dd5b7d109d9    NO    55  Minor glitches. What's the point of this game?!
Kung Fu Master          f57d0af323d4e173fb49ed447f0563d7    NO    60  Near perfect
Mario Bros.             431ca060201ee1f9eb49d44962874049    NO    60  Near perfect
Meteor Shower           c3f6201d6a9388e860328c963a3301cc    YES   60  Near perfect - my favorite homebrew!
Midnight Mutants        bc1e905db1008493a9632aa83ab4682b    NO    38  Too slow to be playable
Ms. Pac-Man             fc0ea52a9fac557251b65ee680d951e5    YES   60  Near perfect
Ninja Golf              220121f771fc4b98cef97dc040e8d378    NO    40  Too slow. Minor glitches.
One On One              74569571a208f8b0b1ccfb22d7c914e1    NO    60  Near perfect
Pac-Man Collection      5013b69cb05b21a1194ce48517df7bfc    YES   60  Near perfect
Pac-Man Collection 40th 044657294450c869c45e7ef61f4870de    YES   60  Near perfect
Planet Smashers         33aea1e2b6634a1dec8c7006d9afda22    NO    51  A bit of slowdown but playable
Pole Position II        584582bb09ee8122e7fc09dc7d1ed813    NO    60  Font hard to read due to screen resolution but playable
Rampage                 ac03806cef2558fc795a7d5d8dba7bc0    NO    60  Near perfect
Realsports Baseball     bfad016d6e77eaccec74c0340aded8b9    NO    10  Way too slow.
Robotron                66ecaafe1b82ae68ffc96267aaf7a4d7    YES   60  Near perfect
Scramble                a3a85e507d6f718972b1464ce1aaf8a4    YES   60  Near perfect
Scrapyard Dog           980c35ae9625773a450aa7ef51751c04    NO    34  Too slow to be playable 
Space Duel              771cb4609347657f63e6f0eb26036e35    YES   60  Near perfect
Space Invaders          6adf79558a3d7f5beca1bb8d34337417    YES   60  Plays perfect. Minor graphical compression artifacts.
Summer Games            cbb0746192540a13b4c7775c7ce2021f    NO    60  Plays fine - font hard to read on small screen.
Super Circus Atariage   81cee326b99d6831de10a566e338bd25    YES   60  Near perfect 
Winter Games            3799d72f78dda2ee87b0ef8bf7b91186    NO    60  Plays fine - font hard to read on small screen.
Xenophobe               05fb699db9eef564e2fe45c568746dbc    NO    36  Too slow to be playable.
Xevious                 d7dc17379aa25e5ae3c14b9e780c6f6d    YES   60  Near perfect
  
 
