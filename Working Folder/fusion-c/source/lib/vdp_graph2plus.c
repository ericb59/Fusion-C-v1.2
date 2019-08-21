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
|                   V1.1b - August - 2019                    |
|                                                           |
|                Eric Boez &  Fernando Garcia               |
|                                                           |
|          C   &   A S M    S O U R C E   C O D E           |
|         compilation : > sdcc -mz80 -c  msx_misc.c         |
|                                                           |
\___________________________________________________________/
*/
/*  Line & BoxFill & BoxLine functions
|    
|     2019 Eric Boez
|    
*/


#include "../../header/vdp_graph2.h"

#define       RPeek( address )     ( *( (volatile unsigned char*)(address) ) )


void SubLine( int X1,  int Y1, int DX,  int DY, int colour, char R45, char OP ) __naked
{ 
X1;Y1;DX;DY;colour;R45;OP;
__asm

  push ix
  ld ix,#0
  add ix,sp

  di
  call    VDPready
  ld    a,#36
  out    (#0x99),a
  ld    a,#128+#17
  out    (#0x99),a    ;R#17 := 36

  ld    c,#0x9b

  ld  a,4(ix)  ;
  out    (c),a    ; R36 X1 low byte

  ld  a,5(ix)  ;
  out    (c),a    ; R37 X1 high byte

  ld  a,6(ix)  ;
  out    (c),a    ; R38 Y1 low byte

  ld  a,7(ix)  ;
  out    (c),a    ; R39 Y1 high byte

  ld  a,8(ix)  ;
  out (c),a     ; R40 NX low byte

  ld  a,9(ix)  ;
  out    (c),a    ; R41 NX high byte

  ld  a,10(ix) ;
  out    (c),a    ; R42 NY low byte

  ld  a,11(ix) ;
  out    (c),a    ; R43 NY high byte

  ld  a,12(ix) ;
  out    (c),a     ; R44 COL low byte

  ld  a,14(ix)
  out    (c),a    ; R45 ARG 

  ld  a,15(ix)

  or ##0b01110000
  out    (c), a   ; R46 do it

  ei
  pop    ix
  ret


VDPready:
  ld  a,#2

  out (#0x99),a          ;select status register 2
  ld  a,#128+#15
  out (#0x99),a
  in  a,(#0x99)

  and #1
  jp  nz, VDPready  ; wait

  xor a
  out (#0x99),a
  ld  a,#128+#15
  out (#0x99),a
  ret
__endasm;

}

void Line(  int x1,  int y1,  int x2,  int y2, char color, char OP ) 
{

    unsigned char R45;
    int dx,dy;
    unsigned int shortside,longside;

    R45=0b00000000;

    dx=x2-x1;
    dy=y2-y1;


    if (dx<0)
    {
        dx=dx*-1;
        R45=R45 | 0b00000100;
    }
   
    //////////////////////////
    if (dy<0)
    {
        dy=dy*-1;
        R45=R45 | 0b00001000;
     }
    
     //////////////////////////
    if (dy>dx)
    {
        longside=dy;
        shortside=dx;
        R45=R45 | 0b00000001;
    } 
    else
    {
        longside=dx;
        shortside=dy;
        R45=R45 | 0b00000000;
    }


    SubLine(x1,y1,longside,shortside,color,R45,OP);
    
}

void BoxFill (int x1, int y1, int x2, int y2, char color, char op )
{
    char scmode;

    scmode=RPeek (0xFCAF);

    if (op==0 && scmode > 7)
    {

        HMMV( x1, y1, x2-x1, y2-y1, color);   // Using HMMV only with Scree8 and no OP

    } 
    else
    {
        LMMV( x1, y1, x2-x1, y2-y1, color, op);
    }

}


void BoxLine( int x1,  int y1,  int x2,  int y2, char color, char OP )
{
  if (OP==255)
  {
    BoxFill( x1, y1, x2, y2, color,0);
  }
  else
  {
    Line(x1,y1,x2,y1,color,OP);
    Line(x1,y1,x1,y2,color,OP);
    Line(x2,y2,x2,y1,color,OP);
    Line(x2,y2,x1,y2,color,OP);
  }

}
