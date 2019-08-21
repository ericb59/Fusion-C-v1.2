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
/*  functionkeys
|    
|     2018 Eric Boez
*/
#include "../../header/msx_fusion.h"


// Hide the Functions Keys
void FunctionKeys (char n)
{
        unsigned char *p;
        unsigned char bios_slot;
        REGS *registers;
        registers = _REGs();
        p = (char *) 0xFCC1;
        bios_slot = p[0];   //gives you bios slot/sublot
        registers->iy=bios_slot;  //bios slot/subslot
        if (n==0)
        {
          registers->ix=0x00cc;   //  Hide Fonction keys
        }
        else 
        {
          registers->ix=0x00cf;   //  Show Fonction keys 
        }
        IntBios();
}

