//
// Fusion-C
// Example : Timer

#include <stdio.h>
#include "fusion-c/header/msx_fusion.h"

void main (void)
{
    long int t;
    unsigned int h,m,s;

    Screen(0);
    VDP50Hz();              // Set MSX2 VDP To 50hz

    SetRealTimer(0);        // Set Timer to 0

    while (Inkey()!=27)
    {
        t=RealTimer();      // Read Current Timer Value

        m=t/3000;           
        s=t/50;

        Locate(0,0);
        printf("%d",t);
        Locate(0,8);
        printf("Minutes : %d",m);
        Locate(0,9);
        printf("Seconds : %d",s);

    }
    Exit(0);

}