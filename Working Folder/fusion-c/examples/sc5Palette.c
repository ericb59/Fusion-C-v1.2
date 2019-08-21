#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/vdp_graph2.h"

 
void main(void)
{ 
	int i,x,y;

    char mypalette[] = 
    {
		0, 0,0,0,
		1, 2,1,1,
		2, 6,5,4,
		3, 5,4,3,
		4, 5,5,3,
		5, 6,5,3,
		6, 7,6,4,
		7, 3,2,1,
		8, 7,5,2,
		9, 6,4,2,
		10, 4,3,2,
		11, 6,0,1,
		12, 5,3,2,
		13, 3,3,2,
		14, 3,1,0,
		15, 6,6,6
	};

	Screen(5);
	SetColors(0,0,0);
	SetSC5Palette((Palette *)mypalette);
	
	x=10;
	y=10;
	
	for ( i = 0; i < 16; ++i)
	{
		Rect( x*i, 100, x*i+15, 140, i, FILL_ALL);
	}

WaitForKey();
Screen(0);
Exit(0);

}
