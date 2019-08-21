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
|   vdp_circle.h : Functions to draw circle on graphic scr  |
|                                                           |
|                                                           |
|                                                           |
\___________________________________________________________/
*                         (°□°)
*	Original code by Eric Boez - 2018	
*	
*/

#ifndef __VDP_CIRCLE__
#define __VDP_CIRCLE__
/* =============================================================================
CircleFilled 
 Draw a filled Circle
 Input    :  [INT] x0 = X coordonate
                   y0 = Y coordonate
                   radius = radius of circle
                   color = color of the circle
                   op = operation mode on color
============================================================================= */
void CircleFilled(int x0, int y0, int radius, int color, int OP);
void SC2CircleFilled(int x0, int y0, int radius, int color); // MSX1 Version
 

/* =============================================================================
Circle
 Draw a Circle
 Input    :  [INT] x0 = X coordonate
                   y0 = Y coordonate
                   radius = radius of circle
                   color = color of the circle
                   op = operation mode on color
============================================================================= */
void Circle(int x0, int y0, int radius, int color, int OP);
void SC2Circle(int x0, int y0, int radius, int color); // MS1 Version

#endif
