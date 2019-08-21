//
// VDP Comands  tests Screen 8
// Use FUSION-C 1.2
//
// Eric Boez

#include <stdio.h>
#include "fusion-c/header/vdp_graph2.h"
#include "fusion-c/header/msx_fusion.h"


/*   MSX Memory pages adress

page 0 #0000 - 3FFFF
page 1 #4000 - 7FFFF
page 2 #8000 - BFFFF
page 3 #C000 - FFFFF

*/

unsigned char  __at(0x8000) buffer[2500];      // Set a Memory buffer in RAM at 0x8000

char mystr[10];                               // Set a str buffer

char FlipBit(char bit)
{
  return (!bit);
}



void main (void)
{
  unsigned char i,j,c;

  c=1;
  Screen(8);
  SetColors(255,0,1);

  // 
  // All Test are copying a rectangle box from page 0 to page 1 , 20 Times at different x,y positions
  // 

  //////////////////////////////////// TEST HMMV Test Filling
  SetDisplayPage(0);
  SetActivePage(0);
  Cls();


  BoxFill(0,0,255,212,99,0);


  for ( i = 0; i < 110; i=i+10)
  {

    HMMV( i, i, 256-(2*i), 212-(2*i), 45+i);
  }
 
  PutText (10,180,"HMMV Fill",8);
  WaitKey();

  Cls();

  //////////////////////////////////// TEST LMMV Test Filling
  for (i=0; i<25; i++)
  {
       LMMV( i*10, 0, 10, 212, 4+i*10, 2);    /// LMMV test Fill with LOGICAL_AND

  }
  PutText (10,180,"LMMV Fill",8);
  WaitKey();

  SetDisplayPage(0);
  SetActivePage(0);
  Cls();



  //////////////////////////////////// TEST YMMM

  BoxFill(0,0,255,212,100,0);

  BoxFill(10,10,50,30,211,0);
  Rect(10,10,50,30,255,0);

  HMMM( 10, 10, 60, 10, 41, 21);      // Fast HMMM Copy to different X positions
  HMMM( 10, 10, 110, 10, 41, 21);
  HMMM( 10, 10, 160, 10, 41, 21);

   PutText (14,14,"YMMM1",8);
   PutText (64,14,"YMMM2",8);
   PutText (114,14,"YMMM3",8);
   PutText (164,14,"YMMM4",8);

  c=1;

  PutText (10,200,"Push a key",8);
  WaitKey();

  SetDisplayPage(1);
  SetActivePage(1);
  for (j=0;j<5;j++)
  {
        YMMM( 10, 10, 256+10+j*40, 20, c);
        c=FlipBit(c);
  }
 
  
  PutText (10,200,"YMMM copy test",8);
  
  WaitKey();
  Cls();
  SetDisplayPage(0);
  SetActivePage(0);
  Cls();


  //////////////////////////////////// TEST HMMM
  SetDisplayPage(0);
  SetActivePage(0);
  BoxFill(0,0,255,211,70,0);

  BoxFill(10,10,50,30,115,0);
  Rect(10,10,50,30,255,0);
  PutText (14,14,"HMMM",8);

  PutText (10,200,"Push a key",8);
  WaitKey();

  SetDisplayPage(1);
  SetActivePage(1);
  for (j=0;j<5;j++)
  {
      for (i=0;i<4;i++)
      {
        HMMM( 10, 10, 10+i*50, 256+10+j*40, 41, 21);
      }
  }

  PutText (10,200,"HMMM copy test",8);

  WaitKey();
  Cls();
  SetDisplayPage(0);
  SetActivePage(0);
  Cls();



  //////////////////////////////////// TEST LMMM
  BoxFill(0,0,256,212,89,0);

  BoxFill(10,10,50,30,120,0);
  Rect(10,10,50,30,255,0);
  PutText (14,14,"LMMM",8);

  PutText (10,200,"Push a key",8);
  WaitKey();
  
  SetDisplayPage(1);
  SetActivePage(1);

  for (j=0;j<5;j++)
  {
      for (i=0;i<4;i++)
      {
        LMMM( 10, 10, 10+i*50, 256+10+j*40, 41, 21,0);
        
      }
  } 

  PutText (10,200,"LMMM copy test",8);

  WaitKey();
  Cls();
  SetDisplayPage(0);
  SetActivePage(0);
  Cls();

  //////////////////////////////////// TEST HMCM & LMMC
  BoxFill(0,0,256,212,124,0);

  BoxFill(10,10,50,30,55,0);
  Rect(10,10,50,30,255,0);
  PutText (14,14,"LMMC",8);

  PutText (10,200,"Push a key",8);
  WaitKey();
  
  SetDisplayPage(1);
  SetActivePage(1);

  HMCM_SC8( 10, 10, 51, 31, buffer, 0 );

  for (j=0;j<5;j++)
  {
      for (i=0;i<4;i++)
      {
      
        LMMC( buffer, 10+i*50, 256+10+j*40, 41, 21, 0 );
        
      }
  }

  PutText (10,200,"HMCM + LMMC copy test",8);

  WaitKey();
  Cls();
  SetDisplayPage(0);
  SetActivePage(0);
  Cls();

  //////////////////////////////////// TEST HMCM & HMMC
  BoxFill(0,0,256,212,3,0);

  BoxFill(10,10,50,30,150,0);
  Rect(10,10,50,30,255,0);
  PutText (14,14,"HMMC",8);

  PutText (10,200,"Push a key",8);
  WaitKey();

  SetDisplayPage(1);
  SetActivePage(1);

  HMCM_SC8( 10, 10, 51, 31, &buffer, 0 );

  for (j=0;j<5;j++)
  {
      for (i=0;i<4;i++)
      {
      
        HMMC( &buffer, 10+i*50, 256+10+j*40, 41, 21 );
        
      }
  }

  PutText (10,200,"HMCM + HMMC copy test",8);
   //////////////////////////////////// TEST Line
  WaitKey();
  Cls();
  SetDisplayPage(0);
  SetActivePage(0);
  Cls();

   for (i=0;i<255;i++)
  {
      Line(128,106,i,0,i,0);
      Line(128,106,i,212,20,0);
  }

  for (i=0;i<212;i++)
  {
      Line(128,106,0,i,i,0);
      Line(128,106,255,i,30,0);
  }
  PutText (10,200,"Line test",8);
  WaitKey();
  Cls();
  SetDisplayPage(0);
  SetActivePage(0);
  Cls();
  Screen(0);

  Exit(0);
}
