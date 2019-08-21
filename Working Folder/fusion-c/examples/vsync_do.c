// Fusion-c Example
// WaitVblank and DO Action
//

#include <stdio.h>
#include "fusion-c/header/msx_fusion.h"
int cpt_test1;
int cpt_test2;
int cpt_test3;
// Affihage des 3 compteurs à l'écran
//
void FT_Print(void)
{
  Locate(7,4);
  printf("%d",cpt_test1);
  Locate(7,6);
  printf("%d",cpt_test2);
  Locate(7,8);
  printf("%d",cpt_test3);
  return;
}
// Routine de tests 
//
void FT_DoSomething(int compteur)
{
  int trigger;
  trigger=1;
   
     cpt_test1++;            // Do that Every Frame
       
    
    trigger=compteur & 1;
    if (trigger == 0)       // Do That Every 2 frames
    {
      cpt_test2++;
    }
    

    trigger=compteur & 3;
    if (trigger == 0)       // Do That Every 4 frames
    {
      cpt_test3++;
    }
    
    FT_Print(); 

    return;
}
void main (void)
{
  int cpt;
  cpt=0;
  cpt_test1=0;
  cpt_test2=0;
  cpt_test3=0;
  // On prépare l'écran
  Screen(0);
  Locate(1,4);
  printf("CPT1:");
  Locate(1,6);
  printf("CPT2:");
  Locate(1,8);
  printf("CPT3:");


  // Loop
  while (1)         
  {
    if (IsVsync())              // Test Vsync / Vlank
    {
 
      cpt++;
      if (cpt>=100)
      {
        cpt=0;
      }
      FT_DoSomething(cpt);
    }
  }
  Screen(0);
  Exit(0);
}