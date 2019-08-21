/**************************************************************************/
/*                                                                        */
/* file:         bin2h.c                                                  */
/* version:      1.0 (May 26, 2007)                                       */
/*                                                                        */
/* description:  This tool converts binary files into c header files.     */
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

int main(int argc, char* argv[])
{
  FILE *output,*input;
  int count=0;
  unsigned char data;
  
  if (argc != 4) {
    printf("usage: %s <name> <ifile> <ofile>\n",argv[0]);
    exit(0);
  }
  
  if ( (input = fopen(argv[2],"rb")) == NULL) {
    fprintf(stderr,"%s: failed opening %s\n",argv[0],argv[2]);
    exit(1);
  }
  
  if ( (output = fopen(argv[3],"wb")) == NULL) {
    fprintf(stderr,"%s: failed creating %s\n",argv[0],argv[3]);
    exit(1);
  }
  
  fprintf(output,"unsigned char %s[] = {\n", argv[1]);
  while(fread(&data,1,1,input)) {
    if(count >= 16) {
      fputc('\n',output);
      count = 0;
    }
    count++;
    fprintf(output,"0x%02x,",data);
  }
  
  fprintf(output,"};\n");
  
  fclose(output);
  fclose(input);
  
  return 0;
}

