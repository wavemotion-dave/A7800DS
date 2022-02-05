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
#include "Database.h"
#include "ProSystem.h"

#define DATABASE_SOURCE "Database.cpp"

extern u8 bForceFrameskip;

bool database_enabled = true;
typedef struct {
  char digest[33];
  char header_name[33];
  uint cardtype;
  byte pokeyType;
  uint cardctrl1;
  uint cardctrl2;
  uint diff1;
  uint diff2;
  uint cardregion;
  bool steals_cycles;
  bool uses_wsync;
  bool hsc;
  int  xOffset;
  int  yOffset;
  int  xScale;
  int  yScale;
  int  forceFrameskip;
} Database_Entry;


Database_Entry game_list[] = {
  {"f5150c0fc1948832211e57852abb0c6e",  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", CT_NORMAL,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  22, 256,  220, 0}, // title=7800 Utility Cart
  {"4332c24e4f3bc72e7fe1b77adf66c2b7",  "3D Asteroids",                     CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=3D Asteroids
  {"0be996d25144966d5541c9eb4919b289",  "Ace of Aces",                      CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Ace Of Aces
  {"877dcc97a775ed55081864b2dbf5f1e2",  "Alien Brigade",                    CT_SUPLRG,    POKEY_NONE, LGN, LGN,  DIFF_A,  DIFF_A, NTSC,  NO_STEALING,    USES_WSYNC, HSC_NO,  7,  30, 264,  230, 0}, // title=Alien Brigade
  {"212ee2a6e66d8bb7fbf26f343cc8dc19",  "Arkanoid",                         CT_SUPCAR,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  17, 256,  200, 0}, // title=Arkanoid 78b Demo (purposely set HSC to false - game HSC is buggy)
  {"a65f79ad4a0bbdecd59d5f7eb3623fd7",  "Asteroids Deluxe (NTSC)",          CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  25, 256,  230, 0}, // title=Asteroids Deluxe
  {"07342c78619ba6ffcc61c10e907e3b50",  "Asteroids",                        CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  25, 256,  230, 0}, // title=Asteroids
  {"3d38281ed8a8d8c7cd457a18c92c8604",  "Astro Blaster",                    CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 30, 17, 320,  210, 0}, // title=Astro Blaster
  {"a51e5df28a0fe8c52e9d28fb5f8e44a6",  "Astro Fighter",                    CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 32, 17, 320,  210, 0}, // title=Astro Fighter
  {"7cdfbe37634e7dcd4dc67db7edbcd3ba",  "Baby Pac-Man",                     CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  32, 22, 320,  220, 0}, // title=Baby Pac Man
  {"8fc3a695eaea3984912d98ed4a543376",  "Ballblazer",                       CT_NORMAL, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Ballblazer
  {"42682415906c21c6af80e4198403ffda",  "Barnyard Blaster",                 CT_SUPCAR, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Barnyard Blaster
  {"f5f6b69c5eb4b55fc163158d1a6b423e",  "Basketbrawl",                      CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 1}, // title=Basketbrawl
  {"78b1061d651ef806becac1dd3fda29a0",  "Beef Drop",                        CT_NORMAL, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    SKIP_WSYNC, HSC_YES, 0,  16, 256,  220, 0}, // title=Beef Drop (Final Atariage)
  {"34483432b92f565f4ced82a141119164",  "Bentley Bear",                     CT_SUPLRG,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  3,  22, 256,  220, 0}, // title=Bentley Bear's Crystal Quest
  {"4e325918a8b3bbcf2f9405040abcfc6d",  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  26, 256,  220, 0}, // title=BonQ (found on Atairage Age site)
  {"9fa7743a016c9b7015ee1d386326f88e",  "b*nQ",                             CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  NO_STEALING,    SKIP_WSYNC, HSC_YES, 0,  26, 256,  220, 0}, // title=BonQ (final Atariage)
  {"4a8a22cff154f479f1ddaa386f21fc39",  "Boom",                             CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  15, 256,  220, 0}, // title=Boom!
  {"5a09946e57dbe30408a8f253a28d07db",  "Centipede",                        CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 24, 24, 300,  230, 0}, // title=Centipede
  {"20660b667df538ec32a8e1b998438604",  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 24, 24, 300,  230, 0}, // title=Centipede - Frameless Hack
  {"a4b5d742860beb25c29def4530194c1e",  "Chase",                            CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  8,  27, 269,  233, 0}, // title=Chase
  {"93e4387864b014c155d7c17877990d1e",  "Choplifter",                       CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Choplifter
  {"2e8e28f6ad8b9b9267d518d880c73ebb",  "Commando",                         CT_SUPCAR, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  20, 256,  220, 0}, // title=Commando
  {"db691469128d9a4217ec7e315930b646",  "Crack'ed",                         CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  25, 256,  229, 0}, // title=Crack'ed
  {"2d2fe4da9f1bae102fa8ca2d8830a626",  "Crazy Otto",                       CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  17, 256,  232, 0}, // title=Crazy Otto
  {"a94e4560b6ad053a1c24e096f1262ebf",  "Crossbow",                         CT_SUPLRG,    POKEY_NONE, LGN, LGN,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  5,  22, 267,  231, 0}, // title=Crossbow
  {"179b76ff729d4849b8f66a502398acae",  "Dark Chambers",                    CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  21, 256,  220, 0}, // title=Dark Chambers
  {"95ac811c7d27af0032ba090f28c107bd",  "Desert Falcon",                    CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  6,  27, 261,  234, 0}, // title=Desert Falcon
  {"731879ea82fc0ca245e39e036fe293e6",  "Dig Dug",                          CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  22, 256,  220, 0}, // title=Dig Dug
  {"5e332fbfc1e0fc74223d2e73271ce650",  "Donkey Kong Jr",                   CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Donkey Kong Jr
  {"19f1ee292a23636bd57d408b62de79c7",  "Donkey Kong",                      CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Donkey Kong
  {"c3107d3e3e17d67e3a11d47a5946a4f3",  "DONKEY KONG XM",                   CT_SUPLRG, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  20, 256,  210, 0}, // title=Donkey Kong XM Demo (purposely set HSC to false - game HSC is buggy)
  {"543484c00ba233736bcaba2da20eeea9",  "Double Dragon",                    CT_ACTVIS,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  NO_STEALING,    SKIP_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Double Dragon
  {"94009ccfdcd4f55d24033ca06269ba6a",  "Dragon's Descent",                 CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  24, 256,  234, 0}, // title=Dragon's Descent 1.7
  {"cad9b532a4ced6793e18ba7237e44d40",  "Dragon's Havoc",                   CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  6,  23, 256,  232, 0}, // title=Dragon's Havoc    
  {"a9f29004412621f20ad9f5c51cc11486",  "Draker Quest II",                  CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  24, 256,  230, 0}, // title=Draker Quest II
  {"fab1290f9a4c4f2b4d831c8a57f969f5",  "Draker Quest",                     CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  25, 256,  230, 0}, // title=Draker Quest
  {"b3143adbbb7d7d189e918e5b29d55a72",  "Dungeon Stalker",                  CT_NORMAL,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  23, 256,  220, 0}, // title=Dungeon Stalker (homebrew)  
  {"7099cc31cbc492d525d218efe88b8685",  "xxxxx",                            CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  25, 256,  230, 0}, // title=E.X.O. (Alpha)
  {"faea27017447dd8bae677d91125a8135",  "xxxxx",                            CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  25, 256,  230, 0}, // title=E.X.O. (Beta)
  {"a44e8b7b7881beb0fe3c71a1a04441c8",  "EXO",                              CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  24, 256,  234, 0}, // title=E.X.O. (RC Demo)
  {"2251a6a0f3aec84cc0aff66fc9fa91e8",  "F-18 Hornet",                      CT_ABSOLU,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=F-18 Hornet
  {"6287727ab36391a62f728bbdee88675c",  "Failsafe",                         CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  22, 257,  220, 0}, // title=Failsafe (homebrew)
  {"d25d5d19188e9f149977c49eb0367cd1",  "Fatal Run",                        CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Fatal Run
  {"07dbbfe612a0a28e283c01545e59f25e",  "Fight Night",                      CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Fight Night
  {"cf76b00244105b8e03cdc37677ec1073",  "Food Fight",                       CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  25, 255,  227, 0}, // title=Food Fight
  {"e7d89669a7f92ec2cc99d9663a28671c",  "Frenzy (w-Berzerk)",               CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  NO_STEALING,    USES_WSYNC, HSC_YES, 30, 14, 320,  206, 0}, // title=Frenzy (with Berzerk) (homebrew)
  {"6053233cb59c0b4ca633623fd76c4576",  "Froggie",                          CT_NORMAL, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 34, 16, 320,  205, 0}, // title=Froggie (homebrew)
  {"4b8f22fc4b022a7ea1b4148195b85d4a",  "Frogus",                           CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  24, 256,  224, 0}, // title=Frogus
  {"fb8d803b328b2e442548f7799cfa9a4a",  "Galaga",                           CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  22, 256,  229, 0}, // title=Galaga
  {"2f4ae1015a345652b36004a8c62a4ac6",  "Galaxian",                         CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 14, 13, 282,  200, 0}, // title=Galaxian
  {"06204dadc975be5e5e37e7cc66f984cf",  "Gato",                             CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Gato   
  {"e443f7fb5be3283dd44c0f5d80c3a7b3",  "GoSub",                            CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  13, 22, 280,  234, 0}, // title=GoSub
  {"1e21bf1d9d7b3c0cebaac576964c9eb2",  "Graze Suit Alpha",                 CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  19, 256,  220, 1}, // title=Graze Suit Alpha
  {"fd9e78e201b6baafddfd3e1fbfe6ba31",  "Hat Trick",                        CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  25, 256,  227, 0}, // title=Hat Trick                       
  {"c3672482ca93f70eafd9134b936c3feb",  "Ikari Warriors",                   CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  25, 256,  230, 0}, // title=Ikari Warriors
  {"baebc9246c087e893dfa489632157180",  "Impossible Mission",               CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Impossible Mission
  {"1745feadabb24e7cefc375904c73fa4c",  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Impossible Mission
  {"045fd12050b7f2b842d5970f2414e912",  "Jinks",                            CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  3,  22, 261,  234, 0}, // title=Jinks
  {"f18b3b897a25ab3885b43b4bd141b396",  "Joust",                            CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  25, 256,  234, 0}, // title=Joust                           
  {"e54edc299e72d22d0ba05d16f3393e8c",  "Jr. Pac-Man (NTSC)",               CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 4,  22, 268,  234, 0}, // title=Jr Pac-Man  
  {"c3a5a8692a423d43d9d28dd5b7d109d9",  "Karateka",                         CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Karateka                        
  {"6b8600aabd11f834448e910801f4e0bc",  "KC Munchkin",                      CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  3,  22, 256,  225, 0}, // title=KC Munchkin
  {"17b3b764d33eae9b5260f01df7bb9d2f",  "Klax",                             CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  28, 256,  234, 0}, // title=Klax (fixed)
  {"5fb805f2b69820a9b196f5fed2a23c99",  "Klax",                             CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  28, 256,  234, 0}, // title=Klax       
  {"1d47c3802135d864dc1d922ec27aa708",  "Knight Guy On Board",              CT_SUPLRG,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  NO_STEALING,    SKIP_WSYNC, HSC_NO,  0,  22, 256,  233, 0}, // title=Knight Guy On Board - 30 Squares Of Fate 
  {"f57d0af323d4e173fb49ed447f0563d7",  "Kung Fu Master",                   CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  NO_STEALING,    SKIP_WSYNC, HSC_NO,  22, 22, 276,  225, 0}, // title=Kung Fu Master
  {"fe657a1b8a34b2c0e1d2171166d40926",  "Legend of Silverpeak",             CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  15, 256,  201, 1}, // title=Legend of Silverpeak
  {"431ca060201ee1f9eb49d44962874049",  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Mario Bros.
  {"37b5692e33a98115e574185fa8398c22",  "Mat Mania Challenge",              CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Mat Mania Challenge
  {"f2f5e5841e4dda89a2faf8933dc33ea6",  "Mean 18 Ultimate Golf",            CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Mean 18 Ultimate Golf
  {"bedc30ec43587e0c98fc38c39c1ef9d0",  "Meltdown",                         CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Meltdown
  {"c3f6201d6a9388e860328c963a3301cc",  "Meteor Shower",                    CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  21, 256,  220, 0}, // title=Meteor Shower
  {"bc1e905db1008493a9632aa83ab4682b",  "Midnight Mutants",                 CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  21, 256,  226, 1}, // title=Midnight Mutants
  {"017066f522908081ec3ee624f5e4a8aa",  "Missing in Action",                CT_SUPLRG,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  NO_STEALING,    SKIP_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Missing in Action
  {"181a9978d9da7a7e21f770808cc681f2",  "Merlain",                          CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  25, 320,  234, 0}, // title=Merlain
  {"5fe8ef9e368acd5699e5a453082b7636",  "Morf",                             CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  25, 256,  224, 0}, // title=Morf
  {"9ff38ea62004201d870caa8bd9463525",  "Moon Cresta (NTSC)",               CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 30, 13, 320,  210, 0}, // title=Moon Cresta
  {"3bc8f554cf86f8132a623cc2201a564b",  "Motor Psycho",                     CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Motor Psycho
  {"fc0ea52a9fac557251b65ee680d951e5",  "Ms. Pac-Man",                      CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  25, 256,  224, 0}, // title=Ms. Pac-Man      
  {"2a17dc5a61be342dd00af719cc335852",  "Ms Pac-Man 320",                   CT_NORMAL, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 5,  24, 265,  230, 0}, // title=Ms Pac-Man 320
  {"220121f771fc4b98cef97dc040e8d378",  "Ninja Golf",                       CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  10, 28, 270,  234, 1}, // title=Ninja Golf
  {"74569571a208f8b0b1ccfb22d7c914e1",  "One On One",                       CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  20, 256,  224, 0}, // title=One On One
  {"60982f430b762343d53e48f70acfa6d0",  "Pac-Man 320",                      CT_NORMAL, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 6,  25, 264,  233, 0}, // title=Pac-Man 320
  {"5013b69cb05b21a1194ce48517df7bfc",  "Pac-Man Collection",               CT_NORMAL, POKEY_AT_4000, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 10, 19, 281,  231, 0}, // title=Pac-Man Collection (homebrew)    
  {"a59d362e3a391ff1482131aa0818ad3e",  "Pac-Man 40th Anniversary",         CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 9,  18, 278,  221, 0}, // title=Pac-Man Collection 40th Anniversary Edition (homebrew)
  {"d0bf3b841ad4bbd356e9588874749a13",  "Pac-Man Plus 320",                 CT_NORMAL, POKEY_AT_450,  JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 10, 22, 274,  233, 0}, // title=Pac-Man Plus 320
  {"1a5207870dec6fae9111cb747e20d8e3",  "Pete Rose Baseball",               CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Pete Rose Baseball
  {"ec206c8db4316eb1ebce9fc960da7d8f",  "Pit Fighter",                      CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Pit Fighter                     
  {"33aea1e2b6634a1dec8c7006d9afda22",  "Planet Smashers",                  CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  226, 0}, // title=Planet Smashers
  {"74f0283c566bdee8543e4fdc5cb8b201",  "Plutos XM",                        CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  25, 256,  234, 0}, // title=Plutos XM
  {"86546808dc60961cdb1b20e761c50ab1",  "Plutos",                           CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  25, 256,  234, 0}, // title=Plutos (non-XM)
  {"584582bb09ee8122e7fc09dc7d1ed813",  "Pole Position II",                 CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  35, 22, 320,  230, 0}, // title=Pole Position II                
  {"1745feadabb24e7cefc375904c73fa4c",  "Possible Mission",                 CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Possible Mission
  {"0710aa2620435230da6a7cf3f620210d",  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", CT_SUPLRG,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  18, 256,  210, 0}, // title=Popeye Demo 2.7f (homebrew)
  {"81187dbc08934214bd55187d4b8149dd",  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", CT_SUPLRG,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  18, 256,  210, 0}, // title=Popeye Demo 2.8 (homebrew)  
  {"640c7827fe63082efbaffc13f7983744",  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", CT_SUPLRG,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  18, 256,  210, 0}, // title=Popeye Demo 2.9 (homebrew)  
  {"631262731c193928d01365d2bc31b64f",  "Popeye",                           CT_SUPLRG,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  18, 256,  210, 0}, // title=Popeye Demo 1.0RC
  {"ac03806cef2558fc795a7d5d8dba7bc0",  "Rampage",                          CT_ACTVIS,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Rampage
  {"383ed9bd1efb9b6cb3388a777678c928",  "Realsports Baseball",              CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 1}, // title=Realsports Baseball
  {"8f7eb10ad0bd75474abf0c6c36c08486",  "Rescue On Fractalus",              CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Rescue On Fractalus
  {"43525a0405184875c2ecfd0196886a34",  "Rip-Off",                          CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  25, 256,  233, 0}, // title=Rip-Off
  {"66ecaafe1b82ae68ffc96267aaf7a4d7",  "Robotron",                         CT_NORMAL,    POKEY_NONE, TWIN,TWIN, DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 5,  22, 270,  234, 0}, // title=Robotron     
  {"b6561537290e6e25e1249394366c3c63",  "Robbo",                            CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Robbo
  {"a3a85e507d6f718972b1464ce1aaf8a4",  "Scramble",                         CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 30, 13, 320,  205, 0}, // title=Scramble (homebrew)
  {"980c35ae9625773a450aa7ef51751c04",  "Scrapyard Dog",                    CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Scrapyard Dog
  {"b697d9c2d1b9f6cb21041286d1bbfa7f",  "Sentinel",                         CT_SUPROM, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Sentinel
  {"9bd70c06d3386f76f8162881699a777a",  "Serpentine",                       CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  22, 256,  220, 0}, // title=Serpentine (homebrew)
  {"96f69b85e0b43bbebbbd59bb8276a372",  "Sick Pickles",                     CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  22, 256,  233, 0}, // title=Sick Pickles
  {"771cb4609347657f63e6f0eb26036e35",  "Space Duel",                       CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  22, 256,  220, 0}, // title=Space Duel (homebrew)
  {"6adf79558a3d7f5beca1bb8d34337417",  "Space Invaders",                   CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 30, 16, 320,  210, 0}, // title=Space Invaders (Homebrew)
  {"19844117863cd38d4e1e4cbc867ae599",  "Spire of the Ancients",            CT_SUPLRG,    POKEY_NONE, SOTA,SOTA, DIFF_A,  DIFF_A, NTSC,  NO_STEALING,    USES_WSYNC, HSC_YES, 31, 25, 320,  230, 0}, // title=Spire of the Ancients
  {"19844117863cd38d4e1e4cbc867ae599",  "SotA",                             CT_SUPLRG,    POKEY_NONE, SOTA,SOTA, DIFF_A,  DIFF_A, NTSC,  NO_STEALING,    USES_WSYNC, HSC_YES, 31, 25, 320,  230, 0}, // title=Spire of the Ancients
  {"cbb0746192540a13b4c7775c7ce2021f",  "Summer Games",                     CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Summer Games
  {"81cee326b99d6831de10a566e338bd25",  "Super Circus AA-NTSC-joy-4000",    CT_NORMAL, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  21, 256,  220, 0}, // title=Super Circus Atariage (Pokey 4000)    
  {"cc18e3b37a507c4217eb6cb1de8c8538",  "Super Huey UH-IX",                 CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Super Huey UH-IX
  {"7ab539bb0e99e1e5a1c89230bde64610",  "Super Pac-Man",                    CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  13, 256,  225, 0}, // title=Super Pac-Man
  {"59b5793bece1c80f77b55d60fb39cb94",  "Super Skatebordin'",               CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  NO_STEALING,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Super Skatebordin'
  {"44f862bca77d68b56b32534eda5c198d",  "Tank Command",                     CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Tank Command
  {"1af475ff6429a160752b592f0f92b287",  "Title Match Pro Wrestling",        CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Title Match Pro Wrestling
  {"a60e4b608505d1fb201703b266f754a7",  "Time Salvo",                       CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  24, 256,  230, 0}, // title=Time Salvo
  {"c3903ab01a51222a52197dbfe6538ecf",  "Tomcat F-14 Simulator",            CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Tomcat F-14 Simulator
  {"208ef955fa90a29815eb097bce89bace",  "Touchdown Football",               CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Touchdown Football
  {"8d64763db3100aadc552db5e6868506a",  "Tower Toppler",                    CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  16, 256,  230, 0}, // title=Tower Toppler
  {"79df20ee86a989e669158bcb9d113e8a",  "UniWarS",                          CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 9,  13, 282,  200, 0}, // title=UniWarS
  {"acf63758ecf3f3dd03e9d654ae6b69b7",  "Water Ski",                        CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Water Ski
  {"3799d72f78dda2ee87b0ef8bf7b91186",  "Winter Games",                     CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220, 0}, // title=Winter Games
  {"846751861993b907c512cc9c10c67035",  "Wizards Dungeon",                  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  26, 256,  224, 0}, // title=Wizards Dungeon
  {"05fb699db9eef564e2fe45c568746dbc",  "Xenophobe",                        CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  15, 22, 284,  234, 0}, // title=Xenophobe
  {"d7dc17379aa25e5ae3c14b9e780c6f6d",  "Xevious",                          CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  24, 256,  220, 0}, // title=Xevious
  {"d7dc17379aa25e5ae3c14b9e780c6f6d",  "Xevious X",                        CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  24, 256,  220, 0}, // title=Xevious X
  {"","",CT_NORMAL,0,0,0,0,0,0,0,0,0,0},
};

// ----------------------------------------------------------------------------
// Load
// ----------------------------------------------------------------------------
bool database_Load(byte *digest) 
{
  bool bFound = false;
  uint i;
  
  // -------------------------------------------------------------------------
  // This happens AFTER The rom is loaded and the header info (if any) has
  // been read out. Here we can adjust based on the hash table above... which
  // is mainly used for headerless roms and a few special cases were we want 
  // to correct the Y-offsets to make the game well centered/scaled on screen.
  // -------------------------------------------------------------------------
  if(database_enabled) 
  {
    /* Look up mapper in game list by md5sum */
    for(i = 0; strlen(game_list[i].digest); i++)
    {
      if (!strcmp(game_list[i].digest,(char *) digest))
      {
        cartridge_type          = game_list[i].cardtype;
        cartridge_pokey         = game_list[i].pokeyType;
        cartridge_controller[0] = game_list[i].cardctrl1;
        cartridge_controller[1] = game_list[i].cardctrl2;
        cartridge_region        = game_list[i].cardregion;
        cartridge_steals_cycles = game_list[i].steals_cycles;
        cartridge_uses_wsync    = game_list[i].uses_wsync;
        cartridge_hsc_enabled   = game_list[i].hsc;
        cartridge_xOffset       = game_list[i].xOffset;
        cartridge_yOffset       = game_list[i].yOffset;
        cartridge_xScale        = game_list[i].xScale;
        cartridge_yScale        = game_list[i].yScale;
        cartridge_diff1         = game_list[i].diff1;
        cartridge_diff2         = game_list[i].diff2;
        bForceFrameskip         = game_list[i].forceFrameskip;
        bFound = true;
          
        break;
      }
    }
    
   
    /* If we didn't find a definitive md5 match above, look up game by cart title in the .A78 header */
    if (!bFound)
    {
      for(i = 0; strlen(game_list[i].header_name); i++)
      {
        if (cartridge_region == NTSC)
        {
          if ( (!strcmp(game_list[i].header_name,(char *) cartridge_title)) ||
               (strstr((char *) cartridge_filename, game_list[i].header_name) != NULL) ) 
          {
            cartridge_type          = game_list[i].cardtype;
            cartridge_pokey         = game_list[i].pokeyType;
            cartridge_controller[0] = game_list[i].cardctrl1;
            cartridge_controller[1] = game_list[i].cardctrl2;
            cartridge_region        = game_list[i].cardregion;
            cartridge_steals_cycles = game_list[i].steals_cycles;
            cartridge_uses_wsync    = game_list[i].uses_wsync;
            cartridge_hsc_enabled   = game_list[i].hsc;
            cartridge_xOffset       = game_list[i].xOffset;
            cartridge_yOffset       = game_list[i].yOffset;
            cartridge_xScale        = game_list[i].xScale;
            cartridge_yScale        = game_list[i].yScale;
            cartridge_diff1         = game_list[i].diff1;
            cartridge_diff2         = game_list[i].diff2;
            bForceFrameskip         = game_list[i].forceFrameskip;
            bFound = true;
            break;
          }
        }
      }
    }
    
  }
  
  // Default scaling options below if not found... these are close enough...
  if (!bFound) 
  {
    cartridge_xOffset = 0;
    cartridge_yOffset = 22;
    cartridge_xScale  = 256;
    cartridge_yScale  = 220;
    cartridge_diff1 = DIFF_A;
    cartridge_diff2 = DIFF_A;
    bForceFrameskip = false;
  }
    
  cartridge_yOffset -= 9;  // To compensate for a smaller scren rendering in Region.c
  
  return true;
}


