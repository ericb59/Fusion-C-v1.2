#include "fusion-c/header/msx_fusion.h"


void main (void)
{
   char *text="FUSION-C Ready !";
   char text_input[20];

   Cls();
   Print (text);
  
  Locate (3,5);
  Print ("What is your surname ?\a");
  InputString(text_input,20);

  Print ("Nice to meet you \t:");
  Print (text_input);

 
}