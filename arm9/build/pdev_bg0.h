
//{{BLOCK(pdev_bg0)

//======================================================================
//
//	pdev_bg0, 256x256@8, 
//	+ palette 256 entries, not compressed
//	+ 55 tiles (t|f reduced) lz77 compressed
//	+ regular map (in SBBs), lz77 compressed, 32x32 
//	Total size: 512 + 1412 + 364 = 2288
//
//	Time-stamp: 2020-12-09, 07:58:19
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.16
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_PDEV_BG0_H
#define GRIT_PDEV_BG0_H

#define pdev_bg0TilesLen 1412
extern const unsigned int pdev_bg0Tiles[353];

#define pdev_bg0MapLen 364
extern const unsigned short pdev_bg0Map[182];

#define pdev_bg0PalLen 512
extern const unsigned short pdev_bg0Pal[256];

#endif // GRIT_PDEV_BG0_H

//}}BLOCK(pdev_bg0)
