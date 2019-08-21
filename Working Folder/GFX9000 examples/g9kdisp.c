/* Fusion-C G9Klib #Loading and show image
 *
 * V9990
 * launch openMSX with "-ext gfx9000" option
 *
 */


#include <stdio.h>
#include <string.h>
#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/g9klib.h"

#define XMAX 512
#define YMAX 424
#define BUFSIZE 0x2000

static G9B_OBJECT g9bo;

extern unsigned char *heap_top;

//static unsigned char buffer[BUFSIZE]={0};
static unsigned char *buffer;

int main(void)
{
	/* definitions */
	unsigned char rc=0; // return code

	unsigned char *p = (void *)heap_top;		// start of heap_top
//	unsigned char *k = (void *)heap_tip;
	//allocate buffer
	if ((buffer = MMalloc(BUFSIZE)) == 0) {
		printf ("malloc buffer failed\n\r");
		return 1;
	}

	printf("@buffer=%x\n\r",buffer);
	printf("@heap_top=%x\n\r",p);
	printf("@HighMem==%x\n\r",Peek(0xF349)+256*Peek(0xF34A));

	/* code */
	if (G9Detect()) {
		printf("GFX9000 detect failed\n\r");
		return 1;
	}

	printf("GFX9000 detect OK\n\r");

	G9Reset();

	G9SetScreenMode(G9MODE_B3,G9SCR0_16BIT,G9SCR0_XIM512,1,0);
//	G9SetScreenMode(G9MODE_B3,G9SCR0_8BIT,G9SCR0_XIM512,0,0);
//	G9SetScreenMode(G9MODE_B2,G9SCR0_16BIT,G9SCR0_XIM256,1,0);
//	G9SetScreenMode(G9MODE_B1,G9SCR0_16BIT,G9SCR0_XIM256,1,0);

	printf("SetScreenMode\n\r");

	G9WaitVsync();

	G9DisplayDisable();

	// Set default blitter settings
	G9WriteReg(G9ARG,0);
	G9WriteReg(G9LOP,G9LOP_WCSC);

	G9SetCmdWriteMask(0xffff);

	// black filled box all screen
	G9DrawFilledBox(0,0,XMAX,YMAX,0xFFFF);

	printf("DrawFilledBox\n\r");

/* IMAGE */

	g9bo.fileHandle=0; // Dos2 file handle of the openend G9B file
	g9bo.bitDepth=0;   // 2,4,8 or 16 bit
	g9bo.colorType=0;  // 0=64 color palette mode,64=256 color fixed ,128=YJK and 192=YUV mode
	g9bo.nrColors=0;   // Number of colors in palette mode
	g9bo.width=0;      // object width
	g9bo.height=0;     // object Height
	g9bo.compression=0;// 0 = no compression, other value = compression used
	g9bo.dataSize[0]=0;
	g9bo.dataSize[1]=0;
	g9bo.dataSize[2]=0;

	if ((rc=G9OpenG9B("LAKE.G9B", &g9bo))!=0) {
//	if ((rc=G9OpenG9B("AUTUMN.G9B", &g9bo))!=0) {
//	if ((rc=G9OpenG9B("EASTERN.G9B", &g9bo))!=0) {
//	if ((rc=G9OpenG9B("ILLIPIKA.G9B", &g9bo))!=0) {
		printf("Open Image failed. Code=%d\n\r", rc);
		return 1;
	}

	printf("OpenG9B OK\n\r");
	printf("fileHandle=%x\n\r",g9bo.fileHandle);
	printf("bitDepth=%x\n\r",g9bo.bitDepth);
	printf("colorType=%x\n\r",g9bo.colorType);
	printf("nrColors=%x\n\r",g9bo.nrColors);
	printf("width=%x\n\r",g9bo.width);
	printf("height=%x\n\r",g9bo.height);
	printf("compression=%x\n\r",g9bo.compression);
	printf("dataSize1=%x\n\r",g9bo.dataSize[0]);
	printf("dataSize2=%x\n\r",g9bo.dataSize[1]);
	printf("dataSize3=%x\n\r",g9bo.dataSize[2]);

	G9SetScrollX(0);
	G9SetScrollY(0);
	G9SetScrollXB(0);
	G9SetScrollYB(0);

	G9DisplayEnable();

	if ((rc=G9ReadG9B(&g9bo,buffer,BUFSIZE,0,0,255))!=0) {
		printf("Read Image failed. Code=%d\n\r", rc);
		return 1;
	}

	printf("ReadG9B OK\n\r");

	G9Close(&g9bo);

	printf("Close OK\n\r");

 	getchar();

/* END IMAGE */

	return 0;
}
