#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/vdp_graph2.h"
#include "fusion-c/header/vdp_circle.h"

#define  HALT __asm halt __endasm   //wait for the next interrupt


// Generates a pause in the execution of n interruptions.
// PAL: 50=1second. ; NTSC: 60=1second. 
void Wait(int cicles)
{
  int i;
  for(i=0;i<cicles;i++) HALT;
  return;
}
 
void main(void)
{ 
    int i=255;

    Screen(2);
  for (i = 255; i > 155; --i)
  {
      Sc2Circle(100, 100,i-120, i, 0);
  }

WaitForKey();
Screen(0);

}
