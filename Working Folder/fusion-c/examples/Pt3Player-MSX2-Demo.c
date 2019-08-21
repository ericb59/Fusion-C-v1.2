#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/vdp_graph2.h"
#include "fusion-c/header/vars_msxSystem.h"
#include "fusion-c/header/pt3replayer.h"

///  MSX2 
// PT3 Music + Screen 8

#define SONG_BUFFER 9000

static FCB file; 

MMMtask  t;

unsigned char song[SONG_BUFFER];  

/* ---------------------------------
            FT_errorHandler

          In case of Error
-----------------------------------*/ 
void FT_errorHandler(char n, char *name)
{
  InitPSG();
  Screen(0);
  SetColors(15,6,6);
  switch (n)
  {
      case 1:
          printf("\n\rFAILED: fcb_open(): %s ",name);
      break;

      case 2:
          printf("\n\rFAILED: fcb_close(): %s",name);
      break;  

      case 3:
          printf("\n\rSORRY: this game does not work on %s",name);
      break; 
  }
Exit(0);
}

/* ---------------------------------
                FT_SetName

    Set the name of a file to load
                (MSX DOS)
-----------------------------------*/ 
void FT_SetName( FCB *p_fcb, const char *p_name ) {
  char i, j;
  memset( p_fcb, 0, sizeof(FCB) );
  for( i = 0; i < 11; i++ ) {
    p_fcb->name[i] = ' ';
  }
  for( i = 0; (i < 8) && (p_name[i] != 0) && (p_name[i] != '.'); i++ ) {
    p_fcb->name[i] =  p_name[i];
  }
  if( p_name[i] == '.' ) {
    i++;
    for( j = 0; (j < 3) && (p_name[i + j] != 0) && (p_name[i + j] != '.'); j++ ) {
      p_fcb->ext[j] =  p_name[i + j] ;
    }
  }
}

/* ---------------------------------
          FT_LoadSc8Image

    Load a SC8 Picture and put datas
  on screen, begining at start_Y line
-----------------------------------*/ 
int FT_LoadSc8Image(char *file_name, unsigned int start_Y, char *buffer)
    {
        int rd=4352;
        int nbl=0;
        InitPSG();
        FT_SetName( &file, file_name );
        if(fcb_open( &file ) != FCB_SUCCESS) 
        {
          FT_errorHandler(1, file_name);
          return (0);
        }
        fcb_read( &file, buffer, 7 );  // Skip 7 first bytes of the file
        while(rd!=0)
        {

            rd = fcb_read( &file, buffer, 4352 );
                if (rd!=0)
                {
   
                  nbl=rd/256;
                  HMMC(buffer, 0,start_Y,256,nbl ); // Move the buffer to VRAM. 17 lines x 256 pixels  from memory
                  start_Y=start_Y+nbl; 
         
              }
        }
      if( fcb_close( &file ) != FCB_SUCCESS ) 
      {
         FT_errorHandler(2, file_name);
          return (0);
      }

return(1);
}


/* ---------------------------------
          FT_LoadData
  Load Data to a specific pointer
  size is the size of data to read
  skip represent the number of Bytes
  you want to skip from the begining of the file
  Example: skip=7 to skip 7 bytes of a MSX bin
-----------------------------------*/ 
int FT_LoadData(char *file_name, char *buffer, int size, int skip)
{
    
    FT_SetName( &file, file_name );
    if(fcb_open( &file ) != FCB_SUCCESS) 
    {
          FT_errorHandler(1, file_name);
          return (0);
    }
    if (skip>0)
    {
        fcb_read( &file, buffer, skip );
    }
   
    
    fcb_read( &file, buffer, size );
    
    if( fcb_close( &file ) != FCB_SUCCESS ) 
    {
      FT_errorHandler(2, file_name);
      return (0);
    }
    return(0);
}

/* ---------------------------------
          FT_ScreenCopy

   Copy a screen zone to another place
   x1 & y1 =Top left coordonate  pixel of the zone to copy
   dx = Width Size in pixels, of the zone to copy
   dy = Height Size in pixels, of the zone to copy
   x2 & y2 = Destination coordonate where to copy the zone
   src = Source Page number of the Zone
   dst = Destination number of the zone
   mode = OP mode of the copy
-----------------------------------*/ 
void FT_ScreenCopy(char x1, char y1, char dx, char dy, char x2, char y2, char src , char dst, char mode)  
{
    int src_y=0;
    int dst_y=0;

    if (src==1)
        src_y=256;
    if (dst==1)
        dst_y=256;
    
    t.X = x1; 
    t.Y = src_y+y1; 
    t.DX = dx;  
    t.DY = dy;     
    t.X2 = x2; 
    t.Y2 = dst_y+y2;           
    t.s0 = 0; 
    t.DI = 0; 
    t.LOP = mode;
    fLMMM(&t);
}

/* ---------------------------------
            FT_CheckFX
     Check if Playing Sound FX
          must be updated
-----------------------------------*/
void FT_CheckFX (void)
{ 
  /*
 if (TestFX()==1)
 {
      if (JIFFY!=0)
      {
       JIFFY=0;
        UpdateFX();
      }
  }
  */
}

char FT_RandomNumber (char a, char b)
{
    char random;
    
    random = rand()%(b-a)+a;  // Random number between a and b-1
    return(random);
}

void main (void)
{
  char k,music;
  int x,y,d;

  char *file[3]={"Music1.pt3","Music2.pt3","Music3.pt3"};
  int size[3]={8417,2501,4637};
  y=0;
  x=0;
  d=2;

  music=0;

  Screen(8);
  FT_LoadSc8Image("supergir.sc8", 256, song);

  if(ReadMSXtype()==3)  // IF MSX is Turbo-R Switch CPU to Z80 Mode
    {
       ChangeCPU(3);
    }

  InitPSG();
  
  FT_LoadData(file[music], song, size[music], 0);
  PT3Init (song+99, 0);
  Cls();
  

  
    while (1)
    {
      Halt();
      k=Inkey();
      if (k==27)
      {
        EnableInterupt(); 
        InitPSG();
        Screen(0);
        Exit(0);
      }

      DisableInterupt();
      PT3Rout();
      PT3Play();
      EnableInterupt();
      x=FT_RandomNumber(0,255);
      y=FT_RandomNumber(0,212);
      FT_ScreenCopy(x, y, 8, 8, x, y, 1 , 0, 0);
      
      
    
    }
   
  

}