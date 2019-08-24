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
|                   V1.2 - 08 2019                          |
|                                                           |
|                Eric Boez &  Fernando Garcia               |
|                                                           |
|          C   &   A S M    S O U R C E   C O D E           |
|         compilation : > sdcc -mz80 -c  msx_misc.c         |
|                                                           |
\___________________________________________________________/
*/
/*  GetKeyMatrix
|    
|     2018 Mvac
*/



/* =============================================================================
   GetKeyMatrix
   Version: 1.0
   Date: 2 March 2016
   Author: mvac7/303bcn

   Function : Returns the value of the specified line from the keyboard matrix.
              Each line provides the status of 8 keys.
              To know which keys correspond, you will need documentation that 
              includes a keyboard table.
   Input    : [char] line 
   Output   : [char] state of the keys. 1 = not pressed; 0 = pressed

   International
  
bit  7     6     5     4     3     2     1     0
----------------------------------------------------
  0  7 &   6 ^   5 %   4 $   3 #   2 @   1 !   0 )
  1  ; :   ] }   [ {   \ |   = +   - _   9 (   8 *
  2  B     A     acent / ?   . >   , <   `     ' "
  3  J     I     H     G     F     E     D     C
  4  R     Q     P     O     N     M     L     K
  5  Z     Y     X     W     V     U     T     S
  6  F3    F2    F1    CODE  CAPS  GRAPH CTRL  SHIFT
  7  RET   SEL   BS    STOP  TAB   ESC   F5    F4
  8  Right Down  Up    Left  DEL   INS   HOME  SPACE
  9  NUM4  NUM3  NUM2  NUM1  NUM0  NUM/  NUM+  NUM*
 10  NUM.  NUM,  NUM-  NUM9  NUM8  NUM7  NUM6  NUM5
============================================================================= */
char GetKeyMatrix(char line) __naked
{
line;
__asm
  push IX
  ld   IX,#0
  add  IX,SP
     
  ld   A,4(IX)
  
  ld   IX,#0x0141
  ld   IY,(0xFCC1-1)
  call 0x001C
  ei
  
  ld   L,A
  
  pop  IX
__endasm;
}

