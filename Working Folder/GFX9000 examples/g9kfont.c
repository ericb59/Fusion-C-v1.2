/* Fusion-C G9Klib #Loading and use fonts
 *
 * V9990
 * launch openMSX with "-ext gfx9000" option
 *
 */

 #include <stdio.h>
#include <string.h>
#include "fusion-c/header/g9klib.h"
#include "fusion-c/header/msx_fusion.h"

#define XMAX 512
#define YMAX 424
//#define XMAX 256
//#define YMAX 212
#define BUFSIZE 0x1000
//#define ZEFONT "ANTIQUE.VFF"
//#define ZEFONT "CP111.VFF"
//#define ZEFONT "COMPUTER.VFF"
#define ZEFONT "DIGITAL.VFF"

static VFF_OBJECT vffo;

extern unsigned char *heap_top;

static unsigned char *buffer;
static unsigned char *fontOffsetTable;

int main(void)
{
	/* definitions */
	unsigned char rc=0; // return code

	unsigned char *p = (void *)heap_top;		// start of heap_top

	printf("@heap_top=%x\n\r",p);
	printf("@HighMem=%x\n\r",Peek(0xF349)+256*Peek(0xF34A));

	/* code */
	if (G9Detect()) {
		printf("GFX9000 detect failed\n\r");
		return 1;
	}

	printf("GFX9000 detect OK\n\r");

	//allocate buffer
	if ((buffer = MMalloc(BUFSIZE)) == 0) {
		printf ("malloc buffer failed\n\r");
		return 1;
	}

	G9Reset();

	G9SetScreenMode(G9MODE_B3,G9SCR0_16BIT,G9SCR0_XIM512,1,0);
//	G9SetScreenMode(G9MODE_B3,G9SCR0_8BIT,G9SCR0_XIM512,0,0);
//	G9SetScreenMode(G9MODE_B2,G9SCR0_16BIT,G9SCR0_XIM256,0,0);
//	G9SetScreenMode(G9MODE_B1,G9SCR0_16BIT,G9SCR0_XIM256,1,0);

	printf("SetScreenMode\n\r");

	G9WaitVsync();

	G9DisplayDisable();

	// Set default blitter settings
	G9WriteReg(G9ARG,0);
	G9WriteReg(G9LOP,G9LOP_WCSC);

	G9SetCmdWriteMask(0xffff);

	// black filled box all screen
	G9DrawFilledBox(0,0,XMAX,YMAX,0xffff);

	printf("DrawFilledBox\n\r");

/* FONT */

	vffo.fileHandle = 0;
	strcpy(vffo.name,"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
	vffo.width=0;			// width
	vffo.height=0;			// height
	vffo.pitch=0;			// 
	vffo.dataSize=0;		// Data size
	vffo.ptrOffsetTab=0;	// Pointer to a font offset table
	vffo.vramOffset=0;		// Pointer to base address of font in vram starting at #70000
	vffo.ramFont=0;			// 0 font data in vram, 1 font data in ram

	if ((rc=G9OpenVff(0, ZEFONT, &vffo))!=0) {
		printf("Open VFF failed. Code=%d\n\r", rc);
		return 1;
	}

	//allocate fontOffsetTable
	if ((fontOffsetTable = MMalloc(vffo.dataSize)) == 0) {
		printf ("malloc fontOffsetTable failed\n\r");
		return 1;
	}

	printf("@buffer=%x\n\r",buffer);
	printf("@fontOffsetTable=%x\n\r",fontOffsetTable);

	printf("OpenVff OK\n\r");
	printf("fileHandle=%x\n\r",vffo.fileHandle);
	printf("name=%s\n\r",vffo.name);
	printf("width=%x\n\r",vffo.width);
	printf("height=%x\n\r",vffo.height);
	printf("pitch=%x\n\r",vffo.pitch);
	printf("dataSize=%x\n\r",vffo.dataSize);
	printf("ptrOffsetTab=%x\n\r",vffo.ptrOffsetTab);
	printf("vramOffset=%x\n\r",vffo.vramOffset);
	printf("ramFont=%x\n\r",vffo.ramFont);

	if ((rc=G9LoadFont(&vffo,buffer,fontOffsetTable))!=0) {
		printf("Load VFF failed. Code=\n\r%d", rc);
		return 1;
	}

	printf("LoadFont OK\n\r");

	G9SetFont(&vffo);

	printf("SetFont\n\r");

	G9Close(&vffo);

	printf("Close\n\r");

	G9SetCmdWriteMask(0xffff);

	G9SetCmdBackColor(0xffff);
	G9SetCmdColor(7*G9RED+3*G9GREEN+2*G9BLUE);

	G9DisplayEnable();

	G9PrintStringVram("Font : ",10,10);
	G9PrintStringVram(ZEFONT,10,30);
	G9PrintStringVram("- ABCDEFGHIJKLMNOPQRSTUVWXYZ -",10,50);
	G9PrintStringVram("- abcdefghijklmnopqrstuvwxyz -",10,70);
	G9PrintStringVram("- 0 1 2 3 4 5 6 7 8 9 ! # @ &-",10,90);

 	getchar();

	return 0;
}
