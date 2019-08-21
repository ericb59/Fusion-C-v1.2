/*
 ___________________________________________________________
/               __           _                              \
|              / _|         (_)                             |
|             | |_ _   _ ___ _  ___  _ __                   |
|             |  _| | | / __| |/ _ \| '_ \                  |
|             | | | |_| \__ \ | (_) | | | |                 |
|             |_|  \__,_|___/_|\___/|_| |_| *               |
|                                                           |
|               The MSX C Library for SDCC                  |
|                   V1.0 - 09-10-11 2018                    |
|                                                           |
|                Eric Boez &  Fernando Garcia               |
|                                                           |
|          C   &   A S M    S O U R C E   C O D E           |
|                                                           |
|                                                           |
\___________________________________________________________/
*/
// SDCC version of malloc, memory right below the code (heap_top=length of program+few bytes) should be free of data or code loaded after at runtime

extern unsigned char    *heap_top; 

void   *MMalloc(unsigned int size)                 
{
  unsigned char *ret = heap_top;
  heap_top += size;
  return (void *) ret;
} 