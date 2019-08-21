//
// Fusion-C
// Example : Defining 16 x16 Sprites
//

#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/vdp_sprites.h"

/* --------------------------------------------------------- */
/*  SPRITE test                        */
/* ========================================================= */


static const unsigned char point_pattern0[] = {
  0b01000000,
  0b11100000,
  0b01000000, 
  0b00000000, 
  0b00000000, 
  0b00000000, 
  0b00000000, 
  0b00000000
};

static const unsigned char point_pattern1[] = {
  0b11000000,
  0b11000000,
  0b00000000, 
  0b00000000, 
  0b00000000, 
  0b00000000, 
  0b00000000, 
  0b00000000};

static const unsigned char point_pattern2[] = {
  0b00000111,
  0b00000111,
  0b00000000,
  0b00000000, 
  0b00000000, 
  0b00000000, 
  0b00000000, 
  0b00000000};

static const unsigned char point_pattern3[] = {
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000, 
  0b00000000, 
  0b00000000, 
  0b00000111, 
  0b00000111 
};


static const unsigned char point_pattern4[] = {
  0b00000000,
  0b00010000,
  0b00010000, 
  0b00010000, 
  0b00010000, 
  0b00010101, 
  0b01010101, 
  0b01011111
};

static const unsigned char point_pattern5[] = {
  0b10100000,
  0b10100000,
  0b10000000, 
  0b01000000, 
  0b00100000, 
  0b00100000, 
  0b00100000, 
  0b00111111
};

static const unsigned char point_pattern6[] = {
  
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000, 
  0b10000000, 
  0b11000000, 
  0b10100000, 
  0b00100000
};

static const unsigned char point_pattern7[] = {
  0b00100000,
  0b01100000,
  0b01000000, 
  0b01000000, 
  0b10000000, 
  0b10000000, 
  0b10000000, 
  0b10000000  
};

/* --------------------------------------------------------- */
void main( void ) {
    char x;
    char y;


    x=0;
    y=50;

  if(ReadMSXtype()==3)  // IF MSX is Turbo-R Switch CPU to Z80 Mode
    {
       ChangeCPU(0);
    }
  //
  Screen(1);
  
  SpriteReset();

  SpriteDouble();
  
  // The 16 x 16 pixels Sprite is made of 4 patterns

  SetSpritePattern( 0, point_pattern0,8 );
  SetSpritePattern( 1, point_pattern1,8 );
  SetSpritePattern( 2, point_pattern2,8 );
  SetSpritePattern( 3, point_pattern3,8 );

  SetSpritePattern( 4, point_pattern4,8 );
  SetSpritePattern( 5, point_pattern5,8 );
  SetSpritePattern( 6, point_pattern6,8 );
  SetSpritePattern( 7, point_pattern7,8 );

  Sprite16();
  



  while (Inkey()!=27)
  {  
      x=(x+1)&255;
      EnableInterupt()
      Halt();
      PutSprite (1,0,x,y,8);
      PutSprite (2,4,x,y+40,10);
  }
  Screen(1);
  Exit(0);
}
