
//{{BLOCK(bgTop)

//======================================================================
//
//	bgTop, 256x256@8, 
//	+ palette 256 entries, not compressed
//	+ 388 tiles (t|f reduced) lz77 compressed
//	+ regular map (in SBBs), lz77 compressed, 32x32 
//	Total size: 512 + 13508 + 1036 = 15056
//
//	Time-stamp: 2020-12-09, 07:58:19
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.16
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BGTOP_H
#define GRIT_BGTOP_H

#define bgTopTilesLen 13508
extern const unsigned int bgTopTiles[3377];

#define bgTopMapLen 1036
extern const unsigned short bgTopMap[518];

#define bgTopPalLen 512
extern const unsigned short bgTopPal[256];

#endif // GRIT_BGTOP_H

//}}BLOCK(bgTop)
