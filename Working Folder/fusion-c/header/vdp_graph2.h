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
|                   V1.1b - August 2019                     |
|                                                           |
|                Eric Boez &  Fernando Garcia               |
|                                                           |
|   vdp_graph2.h : Drawing and graphic functions for MSX2   |
|                                                           |
|                                                           |
|                                                           |
\___________________________________________________________/
*                         (°□°)
*	Some original code from SOLID MSX C - (c) 1997
*	SDCC port 2015  by ?	
*   Some optimisation by BitVision (2019) for FUSION V1
*   New functions and commands implementation Sylvain Cregut & Eric Boez for Fusion-c 1.1b
*	
*	----------------------------------------------------------------------------
*	| SCREEN 5	256 (horizontal) x 212 (vertical) dots  	       |
*	|			16 colours can be displayed at the same time	   |
*	|			each of 16 colours can be selected from 512 colours|
*	| Command:	high speed graphic by VDP command available	       |
*	| Memory requirements:	32kB for screen - 6A00H bytes		   |
*	|			  (4 bits x 256 x 212)				               |
*	---------------------------------------------------------------------------- 
*
*	----------------------------------------------------------------------------
*	| SCREEN 8	256 (horizontal) x 256 (vertical) dots             |
*   |            (Only 212 high are displayes) 	                   |
*	|			256 colours can be displayed at the same time	   |
*	|			each color is unique and can't be changed (0 to 255|
*	| Command:	high speed graphic by VDP command available	       |
*	| Memory requirements: 64K from 0H to FFFFH for                |
*   |                      each of the 2 available pages       	   |
*	|			    (8 bits x 256 x 256)				           |
*	---------------------------------------------------------------------------- 
*	
*/

#ifndef __VDP_GRAPH2__
#define __VDP_GRAPH2__


/* Palette */
typedef struct {
	unsigned char colour;	// colour number 0..15
	unsigned char R;		// 0..7	red brightness
	unsigned char G;		// 0..7	green brightness
	unsigned char B;		// 0..7	blue brightness
} ColRGB;

typedef struct { 
	ColRGB rgb[16]; 
} Palette;


extern int		vMSX( void );												// 1-MSX1 , 2-MSX2  */
extern void		WriteScr( unsigned int addr_from );							// Whole SC5 RAM <=> VRAM dumb copy (0x6A00 bytes)  Writes screen from RAM memory address to VRAM, very dumb one time way, use HMMM! */
extern void		ReadScr( unsigned int addr_to ); 							// Reads SC5 from VRAM to memory address, very dumb one time way, use HMMM! */
extern void		SetSC5Palette (Palette *palette); 							// Set colors defined in given table
extern void		RestoreSC5Palette ( void );									// Sets default MSX palette
	
extern	void	Pset( int X,  int Y,  int colour, int OP );	/* puts pixel in (X,Y), logical OP=0 (just copy)  sends data to VDP chip directly */
extern	void	PsetXY( int X,  int Y,  int colour );		/* method 2 by rst 30h, writes in VRAM */
extern	char	Point( int X,  int Y ); 					/* gets colour 0..15 of pixel at (X,Y)  */
extern	char	PgetXY( int X,  int Y );					/* method 2 by rst 30h */

extern	void	Line( int x1,  int y1,  int x2,  int y2, char color, char OP );  		/* draws line (X1,Y)1-(X2,Y2), with logical operation*/
extern	void	Paint( int X, int Y, int colour, int border_colour );  					/* Paint for small regions. Split them if large. It is not the BASIC-subROM paint! Kind of bugPaint. Requires large stack of memory, slower than any image processing. */
extern	void	Draw( char *drawcommands );												/* Remake of BASICs "draw" with original commands (except A,X) syntax. */
extern  void 	BoxFill (int x1, int y1, int x2, int y2, char color, char op );			/* draws a filled rectangle (X1,Y1)-(X2,Y2), with logical operation and color */
extern	void	BoxLine( int x1,  int y1,  int x2,  int y2, char color, char OP ); 		/* draws rectangle (X1,Y1)-(X2,Y2), with logical operation */


/* VDP COMMANDS
   Hight speed graphic commands for the VDP.
*/
extern	void 	HMMC( void *pixeldatas, int X, int Y, int DX, int DY ); 						// High speed copy from RAM buffer to VRAM (size = DX*DY), X=0..255,Y=0..211
extern	void 	LMMC( void *pixeldatas, int X, int Y, int DX, int DY, unsigned char OP );		// Logical copy from RAM buffer to VRAM (size = DX*DY), X=0..255,Y=0..211
extern	void 	HMCM( int X1, int Y1, int X2, int Y2, void *tobuffer, unsigned char OP );		// High speed rectangle (X1,Y1)-(X2,Y2) copying from VRAM  to RAM buffer, no logical feature
extern	void 	HMCM_SC8( int X1, int Y1, int X2, int Y2, void *tobuffer, unsigned char OP );	// High speed rectangle (X1,Y1)-(X2,Y2) copying from VRAM  to RAM buffer, no logical feature
extern	void 	LMMM( int XS, int YS, int XT, int YT, int DX, int DY, unsigned char OP );		// High speed copy rectangle image with logical OP from VRAM to VRAM at (Xt,Yt) position
extern	void 	HMMM( int XS, int YS, int XT, int YT, int DX, int DY);							// High speed copy rectangle image from VRAM to VRAM at (Xt,Yt) position
extern	void 	YMMM( int XS, int YS, int DY, int NY, char DiRX);								// High speed copy of a part of iage to another Y position (DY) Block to move start at XS,YS, is NY pixels hight, and end at x=255 or x=0 depending of parameter DirX 1 or 0
extern	void 	HMMV( int XS, int YS, int DX, int DY, char COL);								// High speed fill of a reactangle box
extern	void 	LMMV( int XS, int YS, int DX, int DY, char COL, unsigned char OP);				// High speed fill of a reactangle box with logical operation


/*
VDP High speed commands are using the global system coordonates to adress the full 128KB of VRAM

	  (SCREEN 5)                       				  (SCREEN 6)
------------------------------    00000H    ------------------------------
| (0,0)              (255,0) |      |       | (0,0)              (511,0) |
|           Page 0           |      |       |           Page 0           |
| (0,255)          (255,255) |      |       | (0,255)          (511,255) |
|----------------------------|    08000H    |----------------------------|
| (0,256)          (255,256) |      |       | (0,256)          (511,256) |
|           Page 1           |      |       |           Page 1           |
| (0,511)          (255,511) |      |       | (0,511)          (511,511) |
|----------------------------|    10000H    |----------------------------|
| (0,512)          (255,512) |      |       | (0,512)          (511,512) |
|           Page 2           |      |       |           Page 2           |
| (0,767)          (255,767) |      |       | (0,767)          (511,767) |
|----------------------------|    18000H    |----------------------------|
| (0,768)          (255,768) |      |       | (0,768)          (511,768) |
|           Page 3           |      |       |           Page 3           |
| (0,1023)        (255,1023) |      |       | (0,1023)        (511,1023) |
------------------------------    1FFFFH    ------------------------------

    	  (SCREEN 8)                       			  (SCREEN 7)
------------------------------    00000H    ------------------------------
| (0,0)              (255,0) |      |       | (0,0)              (511,0) |
|           Page 0           |      |       |           Page 0           |
| (0,255)          (255,255) |      |       | (0,255)          (511,255) |
|----------------------------|    10000H    |----------------------------|
| (0,256)          (255,256) |      |       | (0,256)          (511,256) |
|           Page 1           |      |       |           Page 1           |
| (0,511)          (255,511) |      |       | (0,511)          (511,511) |
------------------------------    1FFFFH    ------------------------------
*/
// (a) LOP to "or"|: operations by code.
#define opHMMC 0xF0
#define opYMMM 0xE0
#define opHMMM 0xD0
#define opHMMV 0xC0
#define opLMMC 0xB0
#define opLMCM 0xA0
#define opLMMM 0x90
#define opLMMV 0x80

#define DIX_RIGHT 0
#define DIY_DOWN 0
#define DIX_LEFT 4
#define DIY_UP 8

// (b) DI to "or"|: Expanded RAM , default is 0 for VRAM
#define MSX_source_ExpRAM 16
#define MXD_dest_ExpRAM 32
// (b) DI to "or"|: Stop when colour found
#define MSX_EQ_stop 2

typedef struct {
	unsigned int X;		// source X (0 to 511)
	unsigned int Y;		// source Y (0 to 1023)
	unsigned int X2;	// destination X (0 to 511)
	unsigned int Y2;	// destination Y (0 to 1023)
	unsigned int DX; 	// width (0 to 511)
	unsigned int DY; 	// height (0 to 511)
	unsigned char s0;	// set to 0, dummy 1st empty byte sent to chip
	unsigned char DI;	// set to 0 (b), works well from left to right
	unsigned char LOP;	// 0 to copy (a), Logical+Operation ("or"| definitions)	
} MMMtask;

extern	void fLMMM( MMMtask *VDPtask );		// fast copy by structure


#endif
