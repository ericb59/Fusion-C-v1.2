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
/*  width
|    
|     2018 Eric Boez
*/
#include "../../header/msx_fusion.h"


// Width of screen in text mode
void Width(unsigned char n)
{
    unsigned char m;

    m=Peek(0xFCAF); // get actual screen mode

    if (m==0)
    {
        Poke(0xF3AE,n); // set column number for SCREEN 0
        Screen(0);
    }
    if (m==1)
    {
        Poke(0xF3AF,n); // set column number for SCREEN 1
        Screen(1);
    }
}
