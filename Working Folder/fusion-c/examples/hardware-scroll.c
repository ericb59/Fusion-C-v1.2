//
// Fusion-C
// Hardware Scrolling Screen 8 Example
//

#include <string.h>
#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/vdp_graph2.h"

static FCB file;                        // Init the FCB Structure varaible

unsigned char LDbuffer[2400];           // Set A loading Buffer of 2400 Bytes

/* ---------------------------------
                FT_Wait

                Waiting 
-----------------------------------*/ 
void FT_wait(int cicles)
{
  int i;
  for(i=0;i<cicles;i++) 
    {
      Halt();
    }
}

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
	int i,n;
  char rd;
  
  rd=ReadMSXtype();                   // Read MSX Type

  if (rd==0)                          // If MSX1 got to Error !
  {
    FT_errorHandler(3,"msx 1 ");
  }
  
  Screen(8);                          // Init Screen 8
  FillVram(0,0,65535);                // Clear VRAM Pape 0 by Byte 0 (Black)

  FT_LoadSc8Image("SUPERGIR.SC8",0,LDbuffer); // Loading Image To Page 0

  if (rd>=1)      // If Computer is At least MSX2 Do a Vertical Scroll
  {
    for (n = 0; n<3; n++)
    {
      for (i = 0; i < 256; ++i)
      {
        SetScrollV(i);
        FT_wait(1);
      }
    }
  }
  if (rd>=2)      // If computer is At least MSX2+ Do a Horizontal Scroll
  {
    for (n = 0; n<3; n++)
    {
      for (i = 0; i < 256; ++i)
      {
        SetScrollH(i);
        FT_wait(1);
      }
    }
  }

  	 Screen(0);
  	 Exit(0);

}
 
