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
#include "../config.h"

Database_Entry myCartInfo __attribute__((section(".dtcm")));
extern uint cartridge_size;

#if 0
7800 XMAS - Santa vs The Nightmares (20201219u1) (1A114E52).a78
63f9217bbd3ba91c1fc09fda18de9275  -
Aerial (20240715) (1E4593DB).a78
7f207c119a8898d3c2df919bac506ba2  -
Ah Zombies (v03) (20221118) (D0DD190B).a78
909f8879f9b8c47f3b487e402c70227c  -
Alpha Race (20150920) (DC81DDD0).a78
10ff87ebfeec99858cc80293b0c1686e  -
Alpine Avenger (v0_75) (20240711) (E6C07ABB).a78
a03f3afae182a4639e41554df8e37c34  -
Antiair (20240713) (BFCE8DEE).a78
b1c5010c81aa7e3925f49040df066ca0  -
Apple Snaffle (v1_30F) (20100803) (3DD0C2B7).a78
510ea66b6375a848a21db019b36078dd  -
Ascend (20240714) (D581EF7B).a78
15f37e0640c4f255fe33d2d45d3cbdf2  -
Atarimas (20240815) (4A41D546).a78
21bd75947ef58b236d1c3021e2790921  -
Atlas (20111023) (045FE06D).a78
a6c02ae92d6937cb885f6909c0a8a2e1  -
Baseball Dice (v0_1) (20240314) (B16769C5).a78
8d4d60928563093a8ede34095b473f7c  -
Battlot (20240712) (A4199A30).a78
813e5f0b367eeaff15730bfe1c31950e  -
Binary Land (20210208) (AD20DEAA).a78
a541b16e95619afa4960487c23a9a9ff  -
Black Forest (20241219) (7041918B).a78
a0d1816bcf2db91ac2cdf3c3a486d565  -
BlocDrop (20130310) (9882A876).a78
fb805af5b8ee40203df122e21dd94018  -
Block'Em Sock'Em (Demo) (20231129) (FD0F929A).a78
13fd76aa7a041a8f489e39b2c30517e1  -
Blowsub (20240815) (92D9769F).a78
0d161c5ce8d121a0b575aaf65248ab14  -
Bomber (20241024) (A422697C).a78
cba39f2b3656dc9c7cfa5b95c77c15ae  -
Bomber Hero (20241114) (5B32AEFF).a78
adead4210dfde21b3738924823ae477e  -
Bootskell (20240712) (7B3DFD62).a78
c2bfbca2a6858a4bfebdfec3df8d411a  -
Breakout (v08) (20230315) (55D20479).a78
c0b354044148bf9c5b3427e9c3c2702b  -
Bubble Bobble (v75a) (20240110) (D4DCBFAD).a78
2050d5c1176570e27886db202f06a123  -
Bullet Festival (20150831) (ED33EDA6).a78
9f5bbb4b42b4042f6f465953710788b6  -
Cacorm (20240711) (7419E768).a78
b7f1e5a2c6e1bbc6109c8f6a790de106  -
Captain Comic (20200926) (F26C9CDD).a78
51d2a23152ad23d734d7ef1e36fa367d  -
Cavit (20240714) (885C29AF).a78
d2c86a1023bb92db52f082f9910e04ca  -
Chase (Fabrizio Caruso) (20241024) (69F0CC17).a78
7ead9fbdf35b8c5d54e368a17e26895d  -
Chase (Lillapojkenpaon) (20201231) (C2C34433).a78
a4b5d742860beb25c29def4530194c1e  -
Chicago Basement (20160914) (709852DB).a78
993d034677b88612835bc8e566c578be  -
Clean Sweep (20160208) (4BD3D55D).a78
3f87a884858870efa98731834ba42c0e  -
Conversion - batari Basic Demo (20210409) (32867785).a78
5eda75d54e9aa92992513d9e691d5a5c  -
Cosmic Cabbie (20201229) (BAA151F9).a78
60144e642ed0ace13e50e9b19e6ae723  -
Court Pooper (Demo) (20220617) (FD9FEDB3).a78
26b55d71ba1d62fb2c71efd86c83f030  -
Cracky (20240710) (59677A47).a78
c2165dd428b98fcf2538280082d936a5  -
Crazy Tank (20180310) (54089FF2).a78
156be6e5854b2681a773ffb52e42b079  -
Cubicle Chaos (20141205) (9D0B4951).a78
bfb5ac5f3de019e7aff8146bcc1a3142  -
Cyb Ur (RC1) (20190526) (88846A7E).a78
40bd21c9698c6b8e71b703f860c11359  -
Defender (20220128) (03522A22).a78
c19454c55a15d5676a960a0615b4aa6b  -
Destroyer 7800 (v1_3) (20230331) (82A8141D).a78
1b1a9d0676af86bc92dd0c5cef1e1877  -
Donald T's Breakout (20230310) (6BED09E2).a78
581bc87ef23250acf045d83957126a04  -

Drelbs (4000) (20230724) (EE4CCE69).a78
e36725e9e8f3963a17e521a89dea1a85  -
Drone Patrol (Final Rev C) (800) (20240709) (72B02356).a78
8b827c3c29bb727c58248a61bff7df7c  -
DrunkWitch (20221231) (F745683C).a78
c73d04c367d2bed7f1e57bddd0662f67  -
Ducks Away (Joystick) (20231231) (EFD39322).a78
5490ac362a27ddca30051d669bf90fc3  -
Ducks Away (Lightgun) (20231231) (EFD39322).a78
5490ac362a27ddca30051d669bf90fc3  -
Easter Klondike Solitaire (20241222) (ADF17E2F).a78
40fd267fc954985c0b96c640d304cdd8  -
ET Book Cart (Demo) (20150524) (32CE5325).a78
253cd7325a1f454b68eede5136805f30  -
Fat Axl (20160613) (9D14DC33).a78
0338fffc3391f74d30e6ab391d1b4c25  -
Flappy Bird (Beta) (20141221) (E6E1C828).a78
bac78b53f4f6b3f68c0f8073f0073179  -
Frantic Fisher (20240128) (80D54A6D).a78
f495b68611463773276072fa18337d2b  -
Freeway (v0_88) (20221010) (7D03A3B2).a78
c8efc272ca334fa8196ab0542882d78e  -
Get Lost! (20210407) (06F069D1).a78
5483748fcf63464250277cae99348221  -
Ghosts'n Goblins (20211205u1) (EBAA96BB).a78
ffc704d30566a2c24d6e9d845e4c9f11  -
Gorf (20091213) (AD5ACC79).a78
3d12489c553cb1a90c8ebd6534383fa1  -
Guntus (20240714) (45CCA116).a78
eaf5fb9325708d18c8a91ecbdc9cad8a  -
Harpy's Curse (Beta G) (20230920) (F788BD6B).a78
c8405f2f776587117dcfcc2d3993a698  -
Harry's Hen House (1_07F) (20100117) (983BE828).a78
3f14e9b07f9809867facd592ddcda41f  -
Heartlight (v0_98c) (20221108) (BC6A8F89).a78
8362cfc1a62d8172c7adb9d867094b5d  -
Hearty Manslapper (20161129) (349136F9).a78
25ce1f5dfc909bcb46086e414d6a0f30  -
High Card Draw (20220303) (A498EB69).a78
fbf0154e8d8be2a618e61676ba7d8add  -
Hollywood Brawler (20160811) (7ACF469F).a78
26578cedccc6c61984d5a55048a42d7d  -
Home Run Baseball (v0_2) (20240315) (74564BE7).a78
6c3f1e91053f6f3c77b888648f741009  -
Hopman (20240713) (2A0D1E6B).a78
7f52723e84ccd8d22c73cf1f2c2bec8c  -
I C B M (20230507) (C3521748).a78
e5bde14cbfa45d791587cbbef309c98b  -
Impetus (20240717) (A0A0DA0C).a78
6a12c7c8c8851598e1e012313e976e0c  -
I Ran (Demo 01) (20230429) (66A752DF).a78
d85564ef05d607df034ba0231989f5c1  -
Iron Grip - Destroyer Of Worlds (20210518) (BCA56BC5).a78
571875af24243cfea7b13796096884c1  -
Jacks or Better (v1_37) (20221210) (3495BADA).a78
64717953882a4aebe463f453ca00b9be  -
Krull (v06) (20220319) (5168B651).a78
cf521d4eb0d74e87f24e65a32ba15038  -
LadyLady (20200810) (DA306095).a78
02da71dc8beac00dd556d7b33b1edfb6  -
Last Stand Halloween - Holiday Nightmare (Beta 1_1) (20231112) (571A8F8F).a78
37ce9113b420d8aa22f28ddaca2b794c  -
Lemmings Squidginator (20200330) (954062A5).a78
8dda6b319c28cf4e005c9d33de1b04be  -
Lift (20240711) (D34FD674).a78
6765d3723f4256d680d374327f9aa42c  -
Lola Lines (RC2) (20240503) (0CEA2B1B).a78
beb7c20cb567904232040f0c2b3d0f17  -
Mazy (20240715) (47BE8090).a78
2f79bedb51300de9625ad8781dce07c2  -
Monster Maze (20220306) (4AFE78EA).a78
1c860298a8966cc8e176ab8453b172c3  -
Morf (20220314) (BC2124C6).a78
d1b56eae7227c12d0122bb84925c89c0  -
Ms Pac-Man Twin (20160208) (768817BE).a78
a69347c8a681b8e94f79d8d848998007  -
Neuras (20240712) (993046ED).a78
88b0d5f3ec5ada94585f999e9944f182  -
Number Crusher (20210523u1) (33090508).a78
da4b11103175dbf72dd631d6faf78946  -
Orion Assault (Beta 01) (20231112) (POKEY 450) (625B896E).a78
89678ebc7d4605e86cffd2a1ec7cc7b9  -
Osotos (20240827) (B223E461).a78
2c4a302b860fe6cc35ed8e149c4b4049  -
Panda Racer (20151224) (7D702BC6).a78
b5c9f0bf5b5763a923b7f370376b1849  -
PentaGo (Demo) (20230422) (1C2CF2C5).a78
6ac5a7f8b6a3198ed08abb9866753763  -
PentaGo (Tall) (Demo) (20230422) (AAA1069E).a78
badb455b8087b39d66890fe4dd11fe9b  -
Pineapple (20220619) (70991C78).a78
5e92f7926a0d4a8603b1a966cb79e8b6  -
Pirate Cove (20220415) (6A6678BA).a78
a6b91a759b34f6c61462195fc783b6db  -
Plink (20201213) (F945CBD9).a78
851f901cbc78824f6754f362c00ae648  -
Poetiru (20150901) (65804DD9).a78
f0abd58c1de2ba3e577f90c3d52dd7fc  -
Pong (v0_7) (20221210) (8FD41239).a78
514b0e645b4771ba43cd00f679315224  -
Rainbow (20231227) (E8A2E4E3).a78
0484e954908be9d597a7c204321006a8  -
RatTrap (Demo) (20230307) (4492F203).a78
d9dce76429009ac8facf1af9f3ef33a5  -
Realm Of No, The (Book Cart) (20161224) (4C327C8C).a78
4d00e5bbcf1e39e4fb94e6e5cf127f93  -
ReZolve (20141207) (F7D8F766).a78
f2047b149e72be8f97e9671314748ec4  -
Rider of the Night (20190711) (5A2A2DCE).a78
e1b01dd7e842d2b682ef48f689d5a4eb  -
Robbo (20160513) (CFE2179A).a78
b6561537290e6e25e1249394366c3c63  -
Robot Finds Kitten (20050619) (FCB76ABE).a78
9646d482a8ad187ae52cd21c0de365fb  -
Robot Finds Kitten (224 Lines) (20220331) (A7D9EFB4).a78
8994a0560ab6d452cfe59e33de145a3d  -
Robot Zed (20231025) (EA470FC3).a78
f9789bbdae0ddf64f8da3e8073ad3ae2  -
Roof Pooper (20161006) (2ADAB9A3).a78
45c33c3799539545e8bf37719eb6f88c  -
Ruptus (20240715) (056C6D48).a78
9de83d2455c7f8f43b8163ed4de704bd  -
Santa Simon (Valid Signature) (20170511) (5D8CFDB7).a78
2582f89d53a899db0b70160ad5762cb7  -
Shoot (20241024) (7729AF84).a78
fa94f3e9db1c3f3ebb80811e77aa5710  -
Shoot the UFO 2015 (20150328) (15EEAD4B).a78
19586366f625db75b145f62c1d668f8e  -
Shotgun (20241208) (4708730E).a78
5f0eb14a30b9dd4c2e5998660a56a8a9  -
Shuriken (20241024) (26571C05).a78
142537fac731f9ad3a1ab0821e2c2315  -
Side Bounce (20241022) (EBBFC600).a78
19c6381b84d3e5fe7024e2f72b44f6d7  -
Sky Scraper 2115 (6f) (20171019) (66574A8B).a78
1c8139c584e1cf5c6afdd2f3455a2446  -
Smasteroids (20140712) (8ABCA713).a78
4cabc9042ec5097a3136cb680710ec52  -
Snake (20241024) (FE9C85D9).a78
feac5299663d4d30f9127ed8abfcd677  -
Soccer 2 (20140614) (ECF6AA7E).a78
5fb5d1452de1c3fb2d74f8982cf8955d  -
Space Chess (Demo) (20230127) (224F3D84).a78
42ff0f1c55ba940910143c2d50e1406d  -
Space Junk (1b) (20170927) (9F4B7138).a78
cd358a8502bd65702da040f78575fad0  -
Space Peril (NTSC) (v9) (20240831) (4F7BE53A).a78
9bc3907f57f13ad4cb89e7c630f51b2f  -
Space Race (v0_96) (20220925) (E59C249D).a78
9a11430d4eaeb3f7375bc2b92353c5db  -
Spacewar! (20070321) (38DB0756).a78
f7f53ef9cfb32318e8bc11a9f33390c4  -
Stinger (20241024) (865692A5).a78
2c6c7bf46237c7c5a50a9bc1f843c99c  -
StoneAge (20241203) (6A92022E).a78
d2f1736309d8bf4336397f586552e1dd  -
Super Pro Racers (20240819) (A8CD6EDC).a78
685c405036c05453b81a06a18c69de1f  -
Tempest (BBC X-Port 1) (v1_00F) (20100324) (00719763).a78
9cb3848416e39ebf642357dbee3e5970  -
The Big Burrito (20170823) (DA6980F4).a78
a8b71da3375f73c801a5315c6c883dca  -
Time Machine, The (Book Cart) (V1_02) (20230401) (B0FE53BE).a78
43d738e5989ffff9a40e2e62f121b2fc  -
Trex (20241024) (634E51A7).a78
1dd1f3faeaefd236f005b7611568107f  -
Tubes (20050809) (D2A154B0).a78
c5208b29102215b872c6c5ee0313fca6  -
Turret Turmoil - Dreadnought Assault (r0_2) (20240923) (4129663A).a78
4a0eeb1294044a458820d2274b9f8fd5  -
UFO! (20091213) (314E2891).a78
b1ec7bd809ab3deb746c5a5eb2efaecb  -
uSokoban (20220818) (A6127658).a78
d23c8b7b7339fd62dd6aba8723549abe  -
V Blank - Chapter 1 - Into The Void (20210326u1) (9EC1B708).a78
af96ce75837c72148d8e4413313ed1d3  -
Verbix (20241024) (328B3DB9).a78
487e6234c3ab9a5aaad5a886e5ee022c  -
Vong (20091015) (F9F00D58).a78
84d5120717c8dbaf0e3bf99be5106848  -
WarBirds (20091213) (5C527238).a78
c62632545c91823f72f6f14b19766804  -
Wasp! (Standard Edition) (20090923) (CCDC2DBF).a78
412cc5bfa08bd03244b9c4e8d46cd0a0  -
Wilf (Preview 1) (20231230) (C8BB0D25).a78
dbd068a800955089a30e46bea872794f  -
WORDLE (20220308) (02C9EA66).a78
71ce8910b0efd5d0014a9695cce3b7ad  -
Worm! (v1_16F) (20100307) (39296DA2).a78
6813ffff510f930c867b3f0aba78ac85  -
Yewdow (20240713) (AD176C55).a78
9230d0780c43d9ab14154545f7b45bd2  -

#endif

// To get the md5sum of an .a78 file with header in Linux:  dd bs=1 skip=128 if=somefile.a78 | md5sum

Database_Entry game_list[] = {
    
  // The original NTSC Commercial Games
  {"0be996d25144966d5541c9eb4919b289",  "Ace of Aces",                      CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=Ace Of Aces
  {"877dcc97a775ed55081864b2dbf5f1e2",  "Alien Brigade",                    CT_SUPLRG,    POKEY_NONE, LGN, LGN,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  7,  22, 264,  230, 0}, // title=Alien Brigade
  {"07342c78619ba6ffcc61c10e907e3b50",  "Asteroids",                        CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  160,  0,  17, 256,  230, 0}, // title=Asteroids
  {"8fc3a695eaea3984912d98ed4a543376",  "Ballblazer",                       CT_NORMAL, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   240,  6,  12, 262,  220, 0}, // title=Ballblazer
  {"42682415906c21c6af80e4198403ffda",  "Barnyard Blaster",                 CT_SUPCAR, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=Barnyard Blaster
  {"f5f6b69c5eb4b55fc163158d1a6b423e",  "Basketbrawl",                      CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=Basketbrawl
  {"5a09946e57dbe30408a8f253a28d07db",  "Centipede",                        CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  160,  24, 16, 300,  230, 0}, // title=Centipede
  {"93e4387864b014c155d7c17877990d1e",  "Choplifter",                       CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   190,  0,  12, 256,  220, 0}, // title=Choplifter
  {"2e8e28f6ad8b9b9267d518d880c73ebb",  "Commando",                         CT_SUPCAR, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  218, 0}, // title=Commando
  {"db691469128d9a4217ec7e315930b646",  "Crack'ed",                         CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   170,  0,  18, 256,  229, 0}, // title=Crack'ed
  {"a94e4560b6ad053a1c24e096f1262ebf",  "Crossbow",                         CT_SUPLRG,    POKEY_NONE, LGN, LGN,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  9,  17, 272,  234, 0}, // title=Crossbow
  {"179b76ff729d4849b8f66a502398acae",  "Dark Chambers",                    CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   130,  0,  13, 256,  219, 0}, // title=Dark Chambers
  {"95ac811c7d27af0032ba090f28c107bd",  "Desert Falcon",                    CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   190,  6,  19, 261,  234, 0}, // title=Desert Falcon
  {"731879ea82fc0ca245e39e036fe293e6",  "Dig Dug",                          CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  160,  0,  12, 256,  220, 0}, // title=Dig Dug
  {"5e332fbfc1e0fc74223d2e73271ce650",  "Donkey Kong Jr",                   CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,  14, 256,  220, 0}, // title=Donkey Kong Jr
  {"743c47f500d095f2b683437abea2f793",  "Donkey Kong Jr",                   CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,  14, 256,  220, 0}, // title=Donkey Kong Jr (alt) 
  {"19f1ee292a23636bd57d408b62de79c7",  "Donkey Kong Orig",                 CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,  14, 256,  220, 0}, // title=Donkey Kong
  {"543484c00ba233736bcaba2da20eeea9",  "Double Dragon",                    CT_ACTVIS,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   130,  0,  12, 256,  220, 0}, // title=Double Dragon
  {"2251a6a0f3aec84cc0aff66fc9fa91e8",  "F-18 Hornet",                      CT_ABSOLU,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,  12, 256,  220, 0}, // title=F-18 Hornet
  {"d25d5d19188e9f149977c49eb0367cd1",  "Fatal Run",                        CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   155,  0,  12, 256,  220, 0}, // title=Fatal Run
  {"07dbbfe612a0a28e283c01545e59f25e",  "Fight Night",                      CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=Fight Night
  {"cf76b00244105b8e03cdc37677ec1073",  "Food Fight",                       CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  140,  0,  17, 255,  227, 0}, // title=Food Fight
  {"fb8d803b328b2e442548f7799cfa9a4a",  "Galaga",                           CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  140,  0,  12, 256,  229, 0}, // title=Galaga
  {"fd9e78e201b6baafddfd3e1fbfe6ba31",  "Hat Trick",                        CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   240,  0,  17, 256,  227, 0}, // title=Hat Trick
  {"c3672482ca93f70eafd9134b936c3feb",  "Ikari Warriors",                   CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   180,  0,  17, 256,  230, 0}, // title=Ikari Warriors
  {"baebc9246c087e893dfa489632157180",  "Impossible Mission",               CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   155,  0,  12, 256,  220, 0}, // title=Impossible Mission
  {"045fd12050b7f2b842d5970f2414e912",  "Jinks",                            CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   170,  3,  12, 261,  234, 1}, // title=Jinks
  {"f18b3b897a25ab3885b43b4bd141b396",  "Joust",                            CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  140,  0,  17, 256,  234, 0}, // title=Joust                           
  {"c3a5a8692a423d43d9d28dd5b7d109d9",  "Karateka",                         CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   250,  0,  12, 256,  220, 0}, // title=Karateka                        
  {"f57d0af323d4e173fb49ed447f0563d7",  "Kung Fu Master",                   CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   120,  22, 17, 276,  225, 0}, // title=Kung Fu Master
  {"f2f5e5841e4dda89a2faf8933dc33ea6",  "Mean 18 Ultimate Golf",            CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=Mean 18 Ultimate Golf
  {"bc1e905db1008493a9632aa83ab4682b",  "Midnight Mutants",                 CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,  13, 256,  226, 0}, // title=Midnight Mutants
  {"431ca060201ee1f9eb49d44962874049",  "Mario Bros",                       CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   135,  0,  14, 256,  220, 0}, // title=Mario Bros.
  {"37b5692e33a98115e574185fa8398c22",  "Mat Mania Challenge",              CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,  12, 256,  220, 0}, // title=Mat Mania Challenge
  {"bedc30ec43587e0c98fc38c39c1ef9d0",  "Meltdown",                         CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=Meltdown
  {"3bc8f554cf86f8132a623cc2201a564b",  "Motor Psycho",                     CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=Motor Psycho
  {"fc0ea52a9fac557251b65ee680d951e5",  "Ms. Pac-Man",                      CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  160,  0,  17, 256,  224, 0}, // title=Ms. Pac-Man   
  {"220121f771fc4b98cef97dc040e8d378",  "Ninja Golf",                       CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  10, 20, 270,  234, 0}, // title=Ninja Golf
  {"74569571a208f8b0b1ccfb22d7c914e1",  "One On One",                       CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   250,  0,  12, 256,  224, 0}, // title=One On One
  {"1a5207870dec6fae9111cb747e20d8e3",  "Pete Rose Baseball",               CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=Pete Rose Baseball
  {"33aea1e2b6634a1dec8c7006d9afda22",  "Planet Smashers",                  CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   170,  0,  15, 256,  226, 0}, // title=Planet Smashers
  {"584582bb09ee8122e7fc09dc7d1ed813",  "Pole Position II",                 CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   200,  35, 12, 320,  230, 0}, // title=Pole Position II                
  {"ac03806cef2558fc795a7d5d8dba7bc0",  "Rampage",                          CT_ACTVIS,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   130,  0,  12, 256,  220, 0}, // title=Rampage
  {"383ed9bd1efb9b6cb3388a777678c928",  "Realsports Baseball",              CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 1}, // title=Realsports Baseball
  {"66ecaafe1b82ae68ffc96267aaf7a4d7",  "Robotron",                         CT_NORMAL,    POKEY_NONE, TWIN,TWIN, DIFF_A,  DIFF_A, NTSC,  HSC_YES,  200,  5,  13, 270,  234, 0}, // title=Robotron     
  {"980c35ae9625773a450aa7ef51751c04",  "Scrapyard Dog",                    CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   130,  8,  18, 265,  234, 0}, // title=Scrapyard Dog
  {"cbb0746192540a13b4c7775c7ce2021f",  "Summer Games",                     CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  13, 256,  220, 0}, // title=Summer Games
  {"cc18e3b37a507c4217eb6cb1de8c8538",  "Super Huey UH-IX",                 CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   120,  0,  12, 256,  220, 0}, // title=Super Huey UH-IX
  {"59b5793bece1c80f77b55d60fb39cb94",  "Super Skatebordin'",               CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_NO,   120,  0,  12, 256,  220, 0}, // title=Super Skatebordin'
  {"5c4f752371a523f15e9980fea73b874d",  "Tank Command",                     CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=Tank Command
  {"1af475ff6429a160752b592f0f92b287",  "Title Match Pro Wrestling",        CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=Title Match Pro Wrestling
  {"c3903ab01a51222a52197dbfe6538ecf",  "Tomcat F-14 Simulator",            CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   130,  0,  12, 256,  220, 0}, // title=Tomcat F-14 Simulator
  {"208ef955fa90a29815eb097bce89bace",  "Touchdown Football",               CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=Touchdown Football
  {"8d64763db3100aadc552db5e6868506a",  "Tower Toppler",                    CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,   8, 320,  234, 1}, // title=Tower Toppler
  {"427cb05d0a1abb068998e2760d77f4fb",  "Water Ski",                        CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   150,  0,   3, 256,  197, 0}, // title=Water Ski
  {"3799d72f78dda2ee87b0ef8bf7b91186",  "Winter Games",                     CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   170,  0,  13, 256,  220, 0}, // title=Winter Games
  {"90fa275f9f2a65b341796e11b2f551af",  "Winter Games",                     CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   170,  0,  13, 256,  220, 0}, // title=Winter Games (alt)  
  {"05fb699db9eef564e2fe45c568746dbc",  "Xenophobe",                        CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   120,  15, 13, 284,  234, 0}, // title=Xenophobe
  {"d7dc17379aa25e5ae3c14b9e780c6f6d",  "Xevious",                          CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  205,  0,  16, 256,  220, 0}, // title=Xevious

  // Prototypes and Hacks                                                                                                                          
  {"4332c24e4f3bc72e7fe1b77adf66c2b7",  "3D Asteroids",                     CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=3D Asteroids
  {"1745feadabb24e7cefc375904c73fa4c",  "Alternate Impossible Mission",     CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=Impossible Mission
  {"20660b667df538ec32a8e1b998438604",  "Frameless Centipede",              CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  160,  24, 16, 300,  230, 0}, // title=Centipede - Frameless Hack
  {"8f7eb10ad0bd75474abf0c6c36c08486",  "Rescue On Fractalus",              CT_FRACTALUS, POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=Rescue On Fractalus
  {"06204dadc975be5e5e37e7cc66f984cf",  "Gato",                             CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=Gato   
  {"17b3b764d33eae9b5260f01df7bb9d2f",  "Klax",                             CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,  20, 256,  234, 0}, // title=Klax (fixed)
  {"5fb805f2b69820a9b196f5fed2a23c99",  "Klax",                             CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,  20, 256,  234, 0}, // title=Klax       
  {"017066f522908081ec3ee624f5e4a8aa",  "Missing in Action",                CT_SUPLRG,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_NO,   145,  0,  12, 256,  220, 0}, // title=Missing in Action
  {"ec206c8db4316eb1ebce9fc960da7d8f",  "Pit Fighter",                      CT_SUPROM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=Pit Fighter                     
  {"74f0283c566bdee8543e4fdc5cb8b201",  "Plutos XM",                        CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_NO,   180,  0,  17, 256,  234, 0}, // title=Plutos XM
  {"86546808dc60961cdb1b20e761c50ab1",  "Plutos",                           CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_NO,   180,  0,  17, 256,  234, 0}, // title=Plutos (non-XM)
  {"1745feadabb24e7cefc375904c73fa4c",  "Possible Mission",                 CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  12, 256,  220, 0}, // title=Possible Mission
  {"b697d9c2d1b9f6cb21041286d1bbfa7f",  "Sentinel",                         CT_SUPROM, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  5,  15, 258,  220, 0}, // title=Sentinel

  // Bob (pacmanplus) Games                                                                                                                        
  {"89b8b3df46733e0c4d57aeb9bb245e6f",  "Armor Attack II",                  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  165,  0,  12, 256,  220, 0}, // title=Armor Attack II
  {"eea04359df6770d66b0d97c2cea1932f",  "Armor Attack II",                  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  165,  0,  12, 256,  220, 0}, // title=Armor Attack II (20230627)
  {"a65f79ad4a0bbdecd59d5f7eb3623fd7",  "Asteroids Deluxe (NTSC)",          CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  210,  0,  17, 256,  230, 0}, // title=Asteroids Deluxe (NTSC) (20071014)
  {"3d38281ed8a8d8c7cd457a18c92c8604",  "Astro Blaster",                    CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_YES,  245,  30,  9, 320,  210, 0}, // title=Astro Blaster
  {"55ffe535897c368be7a80d582f6a68cb",  "Astro Blaster",                    CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_YES,  245,  30,  9, 320,  210, 0}, // title=Astro Blaster (20230627)
  {"a51e5df28a0fe8c52e9d28fb5f8e44a6",  "Astro Fighter",                    CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  205,  32,  9, 320,  213, 0}, // title=Astro Fighter
  {"8feb090fb1aee5cc19c2d8f36a4f64ae",  "Astro Fighter",                    CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  205,  32,  9, 320,  213, 0}, // title=Astro Fighter (v1.1)
  {"7cdfbe37634e7dcd4dc67db7edbcd3ba",  "Baby Pac-Man",                     CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   250,  32, 12, 320,  222, 0}, // title=Baby Pac Man
  {"2b31dfab41dce110dd136fd06606e1ca",  "Baby Pac-Man",                     CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   250,  32, 12, 320,  222, 0}, // title=Baby Pac Man (20230627)
  {"34483432b92f565f4ced82a141119164",  "Bentley Bear",                     CT_SUPLRG,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  165,  3,  14, 256,  220, 0}, // title=Bentley Bear's Crystal Quest
  {"299d31c8e181fdd011df2014451bdf0f",  "Crazy Brix",                       CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  160,  0,   5, 256,  203, 0}, // title=Crazy Brix   
  {"2d2fe4da9f1bae102fa8ca2d8830a626",  "Crazy Otto",                       CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   165,  0,   9, 256,  232, 0}, // title=Crazy Otto
  {"100551363027dc5f093d049a5fd00933",  "Crazy Otto",                       CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   165,  0,   9, 256,  232, 0}, // title=Crazy Otto (20230627)
  {"6287727ab36391a62f728bbdee88675c",  "Failsafe",                         CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  250,  0,  14, 257,  220, 0}, // title=FailSafe (NTSC) (20100227)
  {"e7d89669a7f92ec2cc99d9663a28671c",  "Frenzy (w-Berzerk)",               CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  255,  30,  6, 320,  206, 0}, // title=Frenzy (with Berzerk) (homebrew)
  {"26031dea7251fb861cb55f86742c9d6e",  "Frenzy (w-Berzerk)",               CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  255,  30,  6, 320,  206, 0}, // title=Frenzy (with Berzerk) (20211025)
  {"2f4ae1015a345652b36004a8c62a4ac6",  "Galaxian",                         CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  210,  14,  5, 282,  211, 0}, // title=Galaxian
  {"686a4e4dde0eca5c7984829c4d30d3cf",  "Galaxian",                         CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  210,  14,  5, 282,  211, 0}, // title=Galaxian (v1.1)
  {"e54edc299e72d22d0ba05d16f3393e8c",  "Jr. Pac-Man (NTSC)",               CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  205,  4,  14, 268,  234, 0}, // title=Jr Pac-Man  
  {"bde3abe40d302d8c4c65c9690c05dbc4",  "Jr. Pac-Man (NTSC)",               CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  205,  4,  14, 268,  234, 0}, // title=Jr Pac-Man  (20230627)
  {"6b8600aabd11f834448e910801f4e0bc",  "KC Munchkin",                      CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  3,  12, 256,  225, 0}, // title=KC Munchkin
  {"aa0b9560d6610378bda58f09f265d6ad",  "KC Munchkin",                      CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  3,  12, 256,  225, 0}, // title=KC Munchkin (20230627)
  {"927edf157f88b8f5863254d9a65f05a8",  "KC Munchkin",                      CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  3,  12, 256,  225, 0}, // title=KC Munchkin (Alt Movement) (20170409)
  {"c3f6201d6a9388e860328c963a3301cc",  "Meteor Shower",                    CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  160,  0,  13, 256,  220, 0}, // title=Meteor Shower
  {"a44241d782ee14b53483487cc8216274",  "Meteor Shower",                    CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  160,  0,  13, 256,  220, 0}, // title=Meteor Shower (v1.1)
  {"9ff38ea62004201d870caa8bd9463525",  "Moon Cresta (NTSC)",               CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  210,  30,  5, 320,  210, 0}, // title=Moon Cresta
  {"a56f26e6d21b2ae4880f0fd410243cc1",  "Moon Cresta (NTSC)",               CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  210,  30,  5, 320,  210, 0}, // title=Moon Cresta (v1.1)
  {"cf007563fe94cacf5ea5295dc93ce9ef",  "Ms. Pac-Man Bob",                  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  200,  0,   9, 256,  217, 0}, // title=Ms. Pac-Man (Bob's Enhancements)    
  {"2a17dc5a61be342dd00af719cc335852",  "Ms Pac-Man 320",                   CT_NORMAL, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  210,  5,  16, 265,  230, 0}, // title=Ms Pac-Man 320
  {"60982f430b762343d53e48f70acfa6d0",  "Pac-Man 320",                      CT_NORMAL, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  220,  6,  17, 264,  233, 0}, // title=Pac-Man 320
  {"5013b69cb05b21a1194ce48517df7bfc",  "Pac-Man Collection",               CT_NORMAL, POKEY_AT_4000, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_YES,  210,  10, 11, 281,  231, 0}, // title=Pac-Man Collection (homebrew)    
  {"6a432fd1e9d42a294bdb04d2c3d1e17f",  "Pac-Man Collection",               CT_NORMAL, POKEY_AT_4000, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_YES,  210,  10, 11, 281,  231, 0}, // title=Pac-Man Collection - Ultimate Edition (POKEY) (4000)
  {"ce1cb2f5d22384493737cc13121a22bc",  "Pac-Man Collection",               CT_NORMAL, POKEY_AT_4000, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_YES,  210,  10, 11, 281,  231, 0}, // title=Pac-Man Collection - Ultimate Edition (POKEY) (4000)
  {"200ef9c7b36afd3b1b6ce32065a259bb",  "Pac-Man Collection - Remastered",  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_YES,  210,  10, 11, 281,  231, 0}, // title=Pac-Man Collection - Remastered (TIA)
  {"f2512870a8abc82df42b2721f8c9e7af",  "Pac-Man 40th Anniversary",         CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  210,  9,  10, 278,  221, 0}, // title=Pac-Man Collection 40th Anniversary Edition (homebrew) - Newest
  {"a59d362e3a391ff1482131aa0818ad3e",  "Pac-Man 40th Anniversary",         CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  220,  9,  10, 278,  221, 0}, // title=Pac-Man Collection 40th Anniversary Edition (homebrew) - Older
  {"1330d23ebad9b5ded92ebeacdf305abd",  "Pac-Man 40th Anniversary",         CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  220,  9,  10, 278,  221, 0}, // title=Pac-Man Collection 40th Anniversary Edition (homebrew) - PMC_XM Newest
  {"2b60de20a55056a11e6412a22445296f",  "Pac-Man XM-S",                     CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  220,  11, 17, 279,  233, 0}, // title=Pac-Man Collection 40th - Short Mazes (2022) - Newest
  {"c80edcd555cd3d81f664e5f02826dc26",  "Pac-Man XM-S",                     CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  220,  11, 17, 279,  233, 0}, // title=Pac-Man Collection 40th - Short Mazes (2022)
  {"39dc7f6f39f9b3e341a5ffea76e71fb1",  "Pac-Man 40th Anniversary",         CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  200,  9,  10, 278,  221, 0}, // title=Pac-Man Collection - 40th Anniversary Edition (20230627)
  {"2686f20449c339f7d31671f1cdec7249",  "Pac-Man 40th Anniversary",         CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  200,  11, 17, 279,  233, 0}, // title=Pac-Man Collection - 40th Anniversary Edition (Short Mazes) (20230627)
  {"2b51ebf2f371d0790a5629e27727ebba",  "Pac-Man Energy Drink",             CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  220,  10, 18, 271,  234, 0}, // title=Pac-Man - Energy Drink Edition (20230707)
  {"d0bf3b841ad4bbd356e9588874749a13",  "Pac-Man Plus 320",                 CT_NORMAL,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  200,  10, 12, 274,  233, 0}, // title=Pac-Man Plus 320
  {"43525a0405184875c2ecfd0196886a34",  "Rip-Off",                          CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  160,  0,  17, 256,  233, 0}, // title=Rip-Off
  {"803743fe18600f292456539906464421",  "Rip-Off",                          CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  160,  0,  17, 256,  233, 0}, // title=Rip-Off (20230627)
  {"a3a85e507d6f718972b1464ce1aaf8a4",  "Scramble",                         CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  250,  30,  5, 320,  205, 0}, // title=Scramble (homebrew)
  {"31b20a4710e691300bb4aa62cf02284c",  "Scramble",                         CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  250,  30,  5, 320,  205, 0}, // title=Scramble (20230627)
  {"771cb4609347657f63e6f0eb26036e35",  "Space Duel",                       CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  200,  0,  12, 256,  220, 0}, // title=Space Duel (homebrew)
  {"6adf79558a3d7f5beca1bb8d34337417",  "Space Invaders",                   CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  165,  30,  8, 320,  210, 0}, // title=Space Invaders (Homebrew)
  {"6a898d52ef050cdb89442e91cc529d14",  "Space Invaders",                   CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  165,  30,  8, 320,  210, 0}, // title=Space Invaders (v1.1)
  {"7ab539bb0e99e1e5a1c89230bde64610",  "Super Pac-Man",                    CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  205,  0,   5, 256,  225, 0}, // title=Super Pac-Man
  {"88b9de0eba37ba516590fa8b860155f0",  "Super Pac-Man",                    CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  205,  0,   5, 256,  225, 0}, // title=Super Pac-Man (20230627)
  {"79df20ee86a989e669158bcb9d113e8a",  "UniWarS",                          CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  215,  9,   5, 282,  202, 0}, // title=UniWarS
  {"0db69825c81171e62fd2bda526665c22",  "UniWarS",                          CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  215,  9,   5, 282,  202, 0}, // title=UniWarS (v1.1)
  {"f41f651417c234104d37296477fa29eb",  "Super Cobra",                      CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  240, 26,  12, 320,  225, 0}, // title=Super Cobra
  
  {"fd9353d42cca5f81fe7af866592b94c3",  "1942",                             CT_SUPRAMX2,POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  190,  0,   6, 256,  203, 1}, // title=1942 (Standard Banking RAM)
  {"e1b290ee690c0cd6525773a2025177d5",  "1942",                             CT_SUPRAMX2,POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  190,  0,   6, 256,  203, 1}, // title=1942 (Standard Banking RAM)
  {"0fec9c1f5973c7a1dc55318ec97d8d17",  "1942",                             CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  190,  0,   6, 256,  203, 1}, // title=1942 (Dragonfly Banking RAM)
  {"6f157f421c7ed5d952b393122d37915e",  "2048",                             CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  130,  22, 14, 299,  230, 0}, // title=2048 (RC1a) (20211113)
  {"a837a34f540fd1371bfcfb8e8af4c375",  "7ix",                              CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  14, 16, 286,  234, 0}, // title=7iX (20220305)
  {"ff056f2858f14fc4725fcb0015d78d3b",  "A Roach in Space",                 CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   130,  3,  15, 261,  234, 0}, // title=A Roach In Space - Part II - Electric Bugaloo (20201119)
  {"d99bff88cd3cce191c26f5755842eb21",  "Arkanoid (ChunkyPixel Games)",     CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   120,  0,   9, 256,  200, 0}, // title=Arkanoid 78b Demo (purposely set HSC to false - game HSC is buggy)
  {"212ee2a6e66d8bb7fbf26f343cc8dc19",  "Arkanoid (ChunkyPixel Games)",     CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   120,  0,   9, 256,  200, 0}, // title=Arkanoid 78b Demo (purposely set HSC to false - game HSC is buggy)
  {"e1da4c3ea0d26ae0a893741b49be2274",  "Arkanoid (ChunkyPixel Games)",     CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   120,  0,   9, 256,  200, 0}, // title=Arkanoid 78b Demo (purposely set HSC to false - game HSC is buggy)  
  {"0d05659a7d0eef02e7fa2cb686c816e7",  "ATRI",                             CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  120,  2,  16, 268,  234, 0}, // title=ARTI Public Demo
  {"4a8a22cff154f479f1ddaa386f21fc39",  "Boom",                             CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   120,  0,   7, 256,  220, 0}, // title=Boom!
  {"9fa7743a016c9b7015ee1d386326f88e",  "b*nQ",                             CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  160,  0,  18, 256,  223, 0}, // title=BonQ (Final Atariage)
  {"78b1061d651ef806becac1dd3fda29a0",  "Beef Drop",                        CT_NORMAL, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  135,  0,   8, 256,  220, 0}, // title=Beef Drop (Final Atariage)
  {"b11b1a2bae8a1d0cd1c180798c6e6169",  "Bernie and the Cubic",             CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  125,  3,  13, 264,  227, 0}, // title=Bernie and the Cubic Conundrum (Alpha 10)
  {"a34cd425d0c087d0315935501d76790a",  "Bernie and the Tower",             CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  125,  6,  16, 264,  233, 0}, // title=Bernie and the Tower of Doom (RC1) (Demo)
  {"000b5888d2489f7e256d80a0848ecd14",  "Cannon in D for Defense",          CT_SUPLRG,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   125,  0,  12, 256,  223, 0}, // title=Cannon in D for Defense (demo 03)
  {"825c03c049306c16bd654d9d0e344cf3",  "Cartesian Chaos",                  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  120,  0,  16, 260,  234, 0}, // title=Cartesian Chaos (v11) (20221218)
  {"a4b5d742860beb25c29def4530194c1e",  "Chase",                            CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   120,  8,  19, 269,  233, 0}, // title=Chase (20201231)
  {"0c2f248a1ae9bfd14b1bcc1bd9f3a41e",  "Danger Zone",                      CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   120,  0,  14, 256,  234, 0}, // title=Danger Zone (RC-4C) (NTSC Demo) (20201231)
  {"fab7b59dd580dce0b28be3e74a7b8433",  "Death Merchant",                   CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  2,  17, 260,  229, 0}, // title=Death Merchant (v1_30)
  {"dd1cfc933d2bfacc613ef745c1366438",  "Donkey Kong PK-XM",                CT_SUPLRG,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  200,  0,  12, 256,  210, 0}, // title=Donkey Kong PK-XM (NTSC) (Demo) (v1.2)
  {"c3107d3e3e17d67e3a11d47a5946a4f3",  "Donkey Kong XM DEMO",              CT_SUPLRG, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  200,  0,  20, 256,  223, 0}, // title=Donkey Kong XM DEMO
  {"312363c7691fa51ef3a1e53134ff2e2c",  "Donkey Kong Remix DEMO",           CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  200,  0,  17, 256,  216, 0}, // title=Donkey Kong Remix DEMO
  {"77164df89ae49b4dd72906a21e787233",  "Dragon's Descent",                 CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  140,  0,  16, 256,  234, 0}, // title=Dragon's Descent (20210731)
  {"8c2798f929a43317f300d3ccbe25918e",  "Dragon's Havoc",                   CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   120,  6,  15, 256,  232, 0}, // title=Dragon's Havoc (Demo Dec 2022)
  {"7b7825ca2c79148f1c4ade6baacc1a76",  "Dragon's Cache",                   CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  140,  6,  15, 256,  232, 0}, // title=Dragon's Cache (20210207)
  {"a9f29004412621f20ad9f5c51cc11486",  "Draker Quest II",                  CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  130,  0,  16, 256,  230, 0}, // title=Draker Quest II
  {"fab1290f9a4c4f2b4d831c8a57f969f5",  "Draker Quest",                     CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  130,  0,  17, 256,  230, 0}, // title=Draker Quest (Beta 4)
  {"b3143adbbb7d7d189e918e5b29d55a72",  "Dungeon Stalker",                  CT_NORMAL,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  130,  0,   9, 256,  231, 0}, // title=Dungeon Stalker (20151022)
  {"a44e8b7b7881beb0fe3c71a1a04441c8",  "EXO",                              CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  125,  0,  16, 256,  234, 0}, // title=E.X.O. (RC Demo A)
  {"6053233cb59c0b4ca633623fd76c4576",  "Froggie",                          CT_NORMAL, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  210,  34,  8, 320,  205, 0}, // title=Froggie (NTSC) (Final Release)
  {"9daaac9b25783a7e3c8858f3987ed18d",  "Frogus",                           CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  130,  0,  16, 256,  224, 0}, // title=Frogus (20221020)
  {"c2e131a091ceed2e04e71f19219b7804",  "Game of the Bear",                 CT_SUPLRG,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  15, 256,  234, 0}, // title=Game of the Bear - Polar Opposites (RC1) (20230211)
  {"e443f7fb5be3283dd44c0f5d80c3a7b3",  "GoSub",                            CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   135,  13, 12, 280,  234, 0}, // title=GoSub
  {"1e21bf1d9d7b3c0cebaac576964c9eb2",  "Graze Suit Alpha",                 CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   160,  0,  11, 256,  220, 0}, // title=Graze Suit Alpha (20170910)
  {"1c9deabc48f07d1bf2c68731fccd27b5",  "Keystone Koppers",                 CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  110,  0,  12, 256,  220, 0}, // title=Keystone Koppers (Demo Dec 22)
  {"d41d8cd98f00b204e9800998ecf8427e",  "Keystone Koppers",                 CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  110,  0,  12, 256,  220, 0}, // title=Keystone Koppers (RC4 Demo)
  {"1d47c3802135d864dc1d922ec27aa708",  "Knight Guy On Board",              CT_SUPLRG,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_NO,   130,  0,  12, 256,  233, 0}, // title=Knight Guy On Board - 30 Squares Of Fate (20210116)
  {"0916973898e3b6b878cf604151a69237",  "Knight Guy On Board",              CT_SUPLRG,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_NO,   130,  0,  12, 256,  233, 0}, // title=Knight Guy On Board - 30 Squares Of Fate (Demo 3)
  {"33dbb58f9ee73e9f476b4ebbc8190c88",  "Knight Guy In Low Res",            CT_SUPLRG,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_NO,   130,  0,  12, 256,  233, 0}, // title=Knight Guy In Low Res World - Castle Days (RC 01-1)
  {"3ec728e116017be89c552a85a8f86d90",  "Knight Guy - Quest",               CT_SUPLRG,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_NO,   130,  0,  12, 256,  233, 0}, // title=Knight Guy - Quest For Something (20210423)
  {"7abd9e0a6321e813d7528aa7f2c8fb40",  "Knight Guy In Another",            CT_SUPLRG,    POKEY_NONE, JOY, JOY,  DIFF_B,  DIFF_B, NTSC,  HSC_NO,   130,  0,  12, 256,  233, 0}, // title=Knight Guy - In Another Castle (RC1b)
  {"271864e0978278a3e2fb04273db69d57",  "Legend of Silverpeak",             CT_SUPCAR,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  130,  0,   7, 256,  201, 0}, // title=Legend of Silverpeak (1.01)
  {"7abd9e0a6321e813d7528aa7f2c8fb40",  "Lunar Patrol",                     CT_SUPLRG,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   135,  6,  16, 264,  234, 0}, // title=Lunar Patrol (v83) (20241231) (2354A1FF)  
  {"23ac803eabfb8c6118b600a8177c94a4",  "Lyra the Tenrec",                  CT_NORMAL,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,  12, 256,  234, 0}, // title=Lyra the Tenrec (20240104)
  {"181a9978d9da7a7e21f770808cc681f2",  "Merlain",                          CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  140,  0,  17, 320,  234, 0}, // title=Merlain
  {"3f80432f156088bf328cff15842766ee",  "Millie and Molly",                 CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  140,  0,  14, 256,  220, 0}, // title=Millie And Molly (Demo) (POKEY 450) (20230305)
  {"1c860298a8966cc8e176ab8453b172c3",  "Monster Maze",                     CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  140,  0,  14, 256,  220, 0}, // title=Monster Maze (20220306)
  {"d1b56eae7227c12d0122bb84925c89c0",  "Morf",                             CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   130,  0,  17, 256,  224, 0}, // title=Morf (20220314)
  {"ac5c99ac01c96ad92832c0544889a702",  "Ninjish Guy - Perilous Island",    CT_SUPLRG,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   130,  0,  16, 256,  234, 0}, // title=Ninjish Guy - Perilous Island (20211107)
  {"3d9c52142f9e53f516d3408daad376f3",  "Oozy The Goo - Gaiden",            CT_SUPLRG,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   120,  0,  17, 256,  234, 0}, // title=Oozy The Goo - Gaiden (20231001)
  {"6ac5a7f8b6a3198ed08abb9866753763",  "PentaGo",                          CT_SUPLRG,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  125,  2,  17, 260,  234, 0}, // title=PentaGo (Demo) (20230422)
  {"a662862f20362fc5eb5c651065cbd51c",  "Petscii Robots",                   CT_BANKSHALT,POKEY_AT_800,SNES,JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,  10, 256,  214, 0}, // title=Attack of the Petscii Robots (Demo) (POKEY 800)
  {"0254afa887fcfc8c4b1a63b41b9ba613",  "Plumb Luck DX",                    CT_SUPRAM,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  140,  16, 17, 289,  220, 0}, // title=Plumb Luck DX (RC2) (20230410)
  {"61e6a16889b62216116eea136269a4c0",  "Popeye",                           CT_SUPLRG,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  140,  0,  10, 256,  210, 0}, // title=Popeye 2.40 Demo
  {"b6561537290e6e25e1249394366c3c63",  "Robbo",                            CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,  12, 256,  220, 0}, // title=Robbo (20160513)
  {"fc525819ec2bdc4a30bb2e55524f8d81",  "Robot's Rumble",                   CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,  11, 256,  217, 0}, // title=Robots Rumble (20220217)
  {"9bd70c06d3386f76f8162881699a777a",  "Serpentine",                       CT_SUPRAM,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  140,  0,  12, 256,  220, 0}, // title=Serpentine (20161029)
  {"96f69b85e0b43bbebbbd59bb8276a372",  "Sick Pickles",                     CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  140,  0,  12, 256,  233, 0}, // title=Sick Pickles (20171202)
  {"40567f50c569a60cc461cdf0e2853ff4",  "Slide Boy in Maze Land",           CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  140,  0,  16, 256,  230, 0}, // title=Slide Boy in Maze Land (RC1) (20210515)
  {"91f4cb1f642ff1de936a74672dce7198",  "Space Peril",                      CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  140,  0,  16, 256,  230, 0}, // title=Space Peril (NTSC) (v8) (20210907)
  {"19844117863cd38d4e1e4cbc867ae599",  "Spire of the Ancients",            CT_SUPLRG,    POKEY_NONE, SOTA,SOTA, DIFF_A,  DIFF_A, NTSC,  HSC_YES,  140,  31, 17, 320,  230, 0}, // title=Spire of the Ancients (NTSC) (20201223)
  {"81cee326b99d6831de10a566e338bd25",  "Super Circus Atari Age",           CT_NORMAL, POKEY_AT_4000, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  200,  0,  13, 256,  220, 0}, // title=Super Circus Atari Age (NTSC) (Joystick) (POKEY 4000)
  {"02508e6df5e173b4063a7e6e63295817",  "Super Circus Atari Age",           CT_NORMAL,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  200,  0,  13, 256,  220, 0}, // title=Super Circus Atari Age (NTSC) (Joystick) (POKEY 0450)
  {"a60e4b608505d1fb201703b266f754a7",  "Time Salvo",                       CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  130,  0,  16, 256,  230, 0}, // title=Time Salvo
  {"ff825fcbed9bf6993edd422fcc592673",  "Touchdown Challenge",              CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  1,  30, 258,  234, 0}, // title=Touchdown Challenge (v2_21) (20230225)
  {"0d7e2674d802b41286e667bbae3bcc94",  "Tunnels of Hyperion",              CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_NO,   140,  0,  13, 256,  233, 0}, // title=Tunnels of Hyperion
  {"f5150c0fc1948832211e57852abb0c6e",  "Utility Cart",                     CT_NORMAL,  POKEY_AT_450, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  160,  0,  12, 256,  220, 0}, // title=7800 Utility Cart
  {"846751861993b907c512cc9c10c67035",  "Wizards Dungeon",                  CT_NORMAL,    POKEY_NONE, JOY, JOY,  DIFF_A,  DIFF_A, NTSC,  HSC_YES,  130,  0,  18, 256,  224, 0}, // title=Wizards Dungeon (20211111)

  {"","",CT_NORMAL,0,0,0,0,0,0,0,0,0,0,0},
};


// -------------------------------------------------------------------------
// This happens AFTER The rom is loaded and the header info (if any) has
// been read out. Here we can adjust based on the hash table above... which
// is mainly used for headerless roms and a few special cases were we want 
// to correct the Y-offsets to make the game well centered/scaled on screen.
// -------------------------------------------------------------------------
bool database_Load(byte *digest) 
{
    extern u8 bNoDatabase;
    bool bFound = false;
    uint16 i;
    
    // See if we've been asked to skip the internal database
    if (!bNoDatabase)
    {
        // --------------------------------------------------------------------------------------
        // First see if we've got a match in our external A7800DS.DAT configuration database...
        // --------------------------------------------------------------------------------------
        for (i=0; i<MAX_CONFIGS; i++)
        {
          if (!strcmp(allConfigs.cart[i].digest,(char *) digest))
          {
              memcpy(&myCartInfo, &allConfigs.cart[i], sizeof(myCartInfo));
              bFound = true;
              break;
          }
        }

        
        // ---------------------------------------------------------------------------------------------
        // If we didn't find it in the config database, we can look in the internal database table...
        // ---------------------------------------------------------------------------------------------
        if (!bFound)
        {
            /* Look up mapper in game list by md5sum */
            for(i = 0; strlen(game_list[i].digest); i++)
            {
              if (!strcmp(game_list[i].digest,(char *) digest))
              {
                memcpy(&myCartInfo, &game_list[i], sizeof(myCartInfo));
                if (!isDSiMode()) myCartInfo.frameSkip = ((cartridge_size <= (48*1024)) ? FRAMESKIP_MEDIUM : FRAMESKIP_AGGRESSIVE); // Older DS-Lite/Phat needs help
                myCartInfo.palette = 1; // Force this if not specifically found by md5
                myCartInfo.xJiggle = 64;
                myCartInfo.yJiggle = 16;
                bFound = true;          
                break;
              }
            }
        }
        
       
        // -----------------------------------------------------------------------------------------------
        // If we didn't find a definitive md5 match above, look up game by cart title in the .A78 header 
        // or even by the name of the ROM filename as it will give us a clue as to the game identity.
        // -----------------------------------------------------------------------------------------------
        if (!bFound)
        {
          for(i = 0; strlen(game_list[i].header_name); i++)
          {
            if (myCartInfo.region == NTSC)
            {
              if ( (!strcmp(game_list[i].header_name,(char *) cartridge_title)) ||
                   (strstr((char *) cartridge_filename, game_list[i].header_name) != NULL) ) 
              {
                memcpy(&myCartInfo, &game_list[i], sizeof(myCartInfo));
                strcpy(myCartInfo.digest, (char *)digest); 
                if (!isDSiMode()) myCartInfo.frameSkip = ((cartridge_size <= (48*1024)) ? FRAMESKIP_MEDIUM : FRAMESKIP_AGGRESSIVE); // Older DS-Lite/Phat needs help
                myCartInfo.palette = 1; // Force this if not specifically found by md5
                myCartInfo.xJiggle = 64;
                myCartInfo.yJiggle = 16;
                bFound = true;
                
                // --------------------------------------------------------------------------------------
                // As a sanity check... if the card type ended up as 'NORMAL' but the ROM is larger 
                // than NORMAL would support - we adjust it here. This prevents soft matches in 
                // the database from causing problems (I'm looking at you various flavors of Donkey Kong)
                // --------------------------------------------------------------------------------------
                if (myCartInfo.cardtype == CT_NORMAL)
                {
                    if (cartridge_size == (144*1024)) myCartInfo.cardtype = CT_SUPLRG;
                    else myCartInfo.cardtype = (cartridge_size <= (52*1024)) ? CT_NORMAL:CT_SUPROM;
                }

                break;
              }
            }
          }
        }
    }
    
    // No matter what... override for Tower Toppler to make it playable...
    if (strcmp((char *) digest, (char *) "8d64763db3100aadc552db5e6868506a") == 0) // Tower Toppler
    {
        use_composite_filtering = 70;
        myCartInfo.frameSkip = FRAMESKIP_AGGRESSIVE; // It's the only way we stand a chance.
        myCartInfo.cardctrl1 = SOTA;
        myCartInfo.xOffset = 32;
        myCartInfo.yOffset = 8;
        myCartInfo.xScale  = 320;
        myCartInfo.yScale  = 234;
        bFound = 1;
    }
    else if (strcmp((char *) digest, (char *) "f5150c0fc1948832211e57852abb0c6e") == 0) // 7800 Utility Cart
    {
        use_composite_filtering = 1;
        bFound = 1;
    }
    // Override for Jinks to enable composite filtering
    else if (strcmp((char *) digest, (char *) "045fd12050b7f2b842d5970f2414e912") == 0) // Jinks
    {
        use_composite_filtering = 1;
        bFound = 1;
    } else use_composite_filtering = 0;
    
    // And there is one game currently that won't play nice with our BIOS handling...
    if (strcmp((char *) digest, (char *) "a94e4560b6ad053a1c24e096f1262ebf") == 0) // Crossbow
    {
        //bSkipBIOS = 1;
    }
    
        
    // --------------------------------------------------------------------------
    // Default scaling options below if not found... these are close enough... 
    // We can make some educated guesses on cart and frameskip...
    // --------------------------------------------------------------------------
    if (!bFound) 
    {
        strcpy(myCartInfo.digest, (char *)digest); 
        strncpy(myCartInfo.header_name, (char *) cartridge_filename, 32);
        myCartInfo.header_name[32] = 0;
        myCartInfo.xOffset       = 0;
        myCartInfo.yOffset       = 13;
        myCartInfo.xScale        = 256;
        myCartInfo.yScale        = 220;
        myCartInfo.diff1         = DIFF_A;
        myCartInfo.diff2         = DIFF_A;
        myCartInfo.xButton       = KEY_MAP_DEFAULT;
        myCartInfo.yButton       = KEY_MAP_DEFAULT;
        myCartInfo.spare2        = 0;
        myCartInfo.spare3        = 0;
        myCartInfo.spare4        = 1;
        myCartInfo.spare5        = 0;
        myCartInfo.palette       = 1;
        myCartInfo.xJiggle       = 64;
        myCartInfo.yJiggle       = 16;
        
        // -------------------------------------------------------------------
        // If the game has no .a78 header, do our best to guess these...
        // -------------------------------------------------------------------
        if (myCartInfo.hasHeader == false)
        {
            myCartInfo.region        = NTSC;
            if (cartridge_size == (144*1024)) myCartInfo.cardtype = CT_SUPLRG;
            else myCartInfo.cardtype = (cartridge_size <= (52*1024)) ? CT_NORMAL:CT_SUPROM;
            myCartInfo.pokeyType     = POKEY_NONE;
            myCartInfo.cardctrl1     = JOY;
            myCartInfo.cardctrl2     = JOY;
            myCartInfo.hsc           = false;
        }
        
        // --------------------------------------------------------
        // Do our best guess as to whether we should frameskip...
        // --------------------------------------------------------
        if (isDSiMode())    // DSi can handle most games in full framerate... default to disable frameskip for the DSi
        {
            myCartInfo.frameSkip = FRAMESKIP_DISABLE;
        }
        else    // DS-Lite defaults to some level of frame skipping
        {
            myCartInfo.frameSkip = ((cartridge_size <= (48*1024)) ? FRAMESKIP_MEDIUM : FRAMESKIP_AGGRESSIVE); // Non-banked carts get light frameskip... otherwise agressive
        }
    }
    
    // Lastly - use the internal database to always try and find a BIOS timeout value... we don't let the user override this one...
    myCartInfo.biosTimeout = 160;
    for(i = 0; strlen(game_list[i].digest); i++)    // Search through entire internal database...
    {
      if (!strcmp(game_list[i].digest,(char *) digest)) // Search by md5sum
      {
          myCartInfo.biosTimeout = game_list[i].biosTimeout;
          break;
      }
      
      if (myCartInfo.region == NTSC) // Search by name...
      {
          if ( (!strcmp(game_list[i].header_name,(char *) cartridge_title)) || (strstr((char *) cartridge_filename, game_list[i].header_name) != NULL) )
          {
              myCartInfo.biosTimeout = game_list[i].biosTimeout;
              // No break here... md5sum will override above
          }
      }
    }
    
    return true;
}

// End of file
