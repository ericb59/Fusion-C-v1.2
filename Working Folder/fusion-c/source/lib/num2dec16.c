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
/*  num2dec16
|    
|     2018 Mvac
*/



/* =============================================================================
 num2Dec16
 
 Description: 
           16-bit Integer to ASCII (decimal)
           Based on the code by baze.
 Input:    (unsigned int) a number
           (char*) Address where the output string is provided.
           (char) empty Char: 32=space, 48=zero
============================================================================= */
char CSTATE;
void num2Dec16(unsigned int aNumber, char *address, char emptyChar)
{
  aNumber;
  address;
  emptyChar;
__asm
  push IX
  ld   IX,#0
  add  IX,SP
  
  ld   L,4(ix)
  ld   H,5(ix)
  
;if (HL<0) Print "-" 
  
  ld   E,6(ix)
  ld   D,7(ix)
  
  ld   A,8(ix)   ;#32
  ld   (#_CSTATE),A
    
  ld   BC,#-10000
  call $Num1
  ld   BC,#-1000
  call $Num1
  ld   BC,#-100
  call $Num1
  ld   C,#-10
  call $Num1
  ;Last figure
  ld   C,B
  ld   A,#48
  ld   (#_CSTATE),A
  call $Num1

;END
  pop  IX
  ret
    
$Num1:  
  ld   A,#47 ; 0 ASCII code - 1
   
$Num2:  
  inc  A
  add  HL,BC
  jr   C,$Num2
  
  sbc  HL,BC
  
  cp   #48   ;"0" ASCII code    
  jr   NZ,$Num3  ;if A!=48 then goto $Num3
  
  ld   A,(#_CSTATE)
  jr   $Num4


$Num3:
  ;change space for 0 zero ASCII code
  push AF
  ld   A,#48
  ld   (#_CSTATE),A
  pop  AF 
  
$Num4:  
  ld   (DE),A   ;save a ascii code
  inc  DE
    
  ret
  
$Num5:
    
__endasm;
}

