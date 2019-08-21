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
|   vdp_spites.h : Managing MSX sprites                     |
|                                                           |
|    Most function Using BIOS.                              |
|                                                           |
\___________________________________________________________/
*                         (°□°)
*	Most original code from SOLID MSX C - (c) 1997
*	Eric Boez using FUSION-C Library functions
*	
*	
*/

#ifndef __VDP_SPRITES__
#define __VDP_SPRITES__

extern void 		Sprite8(void);							// Actiate 8x8 Sprite pattern size
extern void 		Sprite16(void);							// Activate 16x16 Sprite pattern size 
extern void 		SpriteSmall(void);						// Activate Normal Pixel Size sprite
extern void 		SpriteDouble(void);						// Activate Double Pixel Size sprite
extern void 		SpriteOff(void);						// Deactivation of sprites
extern void 		SpriteOn(void);							//Activation of sprites
extern void 		SpriteReset(void);						// Reset all sprite patterns and attributs
extern char 		SpriteCollision(void);					// Return 1 if a sprite collision is detected
extern char 		SpriteCollisionX(void);					// Return the X position of last sprite collision (Only MSX2)
extern char 		SpriteCollisionY(void);					// Return the Y position of last sprite collision (Only MSX2)
extern void 		PutSprite( char sprite_no,  char pattern_no, char x,  char y,  char ec_color );		// Put A sprite on Screen
extern void 		SetSpritePattern(  char pattern_no,  char* p_pattern,  char s_size ) ;				// Set a Sprite Pattern

/* 
MULTICOLORS SPRITES ON MSX2
Set colours of lines for MSX2 sprites over default mono-colour  
"or"| each byte with EC,CC,IC (g) 
---> Use SC5SpriteColors for Screen modes 5 & 6
---> Use SC8SpriteColors for Screen modes 7,8,10,11,12
*/
extern	void		SC5SpriteColors( int spriteNumber, unsigned char *data );	
extern	void		SC8SpriteColors( int spriteNumber, unsigned char *data );

/* For 16x16 sprites:
		1. Edit vertically 16 x 16-bit integers as bits (0b1111111100000000)
		2. Use this to convert to standard sprite (1-4quadr.) definition 32 char bytes
		Returns pointer to new data */
extern	unsigned char	*Sprite32Bytes( unsigned int *bindata );



/// -- Not documented actually better use other functions --
extern	void		SC5SpritePattern( int patternNumber, unsigned char *data );
extern	void		SC8SpritePattern( int patternNumber, unsigned char *data );
extern	void		SC5SpriteAttribs( int spriteNumber, int patternNumber, int X, int Y );
#endif
