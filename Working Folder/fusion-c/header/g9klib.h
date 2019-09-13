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
|                   V1.2 - August  2019.                    |
|                                                           |
|              Eric Boez &  Fernando Garcia                 |
|                                                           |
|   G9klib is Fusion-C Library for GFX9000 Graphic Card     |
|                                                           |
|    This G9KLIB is a port of the ASM G9KLIB by Team Bomba  |
|            https://www.teambomba.net/gfx9klib.html        |
|                      By Sylvain G.                        |
\___________________________________________________________/
*                         (°□°)
* 
*   This G9KLIB is currently in Beta Version. No manual Available at this time.
*/
#ifndef __G9KLIB_H__
#define __G9KLIB_H__

// Mode select defines for SetScreenMode

// Pattern mode 0 256 212
#define G9MODE_P1	0

// Pattern mode 1 512 212
#define G9MODE_P2	1

// Bitmap mode 1 256 212
#define G9MODE_B1	2

// Bitmap mode 2 384 240
#define G9MODE_B2	3

// Bitmap mode 3 512 212
#define G9MODE_B3	4

// Bitmap mode 4 768 240
#define G9MODE_B4	5

// Bitmap mode 5 640 400 (VGA)
#define G9MODE_B5	6

// Bitmap mode 6 640 480 (VGA)
#define G9MODE_B6	7

// Bitmap mode 7 1024 212 (Undocumented v9990 mode)
#define G9MODE_B7	8

// Stand by mode
#define G9SCR0_STANDBY		192
// Select Bit map mode
#define G9SCR0_BITMAP		128
// Select P1 mode
#define G9SCR0_P2			64
// Select P1 mode
#define G9SCR0_P1			0
// Master Dot clock not divided
#define G9SCR0_DTCLK		32
// Master Dot clock divided by 2
#define G9SCR0_DTCLK2		16
// Master Dot clock divided by 4
#define G9SCR0_DTCLK4		0

// 2 bits/dot
#define G9SCR0_2BIT			0
// 4 bits/dot
#define G9SCR0_4BIT			1
// 8 bits/dot
#define G9SCR0_8BIT			2
// 16 bits/dot
#define G9SCR0_16BIT		3

// Image size = 256
#define G9SCR0_XIM256		0
// Image size = 512
#define G9SCR0_XIM512		4
// Image size = 1024
#define G9SCR0_XIM1024		8
// Image size = 2048
#define G9SCR0_XIM2048		12

// G9MODE_B1 256, 212
// G9MODE_B2 384, 240
// G9MODE_B3 512, 212
// G9MODE_B4 768, 240
// G9MODE_B7 1024,212

#define G9ARG 44

//; Bit defines G9ARG
#define G9ARG_MAJ 1
#define G9ARG_NEG 2
#define G9ARG_DIX 4
#define G9ARG_DIY 8

#define G9LOP 45

#define G9LOP_WCSC 12

#define G9RED 32
#define G9GREEN 1024
#define G9BLUE 1

typedef struct {
	int  fileHandle; // Dos2 file handle of the openend G9B file
	char bitDepth;   // 2,4,8 or 16 bit
	char colorType;  // 0=64 color palette mode,64=256 color fixed ,128=YJK and 192=YUV mode
	char nrColors;   // Number of colors in palette mode
	int  width;		 // object width
	int  height;	 // object Height
	char compression;// 0 = no compression, other value = compression used
	char dataSize[3];//	data size (24 bits!) [0] is the less significant byte, [2] is the most significant
} G9B_OBJECT;

typedef struct {
	int  fileHandle;	// Dos2 file handle of the openend VFF file
	char name[16];		// vff font name
	char width;			// width
	char height;		// height
	char pitch;			// 0 = Fixed : 1 = Variable  (Still unsused)
	int  dataSize;		// Data size
	int  ptrOffsetTab;	// Pointer to a font offset table
	int  vramOffset;	// Pointer to base address of font in vram starting at #70000
	char ramFont;	// 0 font data in vram, 1 font data in ram
} VFF_OBJECT;

typedef struct {
	char y;	// sprite y position
	char pat; // sprite pattern number
	char x; // sprite x position
	char attr; // sprite attribute number
} SPRITE_ATTR;

	/* Detect presence of the Gfx9000 */
extern	unsigned char G9Detect( void );

	/*  */
extern	void G9WriteReg( char, char );

	/* Reset and initialize the Gfx9000 */
extern	void G9Reset( void );

	/* Enable display */
extern	void G9DisplayEnable( void );

	/* Disable display */
extern	void G9DisplayDisable( void );

/*  Set screen mode
	inputs :
	1 ; Mode
	2 ; Bit per dot
	3 ; Image size
	4 ; Interlace
	5 ; Palette control register

	output :
	  nothing

*/
extern	void G9SetScreenMode( unsigned char , unsigned char , unsigned char , unsigned char , unsigned char );

/* pointer to palette, number of bytes to write*/
extern	void G9InitPalette( void *, unsigned char );

/* Write palette data to the Gfx9000
	inputs :
	1 ; pointer to palette data
	2 ; Palette pointer offset
	3 ; Number of bytes to write
*/
extern	void G9WritePalette( void *, unsigned char, unsigned char );

/* Read palette data from the Gfx9000
	inputs :
	1 ; pointer to palette data
	2 ; Palette pointer offset
	3 ; Number of bytes to write
*/
extern	void G9ReadPalette( void *, unsigned char, unsigned char );

/* Enable sprites/mouse cursor */
extern	void G9SpritesEnable();

/* Disable sprites/mouse cursor */
extern	void G9SpritesDisable();

/* Disables all sprites by setting PR1 bit of the sprite attribute */
extern	void G9DisableAllSprites();

/* %000xyz0 (not checked)
   xyz: bits A17,A16,A15 of VRAM address of the sprite pattern table start
       usually =4 (=> address is $10000)*/
extern	void G9InitSpritePattern( unsigned char );

	/* sprite number (0 to 124) */
extern	void G9SetSprite( unsigned char );

	/* Set blitter command write mask */
extern	void G9SetCmdWriteMask( unsigned int );

	/* Set backdrop color
	   input : backdropcolor */
extern	void G9SetBackDropColor( unsigned char );

	/* Set blitter command color */
extern	void G9SetCmdColor( int );

	/* Set command back ground color */
extern	void G9SetCmdBackColor( int );

	/*  */
extern	void G9CmdWait( void );

	/*  */
extern	void G9WaitVsync( void );

	/* DrawFilledBox left,top,width,height,color */
//extern	void G9DrawFilledBox( int *, int );
extern	void G9DrawFilledBox( unsigned int, unsigned int, unsigned int, unsigned int, unsigned int );

	/* DrawLine left,top,width,height,color */
extern	void G9DrawLine( unsigned int, unsigned int, unsigned int, unsigned int, unsigned int );

	/*  x, y, color */
extern	void G9SetPoint( unsigned int, unsigned int, unsigned int );

	/*
  inputs : int X int Y
  output : color
  2bpp   b7    b6	 b5 b4 b3 b2 b1 b0
        color code	 unspecified
 
  4bpp  b7 b6 b5 b4    b3 b2 b1 b0
         color code    unspecified
 
  8bpp   b7 b6 b5 b4 b3 b2 b1 b0
               color code
 
  16bpp     b7 b6 b5 b4 b3 b2 b1 b0
  1st byte    color code low  byte
  2nd byte    color code high byte
*/
extern	int  G9GetPoint( unsigned int, unsigned int );

	/* Adjust Gfx9000 display
	horizontal adjust, vertical adjust
	return 0: OK
	return 1: Error in horizontal adjust
	return 2: Error in vertical adjust
	*/
extern	unsigned char G9SetAdjust( unsigned char, unsigned char );

	/*  Set vram write address */
extern	void G9SetVramWrite( unsigned char, unsigned int );

	/*  Set vram read address */
extern	void G9SetVramRead( unsigned char, unsigned int );

	/* pointer to parameters (format: left,top,width,height) */
extern	void G9SetupCopyRamToXY( unsigned int * );

	/* pointer to data, Bytes to copy */
extern	void G9CopyRamToXY( void *, unsigned int );

	/* pointer to parameters (format: left,top,width,height) */
extern	void G9SetupCopyXYToRam( void * );

	/* pointer to data, Bytes to copy */
extern	void G9CopyXYToRam( void *, unsigned int );

	/* pointer to bubber, number of bytes to send */
extern	void G9CopyRamToVram( void *, int );

 /* Pointer to data  (format: SourceX,SourceY,DestX,DestY,Width,height) struct G9COPY_XY_XY */
extern	void G9CopyXYToXY( void * );


extern	void G9InitPatternMode();

	/* pointer to pattern data
	   pattern number
	   layer number (0=A,1=B)
	*/
extern	void G9SetPatternData( void *, unsigned int, unsigned char );

	/* pointer to pattern data
	   pattern number
	   layer number (0=A,1=B) */
extern	void G9GetPatternData( void *, unsigned int, unsigned char );

	/* pattern number in generator table (on screen patterns 0 to A=8159,B=7679)
       pattern number in name table (on screen patterns 0 to 4095)
       layer number (0=A,1=B)			
 	*/
extern	void G9SetPattern( unsigned int, unsigned int, unsigned char );

	/* pattern number in generator table (on screen patterns 0 to A=8159,B=7679)
	   layer number (0=A,1=B)

       return : pattern number in name table (on screen patterns 0 to 4095) 
 	*/
extern	int  G9GetPattern( unsigned int, unsigned char );

	/* not finished ! 
		pointer to data  (format: SrcAddress,DestAddress,NrBytes)
	*/
extern	void G9PCopyVramToVram( void * );

	/* not finished ! */
extern	void G9PCopyVramToVramARegister( void * );

extern	void G9EnableLayerA();

extern	void G9EnableLayerB();

extern	void G9DisableLayerA();

extern	void G9DisableLayerB();

	/* ram address, destination tile, number of tiles */
extern	void G9PrintTilesA( void *, unsigned int, unsigned char );

	/* ram address, destination tile, number of tiles */
extern	void G9PrintTilesB( void *, unsigned int, unsigned char );

	/* interrupt function
	   interruption line
	 */
extern	void G9SetIntLine( unsigned int );

/*  inputs : 
	1 ; 0 = load data in vram, 1 = load font data in ram
	2 ; font name pointer
	3 ; VFFOBJECT pointer
   output :
        error code. O if Ok
*/
extern	unsigned char G9OpenVff( unsigned char, unsigned int *, void * );

/* inputs :
	1 : Pointer to VFF object
	2 : Pointer to font offset table, if it s zero it s not loaded
	3 : Pointer to buffer for loading, or if it s a ram font the destination address
   output :
        error code. O if Ok
*/
extern	unsigned char G9LoadFont( void *, void *, void * );

/* inputs : Pointer to VFF object */
extern	void G9SetFont( void * );

/*  inputs : 
	1 ; Pointer to image file name
	2 ; Pointer to G9B header data
   output :
        error code. O if Ok
*/
extern	unsigned char G9OpenG9B( void *, void * );

/*
 1 : Pointer to G9B object struct
 2 : Pointer to load buffer
 3 : Size of load buffer
 4 : Destination X
 5 : Destination Y
 6 : Palette offset in bytes. Only valid with palette modes. If a = 255 palette not loaded
*/
extern	unsigned char G9ReadG9B( void *, void *, unsigned int, unsigned int, unsigned int, unsigned char );

/*
 1 : Pointer to G9B/VFF object
*/
extern	unsigned char G9Close( void * );

extern	void G9PrintStringVram( void *, unsigned int, unsigned int );

/* pos x, pos y */
extern	void G9Locate( unsigned int, unsigned int );

/* Set scroll mode
   input: 0 = Roll by image space size
          1 = Roll by 256 lines
          2 = Roll by 512 lines
*/
extern	void G9SetScrollMode( unsigned char );

/* Set scroll X Layer A 
   input : number of pixels to scroll X */
extern	void G9SetScrollX( int );

/* Set scroll Y Layer A 
   input : number of pixels to scroll Y */
extern	void G9SetScrollY( int );

/* Set scroll X Layer B
   input : number of pixels to scroll X */
extern	void G9SetScrollXB( int );

/* Set scroll Y Layer B 
   input : number of pixels to scroll Y */
extern	void G9SetScrollYB( int );

#endif
