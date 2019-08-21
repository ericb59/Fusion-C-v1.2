/**************************************************************************/
/*                                                                        */
/* file:         dsk2rom.c                                                */
/* version:      0.80 (June 18, 2007)                                     */
/*                                                                        */
/* description:  This tool converts .dsk images to .rom images.           */
/*                                                                        */
/*                                                                        */
/*  This program is free software; you can redistribute it and/or modify  */
/*  it under the terms of the GNU General Public License as published by  */
/*  the Free Software Foundation; either version 2, or (at your option)   */
/*  any later version. See COPYING for more details.                      */
/*                                                                        */
/*                                                                        */
/* Copyright 2007 Vincent van Dam (vincentd@erg.verweg.com)               */
/*                                                                        */
/**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "dsk2rom.h"
#include "pletter/pletter.h"

#define plet_mode 9
#define sector_size 512

typedef struct {
  int address;
  int size;
  unsigned char* data;
} SECTOR_DATA;

/****************************/
/* show a brief description */
/****************************/

void showUsage(char *progname)
{
  printf("usage: %s [-c 0..2] [-a56pdsfv] <ifile> <ofile>\n"
	 " -c   compression level (0-2, where 0 is uncompressed)\n"
	 " -a   use ascii8 mapper instead of konami scc\n"
	 " -5   switch to pal (50Hz) when booting the ROM on MSX2 and up\n"
	 " -6   switch to ntsc (60Hz) when booting the ROM on MSX2 and up\n"
	 " -p   change to MSX1 palette when booting the ROM on MSX2 and up\n"
	 " -d   disable exclusive diskrom mode (other diskroms will boot too)\n"
	 " -s   safe mode (protect against illegal bank switching)\n"
	 " -f   fill up rom to standard rom size\n"
	 " -v   give verbose information\n", progname);
  return;
}  

/**************/
/* go for it! */
/**************/

int main(int argc, char* argv[])
{
  char *ifile = NULL;
  char *ofile = NULL;
  int  compress_mode = 0;
  int  ascii_mapper = 0;
  int  fillup_rom = 0;
  int  verbose = 0;
  int  pal_mode = 0;
  int  ntsc_mode = 0;
  int  msx1_palette = 0;
  int  exclusive_mode = 1;
  int  safe_mode = 0;

  int  safe_patch_size = 0;
  int  safe_patch_offset = 0;
  int  driver_workarea_size = 0;
  int  bankswitch_size = 0;
  int  bank_size = 0;
  int  bank_offset = 0;

  SECTOR_DATA **sector_data;
  int* sector_hash;

  unsigned char read_sector[sector_size];
  unsigned char *plet_sector;

  FILE *output,*input;
  int  i,j;

  int plet_size;
  int sector;
  int total_sectors;
  int total_size;
  int address;

  /******************************/
  /* parse command line options */
  /******************************/

  for (i=1; i<argc; i++) {

    if (argv[i][0]=='-') {

      for(j=1;j && argv[i][j]!='\0';j++)
	
        switch(argv[i][j]) {
	  
	case 'c': compress_mode=atoi(argv[++i]); j=-1; break;
	case 'f': fillup_rom=1; break;
	case '5': pal_mode=1; break;
	case '6': ntsc_mode=1; break;
	case 'p': msx1_palette=1; break;
	case 'a': ascii_mapper=1; break;
	case 'd': exclusive_mode=0; break;
	case 's': safe_mode=1; break;
	case 'v': verbose=1; break;

        default:
          fprintf(stderr,"%s: invalid option\n",argv[0]);
          exit(1);
        }

      continue;
    }

    if (ifile==NULL) { ifile=argv[i]; continue; }
    if (ofile==NULL) { ofile=argv[i]; continue; }
    
    fprintf(stderr,"%s: invalid option\n",argv[0]);
    exit(1);
  }

  if (ifile==NULL || ofile==NULL) { showUsage(argv[0]); exit(1); }

  if (safe_mode && !compress_mode) compress_mode=1;

  /***************/
  /* preperation */
  /***************/

  /* open input file */
  if ((input=fopen(ifile,"rb"))==NULL) {
    fprintf(stderr,"%s: failed opening %s\n",argv[0],ifile);
    exit(1);
  }

  /* determine number of sectors within image and initialize buffers*/
  fseek(input,0,SEEK_END); 
  total_sectors = ftell(input)/sector_size;
  fseek(input,0,SEEK_SET);

  sector_data = (SECTOR_DATA**)malloc(total_sectors*sizeof(void**));
  sector_hash = (int*)malloc(total_sectors*sizeof(int*));

  /******************************/
  /* read dsk image and process */
  /******************************/

  total_size = 0;
  for(sector=0;sector<total_sectors;sector++) {

    if (fread(read_sector,1,sector_size,input)<sector_size) {
      fprintf(stderr,"%s: unexpected end of file %s\n",argv[0],ifile);
    }

    /***********************/
    /* step 1: compression */
    /***********************/

    if (verbose) printf("sector: %d",sector);

    if (compress_mode==2) {
      if (verbose) printf(" (pletter)");
      pletter(read_sector,sector_size,&plet_sector,&plet_size,plet_mode);
    } else {
      plet_size = sector_size;
      plet_sector = (unsigned char*)malloc(plet_size);
      memcpy(plet_sector,read_sector,plet_size);
    }

    if (verbose) printf(" - size %d (%.2f%%)",plet_size,
			           100-(float)plet_size/sector_size*100.0);

    total_size += plet_size;

    /**********************/
    /* step 2: add sector */
    /**********************/

    sector_data[sector] = (SECTOR_DATA*)malloc(sizeof(SECTOR_DATA));
    sector_data[sector]->data = plet_sector;
    sector_data[sector]->size = plet_size;

    /**********************************/
    /* step 3: create index (hashing) */
    /**********************************/

    sector_hash[sector] = sector;
    if (compress_mode>=1)
      for(i=0;i<sector;i++)
	if(!memcmp(sector_data[i]->data,sector_data[sector]->data,plet_size)) {
	  if (verbose) printf(" (hashed as %d)",i);
	  sector_hash[sector] = i;
	  total_size -= plet_size;
	  break;
	}

    if (verbose) printf("\n");
  }
  fclose(input);

  if (verbose) printf("compressed dsk size = %d\n",total_size);

  /*******************/
  /* write rom image */
  /*******************/

  /* create output file */
  if ((output=fopen(ofile,"wb"))==NULL) {
    fprintf(stderr,"%s: failed creating %s\n",argv[0],ofile);
    exit(1);
  }

  /* mapper patch of kernel (ascii8 vs scc mapper) */
  if (ascii_mapper) {
    /* patch page 1 mapper switches */
    i = 0x3fb0;
    do{ address = dsk2rom[i]+dsk2rom[i+1]*256; i=i+2;
      if (address) {
	dsk2rom[address-0x4000+1]=0x60; /* 6000 page 1 vs 5000 page 1 */
	if (verbose) printf("patching kernel %.4x for ascii mapper\n",address);
      }
    } while (address);
    /* patch page 2 mapper switches */
    do{ address = dsk2rom[i]+dsk2rom[i+1]*256; i=i+2;
      if (address) {
	dsk2rom[address-0x4000+1]=0x68; /* 6800 page 2 vs 7000 page 2 */
	if (verbose) printf("patching kernel %.4x for ascii mapper\n",address);
      }
    } while (address);
  }
  
  /* calculate locations and sizes of dynamic parts */
  safe_patch_offset = dsk2rom[0x3fa0]+dsk2rom[0x3fa1]*256 - 0x4000;
  safe_patch_size   = safe_mode? dsk2rom[0x3fa2]+dsk2rom[0x3fa3]*256 : 0;
  bankswitch_size   = dsk2rom[0x3fa4]+dsk2rom[0x3fa5]*256;
  driver_workarea_size = compress_mode>1? sector_size : 0;
  driver_workarea_size+= safe_mode? bankswitch_size : 0;
  bank_size   = 0x2000-safe_patch_size;
  bank_offset = 0x4000+safe_patch_size;

  /* configure dsk2rom kernel */
  dsk2rom[0x3f00] = compress_mode;
  dsk2rom[0x3f01] = driver_workarea_size%256;
  dsk2rom[0x3f02] = driver_workarea_size/256;
  dsk2rom[0x3f03] = pal_mode? 1 : ntsc_mode? 2 : 0;
  dsk2rom[0x3f04] = msx1_palette;
  dsk2rom[0x3f05] = exclusive_mode;
  dsk2rom[0x3f06] = safe_mode? sector_size%256 : 0;
  dsk2rom[0x3f07] = safe_mode? sector_size/256 : 0;/*offset bankswitch patch */
  dsk2rom[0x3f08] = safe_patch_size%256;
  dsk2rom[0x3f09] = safe_patch_size/256+0x40;
  dsk2rom[0x3f0a] = total_sectors%256;
  dsk2rom[0x3f0b] = total_sectors/256;

  /* write kernel */
  for(i=0;i<0x4000;i++) fputc(dsk2rom[i],output);

  /* write sector index */
  address = total_sectors*5; /* index requires 5 bytes per sector */
  if (compress_mode>0) {

    /* safe mode; make sure diskrom api is available on every page */
    if (safe_mode)
      for (i=0;i<safe_patch_size;i++)
	fputc(dsk2rom[safe_patch_offset+i],output);

    /* create index */
    for(sector=0;sector<total_sectors;sector++) {
      if(sector_hash[sector]!=sector)
	sector_data[sector]->address=sector_data[sector_hash[sector]]->address;
      else {
	sector_data[sector]->address=address;
	address+=sector_data[sector]->size;
      }
      /* write index page(8),start(16),size(16), 1440 sectors = 1c20 */
      fputc(sector_data[sector]->address/bank_size+2,output); /* page */
      fputc((sector_data[sector]->address%bank_size+bank_offset)%256,output);
      fputc((sector_data[sector]->address%bank_size+bank_offset)/256,output);
      fputc((sector_data[sector]->size)%256,output);
      fputc((sector_data[sector]->size)/256,output);
    }
  }
  
  /* write sector data */
  for(sector=0;sector<total_sectors;sector++)
    if (sector_hash[sector]==sector)
      if (safe_mode)
	for(i=0;i<sector_data[sector]->size;i++) {
	  if (!(ftell(output)%0x2000))
	    for (j=0;j<safe_patch_size;j++)
	      fputc(dsk2rom[safe_patch_offset+j],output);
	  fputc(sector_data[sector]->data[i],output);
	}
      else
	fwrite(sector_data[sector]->data,1,sector_data[sector]->size,output);
  
  /* fill up */
  if (safe_mode) {
      total_size = ftell(output);
      if (total_size<=512*1024)
	j=512*1024;
      else 
	j=1024*1024;
      while(ftell(output)<j)
	if (!(ftell(output)%0x2000))
	  for (i=0;i<safe_patch_size;i++)
	    fputc(dsk2rom[safe_patch_offset+i],output);
	else 
	  fputc(0,output);    
  } else {
    if (fillup_rom) {
      total_size = ftell(output);
      j = 1; while(j<total_size) j=j*2;
      for(i=total_size;i<j;i++) fputc(0,output);
    }
  }
  
  /* done; close and cleanup */
  fclose(output);
  
  for(sector=0;sector<total_sectors;sector++) {
    free(sector_data[sector]->data);
    free(sector_data[sector]);
  }
  free(sector_data);
  free(sector_hash);

  return 0;
}

