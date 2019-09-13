/* Fusion-C G9Klib #TILES & SPRITES# test
 *
 * V9990
 * SCREEN P1		  : Pattern Mode 1
 * Display resolution : 32x26.5 patterns (256x212 pixels)
 * Image space		: 64x64 patterns
 * Number of layers   : 2 layers (A and B) with priority control
 * Pattern size	   : 8x8 pixels
 * Simultaneously displayed colors : 30 colors + transparant
 * Color palette	  : 4 palettes of 16 colors out of 32768 colors
 * Number of patterns : Layer "A": 8160 units / Layer "B": 7680 units
 *
 * launch openMSX with "-ext gfx9000" option
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h> // for uint8_t
#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/g9klib.h"

#define XMAX 256
#define YMAX 256
#define SPRLN 0x1800
#define TL1LN 0x1000
#define TL2LN 0x0800
#define MAXBF 0x2000
#define PLTLN 192

static FCB file;

static SPRITE_ATTR spr1;

const unsigned char space_tile[1]={47};

const unsigned char first_line[2]={37,38};

const unsigned char zero_line[1]={43};

/* 0 1 2 3 4 5 6 7 8 9 */
const unsigned char numbers1[10]={1,2,3,4,5,6,7,8,9,10};

/* letters */
const unsigned char letters1[26]={11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36};

/* others */
const unsigned char others1[32]={37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68};
const unsigned char others2[32]={69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100};
const unsigned char others3[32]={101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132};

/* for layer B */
const unsigned char bkgrg[3][3]={{16,17,18},{19,20,21},{22,23,24}};

unsigned char buffer[MAXBF];

unsigned int k,posy;

int LoadBufferFromFile(char * fileName, char * fileExt, int readLen) {
	int retc,retd;
	file.cluster_offset=0;
	file.current_record=0;
	file.random_record=0;

	memcpy(file.name,fileName,8);
	memcpy(file.ext,fileExt,3);
	if((retc=fcb_open( &file )) != FCB_SUCCESS) { printf("Open file error %x\n\r",retc); return 1; }
	if((retd=fcb_read( &file, buffer, readLen )) != readLen) { printf("Read file error %x\n\r",retd); return 1; }
	if((retc=fcb_close( &file )) != FCB_SUCCESS) { printf("Close file error %x\n\r",retc); return 1; }

	return 0;
}

int main(void)
{
	/* declarations */
	int i,j;

	int x,y;
	unsigned char c;

	/* code */

//	getchar();

	if (G9Detect()) {
		printf("GFX9000 detect failed\n\r");
		return 1;
	}

	G9Reset();

/* Init. palettes */

	// set default disk drive
	file.drive_no = 0;

	// Load 4 palettes in V9990
	LoadBufferFromFile("palettes","pal",PLTLN);
	G9InitPalette(buffer,PLTLN);

	printf("palette loaded\n\r");

//	G9DisplayDisable();

	/* SCREEN P1		  : Pattern Mode 1
	 * Display resolution : 32x26.5 patterns (256x212 pixels)
	 * Image space		: 64x64 patterns
	 * Number of layers   : 2 layers with priority control
	 * Pattern size	   : 8x8 pixels
	 * Simultaneously displayed colors : 30 colors + transparant
	 * Color palette	  : 4 palettes of 16 colors out of 32768 colors
	 * Number of patterns : Layer "A": 8160 units / Layer "B": 7680 units
	 */
	G9SetScreenMode(G9MODE_P1,G9SCR0_2BIT,G9SCR0_XIM256,0,4);

	G9InitSpritePattern(4);

	G9InitPatternMode();

	G9DisplayDisable();

/* Init. sprites */
	// load sprites to VRAM
	LoadBufferFromFile("sprites ","sc5",SPRLN);
	G9SetVramWrite(1,0); // sprite pattern starts at $10000
	G9CopyRamToVram(buffer, SPRLN);

	printf("sprites loaded\n\r");

/* Init. tiles A */
	// load tiles layer A to VRAM
	LoadBufferFromFile("tiles   ","sc5",TL1LN);
	G9SetVramWrite(0,0); // tiles layer A at $00000
	G9CopyRamToVram(buffer, TL1LN);

	printf("tilesA loaded\n\r");

/* Init. tiles B */
	// load tiles layer B to VRAM
	LoadBufferFromFile("tilesB  ","sc5",TL2LN);
	G9SetVramWrite(4,0); // tiles layer B at $40000
	G9CopyRamToVram(buffer, TL2LN);

	printf("tilesB loaded\n\r");

/*
PrintTiles :
	2 layers, A and B, are superimposed
	layer A is over layer B
parameters :
	tiles array, beginnig position of tiles printing, number of tiles to print
*/

/* clear screen by space tiles on layers A and B */
	for (i=0; i<4096; i++) {
		G9PrintTilesA(space_tile,i,1);
		G9PrintTilesB(space_tile,i,1);
	}

	G9DisplayEnable();

	printf("Print clear screen layers A and B\n\r");
//	getchar();

/*
	posy=0;
	G9PrintTilesA(numbers1,posy,sizeof(numbers1));

	printf("Layer A : Print numbers\n\r");
	getchar();

	posy+=64;
	G9PrintTilesA(letters1,posy,sizeof(letters1));

	printf("Layer A : Print letters1\n\r");
	getchar();

	posy+=64;
	G9PrintTilesA(others1,posy,sizeof(others1));

	printf("Layer A : Print others1\n\r");
	getchar();

	posy+=64;
	G9PrintTilesA(others2,posy,sizeof(others2));

	printf("Layer A : Print others2\n\r");
	getchar();

	posy+=64;
	G9PrintTilesA(others3,posy,sizeof(others3));

	printf("Layer A : Print others3\n\r");
	getchar();



	posy+=64;
	G9PrintTilesB(numbers1,posy,sizeof(numbers1));

	printf("Layer B : Print numbers\n\r");
	getchar();

	posy+=64;
	G9PrintTilesB(letters1,posy,sizeof(letters1));

	printf("Layer B : Print letters1\n\r");
	getchar();

	posy+=64;
	G9PrintTilesB(others1,posy,sizeof(others1));

	printf("Layer B : Print others1\n\r");
	getchar();

	posy+=64;
	G9PrintTilesB(others2,posy,sizeof(others2));

	printf("Layer B : Print others2\n\r");
	getchar();

	posy+=64;
	G9PrintTilesB(others3,posy,sizeof(others3));

	printf("Layer B : Print others3\n\r");
	getchar();
*/
	/* location, pattern nbr, layer 0=A, 1=B */
//	G9SetPattern(1,1,0);
	c=0;

	/* draw background on layer B */
	for (j=0;j<32;j++) {
		posy=j*64;
		for (i=0;i<11;i++) {
			G9PrintTilesB(bkgrg[c],posy,3);posy+=3;
		}
		if (c<2) {c++;} else {c=0;}
	}

	/* display sprites */
	for (i=0;i<32;i++) {
		spr1.x=(char)((i-16)*8);
		spr1.y=i;
		spr1.attr=0;
		spr1.pat=(char)i;
		G9SetVramWrite(3,0xFE00+(i*4)); // sprite attribute table : $3fe00
		G9CopyRamToVram(&spr1,4);
	}

	G9SetScrollMode(0);
	x=0;y=0;
	posy=0;
	do {
		switch(c=getchar()) {
			case 28:
//				if (posy<864) { posy++; }
				if (x<511) G9SetScrollX(++x);
			break;
			case 29:
				if (x>0) G9SetScrollX(--x);
//				if (posy>0) { posy--; }
			break;
			case 30:
				if (y>0) G9SetScrollY(--y);
//				if (posy>64) { posy-=64; }
			break;
			case 31:
				if (y<511) G9SetScrollY(++y);
//				if (posy<832) { posy+=64; }
			break;
			case '+':
				if (spr1.x<255) {
					spr1.x++;
					G9SetVramWrite(3,0xFE00);
					G9CopyRamToVram(&spr1,4);
				}
			break;
			case '-':
				if (spr1.x>0) {
					spr1.x--;
					G9SetVramWrite(3,0xFE00);
					G9CopyRamToVram(&spr1,4);
				}
			break;
			default:
			break;
		}
//		if (!(posy%32)) posy+=32;
//		if (!(posy%63)) posy-=32;
//		printf("p32= %d c=%d k=%d posy=%d ", posy%32, c, k, posy);
		if (c==32) { k=47; G9SetPattern(posy++,k,0); }
		if (c>=97 && c<=122) { k=c-86; G9SetPattern(posy++,k,0); }
		if (c>=48 && c<=57) { k=c-47; G9SetPattern(posy++,k,0); }
		if (posy==4096) posy=0;
	} while (c!='!');

	return 0;
}
