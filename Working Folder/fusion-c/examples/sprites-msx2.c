#include <stdio.h>
#include "fusion-c/header/vdp_graph2.h"
#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/vdp_sprites.h"
#include "fusion-c/header/vars_msxSystem.h"

/* --------------------------------------------------------- */
/*  SPRITE test                        */
/* ========================================================= */
/*  2006/11/26  t.hara                     */
/* --------------------------------------------------------- */


static const unsigned char ball_pattern[] = {
  0x3C, /* 00111100 */
  0x7E, /* 01111110 */
  0xFF, /* 11111111 */
  0xFF, /* 11111111 */
  0xFF, /* 11111111 */
  0xFF, /* 11111111 */
  0x7E, /* 01111110 */
  0x3C  /* 00111100 */
};
static const unsigned char cross_pattern[] = {
  0b11111111,
  0b10010011,
  0b10010001,
  0b11111111,
  0b10010001,
  0b10010001,
  0b10010011,
  0b11111111
};


int x[32];
int y[32];
int vx[32];
int vy[32];

/* --------------------------------------------------------- */
static void wait( int j ) {
  volatile int i;

  for( i = 0; i < j; i++ ) {
  }
}
void stop(void)
{
  
}

/* --------------------------------------------------------- */
int main( void ) {
  int i, j;
  char col[8];
  
  char LineColors[8]= { 14,10,4,5,10,6,7,8 };

  if(ReadMSXtype()==3)  // IF MSX is Turbo-R Switch CPU to Z80 Mode
    {
       ChangeCPU(0);
    }
  SpriteReset();
  
  Screen(8);

  SpriteSmall();
  SetSpritePattern( 0, cross_pattern,8 );
  SetSpritePattern( 1, ball_pattern,8 );
  
  // MSX2 only
  for( i = 0; i < 32; i++ ) {
    for( j = 0; j < 8; j++ ) {
      col[j]=j+7;//(i & 7) + 8;
    }
    SC8SpriteColors(i,LineColors);
  }
  /// END MSX 2


  /* init */
  for( i = 0; i < 32; i++ ) {
    x[i] = (i * 37 + 5) & 255;
    y[i] = (i * 19 + 7) % 212;
    vx[i] = (((i * 23) & 1) << 3) - 4;
    vy[i] = (((i * 57) & 1) << 3) - 4;
  }
  SetColors(255,0,110);
  PutText(0,10,"SCREEN 8");
  /* loop */
  for( j = 0; j < 200; j++ ) {
    for( i = 0; i < 32; i++ ) {
      PutSprite( i, 1, x[i], y[i],  (i & 7) + 8 );
      wait(5);
      x[i] = x[i] + vx[i];


      if( x[i] > 255 ) {
        x[i] = 255;
        vx[i] = -vx[i];
      }
      else if( x[i] < 0 ) {
        x[i] = 0;
        vx[i] = -vx[i];
      }

      y[i] = y[i] + vy[i];
      if( y[i] > 211 ) {
        y[i] = 211;
        vy[i] = -vy[i];
      }
      else if( y[i] < 0 ) {
        y[i] = 0;
        vy[i] = -vy[i];
      }
    }
  }
  Screen(0);
  return 0;
}
