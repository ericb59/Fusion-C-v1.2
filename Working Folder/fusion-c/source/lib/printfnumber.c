/*
 ___________________________________________________________
/               __           _                              \
|              / _|         (_)                             |
|             | |_ _   _ ___ _  ___  _ __                   |
|             |  _| | | / __| |/ _ \| '_ \                  |
|             | | | |_| \__ \ | (_) | | | |                 |
|             |_|  \__,_|___/_|\___/|_| |_| *               |
|                                                           |
|               The MSX C Library for SDCC                  |
|                   V1.0 - 09-10-11 2018                    |
|                                                           |
|                Eric Boez &  Fernando Garcia               |
|                                                           |
|          C   &   A S M    S O U R C E   C O D E           |
|         compilation : > sdcc -mz80 -c  msx_misc.c         |
|                                                           |
\___________________________________________________________/
*/
/*  printFnumber
|    
|     2018 Mvac
*/


#include "../../header/msx_fusion.h"

/* =============================================================================
 PrintFNumber

 Description: 
           Prints an unsigned integer on the screen with formatting parameters.
 Input:    (unsigned int) numeric value
           (char) empty Char: (32=' ', 48='0', etc.)
           (char) length: 1 to 5          
 Output:   -
============================================================================= */
void PrintFNumber(unsigned int value, char emptyChar, char length)
{
  char pos=5;
  char text[]="     ";

  num2Dec16(value, text,emptyChar); //32=space, 48=zero 
  
  if(length>5) length=5;
  if(length==0) length=5;
  //coloca el puntero en la posicion donde se ha de empezar a mostrar 
  pos-=length;
  
  // muestra el numero en la pantalla
  while (length-->0){ bchput(text[pos++]);}
}


