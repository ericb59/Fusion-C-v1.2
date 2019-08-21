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
|                C   S O U R C E   C O D E                  |
|                                                           |
|                        CIRCLES                            |
\___________________________________________________________/
*/

#include "../../header/vdp_graph2.h"

// Draw a Circle
void Circle(int x0, int y0, int radius, int color, int OP)
{
    int x = radius;
    int y = 0;
    int err = 0;
 
    while (x >= y)
    {
    Pset(x0 + x, y0 + y, color, OP);
    Pset(x0 + y, y0 + x, color, OP);
    Pset(x0 - y, y0 + x, color, OP);
    Pset(x0 - x, y0 + y, color, OP);
    Pset(x0 - x, y0 - y, color, OP);
    Pset(x0 - y, y0 - x, color, OP);
    Pset(x0 + y, y0 - x, color, OP);
    Pset(x0 + x, y0 - y, color, OP);
 
    if (err <= 0)
    {
        y += 1;
        err += 2*y + 1;
    }
 
    if (err > 0)
    {
        x -= 1;
        err -= 2*x + 1;
    }
    }
}

