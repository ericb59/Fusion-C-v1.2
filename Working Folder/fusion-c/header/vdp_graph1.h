/*
 ___________________________________________________________
/               __           _                              \
|              / _|         (_)                             |
|             | |_ _   _ ___ _  ___  _ __                   |
|             |  _| | | / __| |/ _ \| '_ \                  |
|             | | | |_| \__ \ | (_) | | | |                 |
|             |_|  \__,_|___/_|\___/|_| |_| *               |
|                                                           |
|               The MSX C Library for SDCC                  |
|                   V1.0 - 09-10-11 2018                    |
|                                                           |
|                Eric Boez &  Fernando Garcia               |
|                                                           |
|   vdp_graph1.h : Drawing and graphic functions for MSX1   |
|                                                           |
|    Using BIOS only. Very basis, small code.               |
|                                                           |
\___________________________________________________________/
*                         (°□°)
*	Most original code from SOLID MSX C - (c) 1997
*	SDCC port 2015  by ?	
*	
*
*	Screen 2 limit: for each 8-pixels horizontally only 2 colours possible.
*	So, graphics of drawings looks awkward! Anyway, games as Arkanoid use exactly 8-pixel
*	 drawings and graphics looks perfect. Do this way and will get all MSX compatible
*	 good looking game.
*	TMS9918 VDP chip of MSX1 lacks fast built-in graphics copying functions RAM-VRAM-VRAM.
*	Single colour sprites 8x8,16x16 are supported.
*
*	SCREEN 2 mode resolution 256 pixels x 192 lines (16kB for screen)
* --------------------------------------------------------------------------
* | screen:		32 (horizontal) x 24 (vertical) patterns	   |
* |			16 from 512 colours can be displayed		       |
* |			at the same time				                   |
* | pattern:		768 kinds of patterns are available		   |
* |			pattern size is 8 (horizontal) x 8 (vertical) dots |
* |			any Figure can be defined for each pattern	       |
* |			only two colours can be used in horizontal 8 dots  |
* | memory:		for pattern font  6144 bytes	   		       |
* |    required:	for colour table  6144 bytes	   		   |
* | sprite:		sprite mode 1, 32 mono-colour sprites		   |
* | colours:		static table of MSX 16 colours			   |
* | BASIC:		compatible to SCREEN 2 for GRAPHIC 2		   |
* --------------------------------------------------------------------------
*
*	Memory map:	
*
*	0x0000 - 0x17FF		6144 bytes pattern generator (3 blocks x 256 patterns of 8 bytes)
*	0x1800 - 0x1AFF		768 bytes pattern layout (textual representation of 32x24 screen)
*	0x1B00 - 0x1B7F		128 bytes for sprite attributes
*	0x2000 - 0x37FF		6144 bytes pattern colours
*				(for "0" and "1")2 x 4bits per colour x 256 patterns of 8 bytes
*	0x3800 - 0x3FFF		2048 (256 x 8) bytes for sprite patterns
*
*/


#ifndef __VDP_GRAPH1__
#define __VDP_GRAPH1__



// filling operation 
#define	NO_FILL		0x00
#define	FILL_ALL	0xFF

/* structure to set/get colour of 8 pixels */
typedef struct {
	int col;	// colour number 0..15 for pixels of pattern
	int bg;		// background colour number 0..15
} pxColor; 


/* Whole screen RAM <=> VRAM dumb copy (2 x  0x1800 bytes) */
extern	void	SC2WriteScr( unsigned int addr_fromPalettes, unsigned int addr_fromColours ); /* Writes screen from RAM memory addresses to VRAM, very dumb one time way (2x6144 bytes) */
extern	void	SC2ReadScr( unsigned int addr_toPalettes, unsigned int addr_toColours ); 	/* Reads screen from VRAM to memory addresses, very dumb one time way (2x6144 bytes) */

	
	/*	VRAM => RAM (copy block to memory)
		(X,Y) - left upper corner of screen position to copy
		dx,dy - count of columns and rows of pixels
		So, the block (X,Y)-(X+dx-1,Y+dy-1) will be copied. 
		X,dx should be 0,8,16,24,32,... 8*n  because
		complete 8-pixel patterns will be copied
		Requires 2 memory blocks size of   (dx/8)*dy*/
	
extern	void	ReadBlock( int X, int Y, int dx, int dy,  unsigned int addr_toPalettes, unsigned int addr_toColours );
	
	/*	RAM => VRAM (put from memory to screen)
		(X,Y) - where to put on screen
		Opposite to Read_Block().*/
	
extern	void	WriteBlock( int X, int Y, int dx, int dy, unsigned int addr_fromPalettes, unsigned int addr_fromColours );


/* 8-pixel pattern functions, that does not change colour */		
extern	int		Get8px( int X,  int Y ); 		 	/* get byte of 8-pixels at (X,Y). b11001000 means: 0,1,4 pixel is set. */
extern	int		Get1px( int X,  int Y );  			/* get pixel of 8-pixels at (X,Y). Returns 0 if not set. */
extern	void	Set8px( int X,  int Y );  			/* sets whole byte of 8-pixels at (X,Y) */
extern	void	Set1px( int X,  int Y );  			/* sets pixel of 8-pixels at (X,Y) */
extern	void	Clear8px( int X,  int Y );  		/* clears byte (sets=0) of 8-pixels at (X,Y) */
extern	void	Clear1px( int X,  int Y );  		/* clears pixel (sets=0) of 8-pixels at (X,Y) */
		
extern	void	GetCol8px( int X,  int Y, pxColor *C ); 		/* get colours of 8-pixel pattern at (X,Y) */
extern	void	SetCol8px( int X,  int Y, pxColor *C ); 		/* sets new colour in (X,Y) for 8-pixel pattern */
	
/* functions that we know, by ignoring same 8-pixel pattern */
extern	int		SC2Point( int X,  int Y ); 											/* gets colour 0..15 of pixel at (X,Y), the same for 8-pixel pattern  */
extern	void	SC2Pset( int X,  int Y, int color ); 								/* puts pixel in (X,Y), sets colour of whole 8-pixel pattern */
extern	void	SC2Line( int X,  int Y, int X2,  int Y2, int color ); 				/* draws line (X,Y)-(X2,Y2), sets pixels, sets colour, does not change background colour */
extern	void	SC2Rect( int X1, int Y1, int X2, int Y2, int colour, int OP ); 		/* draws rectangle (X,Y)-(X2,Y2), with filling operation, fills if OP is FILL_ALL */

/* Paint for small regions. Split them if large.
	It is not the BASIC-subROM paint! Kind of bugPaint.
	Requires large stack of memory, slower than any image processing.
	Paint stops when "set" pixel is reached, not exact colour. Sets colour for whole pattern.*/
	
extern	void	SC2Paint( int X, int Y, int colour );
extern	void	SC2Draw( char *drawcommands );						/* Remake of BASICs "draw" with original commands (except A,X) syntax. */



#endif
