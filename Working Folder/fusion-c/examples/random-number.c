//
// Fusion-C
// Pseudo Random Number
//
#include <stdlib.h>
#include <stdio.h>
#include "fusion-c/header/msx_fusion.h"

TIME tm;  //Init the Time Structure variable


/* ---------------------------------
                FT_Random
           Return a Random Number 
             between a and b-1
-----------------------------------*/ 
char FT_RandomNumber (char a, char b)
{
    char random;
 
    random = rand()%(b-a)+a;  
    return(random);
}


void main(void) 
{
    char n,i;

    GetTime(&tm);               // Retreive MSX-DOS Clock, and set the tm strucutre with clock's values
    srand(tm.sec);              // use current clock seconds as seed in the random generator

    for(i=0;i < 10; i++)
    {
        n=FT_RandomNumber(1,101);     // Choose random number between 1 and 100 
        printf("%d,",n);
    }
 
    Exit(0);
}
 