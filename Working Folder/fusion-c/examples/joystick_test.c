#include <stdlib.h>
#include <stdio.h>
#include "fusion-c/header/msx_fusion.h"

#define  HALT __asm halt __endasm   //wait for the next interrupt


// Generates a pause in the execution of n interruptions.
// PAL: 50=1second. ; NTSC: 60=1second. 
void Wait(int cicles)
{
  int i;
  for(i=0;i<cicles;i++) HALT;
  return;
}

void ft_UP(void)
{
    Locate (20,5);
    Print("O");
    Locate (20,6);
    Print("|");
}
void ft_DOWN(void)
{
    Locate (20,8);
    Print("|");
    Locate (20,9);
    Print("O");
}
void ft_RIGHT(void)
{
    Locate (22,7);
    Print("-");
    Locate (23,7);
    Print("O");
}
void ft_LEFT(void)
{
    Locate (18,7);
    Print("-");
    Locate (17,7);
    Print("O");
}

void ft_direction (char a)
{
       switch (a) 
        {
            case 0:
                Cls();
                break;
            case 1:
                ft_UP();
                break;
            case 2:
                ft_UP();
                ft_RIGHT();
                break;
            case 3:
                ft_RIGHT();
                break;
            case 4:
                ft_RIGHT();
                ft_DOWN();
            case 5:
                ft_DOWN();
                break;
            case 6:
                ft_DOWN();
                ft_LEFT();
                break;
            case 7:
                ft_LEFT();
                break;
            case 8:
                ft_LEFT();
                ft_UP();
                break;
            default:
                Cls();
        
        }
}

void main(void)
{ 

    
    int a;
    int b;
    int j=0; /// Joystick to Test    

     Cls();
    

    while(Inkey()!=27)
    {

        a = JoystickRead(j);
        b = TriggerRead(j);

        ft_direction(a);

        if (b!=0)
        {
         Locate (1,1);
            printf("\n\r -> FIRE");
        }

    }
    



}
