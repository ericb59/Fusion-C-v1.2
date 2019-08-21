//
// FUSION-C  Example
// Load a SC8 image by classic function, then save the image data directly to sector, then reload the images from sectors
// Use a free floppy disk to test this example, because it may erase data on the disk
// 
//
#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/vdp_graph2.h"
#include "fusion-c/header/io.h"
#include <stdio.h>
#include <string.h>


static FCB file;                        // Used by MSXDOS loading routines

unsigned char LDbuffer[2816];           // Load Buffer to store  data
unsigned char ReadBuffer[2816];         // Sector buffer to store data

/* ---------------------------------
                FT_SetName

    Set the name of a file to load
                (MSX DOS)
-----------------------------------*/ 
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

/* ---------------------------------
          FT_LoadSc8Image

    Load a SC8 Picture and put datas
  on screen, begining at start_Y line
-----------------------------------*/ 
int FT_LoadSc8Image(char *file_name, unsigned int start_Y, char *buffer)
    {
        int rd=2560;
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
            rd = fcb_read( &file, buffer, 2560 );
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

     int i;
     unsigned int t1,t2,t3;

    Screen(8);
    SetRealTimer(0);
   
    FT_LoadSc8Image("supergir.SC8",0,LDbuffer);  // Load a SC8 image to screen
    t1=RealTimer();

    SetDiskTrAdress(&LDbuffer[0]);              // Set Transfer Disk Adress to Buffer
    SetRealTimer(0);
    for ( i = 0; i < 22; ++i)
    { 
        HMCM_SC8( 0, i*10, 256, (i*10)+10, &LDbuffer ); // Copy Vram to Ram
        SectorWrite(1320+i*5, 0, 5);                    // Write Ram Buffer to sectors. Starting at sector 1320. 1 sectore store 512Bytes, thus 2 image's lines
    }

    t2=RealTimer();

    Cls();
    SetDiskTrAdress(&ReadBuffer[0]);                    // Set Disk transfert adress to the Sector Read buffer
    SetRealTimer(0);
    for ( i = 0; i < 22; ++i)
    { 
        SectorRead(1320+i*5, 0, 5);                     // Read  sectors to Buffer
        HMMC( &ReadBuffer, 0, i*10, 256, 10);           // transfer Buffer to VRAM
    }
    t3=RealTimer();
    
    WaitForKey();
    Screen(0);
    printf("\n\r Time used for Classic Read : %d",t1);       // Show time needed for each load/write operation
    printf("\n\r Time used for Sector write : %d",t2);
    printf("\n\r Time used for Sector Read : %d",t3);
    Exit(0);
}