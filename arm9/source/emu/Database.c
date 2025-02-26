// ----------------------------------------------------------------------------
//   ___  ___  ___  ___       ___  ____  ___  _  _
//  /__/ /__/ /  / /__  /__/ /__    /   /_   / |/ /
// /    / \  /__/ ___/ ___/ ___/   /   /__  /    /  emulator
//
// ----------------------------------------------------------------------------
// Copyright 2005 Greg Stanton
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
// ----------------------------------------------------------------------------
// Database.cpp
// ----------------------------------------------------------------------------
#include <ctype.h>

#include "Database.h"
#include "ProSystem.h"
#include "../config.h"

Database_Entry myCartInfo __attribute__((section(".dtcm")));
extern uint cartridge_size;

// To get the md5sum of an .a78 file with header in Linux:  dd bs=1 skip=128 if=somefile.a78 | md5sum

Database_Entry game_list[] = {
    
  // The original NTSC Commercial Games
  {"0be996d25144966d",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=Ace Of Aces
  {"877dcc97a775ed55",  CT_SUPLRG,    POKEY_NONE, LGN, LGN,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  7,  22, 264,  230, 0}, // title=Alien Brigade
  {"07342c78619ba6ff",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  160,  0,  17, 256,  230, 0}, // title=Asteroids
  {"8fc3a695eaea3984",  CT_NORMAL, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   240,  6,  12, 262,  220, 0}, // title=Ballblazer
  {"42682415906c21c6",  CT_SUPCAR, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   155,  0,  12, 256,  220, 0}, // title=Barnyard Blaster
  {"f5f6b69c5eb4b55f",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  2,  15, 262,  226, 0}, // title=Basketbrawl
  {"5a09946e57dbe304",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  160,  24, 16, 300,  230, 0}, // title=Centipede
  {"93e4387864b014c1",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   210,  0,  15, 256,  226, 0}, // title=Choplifter
  {"2e8e28f6ad8b9b92",  CT_SUPCAR, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  218, 0}, // title=Commando
  {"db691469128d9a42",  CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   165,  0,  18, 256,  229, 0}, // title=Crack'ed
  {"a94e4560b6ad053a",  CT_SUPLRG,    POKEY_NONE, LGN, LGN,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   150,  9,  17, 272,  234, 0}, // title=Crossbow
  {"179b76ff729d4849",  CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   125,  0,  13, 256,  219, 0}, // title=Dark Chambers
  {"95ac811c7d27af00",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   240,  6,  19, 261,  234, 0}, // title=Desert Falcon
  {"731879ea82fc0ca2",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  160,  0,  12, 256,  220, 0}, // title=Dig Dug
  {"5e332fbfc1e0fc74",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,  14, 256,  220, 0}, // title=Donkey Kong Jr
  {"743c47f500d095f2",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,  14, 256,  220, 0}, // title=Donkey Kong Jr (alt) 
  {"19f1ee292a23636b",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,  14, 256,  220, 0}, // title=Donkey Kong
  {"543484c00ba23373",  CT_ACTVIS,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   130,  0,  12, 256,  220, 0}, // title=Double Dragon
  {"2251a6a0f3aec84c",  CT_ABSOLU,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,  12, 256,  220, 0}, // title=F-18 Hornet
  {"d25d5d19188e9f14",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   155,  0,  12, 256,  220, 0}, // title=Fatal Run
  {"07dbbfe612a0a28e",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160, 12,  14, 282,  226, 0}, // title=Fight Night
  {"cf76b00244105b8e",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  140,  0,  17, 255,  227, 0}, // title=Food Fight
  {"fb8d803b328b2e44",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  140,  0,  12, 256,  229, 0}, // title=Galaga
  {"fd9e78e201b6baaf",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   250,  0,  17, 256,  227, 0}, // title=Hat Trick
  {"c3672482ca93f70e",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   180,  0,  17, 256,  230, 0}, // title=Ikari Warriors
  {"baebc9246c087e89",  CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   165,  0,  12, 256,  220, 0}, // title=Impossible Mission
  {"045fd12050b7f2b8",  CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   165,  3,  12, 261,  234, 1}, // title=Jinks
  {"f18b3b897a25ab38",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  140,  0,  17, 256,  234, 0}, // title=Joust                           
  {"c3a5a8692a423d43",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   250,  0,  12, 256,  220, 0}, // title=Karateka                        
  {"f57d0af323d4e173",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   125,  22, 17, 276,  225, 0}, // title=Kung Fu Master
  {"f2f5e5841e4dda89",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=Mean 18 Ultimate Golf
  {"bc1e905db1008493",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   120,  0,  13, 256,  226, 0}, // title=Midnight Mutants
  {"431ca060201ee1f9",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,  14, 256,  220, 0}, // title=Mario Bros.
  {"37b5692e33a98115",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   125,  0,  12, 256,  220, 0}, // title=Mat Mania Challenge
  {"bedc30ec43587e0c",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   165,  0,  12, 256,  220, 0}, // title=Meltdown
  {"3bc8f554cf86f813",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=Motor Psycho
  {"fc0ea52a9fac5572",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  170,  0,  17, 256,  224, 0}, // title=Ms. Pac-Man   
  {"220121f771fc4b98",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  10, 20, 270,  234, 0}, // title=Ninja Golf
  {"74569571a208f8b0",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   245,  0,  12, 256,  224, 0}, // title=One On One
  {"1a5207870dec6fae",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   120,  0,  12, 256,  220, 0}, // title=Pete Rose Baseball
  {"33aea1e2b6634a1d",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   175,  0,  15, 256,  226, 0}, // title=Planet Smashers
  {"584582bb09ee8122",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   210,  39, 15, 320,  230, 0}, // title=Pole Position II                
  {"ac03806cef2558fc",  CT_ACTVIS,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   130,  1,  16, 259,  234, 0}, // title=Rampage
  {"383ed9bd1efb9b6c",  CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 1}, // title=Realsports Baseball
  {"66ecaafe1b82ae68",  CT_NORMAL,    POKEY_NONE, TWIN,TWIN, DIFF_A,  DIFF_A, NTSC,  HSC_YES,  205,  5,  13, 270,  234, 0}, // title=Robotron     
  {"980c35ae9625773a",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   130,  8,  18, 265,  234, 0}, // title=Scrapyard Dog
  {"cbb0746192540a13",  CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   165,  0,  13, 256,  220, 0}, // title=Summer Games
  {"cc18e3b37a507c42",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   125,  0,  12, 256,  220, 0}, // title=Super Huey UH-IX
  {"59b5793bece1c80f",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_NO,   130,  0,  12, 256,  220, 0}, // title=Super Skatebordin'
  {"5c4f752371a523f1",  CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   150,  0,  12, 256,  220, 0}, // title=Tank Command
  {"1af475ff6429a160",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   125,  0,  12, 256,  220, 0}, // title=Title Match Pro Wrestling
  {"c3903ab01a51222a",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   120,  0,  12, 256,  220, 0}, // title=Tomcat F-14 Simulator
  {"208ef955fa90a298",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=Touchdown Football
  {"8d64763db3100aad",  CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,   8, 320,  234, 1}, // title=Tower Toppler
  {"427cb05d0a1abb06",  CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   150,  0,   3, 256,  197, 0}, // title=Water Ski
  {"3799d72f78dda2ee",  CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   170,  0,  13, 256,  220, 0}, // title=Winter Games
  {"90fa275f9f2a65b3",  CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   170,  0,  13, 256,  220, 0}, // title=Winter Games (alt)  
  {"05fb699db9eef564",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   125,  15, 13, 284,  234, 0}, // title=Xenophobe
  {"d7dc17379aa25e5a",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  205,  0,  16, 256,  220, 0}, // title=Xevious

  // Prototypes and Hacks                                                                                                                          
  {"4332c24e4f3bc72e",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=3D Asteroids
  {"1745feadabb24e7c",  CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=Impossible Mission
  {"20660b667df538ec",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  160,  24, 16, 300,  230, 0}, // title=Centipede - Frameless Hack
  {"8f7eb10ad0bd7547",  CT_FRACTALUS, POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   180,  0,  12, 256,  220, 0}, // title=Rescue On Fractalus
  {"06204dadc975be5e",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=Gato   
  {"17b3b764d33eae9b",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   130,  0,  20, 256,  234, 0}, // title=Klax (fixed)
  {"5fb805f2b69820a9",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   130,  0,  20, 256,  234, 0}, // title=Klax       
  {"017066f522908081",  CT_SUPLRG,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_NO,   145,  0,  12, 256,  220, 0}, // title=Missing in Action
  {"ec206c8db4316eb1",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=Pit Fighter                     
  {"74f0283c566bdee8",  CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_NO,   180,  0,  17, 256,  234, 0}, // title=Plutos XM
  {"86546808dc60961c",  CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_NO,   180,  0,  17, 256,  234, 0}, // title=Plutos (non-XM)
  {"1745feadabb24e7c",  CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=Possible Mission
  {"b697d9c2d1b9f6cb",  CT_SUPROM, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  5,  15, 258,  220, 0}, // title=Sentinel

  // Bob (pacmanplus) Games                                                                                                                        
  {"89b8b3df46733e0c",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  165,  0,  12, 256,  220, 0}, // title=Armor Attack II
  {"eea04359df6770d6",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  165,  0,  12, 256,  220, 0}, // title=Armor Attack II (20230627)
  {"a65f79ad4a0bbdec",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  210,  0,  17, 256,  230, 0}, // title=Asteroids Deluxe (NTSC) (20071014)
  {"3d38281ed8a8d8c7",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_YES,  245,  30,  9, 320,  210, 0}, // title=Astro Blaster
  {"55ffe535897c368b",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_YES,  245,  30,  9, 320,  210, 0}, // title=Astro Blaster (20230627)
  {"a51e5df28a0fe8c5",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  205,  32,  9, 320,  213, 0}, // title=Astro Fighter
  {"8feb090fb1aee5cc",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  205,  32,  9, 320,  213, 0}, // title=Astro Fighter (v1.1)
  {"7cdfbe37634e7dcd",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   250,  32, 12, 320,  222, 0}, // title=Baby Pac Man
  {"2b31dfab41dce110",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   250,  32, 12, 320,  222, 0}, // title=Baby Pac Man (20230627)
  {"34483432b92f565f",  CT_SUPLRG,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  165,  3,  14, 256,  220, 0}, // title=Bentley Bear's Crystal Quest
  {"299d31c8e181fdd0",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  160,  0,   5, 256,  203, 0}, // title=Crazy Brix   
  {"2d2fe4da9f1bae10",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   165,  0,   9, 256,  232, 0}, // title=Crazy Otto
  {"100551363027dc5f",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   165,  0,   9, 256,  232, 0}, // title=Crazy Otto (20230627)
  {"6287727ab36391a6",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  250,  0,  14, 257,  220, 0}, // title=FailSafe (NTSC) (20100227)
  {"e7d89669a7f92ec2",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  255,  30,  6, 320,  206, 0}, // title=Frenzy (with Berzerk) (homebrew)
  {"26031dea7251fb86",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  255,  30,  6, 320,  206, 0}, // title=Frenzy (with Berzerk) (20211025)
  {"2f4ae1015a345652",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  210,  14, 17, 283,  227, 0}, // title=Galaxian
  {"686a4e4dde0eca5c",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  210,  14, 17, 283,  227, 0}, // title=Galaxian (v1.1)
  {"e54edc299e72d22d",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  205,  4,  14, 268,  234, 0}, // title=Jr Pac-Man  
  {"bde3abe40d302d8c",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  205,  4,  14, 268,  234, 0}, // title=Jr Pac-Man  (20230627)
  {"6b8600aabd11f834",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  3,  12, 256,  225, 0}, // title=KC Munchkin
  {"aa0b9560d6610378",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  3,  12, 256,  225, 0}, // title=KC Munchkin (20230627)
  {"927edf157f88b8f5",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  3,  12, 256,  225, 0}, // title=KC Munchkin (Alt Movement) (20170409)
  {"c3f6201d6a9388e8",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  160,  0,  13, 256,  220, 0}, // title=Meteor Shower
  {"a44241d782ee14b5",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  160,  0,  13, 256,  220, 0}, // title=Meteor Shower (v1.1)
  {"9ff38ea62004201d",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  210,  30, 20, 320,  234, 0}, // title=Moon Cresta
  {"a56f26e6d21b2ae4",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  210,  30, 20, 320,  234, 0}, // title=Moon Cresta (v1.1)
  {"cf007563fe94cacf",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  215,  0,   9, 256,  217, 0}, // title=Ms. Pac-Man (Bob's Enhancements)    
  {"2a17dc5a61be342d",  CT_NORMAL, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  210,  5,  16, 265,  230, 0}, // title=Ms Pac-Man 320
  {"60982f430b762343",  CT_NORMAL, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  220,  6,  17, 264,  233, 0}, // title=Pac-Man 320
  {"5013b69cb05b21a1",  CT_NORMAL, POKEY_AT_4000, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_YES,  210,  10, 11, 281,  231, 0}, // title=Pac-Man Collection (homebrew)    
  {"6a432fd1e9d42a29",  CT_NORMAL, POKEY_AT_4000, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_YES,  210,  10, 11, 281,  231, 0}, // title=Pac-Man Collection - Ultimate Edition (POKEY) (4000)
  {"ce1cb2f5d2238449",  CT_NORMAL, POKEY_AT_4000, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_YES,  210,  10, 11, 281,  231, 0}, // title=Pac-Man Collection - Ultimate Edition (POKEY) (4000)
  {"200ef9c7b36afd3b",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_YES,  210,  10, 11, 281,  231, 0}, // title=Pac-Man Collection - Remastered (TIA)
  {"f2512870a8abc82d",  CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  210,  9,  10, 278,  221, 0}, // title=Pac-Man Collection 40th Anniversary Edition (homebrew) - Newest
  {"a59d362e3a391ff1",  CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  220,  9,  10, 278,  221, 0}, // title=Pac-Man Collection 40th Anniversary Edition (homebrew) - Older
  {"1330d23ebad9b5de",  CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  220,  9,  10, 278,  221, 0}, // title=Pac-Man Collection 40th Anniversary Edition (homebrew) - PMC_XM Newest
  {"2b60de20a55056a1",  CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  205,  11, 17, 279,  233, 0}, // title=Pac-Man Collection 40th - Short Mazes (2022) - Newest
  {"c80edcd555cd3d81",  CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  205,  11, 17, 279,  233, 0}, // title=Pac-Man Collection 40th - Short Mazes (2022)
  {"39dc7f6f39f9b3e3",  CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  200,  9,  10, 278,  221, 0}, // title=Pac-Man Collection - 40th Anniversary Edition (20230627)
  {"2686f20449c339f7",  CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  200,  11, 17, 279,  233, 0}, // title=Pac-Man Collection - 40th Anniversary Edition (Short Mazes) (20230627)
  {"2b51ebf2f371d079",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  210,  10, 18, 271,  234, 0}, // title=Pac-Man - Energy Drink Edition (20230707)
  {"d0bf3b841ad4bbd3",  CT_NORMAL,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  200,  10, 12, 274,  233, 0}, // title=Pac-Man Plus 320
  {"43525a0405184875",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  160,  0,  17, 256,  233, 0}, // title=Rip-Off
  {"803743fe18600f29",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  160,  0,  17, 256,  233, 0}, // title=Rip-Off (20230627)
  {"a3a85e507d6f7189",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  250,  30,  5, 320,  205, 0}, // title=Scramble (homebrew)
  {"31b20a4710e69130",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  250,  30,  5, 320,  205, 0}, // title=Scramble (20230627)
  {"771cb4609347657f",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  200,  0,  12, 256,  220, 0}, // title=Space Duel (homebrew)
  {"6adf79558a3d7f5b",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  165,  30,  8, 320,  210, 0}, // title=Space Invaders (Homebrew)
  {"6a898d52ef050cdb",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  165,  30,  8, 320,  210, 0}, // title=Space Invaders (v1.1)
  {"7ab539bb0e99e1e5",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  205,  0,   5, 256,  225, 0}, // title=Super Pac-Man
  {"88b9de0eba37ba51",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  205,  0,   5, 256,  225, 0}, // title=Super Pac-Man (20230627)
  {"79df20ee86a989e6",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  210,  9,  18, 282,  230, 0}, // title=UniWarS
  {"0db69825c81171e6",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  210,  9,  18, 282,  230, 0}, // title=UniWarS (v1.1)
  {"f41f651417c23410",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  240, 26,  12, 320,  225, 0}, // title=Super Cobra
                        
  {"fd9353d42cca5f81",  CT_SUPRAMX2,POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  190,  0,   6, 256,  203, 1}, // title=1942 (Standard Banking RAM)
  {"e1b290ee690c0cd6",  CT_SUPRAMX2,POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  190,  0,   6, 256,  203, 1}, // title=1942 (Standard Banking RAM)
  {"0fec9c1f5973c7a1",  CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  190,  0,   6, 256,  203, 1}, // title=1942 (Dragonfly Banking RAM)
  {"6f157f421c7ed5d9",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  130,  22, 14, 299,  230, 0}, // title=2048 (RC1a) (20211113)
  {"a837a34f540fd137",  CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  14, 16, 286,  234, 0}, // title=7iX (20220305)
  {"ff056f2858f14fc4",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   130,  3,  15, 261,  234, 0}, // title=A Roach In Space - Part II - Electric Bugaloo (20201119)
  {"d99bff88cd3cce19",  CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,   9, 256,  200, 0}, // title=Arkanoid 78b Demo (purposely set HSC to false - game HSC is buggy)
  {"212ee2a6e66d8bb7",  CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,   9, 256,  200, 0}, // title=Arkanoid 78b Demo (purposely set HSC to false - game HSC is buggy)
  {"e1da4c3ea0d26ae0",  CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,   9, 256,  200, 0}, // title=Arkanoid 78b Demo (purposely set HSC to false - game HSC is buggy)  
  {"0d05659a7d0eef02",  CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  140,  1,  16, 268,  234, 0}, // title=ARTI Public Demo
  {"4a8a22cff154f479",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   120,  0,   7, 256,  220, 0}, // title=Boom!
  {"9fa7743a016c9b70",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  160,  0,  27, 256,  234, 0}, // title=BonQ (Final Atariage)
  {"78b1061d651ef806",  CT_NORMAL, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  135,  0,  16, 256,  234, 0}, // title=Beef Drop (Final Atariage)
  {"b11b1a2bae8a1d0c",  CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  125,  3,  13, 264,  227, 0}, // title=Bernie and the Cubic Conundrum (Alpha 10)
  {"a34cd425d0c087d0",  CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  125,  6,  16, 264,  233, 0}, // title=Bernie and the Tower of Doom (RC1) (Demo)
  {"000b5888d2489f7e",  CT_SUPLRG,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   125,  0,  12, 256,  223, 0}, // title=Cannon in D for Defense (demo 03)
  {"825c03c049306c16",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  125,  0,  16, 260,  234, 0}, // title=Cartesian Chaos (v11) (20221218)
  {"a4b5d742860beb25",  CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   120,  8,  19, 269,  233, 0}, // title=Chase (20201231)
  {"0c2f248a1ae9bfd1",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,  14, 256,  234, 0}, // title=Danger Zone (RC-4C) (NTSC Demo) (20201231)
  {"fab7b59dd580dce0",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  2,  17, 260,  229, 0}, // title=Death Merchant (v1_30)
  {"dd1cfc933d2bfacc",  CT_SUPLRG,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  185,  0,  12, 256,  210, 0}, // title=Donkey Kong PK-XM (NTSC) (Demo) (v1.2)
  {"c3107d3e3e17d67e",  CT_SUPLRG, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  185,  0,  20, 256,  223, 0}, // title=Donkey Kong XM DEMO
  {"312363c7691fa51e",  CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  180,  0,  17, 256,  216, 0}, // title=Donkey Kong Remix DEMO
  {"77164df89ae49b4d",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  125,  0,  16, 256,  234, 0}, // title=Dragon's Descent (20210731)
  {"8c2798f929a43317",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   125,  6,  15, 256,  232, 0}, // title=Dragon's Havoc (Demo Dec 2022)
  {"7b7825ca2c79148f",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  125,  6,  15, 256,  232, 0}, // title=Dragon's Cache (20210207)
  {"a9f29004412621f2",  CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  130,  0,  16, 256,  230, 0}, // title=Draker Quest II
  {"fab1290f9a4c4f2b",  CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  120,  0,  17, 256,  230, 0}, // title=Draker Quest (Beta 4)
  {"b3143adbbb7d7d18",  CT_NORMAL,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  130,  0,   9, 256,  231, 0}, // title=Dungeon Stalker (20151022)
  {"a44e8b7b7881beb0",  CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  135,  0,  16, 256,  234, 0}, // title=E.X.O. (RC Demo A)
  {"6053233cb59c0b4c",  CT_NORMAL, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  210,  34,  4, 320,  216, 0}, // title=Froggie (NTSC) (Final Release)
  {"9daaac9b25783a7e",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  130,  0,  16, 256,  224, 0}, // title=Frogus (20221020)
  {"c2e131a091ceed2e",  CT_SUPLRG,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  15, 256,  234, 0}, // title=Game of the Bear - Polar Opposites (RC1) (20230211)
  {"e443f7fb5be3283d",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   130,  13, 12, 280,  234, 0}, // title=GoSub
  {"1e21bf1d9d7b3c0c",  CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  11, 256,  220, 0}, // title=Graze Suit Alpha (20170910)
  {"1c9deabc48f07d1b",  CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  115,  0,  12, 256,  220, 0}, // title=Keystone Koppers (Demo Dec 22)
  {"d41d8cd98f00b204",  CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  115,  0,  12, 256,  220, 0}, // title=Keystone Koppers (RC4 Demo)
  {"1d47c3802135d864",  CT_SUPLRG,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_NO,   130,  0,  12, 256,  233, 0}, // title=Knight Guy On Board - 30 Squares Of Fate (20210116)
  {"0916973898e3b6b8",  CT_SUPLRG,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_NO,   130,  0,  12, 256,  233, 0}, // title=Knight Guy On Board - 30 Squares Of Fate (Demo 3)
  {"33dbb58f9ee73e9f",  CT_SUPLRG,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_NO,   130,  0,  12, 256,  233, 0}, // title=Knight Guy In Low Res World - Castle Days (RC 01-1)
  {"3ec728e116017be8",  CT_SUPLRG,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_NO,   130,  0,  12, 256,  233, 0}, // title=Knight Guy - Quest For Something (20210423)
  {"7abd9e0a6321e813",  CT_SUPLRG,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_NO,   130,  0,  12, 256,  233, 0}, // title=Knight Guy - In Another Castle (RC1b)
  {"271864e0978278a3",  CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  130,  0,   7, 256,  201, 0}, // title=Legend of Silverpeak (1.01)
  {"7abd9e0a6321e813",  CT_SUPLRG,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   135,  6,  16, 264,  234, 0}, // title=Lunar Patrol (v83) (20241231) (2354A1FF)  
  {"23ac803eabfb8c61",  CT_NORMAL,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,  12, 256,  234, 0}, // title=Lyra the Tenrec (20240104)
  {"181a9978d9da7a7e",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  140,  0,  17, 320,  234, 0}, // title=Merlain
  {"3f80432f156088bf",  CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  140,  0,  14, 256,  220, 0}, // title=Millie And Molly (Demo) (POKEY 450) (20230305)
  {"1c860298a8966cc8",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  140,  0,  14, 256,  220, 0}, // title=Monster Maze (20220306)
  {"d1b56eae7227c12d",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   130,  0,  17, 256,  224, 0}, // title=Morf (20220314)
  {"ac5c99ac01c96ad9",  CT_SUPLRG,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   130,  0,  16, 256,  234, 0}, // title=Ninjish Guy - Perilous Island (20211107)
  {"3d9c52142f9e53f5",  CT_SUPLRG,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,  17, 256,  234, 0}, // title=Oozy The Goo - Gaiden (20231001)
  {"6ac5a7f8b6a3198e",  CT_SUPLRG,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  125,  2,  17, 260,  234, 0}, // title=PentaGo (Demo) (20230422)
  {"a662862f20362fc5",  CT_BANKSHALT,POKEY_AT_800,SNES,JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,  10, 256,  214, 0}, // title=Attack of the Petscii Robots (Demo) (POKEY 800)
  {"0254afa887fcfc8c",  CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  140,  16, 17, 289,  220, 0}, // title=Plumb Luck DX (RC2) (20230410)
  {"61e6a16889b62216",  CT_SUPLRG,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  140,  0,  17, 256,  220, 0}, // title=Popeye 2.40 Demo
  {"fab49206f4b041fa",  CT_SUPLRG,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  140,  0,  17, 256,  220, 0}, // title=Popeye 2.40 j7800 Demo
  {"b6561537290e6e25",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,  12, 256,  220, 0}, // title=Robbo (20160513)
  {"fc525819ec2bdc4a",  CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,  11, 256,  217, 0}, // title=Robots Rumble (20220217)
  {"9bd70c06d3386f76",  CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  130,  0,  12, 256,  220, 0}, // title=Serpentine (20161029)
  {"96f69b85e0b43bbe",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  120,  0,  12, 256,  233, 0}, // title=Sick Pickles (20171202)
  {"40567f50c569a60c",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  130,  0,  16, 256,  230, 0}, // title=Slide Boy in Maze Land (RC1) (20210515)
  {"91f4cb1f642ff1de",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  110,  0,  16, 256,  230, 0}, // title=Space Peril (NTSC) (v8) (20210907)
  {"19844117863cd38d",  CT_SUPLRG,    POKEY_NONE, SOTA,SOTA, DIFF_A,  DIFF_A, NTSC,  HSC_YES,  130,  31, 17, 320,  230, 0}, // title=Spire of the Ancients (NTSC) (20201223)
  {"81cee326b99d6831",  CT_NORMAL, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  200,  0,  13, 256,  220, 0}, // title=Super Circus Atari Age (NTSC) (Joystick) (POKEY 4000)
  {"02508e6df5e173b4",  CT_NORMAL,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  200,  0,  13, 256,  220, 0}, // title=Super Circus Atari Age (NTSC) (Joystick) (POKEY 0450)
  {"a60e4b608505d1fb",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  130,  0,  16, 256,  230, 0}, // title=Time Salvo
  {"ff825fcbed9bf699",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   120,  0,  17, 256,  220, 0}, // title=Touchdown Challenge (v2_21) (20230225)
  {"0d7e2674d802b412",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   120,  0,  13, 256,  233, 0}, // title=Tunnels of Hyperion
  {"f5150c0fc1948832",  CT_NORMAL,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  160,  0,  12, 256,  240, 0}, // title=7800 Utility Cart
  {"846751861993b907",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  130,  0,  16, 256,  231, 0}, // title=Wizards Dungeon (20211111)

  {"",CT_NORMAL,0,0,0,0,0,0,0,0,0,0,0},
};



NameMap_t NameMap[] =
{
    {"POPEYE",      "POPEYE",       "61e6a16889b62216"},
    {"EXO",         "EXO",          "a44e8b7b7881beb0"},
    {"KOPPERS",     "KOPPERS",      "d41d8cd98f00b204"},
    {"TIME",        "SALVO",        "a60e4b608505d1fb"},
    {"CHRISTMAS",   "SALVO",        "a60e4b608505d1fb"},
    {"MERLAIN",     "MERLAIN",      "181a9978d9da7a7e"},
    {"TOUCHDOWN",   "CHALLENGE",    "ff825fcbed9bf699"},
    {"OOZY",        "GAIDEN",       "3d9c52142f9e53f5"},
    {"MILLIE",      "MOLLY",        "3f80432f156088bf"},
    {"SILVERPEAK",  "SILVERPEAK",   "271864e0978278a3"},
    {"LUNAR",       "PATROL",       "7abd9e0a6321e813"},
    {"ATTACK",      "PETSCII",      "a662862f20362fc5"},
    {"KNIGHT",      "BOARD",        "0916973898e3b6b8"},
    {"KNIGHT",      "WORLD",        "33dbb58f9ee73e9f"},
    {"KNIGHT",      "SOMETHING",    "3ec728e116017be8"},
    {"KNIGHT",      "ANOTHER",      "7abd9e0a6321e813"},
    {"DRAGON",      "DESCENT",      "77164df89ae49b4d"},
    {"DRAGON",      "HAVOC",        "8c2798f929a43317"},
    {"DRAGON",      "CACHE",        "7b7825ca2c79148f"},
    {"ARKANOID",    "ARKANOID",     "d99bff88cd3cce19"},
    {"ARTI",        "ATRI",         "0d05659a7d0eef02"},
    {"CANNON",      "DEFENSE",      "000b5888d2489f7e"},
    {"1942",        "1942",         "fd9353d42cca5f81"},   
    {"WIZARD",      "DUNGEON",      "846751861993b907"},   
    {"UTILITY",     "CART",         "f5150c0fc1948832"},
    {"BONQ",        "BONQ",         "9fa7743a016c9b70"},
    {"BEEF",        "DROP",         "78b1061d651ef806"},
    {"BERNIE",      "CUBIC",        "b11b1a2bae8a1d0c"},
    {"BERNIE",      "TOWER",        "a34cd425d0c087d0"},
    {"DEATH",       "MERCHANT",     "fab7b59dd580dce0"},
    {"TUNNELS",     "HYPERION",     "0d7e2674d802b412"},   
    {"SLIDE",       "MAZE",         "40567f50c569a60c"},
    {"CARTESIAN",   "CHAOS",        "825c03c049306c16"},
    {"DANGER",      "ZONE",         "0c2f248a1ae9bfd1"},
    {"SCRAMBLE",    "SCRAMBLE",     "a3a85e507d6f7189"},
    {"KLAX",        "KLAX",         "17b3b764d33eae9b"},
    {"PENTAGO",     "PENTAGO",      "6ac5a7f8b6a3198e"},    
    
    {"","",""}
};

// -------------------------------------------------------------------------
// This happens AFTER The rom is loaded and the header info (if any) has
// been read out. Here we can adjust based on the hash table above... which
// is mainly used for headerless roms and a few special cases were we want 
// to correct the Y-offsets to make the game well centered/scaled on screen.
// -------------------------------------------------------------------------
bool database_Load(byte * digest)
{
    extern u8 bNoDatabase;
    bool bFound = false;
    uint16 i;

    // Uppercase the filename... to make searching easier.
    for(int j = 0; j < strlen(cartridge_filename); j++) cartridge_filename[j] = toupper(cartridge_filename[j]);

    // See if we've been asked to skip the internal database
    if(!bNoDatabase)
    {
        // --------------------------------------------------------------------------------------
        // First see if we've got a match in our external A7800DS.DAT configuration database...
        // --------------------------------------------------------------------------------------
        for(i = 0; i < MAX_CONFIGS; i++)
        {
            if(!strncmp(allConfigs.cart[i].half_digest, (char *) digest, 16))
            {
                memcpy( & myCartInfo, & allConfigs.cart[i], sizeof(myCartInfo));
                bFound = true;
                break;
            }
        }

        // ---------------------------------------------------------------------------------------------
        // If we didn't find it in the config database, we can look in the internal database table...
        // ---------------------------------------------------------------------------------------------
        if(!bFound)
        {
            /* Look up mapper in game list by md5sum */
            for(i = 0; strlen(game_list[i].half_digest); i++)
            {
                if(!strncmp(game_list[i].half_digest, (char *) digest, 16))
                {
                    memcpy( & myCartInfo, & game_list[i], sizeof(myCartInfo));
                    if(!isDSiMode()) myCartInfo.frameSkip = ((cartridge_size <= (48 * 1024)) ? FRAMESKIP_MEDIUM : FRAMESKIP_AGGRESSIVE); // Older DS-Lite/Phat needs help
                    myCartInfo.palette = 1; // Force this if not specifically found by md5
                    myCartInfo.xJiggle = 64;
                    myCartInfo.yJiggle = 16;
                    bFound = true;
                    break;
                }
            }
        }


        // -----------------------------------------------------------------------------------------------
        // If we didn't find a definitive md5 match above, look up game by name in our name mapping table.
        // -----------------------------------------------------------------------------------------------
        if(!bFound)
        {
            for(int k = 0; strlen(NameMap[k].name1); k++)
            {
                if(myCartInfo.region == NTSC)
                {
                    if((strstr(cartridge_filename, NameMap[k].name1)) && (strstr(cartridge_filename, NameMap[k].name2))) // If both names are found in the filename...
                    {
                        /* Look up mapper in game list by md5sum from the Name Mapper table */
                        for(i = 0; strlen(game_list[i].half_digest); i++)
                        {
                            if(!strncmp(game_list[i].half_digest, (char *) NameMap[k].half_digest, 16))
                            {
                                memcpy( & myCartInfo, & game_list[i], sizeof(myCartInfo));
                                if(!isDSiMode()) myCartInfo.frameSkip = ((cartridge_size <= (48 * 1024)) ? FRAMESKIP_MEDIUM : FRAMESKIP_AGGRESSIVE); // Older DS-Lite/Phat needs help
                                myCartInfo.palette = 1; // Force this if not specifically found by md5
                                myCartInfo.xJiggle = 64;
                                myCartInfo.yJiggle = 16;
                                bFound = true;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        }
    }

    // No matter what... override for Tower Toppler to make it playable...
    if(strcmp((char *) digest, (char *) "8d64763db3100aadc552db5e6868506a") == 0) // Tower Toppler
    {
        use_composite_filtering = 76;
        myCartInfo.frameSkip = FRAMESKIP_AGGRESSIVE; // It's the only way we stand a chance.
        myCartInfo.cardctrl1 = SOTA;
        myCartInfo.xOffset = 32;
        myCartInfo.yOffset = 8;
        myCartInfo.xScale = 320;
        myCartInfo.yScale = 234;
        bFound = 1;
    }
    else if(strcmp((char *) digest, (char *) "f5150c0fc1948832211e57852abb0c6e") == 0) // 7800 Utility Cart
    {
        use_composite_filtering = 1;
        bFound = 1;
    }
    // Override for Jinks to enable composite filtering
    else if(strcmp((char *) digest, (char *) "045fd12050b7f2b842d5970f2414e912") == 0) // Jinks
    {
        use_composite_filtering = 1;
        bFound = 1;
    }
    else use_composite_filtering = 0;


    // --------------------------------------------------------------------------
    // Default scaling options below if not found... these are close enough... 
    // We can make some educated guesses on cart and frameskip...
    // --------------------------------------------------------------------------
    if(!bFound)
    {
        strncpy(myCartInfo.half_digest, (char *) digest, 16);
        myCartInfo.half_digest[16] = 0;
        myCartInfo.xOffset = 0;
        myCartInfo.yOffset = 13;
        myCartInfo.xScale = 256;
        myCartInfo.yScale = 220;
        myCartInfo.diff1 = DIFF_A;
        myCartInfo.diff2 = DIFF_A;
        myCartInfo.xButton = KEY_MAP_DEFAULT;
        myCartInfo.yButton = KEY_MAP_DEFAULT;
        myCartInfo.spare2 = 0;
        myCartInfo.spare3 = 0;
        myCartInfo.spare4 = 1;
        myCartInfo.spare5 = 0;
        myCartInfo.palette = 1;
        myCartInfo.xJiggle = 64;
        myCartInfo.yJiggle = 16;

        // -------------------------------------------------------------------
        // If the game has no .a78 header, do our best to guess these...
        // -------------------------------------------------------------------
        if(myCartInfo.hasHeader == false)
        {
            myCartInfo.region = NTSC;
            if(cartridge_size == (144 * 1024)) myCartInfo.cardtype = CT_SUPLRG;
            else myCartInfo.cardtype = (cartridge_size <= (52 * 1024)) ? CT_NORMAL : CT_SUPROM;
            myCartInfo.pokeyType = POKEY_NONE;
            myCartInfo.cardctrl1 = JOY;
            myCartInfo.cardctrl2 = JOY;
            myCartInfo.hsc = false;
        }

        // --------------------------------------------------------
        // Do our best guess as to whether we should frameskip...
        // --------------------------------------------------------
        if(isDSiMode()) // DSi can handle most games in full framerate... default to disable frameskip for the DSi
        {
            myCartInfo.frameSkip = FRAMESKIP_DISABLE;
        }
        else // DS-Lite defaults to some level of frame skipping
        {
            myCartInfo.frameSkip = ((cartridge_size <= (48 * 1024)) ? FRAMESKIP_MEDIUM : FRAMESKIP_AGGRESSIVE); // Non-banked carts get light frameskip... otherwise agressive
        }
    }

    // Lastly - use the internal database to always try and find a BIOS timeout value... we don't let the user override this one...
    myCartInfo.biosTimeout = 160;
    bFound = 0;
    for(i = 0; strlen(game_list[i].half_digest); i++) // Search through entire internal database...
    {
        if(!strncmp(game_list[i].half_digest, (char *) digest, 16)) // Search by md5sum
        {
            myCartInfo.biosTimeout = game_list[i].biosTimeout;
            bFound = 1;
            break;
        }
    }

    if(!bFound) // And if not found - search by name to find a reasonable bios timeout value
    {
        for(int k = 0; strlen(NameMap[k].name1); k++)
        {
            if(myCartInfo.region == NTSC)
            {
                if((strstr(cartridge_filename, NameMap[k].name1)) && (strstr(cartridge_filename, NameMap[k].name2))) // If both names are found in the filename...
                {
                    /* Look up bios timeout in game list by md5sum from the Name Mapper table */
                    for(i = 0; strlen(game_list[i].half_digest); i++)
                    {
                        if(!strncmp(game_list[i].half_digest, (char *) NameMap[k].half_digest, 16))
                        {
                            myCartInfo.biosTimeout = game_list[i].biosTimeout;
                            bFound = true;
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }

    return true;
}

// End of file
