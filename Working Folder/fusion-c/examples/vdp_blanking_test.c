/* --------------------------------------------------------- */
/*  VDP BLANKING TEST FOR V9958                              */
/* ========================================================= */
/*  2008/05/04  t.hara                                       */
/* --------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "fusion-c/header/msx_fusion.h"


#define TEST_TIMES  10


/* --------------------------------------------------------- */
static int get_h_blanking_time( void ) {
    unsigned char start_time, end_time;
    int diff;

    DisableInterupt();
    VDPwriteNi( 15, 2 );                  /* VDP status register S#2 read */
    while( (InPort(0x99) & 0x20) != 0 );    /* wait HR=1 */
    start_time = InPort(0xE6);
    while( (InPort(0x99) & 0x20) == 0 );    /* wait HR=0 */
    end_time = InPort(0xE6);
    VDPwriteNi( 15, 0 );
    EnableInterupt();

    diff = (int)end_time - (int)start_time;
    if( diff < 0 ) diff += 256;
    return diff;
}

/* --------------------------------------------------------- */
static int get_v_blanking_time( void ) {
    unsigned char start_time, end_time;
    int diff;

    DisableInterupt();
    VDPwriteNi( 15, 2 );                  /* VDP status register S#2 read */
    while( (InPort(0x99) & 0x40) != 0 );    /* wait VR=1 */
    start_time = InPort(0xE6);
    while( (InPort(0x99) & 0x40) == 0 );    /* wait VR=0 */
    end_time = InPort(0xE6);
    VDPwriteNi( 15, 0 );
    EnableInterupt();

    diff = (int)end_time - (int)start_time;
    if( diff < 0 ) diff += 256;
    return diff;
}

long int Timer(void)
{
    return(Peekw(0xFC9E));
}

void SetTimer(int value)
{
    Pokew(0xFC9E,value);
}
/* --------------------------------------------------------- */
int main( void ) {
    int i;
    int hd_time[ TEST_TIMES ];
    int vd_time[ TEST_TIMES ];
    int nb=19;
char src[4]="bag";

    SetRealTimer(0);
    
    Screen(0);

    for( i = 0; i < TEST_TIMES; i++ ) {
        hd_time[i] = get_h_blanking_time();
    }
    for( i = 0; i < TEST_TIMES; i++ ) {
        vd_time[i] = get_v_blanking_time();
    }

    for( i = 0; i < TEST_TIMES; i++ ) {
        printf( "\n\rHD[%d] = %d\n", i + 1, hd_time[i] );
    }
    for( i = 0; i < TEST_TIMES; i++ ) {
        printf( "\n\rVD[%d] = %d\n", i + 1, vd_time[i] );
    }
    
    return 0;
}
