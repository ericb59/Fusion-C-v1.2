//
// Fusion-C
// example : arguments
// 
//

// 
// Call the program with command line arguments
// >argument test1 test2 test3
#include <stdio.h>

int main(char *argv[], int argc)
{
  char i;

  if (argc>1)
  {
    for ( i = 0; i < argc; ++i)
    {
      printf("Argument Numb %d passed : %s \n\r",i,argv[i]);
    }

  }

  return(0);
}