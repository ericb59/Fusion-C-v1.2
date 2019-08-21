// Loadding Font fiel for Screen1
// Vram Copy & Ram copy Eample
// Fusion-c 1.1

#include "fusion-c/header/msx_fusion.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static FCB file;    			// Initialisation of the file System Structure variable (Mandatory)

unsigned char buff[2048];		// Buffer Array of 2048 bytes. The size of the whole font data
unsigned char buff2[2048];

// This function Set the Name in File System variable Structure 
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


// This function is loading data 
void LoadingData(char *file_name, unsigned int size)
{
    char rd;

     FT_SetName( &file, file_name );

     fcb_open( &file );						      // open the File
     rd = fcb_read( &file, buff, 7);		// Read the first 7 byte of the file (We do not need them, it's the Ninary Header)

     rd = fcb_read( &file, buff, size );	// Loading all the binary in one way and store them in buff
     fcb_close(&file);
}
char FT_RandomNumber (char a, char b)
{
    char random;
    
    random = rand()%(b-a)+a;  // Random number between a and b-1
    return(random);
}
// Main 
void main(void)							
{

	unsigned int vram_adr;			
 
  
	Screen(1);							
	vram_adr=Peekw(0xF3C1);				// Read the adress where charaters data are stored in VRAM (It may change, but the correct value is alway at 0xF3B7)

	printf("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ\n\r");	

	LoadingData("FONT.ALF",2048);              // Loading Font binary file
  
	CopyRamToVram( &buff, vram_adr,2048);      // Copy the font binary to VRAM at the adress where the character pattern are stored
    
  printf(" Ram Source adr : %ul \n\r",&buff);

 WaitKey();

 CopyVramToRam( vram_adr, buff2, 2048);    // Copy the VRAM bytes to another Ram variable : buff2

 MemFill( buff, 0, 2048 );                  // File The Ram varaible BUFF with 0

 CopyRamToVram(buff, vram_adr, 2048);       // Copy buff to VRAM, all characters musy disapear

 WaitKey();

 CopyRamToVram( buff2, vram_adr, 2048);   // Now copy buff2 to Vram, characters comes back

 WaitKey();
 


 
Exit(0);
  
}