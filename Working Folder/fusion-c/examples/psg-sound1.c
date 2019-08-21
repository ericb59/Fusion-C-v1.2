/* =============================================================================
   SDCC AY-3-8910 PlayFX Lib Test
   Version: 1.0
   Date: 17/06/2018
   Author: mvac7/303bcn
   Architecture: MSX
   Format: C Object (SDCC .rel)
   Programming language: C
   WEB: 
   mail: mvac7303b@gmail.com

   Description:
      Test the functions of the library PSG_AY38910.

### 4.1 Envelope shapes

The header file defines envelope shapes in case you prefer to use it instead 
of the numerical form:
  
* ENV_LowerBeat      0 
* ENV_Upper          4 
* ENV_LeftSaw        8
* ENV_LowerTriangle 10 
* ENV_LowerHold     11 
* ENV_RightSaw      12
* ENV_UpperHold     13
* ENV_UpperTriangle 14


### 4.2 FX struct
  
- [boolean]      isTone    enable tone
- [boolean]      isNoise   enable noise 
- [unsigned int] Tone      Tone period      (0 - 4095)
- [char]         Noise     Noise period     (0 - 31)
- [unsigned int] Period    Envelope Period  (0 - 65535)
- [char]         Shape     Envelope shape   (0-15)
  
  
  
## 5 Functions


* SOUND(register, value) - Write into a register of PSG
* PlayFX(channel, soundat) - Play FX
* SetChannel(channel, isTone, isNoise) - Mixer. Enable/disable Tone and Noise channels.
* SilencePSG() - Silence the three PSG channels.


## 6 How to use
                           
This library provides functions to play simple effects (using the hardware),
designed for to be used in videogames.

In our program we can include all the effects we need using the FX structure,
which is defined in the library. This allows us to indicate a tone value, the
noise, the state of these channels, an envelope period and a type of wave 
envelope. 

To play an FX, we must use the PlayFX function. It allows us to select one of
the three available channels, but because it is based on sounds produced by the
wave envelope hardware and the AY-3-8910 can only play one at the same time, it
is recommended to always send them through the same chanel to avoid generating
unwanted sounds. If an FX is played without having finished the previous one,
this last one will have priority over the previous one since it will overwrite
the PSG registers.

If you want to take advantage of the other channels to include music using one
of the existing AY players (PT3, WYZ, Arkos, ...), these functions will probably
not work since the music players use a buffer that is modified every time it is
executed and it is dumped directly to PSG on every VBLANK interruption. In order
to get this working, this library should be modified so that instead of writing
directly to the registers, it does it over the player's buffer.
============================================================================= */

#include "fusion-c/header/vars_msxSystem.h"
#include "fusion-c/header/vars_msxBios.h"
#include "fusion-c/header/vars_msxDos.h"

#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/psg.h"



#define  HALT __asm halt __endasm   //wait for the next interrupt

#define PAUSE_TIME 30

#define AY0index 0xA0
#define AY0write 0xA1
#define AY0read  0xA2

#define  SYSTEM 0x0005  // MSX-DOS entry



// Function Declarations -------------------------------------------------------
void System(char code);

void testSOUND();
void testFX(); // test SOUND

void Play(char fx);
void Silent();


void WAIT(int cicles);

boolean NOT(boolean value);



// constants  ------------------------------------------------------------------
const char text01[] = "Test SDCC AY38910 PSG Lib";
const char text02[] = "v1.0 (17/06/2018)";


// global variable definition --------------------------------------------------

FX   fx01;
FX   fx02;
FX   fx03;


// Functions -------------------------------------------------------------------


//
void main(void)
{
  char colorInk=0;
  char colorBG=0;
  char colorBDR=0;
  char scrcolumns=0;
  //char MSXsion=0;
  
  colorInk=Peek(FORCLR);
  colorBG=Peek(BAKCLR);
  colorBDR=Peek(BDRCLR);
  scrcolumns=Peek(LINLEN);
  
  
  
  SetColors(WHITE,LIGHT_BLUE,DARK_BLUE);      
  Screen(1);
  Width(32);
  
  
  fx01.isTone=true;
  fx01.isNoise=false;
  fx01.Tone=1024;
  fx01.Noise=0;
  fx01.Period=3200;
  fx01.Shape=8;  
  
  fx02.isTone=false;
  fx02.isNoise=true;
  fx02.Tone=0;
  fx02.Noise=24;
  fx02.Period=10000;
  fx02.Shape=1;
  
  fx03.isTone=true;
  fx03.isNoise=true;
  fx03.Tone=1024;
  fx03.Noise=0;
  fx03.Period=555;
  fx03.Shape=12;
  
  
  testSOUND();  
  testFX();

//EXIT MSXDOS ------------------------------------------------------------------
  ClearKeyBuffer(); 
  //restore MSXDOS screen
  SetColors(colorInk,colorBG,colorBDR);
  Width(scrcolumns);

  if(scrcolumns<33) Screen(1);
  else Screen(0);
  //
  
  Print("END");
    
  System(_TERM0); 
//--------------------------------------------------------------------- end EXIT

}



// call system functions 
// see MSX Assembly Page > MSX-DOS 2 function calls
// http://map.grauw.nl/resources/dos2_functioncalls.php
void System(char code)
{
code;
__asm
    push IX
    ld   IX,#0
    add  IX,SP

    ld   C,4(IX)
    call SYSTEM

    pop  IX
__endasm; 
}



// Generates a pause in the execution of n interruptions.
// PAL: 50=1second. ; NTSC: 60=1second. 
void WAIT(int cicles)
{
  int i;
  for(i=0;i<cicles;i++) HALT;
  return;
}



boolean NOT(boolean value)
{
  if (value) return false;
  return true;
}



void testSOUND()
{
  Locate(0,0);
  Print(text01);
  Locate(0,1);
  Print(text02); 
   
  Locate(0,4);
  Print(">Test SOUND function");
  Locate(0,6);
  Print(" Press a key to stop sound");

  Sound(0,300);//tone period channel A (2B)
  Sound(1,0);
  
  Sound(2,900);//tone period channel B (2B)
  Sound(3,0);
  
  Sound(4,200);//tone period channel C (2B)
  Sound(5,10);
   
  Sound(6,100); //noise
  Sound(7,0b11110000); //Mixer > active tone in three channels and noise only in A
  
  Sound(8,16);  //channel A envelope on
  Sound(9,16);  //channel B envelope on
  Sound(10,16); //channel C envelope on
  
  Sound(11,100);//envelope period (2B)
  Sound(12,10); // 
  Sound(13,14); //envelope wave type
  
  WaitKey();
  
  SilencePSG();
  
  WAIT(PAUSE_TIME);    
}



// Test PlayEnvelope and SetChannel
void testFX()
{
  char pressKey;
  boolean isContinue = true;
  boolean keyB6semaphore = false;
  boolean keyB7semaphore = false;

  Cls();
  
  Locate(0,0);
  Print(text01);
  Locate(0,1);
  Print(text02);
  
  Locate(0,4);
  Print("Press F1,F2 or F3 to play FX");
  Print("\nPress STOP for silent");
  Print("\nPress ESC for exit");
 
  
  while(isContinue)  //bucle infinito
  {
    HALT;    
    
    pressKey = GetKeyMatrix(6);      
    if (pressKey!=255)
    {
      if (!keyB6semaphore)
      {
        keyB6semaphore = true;
        if (pressKey==0b11011111) Play(0);//F1
        if (pressKey==0b10111111) Play(1);//F2
        if (pressKey==0b01111111) Play(2);//F3
      }      
    }else keyB6semaphore=false;
    
    
    pressKey = GetKeyMatrix(7);      
    if (pressKey!=255)
    {
      if (!keyB7semaphore)
      {
        keyB7semaphore = true;
        if (pressKey==0b11101111) Silent();//STOP
        if (pressKey==0b11111011) isContinue = false;//ESC

      }      
    }else keyB7semaphore=false;

  } //END while
  
  SilencePSG();

}



void Silent()
{
  Locate(10,8);
  Print("      ");
  SilencePSG();
}



void Play(char fx)
{
  Locate(10,8);
  switch (fx) 
  {
    case 0:
      Print("FX01  ");
      SoundFX(0,&fx01);
      break;
      
    case 1:
      Print("FX02  ");
      SoundFX(0,&fx02);
      break;
      
    case 2:
      Print("FX03  ");
      SoundFX(0,&fx03);
      break;
  }

}
