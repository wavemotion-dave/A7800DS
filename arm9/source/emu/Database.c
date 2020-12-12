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
  bool pokey;
  uint cardctrl1;
  uint cardctrl2;
  uint cardregion;
  uint cardflags;
  int  yOffset;
} Database_Entry;

Database_Entry game_list[] = {
  {"4332c24e4f3bc72e7fe1b77adf66c2b7",0,false,1,1,0,0, 22}, // title=3D Asteroids
  {"0be996d25144966d5541c9eb4919b289",4,false,1,1,0,0, 22}, // title=Ace Of Aces
  {"aadde920b3aaba03bc10b40bd0619c94",4,false,1,1,1,0, 22}, // title=Ace Of Aces
  {"877dcc97a775ed55081864b2dbf5f1e2",2,false,3,3,0,0, 22}, // title=Alien Brigade
  {"de3e9496cb7341f865f27e5a72c7f2f5",2,false,3,3,1,0, 22}, // title=Alien Brigade
  {"07342c78619ba6ffcc61c10e907e3b50",0,false,1,1,0,0, 25}, // title=Asteroids
  {"8fc3a695eaea3984912d98ed4a543376",0,true ,1,1,0,0, 22}, // title=Ballblazer
  {"b558814d54904ce0582e2f6a801d03af",0,true ,1,1,1,0, 22}, // title=Ballblazer
  {"42682415906c21c6af80e4198403ffda",1,true ,2,1,0,0, 22}, // title=Barnyard Blaster
  {"babe2bc2976688bafb8b23c192658126",1,true, 2,1,1,0, 22}, // title=Barnyard Blaster
  {"f5f6b69c5eb4b55fc163158d1a6b423e",4,false,1,1,0,0, 22}, // title=Basketbrawl
  {"fba002089fcfa176454ab507e0eb76cb",4,false,1,1,1,0, 22}, // title=Basketbrawl
  {"78b1061d651ef806becac1dd3fda29a0",0,true ,1,1,1,0, 26}, // title=Beef Drop (Final Atariage)
  {"4e325918a8b3bbcf2f9405040abcfc6d",0,false,1,1,0,0, 26}, // title=BonQ (found on Atairage Age site ... no graphical glitches)
  {"9fa7743a016c9b7015ee1d386326f88e",0,false,1,1,0,0, 26}, // title=BonQ (final Atariage ... some graphical glitches)
  {"5a09946e57dbe30408a8f253a28d07db",0,false,1,1,0,0, 22}, // title=Centipede
  {"38c056a48472d9a9e16ebda5ed91dae7",0,false,1,1,1,0, 22}, // title=Centipede
  {"93e4387864b014c155d7c17877990d1e",0,false,1,1,0,0, 22}, // title=Choplifter
  {"59d4edb0230b5acc918b94f6bc94779f",0,false,1,1,1,0, 22}, // title=Choplifter
  {"2e8e28f6ad8b9b9267d518d880c73ebb",1,true ,1,1,0,0, 22}, // title=Commando
  {"55da6c6c3974d013f517e725aa60f48e",1,true ,1,1,1,0, 22}, // title=Commando
  {"db691469128d9a4217ec7e315930b646",1,false,1,1,0,0, 22}, // title=Crack'ed
  {"7cbe78fa06f47ba6516a67a4b003c9ee",1,false,1,1,1,0, 22}, // title=Crack'ed
  {"a94e4560b6ad053a1c24e096f1262ebf",2,false,3,3,0,0, 22}, // title=Crossbow
  {"63db371d67a98daec547b2abd5e7aa95",2,false,3,3,1,0, 22}, // title=Crossbow
  {"179b76ff729d4849b8f66a502398acae",1,false,1,1,0,0, 21}, // title=Dark Chambers
  {"a2b8e2f159642c4b91de82e9a2928494",1,false,1,1,1,0, 21}, // title=Dark Chambers
  {"95ac811c7d27af0032ba090f28c107bd",0,false,1,1,0,0, 22}, // title=Desert Falcon
  {"2d5d99b993a885b063f9f22ce5e6523d",0,false,1,1,1,0, 22}, // title=Desert Falcon
  {"731879ea82fc0ca245e39e036fe293e6",0,false,1,1,0,0, 22}, // title=Dig Dug
  {"408dca9fc40e2b5d805f403fa0509436",0,false,1,1,1,0, 22}, // title=Dig Dug
  {"5e332fbfc1e0fc74223d2e73271ce650",0,false,1,1,0,0, 22}, // title=Donkey Kong Jr
  {"4dc5f88243250461bd61053b13777060",0,false,1,1,1,0, 22}, // title=Donkey Kong Jr
  {"19f1ee292a23636bd57d408b62de79c7",0,false,1,1,0,0, 22}, // title=Donkey Kong
  {"8e96ef14ce9b5d84bcbc996b66d6d4c7",0,false,1,1,1,0, 22}, // title=Donkey Kong
  {"543484c00ba233736bcaba2da20eeea9",6,false,1,1,0,0, 22}, // title=Double Dragon
  {"de2ebafcf0e37aaa9d0e9525a7f4dd62",6,false,1,1,1,0, 22}, // title=Double Dragon
  {"2251a6a0f3aec84cc0aff66fc9fa91e8",5,false,1,1,0,0, 22}, // title=F-18 Hornet
  {"e7709da8e49d3767301947a0a0b9d2e6",5,false,1,1,1,0, 22}, // title=F-18 Hornet
  {"d25d5d19188e9f149977c49eb0367cd1",4,false,1,1,0,0, 22}, // title=Fatal Run
  {"23505651ac2e47f3637152066c3aa62f",4,false,1,1,1,0, 22}, // title=Fatal Run
  {"07dbbfe612a0a28e283c01545e59f25e",4,false,1,1,0,0, 22}, // title=Fight Night
  {"e80f24e953563e6b61556737d67d3836",4,false,1,1,1,0, 22}, // title=Fight Night
  {"cf76b00244105b8e03cdc37677ec1073",0,false,1,1,0,0, 22}, // title=Food Fight
  {"de0d4f5a9bf1c1bddee3ed2f7ec51209",0,false,1,1,1,0, 22}, // title=Food Fight
  {"6053233cb59c0b4ca633623fd76c4576",0,true, 1,1,1,0, 24}, // title=Froggie
  {"fb8d803b328b2e442548f7799cfa9a4a",0,false,1,1,0,0, 22}, // title=Galaga
  {"f5dc7dc8e38072d3d65bd90a660148ce",0,false,1,1,1,0, 22}, // title=Galaga
  {"06204dadc975be5e5e37e7cc66f984cf",0,false,1,1,0,0, 22}, // title=Gato                  
  {"fd9e78e201b6baafddfd3e1fbfe6ba31",0,false,1,1,0,0, 22}, // title=Hat Trick                       
  {"0baec96787ce17f390e204de1a136e59",0,false,1,1,1,0, 22}, // title=Hat Trick                       
  {"c3672482ca93f70eafd9134b936c3feb",4,false,1,1,0,0, 22}, // title=Ikari Warriors
  {"8c2c2a1ea6e9a928a44c3151ba5c1ce3",4,false,1,1,1,0, 22}, // title=Ikari Warriors
  {"baebc9246c087e893dfa489632157180",3,false,1,1,0,0, 22}, // title=Impossible Mission
  {"1745feadabb24e7cefc375904c73fa4c",3,false,1,1,0,0, 22}, // title=Impossible Mission
  {"80dead01ea2db5045f6f4443faa6fce8",3,false,1,1,1,0, 22}, // title=Impossible Mission
  {"045fd12050b7f2b842d5970f2414e912",3,false,1,1,0,0, 22}, // title=Jinks
  {"dfb86f4d06f05ad00cf418f0a59a24f7",3,false,1,1,1,0, 22}, // title=Jinks
  {"f18b3b897a25ab3885b43b4bd141b396",0,false,1,1,0,0, 22}, // title=Joust                           
  {"f2dae0264a4b4a73762b9d7177e989f6",0,false,1,1,1,0, 22}, // title=Joust                           
  {"c3a5a8692a423d43d9d28dd5b7d109d9",0,false,1,1,0,0, 22}, // title=Karateka                        
  {"5e0a1e832bbcea6facb832fde23a440a",1,false,1,1,1,0, 22}, // title=Karateka                        
  {"17b3b764d33eae9b5260f01df7bb9d2f",4,false,1,1,0,0, 22}, // title=Klax       
  {"f57d0af323d4e173fb49ed447f0563d7",0,false,1,1,0,0, 22}, // title=Kung Fu Master
  {"2931b75811ad03f3ac9330838f3d231b",0,false,1,1,1,0, 22}, // title=Kung Fu Master
  {"431ca060201ee1f9eb49d44962874049",0,false,1,1,0,0, 22}, // title=Mario Bros.
  {"d2e861306be78e44248bb71d7475d8a3",0,false,1,1,1,0, 22}, // title=Mario Bros.
  {"37b5692e33a98115e574185fa8398c22",4,false,1,1,0,0, 22}, // title=Mat Mania Challenge
  {"6819c37b96063b024898a19dbae2df54",4,false,1,1,1,0, 22}, // title=Mat Mania Challenge
  {"f2f5e5841e4dda89a2faf8933dc33ea6",4,false,1,1,0,0, 22}, // title=Mean 18 Ultimate Golf
  {"2e9dbad6c0fa381a6cd1bb9abf98a104",4,false,1,1,1,0, 22}, // title=Mean 18 Ultimate Golf
  {"bedc30ec43587e0c98fc38c39c1ef9d0",4,false,2,2,0,0, 22}, // title=Meltdown
  {"c80155d7eec9e3dcb79aa6b83c9ccd1e",4,false,2,2,1,0, 22}, // title=Meltdown
  {"c3f6201d6a9388e860328c963a3301cc",0,false,1,1,0,0, 21}, // title=Meteor Shower
  {"bc1e905db1008493a9632aa83ab4682b",4,false,1,1,0,0, 22}, // title=Midnight Mutants
  {"6794ea31570eba0b88a0bf1ead3f3f1b",4,false,1,1,1,0, 22}, // title=Midnight Mutants
  {"017066f522908081ec3ee624f5e4a8aa",2,false,1,1,0,3, 22}, // title=Missing in Action
  {"3bc8f554cf86f8132a623cc2201a564b",4,false,1,1,0,0, 22}, // title=Motor Psycho
  {"5330bfe428a6b601b7e76c2cfc4cd049",4,false,1,1,1,0, 22}, // title=Motor Psycho
  {"fc0ea52a9fac557251b65ee680d951e5",0,false,1,1,0,0, 22}, // title=Ms. Pac-Man                     
  {"56469e8c5ff8983c6cb8dadc64eb0363",0,false,1,1,1,0, 22}, // title=Ms. Pac-Man                     
  {"220121f771fc4b98cef97dc040e8d378",4,false,1,1,0,0, 22}, // title=Ninja Golf
  {"ea0c859aa54fe5eaf4c1f327fab06221",4,false,1,1,1,0, 22}, // title=Ninja Golf
  {"74569571a208f8b0b1ccfb22d7c914e1",0,false,1,1,0,0, 12}, // title=One On One
  {"8dba0425f0262e5704581d8757a1a6e3",0,false,1,1,1,0, 12}, // title=One On One
  {"5013b69cb05b21a1194ce48517df7bfc",0,true, 1,1,0,0, 19}, // title=Pac-Man Collection (homebrew)
  {"044657294450c869c45e7ef61f4870de",0,true, 1,1,0,0, 35}, // title=Pac-Man Collection 40th Anniversary Edition (homebrew)
  {"1a5207870dec6fae9111cb747e20d8e3",0,false,1,1,0,0, 22}, // title=Pete Rose Baseball
  {"386bded4a944bae455fedf56206dd1dd",0,false,1,1,1,0, 22}, // title=Pete Rose Baseball
  {"ec206c8db4316eb1ebce9fc960da7d8f",4,false,1,1,0,0, 22}, // title=Pit Fighter                     
  {"33aea1e2b6634a1dec8c7006d9afda22",4,false,1,1,0,0, 18}, // title=Planet Smashers
  {"2837a8fd49b7fc7ccd70fd45b69c5099",4,false,1,1,1,0, 18}, // title=Planet Smashers
  {"584582bb09ee8122e7fc09dc7d1ed813",0,false,1,1,0,0, 22}, // title=Pole Position II                
  {"865457e0e0f48253b08f77b9e18f93b2",0,false,1,1,1,0, 22}, // title=Pole Position II                
  {"1745feadabb24e7cefc375904c73fa4c",3,false,1,1,0,0, 22}, // title=Possible Mission
  {"ac03806cef2558fc795a7d5d8dba7bc0",6,false,1,1,0,0, 22}, // title=Rampage
  {"bfad016d6e77eaccec74c0340aded8b9",1,false,1,1,0,0, 22}, // title=Realsports Baseball
  {"8f7eb10ad0bd75474abf0c6c36c08486",0,false,1,1,0,0, 22}, // title=Rescue On Fractalus
  {"66ecaafe1b82ae68ffc96267aaf7a4d7",0,false,1,1,0,0, 22}, // title=Robotron                        
  {"980c35ae9625773a450aa7ef51751c04",4,false,1,1,0,0, 22}, // title=Scrapyard Dog
  {"53db322c201323fe2ca8f074c0a2bf86",4,false,1,1,1,0, 22}, // title=Scrapyard Dog
  {"b697d9c2d1b9f6cb21041286d1bbfa7f",4,true ,2,2,0,0, 22}, // title=Sentinel
  {"5469b4de0608f23a5c4f98f331c9e75f",4,true ,2,2,1,0, 22}, // title=Sentinel
  {"6adf79558a3d7f5beca1bb8d34337417",0,false,1,1,0,0, 99}, // title=Space Invaders (Homebrew) Special 99=340 pixel mode...
  {"cbb0746192540a13b4c7775c7ce2021f",3,false,1,1,0,0, 22}, // title=Summer Games
  {"81cee326b99d6831de10a566e338bd25",0,true, 1,1,0,0, 21}, // title=Super Circus Atariage (Pokey 4000)    
  {"cc18e3b37a507c4217eb6cb1de8c8538",0,false,1,1,0,0, 22}, // title=Super Huey UH-IX
  {"162f9c953f0657689cc74ab20b40280f",0,false,1,1,1,0, 22}, // title=Super Huey UH-IX
  {"59b5793bece1c80f77b55d60fb39cb94",0,false,1,1,0,0, 22}, // title=Super Skatebordin'
  {"95d7c321dce8f57623a9c5b4947bb375",0,false,1,1,1,0, 22}, // title=Super Skatebordin'
  {"44f862bca77d68b56b32534eda5c198d",1,false,1,1,0,0, 22}, // title=Tank Command
  {"1af475ff6429a160752b592f0f92b287",0,false,1,1,0,0, 22}, // title=Title Match Pro Wrestling
  {"3bb9c8d9adc912dd7f8471c97445cd8d",0,false,1,1,1,0, 22}, // title=Title Match Pro Wrestling
  {"c3903ab01a51222a52197dbfe6538ecf",0,false,1,1,0,0, 22}, // title=Tomcat F-14 Simulator
  {"682338364243b023ecc9d24f0abfc9a7",0,false,1,1,1,0, 22}, // title=Tomcat F-14 Simulator
  {"208ef955fa90a29815eb097bce89bace",4,false,1,1,0,0, 22}, // title=Touchdown Football
  {"d12e665347f354048b9d13092f7868c9",3,false,1,1,0,0, 22}, // title=Tower Toppler
  {"32a37244a9c6cc928dcdf02b45365aa8",3,false,1,1,1,0, 22}, // title=Tower Toppler
  {"acf63758ecf3f3dd03e9d654ae6b69b7",1,false,1,1,0,0, 22}, // title=Water Ski
  {"3799d72f78dda2ee87b0ef8bf7b91186",3,false,1,1,0,0, 22}, // title=Winter Games
  {"05fb699db9eef564e2fe45c568746dbc",4,false,1,1,0,0, 22}, // title=Xenophobe
  {"70937c3184f0be33d06f7f4382ca54de",4,false,1,1,1,0, 22}, // title=Xenophobe
  {"d7dc17379aa25e5ae3c14b9e780c6f6d",0,false,1,1,0,0, 22}, // title=Xevious
  {"b1a9f196ce5f47ca8caf8fa7bc4ca46c",0,false,1,1,1,0, 22}, // title=Xevious

  {"",0,false,0,0,0,0},
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
        cartridge_type = game_list[i].cardtype;
        cartridge_pokey = game_list[i].pokey;
        cartridge_controller[0] = game_list[i].cardctrl1;
        cartridge_controller[1] = game_list[i].cardctrl2;
        cartridge_region = game_list[i].cardregion;
        cartridge_flags = game_list[i].cardflags;
        cartridge_yOffset = game_list[i].yOffset;
        
        cartridge_wsync =  !( cartridge_flags & CARTRIDGE_WSYNC_MASK );
        cartridge_cycle_stealing = !( cartridge_flags & CARTRIDGE_CYCLE_STEALING_MASK );
        bFound = true;
        break;
      }
    }  
  }
  
  if (!bFound) 
  {
    cartridge_yOffset = 22;
  }
  cartridge_xOffset = 0;

  return true;
}


