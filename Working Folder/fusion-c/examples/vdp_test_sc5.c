//
// VRAM COPY tests
// Use FUSION-C 1.1b
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

unsigned char  buffer[5000];      // Set a Memory buffer in RAM at 0x8000

char mystr[10];                               // Set a str buffer

void main (void)
{
  unsigned char i,j;

  
  Screen(5);
  SetColors(1,0,1);

  // 
  // All Test are copying a rectangle box from page 0 to page 1 , 20 Times at different x,y positions
  // 

  //////////////////////////////////// TEST YMMM
  SetDisplayPage(0);
  SetActivePage(0);
  Rect(0,0,255,211,4,255);
  WaitKey();
  HMMV( 0, 0, 256, 212, #0b01100110);    /// HMMV test Fill 
  PutText (10,180,"HMMV Fill",8);
  WaitKey();

  Rect(10,10,50,30,211,255);
  Rect(10,10,50,30,255,0);

  HMMM( 10, 10, 60, 10, 41, 21);      // Fast HMMM Copy to different X positions
  HMMM( 10, 10, 110, 10, 41, 21);
  HMMM( 10, 10, 160, 10, 41, 21);

   PutText (14,14,"YMMM1",8);
   PutText (64,14,"YMMM2",8);
   PutText (114,14,"YMMM3",8);
   PutText (164,14,"YMMM4",8);


  PutText (10,200,"Push a key",8);
  WaitKey();

  SetDisplayPage(1);
  SetActivePage(1);
  for (j=0;j<5;j++)
  {
        YMMM( 10, 10, 0, 256+10+j*40, 41, 21);
  }
 
  SetColors(15,0,1);
  PutText (10,200,"YMMM copy test",8);
  SetColors(1,0,1);

  WaitKey();
  Cls();
  SetDisplayPage(0);
  SetActivePage(0);
  Cls();

  //////////////////////////////////// TEST HMMM
  SetDisplayPage(0);
  SetActivePage(0);
  Rect(0,0,255,211,7,255);

  Rect(10,10,50,30,115,255);
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

  SetColors(15,0,1);
  PutText (10,200,"HMMM copy test",8);
  SetColors(1,0,1);

  WaitKey();
  Cls();
  SetDisplayPage(0);
  SetActivePage(0);
  Cls();

  //////////////////////////////////// TEST LMMM
  Rect(0,0,255,211,8,255);

  Rect(10,10,50,30,120,255);
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

  SetColors(15,0,1);
  PutText (10,200,"LMMM copy test",8);
  SetColors(1,0,1);

  WaitKey();
  Cls();
  SetDisplayPage(0);
  SetActivePage(0);
  Cls();

  //////////////////////////////////// TEST HMCM & LMMC
  Rect(0,0,255,211,9,255);

  Rect(10,10,50,30,55,255);
  Rect(10,10,50,30,255,0);
  PutText (14,14,"LMMC",8);

  PutText (10,200,"Push a key",8);
  WaitKey();
  
  SetDisplayPage(1);
  SetActivePage(1);

  HMCM_SC5( 10, 10, 52, 32, buffer, 0 );

  for (j=0;j<5;j++)
  {
      for (i=0;i<4;i++)
      {
      
        LMMC( buffer, 10+i*50, 256+10+j*40, 42, 22, 0 );
        
      }
  }
  SetColors(15,0,1);
  PutText (10,200,"HMCM + LMMC copy test",8);
  SetColors(1,0,1); 

  WaitKey();
  Cls();
  SetDisplayPage(0);
  SetActivePage(0);
  Cls();

  //////////////////////////////////// TEST HMCM & HMMC
  Rect(0,0,255,211,6,255);

  Rect(10,10,50,30,149,255);
  Rect(10,10,50,30,255,0);
  PutText (14,14,"HMMC",8);

  PutText (10,200,"Push a key",8);
  WaitKey();

  SetDisplayPage(1);
  SetActivePage(1);

  HMCM_SC5( 10, 10, 52, 32, &buffer, 0 );

  for (j=0;j<5;j++)
  {
      for (i=0;i<4;i++)
      {
      
        HMMC( &buffer, 10+i*50, 256+10+j*40, 42, 22 );
        
      }
  }
  SetColors(15,0,1);
  PutText (10,200,"HMCM + HMMC copy test",8);


  WaitKey();
  Cls();
  SetDisplayPage(0);
  SetActivePage(0);
  Cls();



  Screen(0);

  Exit(0);
}
