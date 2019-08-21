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
|                 C     S O U R C E   C O D E               |
|         compilation : > sdcc -mz80 -c  msx_misc.c         |
|                                                           |
\___________________________________________________________/
*/
/*  ispositiv
|    
|     Eric Boez 2018
*/


// Return 1 if positive
// Return -1 if negative
// return 0 if equal to 0

char IsPositive(int c)         // Check if number is positive
{
  int n=0;

  if (c>0)
  {
    n=1;
  }
  else
  {
    n=-1;
  }
  if (c==0)
  {
    n=0;
  }
  return (n);
}






