#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/vdp_graph2.h"
#include <string.h>
 
static FCB file;                            // Initialisatio de la structure pour le systeme de fichiers
 
 
unsigned char LDbuffer[2560];               // Création d'un buffer de 2560 octets (20 lignes en Sc5)

 
 void FT_SetName( FCB *p_fcb, const char *p_name )  // Routine servant à vérifier le format du nom de fichier
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
 
void FT_errorHandler(char n, char *name)            // Gère les erreurs
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
 
int FT_LoadSc5Image(char *file_name, unsigned int start_Y, char *buffer)        // Charge les données d'un fichiers
    {

        int rd=2560;

        FT_SetName( &file, file_name );
        if(fcb_open( &file ) != FCB_SUCCESS) 
        {
              FT_errorHandler(1, file_name);
              return (0);
        }

        fcb_read( &file, buffer, 7 );  // Skip 7 first bytes of the file  
        while (rd!=0)
        {
             rd = fcb_read( &file, buffer, 2560 );  // Read 20 lines of image data (128bytes per line in screen5)
             HMMC(buffer, 0,start_Y,256,20 ); // Move the buffer to VRAM. 
             start_Y=start_Y+20;
         }

return(1);
}
 
 
void main(void)
{ 

    char mypalette[] = {                // Palette SC5
    0, 0,0,0,
    1, 2,1,1,
    2, 6,5,4,
    3, 5,4,3,
    4, 5,5,3,
    5, 6,5,3,
    6, 7,6,4,
    7, 3,2,1,
    8, 7,5,2,
    9, 6,4,2,
    10, 4,3,2,
    11, 4,3,1,
    12, 5,3,2,
    13, 3,3,2,
    14, 3,1,0,
    15, 5,2,0
    };
 
    Screen(5);
    SetColors(15,0,0);    
    Cls();
    PutText(0,0,"Page 0-(Push a key to continue)",0);        // Page 0
    WaitForKey();                   // Attend une touche
  
    SetDisplayPage (2);             // Page 2
    SetActivePage(2);
    Cls();
    PutText(0,10,"Page 2-(Push a key to continue)",0);
    WaitForKey();                   // Attend une touche

     SetActivePage(0);
    FT_LoadSc5Image("mona.sc5",512,LDbuffer);       // On charge l'umage
 
    SetSC5Palette((Palette *)mypalette);
 
 
    WaitForKey();
    Screen(0);
    Exit(0);
 
}