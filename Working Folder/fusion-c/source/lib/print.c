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
/*  print
|    
|     2018 Mvac
*/
#include "../../header/msx_fusion.h"
/* =============================================================================
 PRINT
  
 Description: 
           Displays a text string on the screen.             
                        
 Input:    (char*) String    
 Output:   -
 Notes:
            Supports escape sequences:
             \a (0x07) - Beep
             \b (0x08) - Backspace. Cursor left, wraps around to previous line, 
                         stop at top left of screen.
             \t (0x09) - Horizontal Tab. Tab, overwrites with spaces up to next 
                         8th column, wraps around to start of next line, scrolls
                         at bottom right of screen.
             \n (0x0A) - Newline > Line Feed and Carriage Return (CRLF) 
                         Note: CR added in this Lib.
             \v (0x0B) - Cursor home. Place the cursor at the top of the screen.
             \f (0x0C) - Formfeed. Clear screen and place the cursor at the top. 
             \r (0x0D) - CR (Carriage Return)
            
             \" (0x22) - Double quotation mark
             \' (0x27) - Single quotation mark
             \? (0x3F) - Question mark
             \\ (0x5C) - Backslash
============================================================================= */
void Print(char* text)
{
  char character;
  
  while(*(text)) 
  {
    character=*(text++);
    if (character=='\n')
    {
      PrintChar(10); //LF (Line Feed)
      PrintChar(13); //CR (Carriage Return)
    }else{ 
      PrintChar(character);
    } 
  }
}
