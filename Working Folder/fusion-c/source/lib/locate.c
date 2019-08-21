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
|                                                           |
|                                                           |
\___________________________________________________________/
*/
/* =============================================================================
 LOCATE  by Mvac
 
 Description: 
           Moves the cursor to the specified location.
 Input:    (char) Position X of the cursor. (0 to 31 or 79)
           (char) Position Y of the cursor. (0 to 23)         
 Output:   -
============================================================================= */
#define EXPTBL  0xFCC1
#define CALSLT  0x001C
#define POSIT   0x00c6
void Locate(char x, char y)
{
x;y;
__asm
  push IX
  ld   IX,#0
  add  IX,SP
  
  ld   A,4(IX) ;x
  inc  A       ;incrementa las posiciones para que se situen correctamente en la pantalla
  ld   H,A
  ld   A,5(IX) ;y
  inc  A
  ld   L,A
     
  ld   IX,#POSIT
  ld   IY,(EXPTBL-1)
  call CALSLT
  ei
  
  pop  IX
__endasm;
}