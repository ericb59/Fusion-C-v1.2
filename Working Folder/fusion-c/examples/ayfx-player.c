//
// Fusion-C
// example : Ayfx Player
// 
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/vars_msxSystem.h"
#include "fusion-c/header/ayfx_player.h"



#define AFB_SIZE 1207

static FCB file; 

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
            FT_CheckFX
     Check if Playing Sound FX
          must be updated
-----------------------------------*/
void FT_CheckFX (void)
{ 
 if (TestFX()==1)
 {
      if (JIFFY!=0)
      {
       JIFFY=0;
        UpdateFX();
      }
  }
}

void main (void)
{
  char k;

  afbdata=MMalloc(AFB_SIZE);
  
  InitPSG();
  InitFX();

  
  FT_LoadData("test.afb", afbdata, AFB_SIZE, 0);
  Cls();
  
  printf("Ay FX Player --- ESC to EXIT");
  printf("\n\rPush on key a,b,c,d,e,f,g,h,i,j or k");
  printf("\n\rTo play sound fx");
    while (1)
    {
      k=Inkey();
      switch (k)
      {
          case 97:
             PlayFX(1);;
          break;

          case 98:
              PlayFX(2);
          break;  

          case 99:
             PlayFX(3);
           break;

            case 100:
             PlayFX(4);
           break;

            case 101:
             PlayFX(5);
           break;

            case 102:
             PlayFX(6);
           break;

            case 103:
             PlayFX(7);
           break;

            case 104:
             PlayFX(8);
           break;

            case 105:
             PlayFX(9);
           break;

            case 106:
             PlayFX(10);
           break;

           case 107:
             PlayFX(11);
           break;

           case 108:
             PlayFX(12);
           break;

           case 27:
               Exit(0);
           break; 
      }
      
      FT_CheckFX();
    
    }

  

}