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
/*  printnumber
|    
|     2018 Mvac
*/


#include "../../header/msx_fusion.h"

/* =============================================================================
 PrintNumber

 Description: 
           Prints an unsigned integer on the screen.  
 Input:    (unsigned int) numeric value          
 Output:   -
============================================================================= */
void PrintNumber(unsigned int value)
{
  char character;
  char text[]="     ";
  char *p;  

  num2Dec16(value, text,32); 
  
  p = text;  
  
  while(*(p))
  {
    character=*(p++);
    if (character!=32) bchput(character);
  }   
}

