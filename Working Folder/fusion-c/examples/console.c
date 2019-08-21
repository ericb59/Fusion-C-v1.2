#include <stdio.h>
#include "fusion-c/header/msx_fusion.h"


void main(void)
{
  char surname[20];
  char age[2];

  DATE mydate;

  Screen(0);
  Width(40);
  Cls();

  GetDate (&mydate);
  Locate (0,0);
  Print("Current Date : ");
  PrintDec(mydate.year);
  PrintChar('-');
  PrintDec(mydate.month);
  PrintChar('-');
  PrintDec(mydate.day);

  Print("\n\rHello ...");
  Locate (3,5);
  Print("What is you surname ? :");
  InputString(surname,19);


  Cls();
  Beep();
  Print("Nice to meet you ");
  Print(surname);

  Locate (3,5); 
  Print("How old are you ?:");
  InputString(age,5);
  Locate(3,6);
  PrintDec(95);
  Print("...");
  PrintChar('?');
  Print("\n\r You 're so Old !!");


  Exit(0);

}