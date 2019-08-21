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
/*  width
|    
|     2018 Eric Boez
*/
#include "../../header/msx_fusion.h"


// Width of screen in text mode
void Width(char n)
{
    int m;

    m=Peek(0xFCAF);

    if (m==0)
    {
        Poke(0xF3AE,n);
        Screen(0);
    }
    if (m==1)
    {
        Poke(0xF3AF,n);
        Screen(1);
    }
}
