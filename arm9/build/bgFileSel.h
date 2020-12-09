
//{{BLOCK(bgFileSel)

//======================================================================
//
//	bgFileSel, 256x256@8, 
//	+ palette 256 entries, not compressed
//	+ 552 tiles (t|f reduced) lz77 compressed
//	+ regular map (in SBBs), lz77 compressed, 32x32 
//	Total size: 512 + 9548 + 1372 = 11432
//
//	Time-stamp: 2020-12-09, 07:58:19
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.16
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BGFILESEL_H
#define GRIT_BGFILESEL_H

#define bgFileSelTilesLen 9548
extern const unsigned int bgFileSelTiles[2387];

#define bgFileSelMapLen 1372
extern const unsigned short bgFileSelMap[686];

#define bgFileSelPalLen 512
extern const unsigned short bgFileSelPal[256];

#endif // GRIT_BGFILESEL_H

//}}BLOCK(bgFileSel)
