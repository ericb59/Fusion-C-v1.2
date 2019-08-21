//
// Fusion-C
// example : Covox Play
// 
// Load Data to VRAM then Send Data to Covox Module
//
// Add covox module to OpenMSX Emulator
// open Command line interface of OpenMSX
// Press F10 (Windows) or CMD+L (MAC)
// in the new window enter :
// plug printerport simpl
// <enter>
// Then close the command line  windows, as the same way you opened it
// Now Covox Module is active in OpenMSX Emulator
//
#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/vdp_graph2.h"
#include <string.h>
#include <stdio.h>

static FCB file;                        // Used by MSXDOS loading routines
MMMtask  t;

unsigned char LDbuffer[2400];

void FT_errorHandler(char n, char *name)
{
  InitPSG();
  Screen(0);
  SetColors(15,6,6);
  switch (n)
  {
      case 1:
          Print("\n\rFAILED: fcb_open(): ");
          Print(name);
      break;

      case 2:
          Print("\n\rFAILED: fcb_close():");
          Print(name);
      break;  

      case 3:
          Print("\n\rStop Kidding, run me on MSX2 !");
      break; 
  }
Exit(0);
}

void FT_SetName( FCB *p_fcb, const char *p_name ) 
{
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

int FT_LoadData(char *file_name, char *buffer, int size, int skip)
{
    InitPSG();
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
    SetColors(15,0,80);
    
    fcb_read( &file, buffer, size );
    
    if( fcb_close( &file ) != FCB_SUCCESS ) 
    {
      FT_errorHandler(2, file_name);
      return (0);
    }
    SetColors(16,0,0);
    return(0);
}

/* ---------------------------------
          FT_LoadSc8Image

    Load a SC8 Picture and put datas
  on screen, begining at start_Y line
-----------------------------------*/ 
int FT_LoadSc8Image(char *file_name, unsigned int start_Y, char *buffer)
    {
        int rd=2304;
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
            SetColors(15,0,buffer[192]);
            rd = fcb_read( &file, buffer, 2304 );
                if (rd!=0)
                {
                  SetColors(15,0,0);
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



void main(void)
{

    Screen(8);
    if(ReadMSXtype()==3)  // If MSX is Turbo-R Switch CPU to Z80 Mode
    {
      ChangeCPU(0);
    }
    PutText(1,1,"Loading ...");
    FT_LoadSc8Image("sample.msx",0,LDbuffer); /// Load Sample to Vram Page 0
    PutText(1,200,"Playing ...");
    while (Inkey()!=27)
    {
            CovoxPlay(0, 100, 21900, 12);
            WaitForKey();
    }
   
    Screen(0);
    Exit(0);
}