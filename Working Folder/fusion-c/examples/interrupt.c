// Interrupt functions Test
// Fusion-c 1.1

#include "fusion-c/header/msx_fusion.h"
#include <stdio.h>



static unsigned int count = 0;

/* --------------------------------------------------------- */
static char my_interrupt( void ) {

  if( IsVsync() == 0 ) return 0;

  count++;
  return 1;
}

/* --------------------------------------------------------- */
int main( void ) {
  unsigned int prev_count;
  InitInterruptHandler();

  SetInterruptHandler( my_interrupt );

  prev_count = 0;
  while( prev_count < 600 ) {

    DisableInterrupt();
    prev_count = count;
    EnableInterrupt();

    printf( "%d\n\r", prev_count );
  }
  EndInterruptHandler();
  return 0;
}
