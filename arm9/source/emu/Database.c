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

#define DATABASE_SOURCE "Database.cpp"

bool database_enabled = true;
typedef struct {
  char digest[33];
  uint cardtype;
  byte pokeyType;
  uint cardctrl1;
  uint cardctrl2;
  uint cardregion;
  bool steals_cycles;
  bool uses_wsync;
  bool hsc;
  int  xOffset;
  int  yOffset;
  int  xScale;
  int  yScale;
} Database_Entry;


Database_Entry game_list[] = {
  {"f5150c0fc1948832211e57852abb0c6e",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  22, 256,  220}, // title=7800 Utility Cart
  {"4332c24e4f3bc72e7fe1b77adf66c2b7",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=3D Asteroids
  {"0be996d25144966d5541c9eb4919b289",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Ace Of Aces
  {"aadde920b3aaba03bc10b40bd0619c94",  CT_SUPROM,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Ace Of Aces
  {"877dcc97a775ed55081864b2dbf5f1e2",  CT_SUPLRG,    POKEY_NONE, LGN, LGN,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Alien Brigade
  {"de3e9496cb7341f865f27e5a72c7f2f5",  CT_SUPLRG,    POKEY_NONE, LGN, LGN,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Alien Brigade
  {"212ee2a6e66d8bb7fbf26f343cc8dc19",  CT_SUPCAR,  POKEY_AT_450, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  17, 256,  200}, // title=Arkanoid 78b Demo (purposely set HSC to false - game HSC is buggy)
  {"07342c78619ba6ffcc61c10e907e3b50",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  25, 256,  220}, // title=Asteroids
  {"a65f79ad4a0bbdecd59d5f7eb3623fd7",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  22, 256,  220}, // title=Asteroids Deluxe
  {"3d38281ed8a8d8c7cd457a18c92c8604",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 30, 17, 320,  210}, // title=Astro Blaster
  {"a51e5df28a0fe8c52e9d28fb5f8e44a6",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 30, 16, 320,  210}, // title=Astro Fighter
  {"7cdfbe37634e7dcd4dc67db7edbcd3ba",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  30, 22, 320,  220}, // title=Baby Pac Man
  {"8fc3a695eaea3984912d98ed4a543376",  CT_NORMAL, POKEY_AT_4000, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Ballblazer
  {"b558814d54904ce0582e2f6a801d03af",  CT_NORMAL, POKEY_AT_4000, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Ballblazer
  {"42682415906c21c6af80e4198403ffda",  CT_SUPCAR, POKEY_AT_4000, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Barnyard Blaster
  {"babe2bc2976688bafb8b23c192658126",  CT_SUPCAR, POKEY_AT_4000, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Barnyard Blaster
  {"f5f6b69c5eb4b55fc163158d1a6b423e",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Basketbrawl
  {"fba002089fcfa176454ab507e0eb76cb",  CT_SUPROM,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Basketbrawl
  {"78b1061d651ef806becac1dd3fda29a0",  CT_NORMAL, POKEY_AT_4000, JOY, JOY,  NTSC,  STEAL_CYCLE,    SKIP_WSYNC, HSC_YES, 0,  16, 256,  220}, // title=Beef Drop (Final Atariage)
  {"4e325918a8b3bbcf2f9405040abcfc6d",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  26, 256,  220}, // title=BonQ (found on Atairage Age site)
  {"9fa7743a016c9b7015ee1d386326f88e",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    SKIP_WSYNC, HSC_YES, 0,  26, 256,  220}, // title=BonQ (final Atariage)
  {"5a09946e57dbe30408a8f253a28d07db",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  22, 256,  220}, // title=Centipede
  {"20660b667df538ec32a8e1b998438604",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  22, 256,  220}, // title=Centipede - Frameless Hack
  {"38c056a48472d9a9e16ebda5ed91dae7",  CT_NORMAL,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Centipede  
  {"93e4387864b014c155d7c17877990d1e",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Choplifter
  {"59d4edb0230b5acc918b94f6bc94779f",  CT_NORMAL,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Choplifter
  {"2e8e28f6ad8b9b9267d518d880c73ebb",  CT_SUPCAR, POKEY_AT_4000, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  20, 256,  220}, // title=Commando
  {"55da6c6c3974d013f517e725aa60f48e",  CT_SUPCAR, POKEY_AT_4000, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  20, 256,  220}, // title=Commando
  {"db691469128d9a4217ec7e315930b646",  CT_SUPCAR,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Crack'ed
  {"7cbe78fa06f47ba6516a67a4b003c9ee",  CT_SUPCAR,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Crack'ed
  {"a94e4560b6ad053a1c24e096f1262ebf",  CT_SUPLRG,    POKEY_NONE, LGN, LGN,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Crossbow
  {"63db371d67a98daec547b2abd5e7aa95",  CT_SUPLRG,    POKEY_NONE, LGN, LGN,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Crossbow
  {"179b76ff729d4849b8f66a502398acae",  CT_SUPCAR,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  21, 256,  220}, // title=Dark Chambers
  {"a2b8e2f159642c4b91de82e9a2928494",  CT_SUPCAR,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  21, 256,  220}, // title=Dark Chambers
  {"95ac811c7d27af0032ba090f28c107bd",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Desert Falcon
  {"2d5d99b993a885b063f9f22ce5e6523d",  CT_NORMAL,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Desert Falcon
  {"731879ea82fc0ca245e39e036fe293e6",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  22, 256,  220}, // title=Dig Dug
  {"408dca9fc40e2b5d805f403fa0509436",  CT_NORMAL,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Dig Dug
  {"5e332fbfc1e0fc74223d2e73271ce650",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Donkey Kong Jr
  {"4dc5f88243250461bd61053b13777060",  CT_NORMAL,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Donkey Kong Jr
  {"19f1ee292a23636bd57d408b62de79c7",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Donkey Kong
  {"8e96ef14ce9b5d84bcbc996b66d6d4c7",  CT_NORMAL,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Donkey Kong
  {"c3107d3e3e17d67e3a11d47a5946a4f3",  CT_SUPLRG, POKEY_AT_4000, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  20, 256,  220}, // title=Donkey Kong XM Demo (purposely set HSC to false - game HSC is buggy)
  {"543484c00ba233736bcaba2da20eeea9",  CT_ACTVIS,    POKEY_NONE, JOY, JOY,  NTSC,  NO_STEALING,    SKIP_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Double Dragon
  {"de2ebafcf0e37aaa9d0e9525a7f4dd62",  CT_ACTVIS,    POKEY_NONE, JOY, JOY,   PAL,  NO_STEALING,    SKIP_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Double Dragon
  {"b3143adbbb7d7d189e918e5b29d55a72",  CT_NORMAL,  POKEY_AT_450, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  23, 256,  220}, // title=Dungeon Stalker (homebrew)  
  {"2251a6a0f3aec84cc0aff66fc9fa91e8",  CT_ABSOLU,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=F-18 Hornet
  {"e7709da8e49d3767301947a0a0b9d2e6",  CT_ABSOLU,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=F-18 Hornet
  {"6287727ab36391a62f728bbdee88675c",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  22, 256,  220}, // title=Failsafe (homebrew)
  {"d25d5d19188e9f149977c49eb0367cd1",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Fatal Run
  {"23505651ac2e47f3637152066c3aa62f",  CT_SUPROM,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Fatal Run
  {"07dbbfe612a0a28e283c01545e59f25e",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Fight Night
  {"e80f24e953563e6b61556737d67d3836",  CT_SUPROM,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Fight Night
  {"cf76b00244105b8e03cdc37677ec1073",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  22, 256,  220}, // title=Food Fight
  {"de0d4f5a9bf1c1bddee3ed2f7ec51209",  CT_NORMAL,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Food Fight
  {"e7d89669a7f92ec2cc99d9663a28671c",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 30, 15, 320,  205}, // title=Frenzy (with Berzerk) (homebrew)
  {"6053233cb59c0b4ca633623fd76c4576",  CT_NORMAL, POKEY_AT_4000, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 34, 16, 320,  205}, // title=Froggie (homebrew)
  {"fb8d803b328b2e442548f7799cfa9a4a",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  22, 256,  220}, // title=Galaga
  {"f5dc7dc8e38072d3d65bd90a660148ce",  CT_NORMAL,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Galaga
  {"06204dadc975be5e5e37e7cc66f984cf",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Gato                  
  {"fd9e78e201b6baafddfd3e1fbfe6ba31",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Hat Trick                       
  {"0baec96787ce17f390e204de1a136e59",  CT_NORMAL,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Hat Trick                       
  {"c3672482ca93f70eafd9134b936c3feb",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Ikari Warriors
  {"8c2c2a1ea6e9a928a44c3151ba5c1ce3",  CT_SUPROM,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Ikari Warriors
  {"baebc9246c087e893dfa489632157180",  CT_SUPRAM,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Impossible Mission
  {"1745feadabb24e7cefc375904c73fa4c",  CT_SUPRAM,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Impossible Mission
  {"80dead01ea2db5045f6f4443faa6fce8",  CT_SUPRAM,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Impossible Mission
  {"045fd12050b7f2b842d5970f2414e912",  CT_SUPRAM,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Jinks
  {"dfb86f4d06f05ad00cf418f0a59a24f7",  CT_SUPRAM,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Jinks
  {"f18b3b897a25ab3885b43b4bd141b396",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  22, 256,  220}, // title=Joust                           
  {"f2dae0264a4b4a73762b9d7177e989f6",  CT_NORMAL,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Joust                           
  {"e54edc299e72d22d0ba05d16f3393e8c",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  22, 256,  220}, // title=Jr Pac-Man  
  {"c3a5a8692a423d43d9d28dd5b7d109d9",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Karateka                        
  {"5e0a1e832bbcea6facb832fde23a440a",  CT_SUPCAR,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Karateka                        
  {"17b3b764d33eae9b5260f01df7bb9d2f",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Klax       
  {"f57d0af323d4e173fb49ed447f0563d7",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  NO_STEALING,    SKIP_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Kung Fu Master
  {"2931b75811ad03f3ac9330838f3d231b",  CT_NORMAL,    POKEY_NONE, JOY, JOY,   PAL,  NO_STEALING,    SKIP_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Kung Fu Master
  {"431ca060201ee1f9eb49d44962874049",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Mario Bros.
  {"d2e861306be78e44248bb71d7475d8a3",  CT_NORMAL,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Mario Bros.
  {"37b5692e33a98115e574185fa8398c22",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Mat Mania Challenge
  {"6819c37b96063b024898a19dbae2df54",  CT_SUPROM,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Mat Mania Challenge
  {"f2f5e5841e4dda89a2faf8933dc33ea6",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Mean 18 Ultimate Golf
  {"2e9dbad6c0fa381a6cd1bb9abf98a104",  CT_SUPROM,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Mean 18 Ultimate Golf
  {"bedc30ec43587e0c98fc38c39c1ef9d0",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Meltdown
  {"c80155d7eec9e3dcb79aa6b83c9ccd1e",  CT_SUPROM,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Meltdown
  {"c3f6201d6a9388e860328c963a3301cc",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  21, 256,  220}, // title=Meteor Shower
  {"bc1e905db1008493a9632aa83ab4682b",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Midnight Mutants
  {"6794ea31570eba0b88a0bf1ead3f3f1b",  CT_SUPROM,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Midnight Mutants
  {"017066f522908081ec3ee624f5e4a8aa",  CT_SUPLRG,    POKEY_NONE, JOY, JOY,  NTSC,  NO_STEALING,    SKIP_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Missing in Action
  {"181a9978d9da7a7e21f770808cc681f2",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  25, 320,  230}, // title=Merlain
  {"9ff38ea62004201d870caa8bd9463525",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 30, 13, 320,  210}, // title=Moon Cresta
  {"3bc8f554cf86f8132a623cc2201a564b",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Motor Psycho
  {"5330bfe428a6b601b7e76c2cfc4cd049",  CT_SUPROM,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Motor Psycho
  {"fc0ea52a9fac557251b65ee680d951e5",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  22, 256,  220}, // title=Ms. Pac-Man                     
  {"56469e8c5ff8983c6cb8dadc64eb0363",  CT_NORMAL,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Ms. Pac-Man                     
  {"220121f771fc4b98cef97dc040e8d378",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Ninja Golf
  {"ea0c859aa54fe5eaf4c1f327fab06221",  CT_SUPROM,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Ninja Golf
  {"74569571a208f8b0b1ccfb22d7c914e1",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  12, 256,  220}, // title=One On One
  {"8dba0425f0262e5704581d8757a1a6e3",  CT_NORMAL,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  12, 256,  220}, // title=One On One
  {"5013b69cb05b21a1194ce48517df7bfc",  CT_NORMAL, POKEY_AT_4000, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  19, 256,  216}, // title=Pac-Man Collection (homebrew)
  {"044657294450c869c45e7ef61f4870de",  CT_SUPCAR,  POKEY_AT_450, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  27, 256,  220}, // title=Pac-Man Collection 40th Anniversary Edition (homebrew)
  {"1a5207870dec6fae9111cb747e20d8e3",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Pete Rose Baseball
  {"386bded4a944bae455fedf56206dd1dd",  CT_NORMAL,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Pete Rose Baseball
  {"ec206c8db4316eb1ebce9fc960da7d8f",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Pit Fighter                     
  {"33aea1e2b6634a1dec8c7006d9afda22",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  18, 256,  220}, // title=Planet Smashers
  {"2837a8fd49b7fc7ccd70fd45b69c5099",  CT_SUPROM,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  18, 256,  220}, // title=Planet Smashers
  {"584582bb09ee8122e7fc09dc7d1ed813",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  35, 22, 320,  230}, // title=Pole Position II                
  {"865457e0e0f48253b08f77b9e18f93b2",  CT_NORMAL,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  36, 22, 320,  240}, // title=Pole Position II                
  {"1745feadabb24e7cefc375904c73fa4c",  CT_SUPRAM,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Possible Mission
  {"0710aa2620435230da6a7cf3f620210d",  CT_SUPLRG,  POKEY_AT_450, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  17, 256,  210}, // title=Popeye Demo 2.7f (homebrew)
  {"ac03806cef2558fc795a7d5d8dba7bc0",  CT_ACTVIS,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Rampage
  {"bfad016d6e77eaccec74c0340aded8b9",  CT_SUPCAR,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Realsports Baseball
  {"8f7eb10ad0bd75474abf0c6c36c08486",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Rescue On Fractalus
  {"66ecaafe1b82ae68ffc96267aaf7a4d7",  CT_NORMAL,    POKEY_NONE, TWIN,TWIN, NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  22, 256,  220}, // title=Robotron     
  {"a3a85e507d6f718972b1464ce1aaf8a4",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 30, 13, 320,  205}, // title=Scramble (homebrew)
  {"980c35ae9625773a450aa7ef51751c04",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Scrapyard Dog
  {"53db322c201323fe2ca8f074c0a2bf86",  CT_SUPROM,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Scrapyard Dog
  {"b697d9c2d1b9f6cb21041286d1bbfa7f",  CT_SUPROM, POKEY_AT_4000, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Sentinel
  {"5469b4de0608f23a5c4f98f331c9e75f",  CT_SUPROM, POKEY_AT_4000, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Sentinel
  {"9bd70c06d3386f76f8162881699a777a",  CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  22, 256,  220}, // title=Serpentine (homebrew)
  {"771cb4609347657f63e6f0eb26036e35",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  22, 256,  220}, // title=Space Duel (homebrew)
  {"6adf79558a3d7f5beca1bb8d34337417",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 30, 16, 320,  205}, // title=Space Invaders (Homebrew)
  {"cbb0746192540a13b4c7775c7ce2021f",  CT_SUPRAM,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Summer Games
  {"81cee326b99d6831de10a566e338bd25",  CT_NORMAL, POKEY_AT_4000, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  21, 256,  220}, // title=Super Circus Atariage (Pokey 4000)    
  {"cc18e3b37a507c4217eb6cb1de8c8538",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Super Huey UH-IX
  {"162f9c953f0657689cc74ab20b40280f",  CT_NORMAL,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Super Huey UH-IX
  {"59b5793bece1c80f77b55d60fb39cb94",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Super Skatebordin'
  {"95d7c321dce8f57623a9c5b4947bb375",  CT_NORMAL,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Super Skatebordin'
  {"44f862bca77d68b56b32534eda5c198d",  CT_SUPCAR,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Tank Command
  {"1af475ff6429a160752b592f0f92b287",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Title Match Pro Wrestling
  {"3bb9c8d9adc912dd7f8471c97445cd8d",  CT_NORMAL,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Title Match Pro Wrestling
  {"c3903ab01a51222a52197dbfe6538ecf",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Tomcat F-14 Simulator
  {"682338364243b023ecc9d24f0abfc9a7",  CT_NORMAL,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Tomcat F-14 Simulator
  {"208ef955fa90a29815eb097bce89bace",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Touchdown Football
  {"d12e665347f354048b9d13092f7868c9",  CT_SUPRAM,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Tower Toppler
  {"32a37244a9c6cc928dcdf02b45365aa8",  CT_SUPRAM,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Tower Toppler
  {"acf63758ecf3f3dd03e9d654ae6b69b7",  CT_SUPCAR,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Water Ski
  {"3799d72f78dda2ee87b0ef8bf7b91186",  CT_SUPRAM,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Winter Games
  {"05fb699db9eef564e2fe45c568746dbc",  CT_SUPROM,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Xenophobe
  {"70937c3184f0be33d06f7f4382ca54de",  CT_SUPROM,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Xenophobe
  {"d7dc17379aa25e5ae3c14b9e780c6f6d",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  NTSC,  STEAL_CYCLE,    USES_WSYNC, HSC_YES, 0,  22, 256,  220}, // title=Xevious
  {"b1a9f196ce5f47ca8caf8fa7bc4ca46c",  CT_NORMAL,    POKEY_NONE, JOY, JOY,   PAL,  STEAL_CYCLE,    USES_WSYNC, HSC_NO,  0,  22, 256,  220}, // title=Xevious

  {"",CT_NORMAL,0,0,0,0,0,0,0,0,0,0},
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
    /* Look up mapper in game list */
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
        bFound = true;
        break;
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
  }
  
  return true;
}


