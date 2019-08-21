
#include <stdio.h>
#include "fusion-c/header/msx_fusion.h"
 

void main (void)
{

   Screen(0); 
      printf("\n\rTPA Read from (0x06 & 0x07): %u",Peekw(0x0006));
      printf("\n\r Please come back");
   

  }