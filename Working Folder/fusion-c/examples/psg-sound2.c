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


* Sound(register, value) - Write into a register of PSG
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



// Function Declarations -------------------------------------------------------
void test1(); // test SOUND
void test2(); // test SetTonePeriod
void test3(); // test SetNoisePeriod
void test4(); // test SetVolume
void test5(); // test SetEnvelopePeriod
void test6(); // test PlayEnvelope and SetChannel

void setEnv(char envType);

void  PrintChannel(char chan, boolean stateTone, boolean stateNoise);

void WAIT(int cicles);

boolean NOT(boolean value);



// constants  ------------------------------------------------------------------
const char text01[] = "Test SDCC PSG Lib";
const char text02[] = "v1.2 (18/06/2018)";


// global variable definition --------------------------------------------------
char _lastEnv;


// Functions -------------------------------------------------------------------


//
void main(void)
{
  SetColors(BLACK,GRAY,DARK_BLUE);      
  Screen(1);
  Width(32);
    
  test1(); // test SOUND
  test2(); // test SetTonePeriod
  test3(); // test SetNoisePeriod
  test4(); // test SetVolume
  test5(); // test SetEnvelopePeriod
  test6(); // test PlayEnvelope and SetChannel
 

//EXIT MSXDOS
/*  screen(0);
    
__asm
    ld b,4(ix)
    ld c,#0x62
    call 5 
__endasm;*/
//end EXIT

  return;
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



// test SOUND
void test1()
{  
  Cls();

  Locate(0,0);
  Print(text01);
  Locate(0,1);
  Print(text02);
  
  Locate(0,4);
  Print(">Test SOUND function");
  Locate(0,6);
  Print(" Press a key to stop sound");

  Sound(0,100);//tone period channel A (2B)
  Sound(1,10); 
  Sound(6,100); //noise
  Sound(7,0B00110110); //Mixer
  Sound(8,16); //channel A envelope on
  Sound(11,100);//envelope period (2B)
  Sound(12,10); // 
  Sound(13,14); //envelope wave type
  
  WaitKey();
  
  Sound(8,0); //silencia
  Sound(13,0);
  
  WAIT(PAUSE_TIME);
}



// test SetTonePeriod
void test2()
{
  int period=0;
  
  Cls();
  
  Locate(0,0);
  Print(text01);
  Locate(0,1);
  Print(text02);
  
  Locate(0,4);
  Print(">Test SetTonePeriod function");
  Locate(0,6);
  Print(" Press space to end");
  
  Locate(0,8);
  Print(" Tone Period A:");
  
  Sound(7,0B00111110); //Mixer
  Sound(8,12);
  while(1)
  {
    HALT;
    
    if (GetKeyMatrix(8)!=255) break;
    
    Locate(16,8);
    PrintNumber(period);
    
    SetTonePeriod(0, period++);
    if (period>4094) period=0;
  
  }  
  Sound(8,10);
  WAIT(PAUSE_TIME);
  
  Locate(0,9);
  Print(" Tone Period B:");
  period = 0;
  Sound(7,0B00111100); //Mixer
  Sound(9,12);
  while(1)
  {
    HALT;
    
    if (GetKeyMatrix(8)!=255) break;
    
    Locate(16,9);
    PrintNumber(period);
    
    SetTonePeriod(1, period++);
    if (period>4094) period=0;
  
  }  
  Sound(9,10);
  WAIT(PAUSE_TIME);
  
  Locate(0,10);
  Print(" Tone Period C:");
  period = 0;
  Sound(7,0B00111000); //Mixer
  Sound(10,12);
  while(1)
  {
    HALT;
    
    if (GetKeyMatrix(8)!=255) break;
    
    Locate(16,10);
    PrintNumber(period);
    
    SetTonePeriod(2, period++);
    if (period>4094) period=0;
  
  }
  
  WAIT(PAUSE_TIME);
    
  Sound(8,0);
  Sound(9,0);
  Sound(10,0);    

}



// Test SetNoisePeriod
void test3()
{
  char noise=0;  
  
  Cls();
  
  Locate(0,0);
  Print(text01);
  Locate(0,1);
  Print(text02);
  
  Locate(0,4);
  Print(">Test SetNoisePeriod function");
  Locate(0,6);
  Print(" Press space to end");
  Locate(0,8);
  Print(" Noise Period:");
  Sound(7,0B00110111); //Mixer
  Sound(8,11);
  while(1)
  {
    if (GetKeyMatrix(8)!=255) break;
    
    Locate(14,8);
    PrintNumber(noise);
    
    SetNoisePeriod(noise++);
    if (noise>31) noise=0;
    
    WAIT(4);  
  }
  
  WAIT(PAUSE_TIME);
  
  Sound(8,0);
  Sound(9,0);
  Sound(10,0);  
  
}



// Test SetVolume
void test4()
{
  char amp=0;
  
  Cls();
  
  Locate(0,0);
  Print(text01);
  Locate(0,1);
  Print(text02);
     
  Locate(0,4);
  Print(">Test SetVolume function");
  Locate(0,6);
  Print(" Press space to end");
  
  Locate(0,8);
  Print(" Volume A:");
  Sound(0,100);
  Sound(1,10); //tone period
  Sound(7,0B11110110); //Mixer > Test write to 7 AY register
  while(1)
  {    
    if (GetKeyMatrix(8)!=255) break;

    Locate(10,8);
    PrintNumber(amp);
    
    SetVolume(0,amp++);
    if (amp>15) amp=0;
  
    WAIT(5);
  }
  
  WAIT(PAUSE_TIME);
  
  Locate(0,9);
  Print(" Volume B:");
  amp=0;
  Sound(2,300);
  Sound(3,10); //tone period
  Sound(7,0B00101101); //Mixer > Test write to 7 AY register
  while(1)
  {    
    if (GetKeyMatrix(8)!=255) break;

    Locate(10,9);
    PrintNumber(amp);
    
    SetVolume(1,amp++);
    if (amp>15) amp=0;
  
    WAIT(5);
  }
  
  WAIT(PAUSE_TIME);
  
  Locate(0,10);
  Print(" Volume C:");
  amp=0;
  Sound(4,600);
  Sound(5,10); //tone period
  Sound(7,0B00011011); //Mixer > Test write to 7 AY register
  while(1)
  {    
    if (GetKeyMatrix(8)!=255) break;

    Locate(10,10);
    PrintNumber(amp);
    
    SetVolume(2,amp++);
    if (amp>15) amp=0;
  
    WAIT(5);
  }
  
  WAIT(PAUSE_TIME);
  
  Sound(8,0);//enjoy the silence
  Sound(9,0);
  Sound(10,0);  
  
}


// Test SetEnvelopePeriod
void test5()
{
  int period=0;
  char i;  
  boolean isEnd=false;
  
  Cls();
  
  Locate(0,0);
  Print(text01);
  Locate(0,1);
  Print(text02);
    
  Locate(0,4);
  Print(">Test SetEnvelopePeriod function");
  Locate(0,6);
  Print(" Press space to end");
  Locate(0,8);
  Print(" Env Period:");
  
  Sound(0,100);
  Sound(1,10); //tone period
  Sound(7,0B00111110); //Mixer
  Sound(8,16);
  Sound(13,14);
  
  while(!isEnd)
  {
    Locate(12,8);
    PrintNumber(period);

    SetEnvelopePeriod(period++);
    if (period>65533) period=0;
    
    for(i=0;i<20;i++)
    {
      HALT;     
      if (GetKeyMatrix(8)!=255){isEnd=true;break;}
    }    
    //if (joytrig(0)) break;
  }
  
  Sound(8,0);
  
  WAIT(PAUSE_TIME);
  
}



// Test PlayEnvelope and SetChannel
void test6()
{
  char pressKey;
  boolean chanA = true;
  boolean chanB = true;
  boolean chanC = true;
  boolean noiseA = false;
  boolean noiseB = false;
  boolean noiseC = false;
  boolean keyB0semaphore = false;
  boolean keyB1semaphore = false;
  boolean keyB3semaphore = false;
  boolean keyB6semaphore = false;
  
  _lastEnv=0;
  
  Cls();
  
  Locate(0,0);
  Print(text01);
  Locate(0,1);
  Print(text02);

  Locate(0,4);
  Print(">Test PlayEnvelope function");
  Locate(0,6);
  Print("Press 0-7 to fire Envelope");
  
  Locate(0,8);
  Print("           A        B        C  ");
  Print("      -------- -------- --------");
  Locate(0,10);
  Print("Tone :");
  Locate(0,11);
  Print("Noise:");
  
  Locate(0,14);
  Print(" 1 - \\_____\n");
  Print(" 2 - /|____\n");
  Print(" 3 - |\\|\\|\\ \n");
  Print(" 4 - \\/\\/\\/\n");
  Print(" 5 - \\|----\n");
  Print(" 6 - /|/|/|\n");
  Print(" 7 - /-----\n");
  Print(" 8 - /\\/\\/\\ \n");
  
  
  SetTonePeriod(0, 1000);
  SetTonePeriod(1, 1100);
  SetTonePeriod(2, 1200);
  SetVolume(0,16);
  SetVolume(1,16);
  SetVolume(2,16);
  SetEnvelopePeriod(7000);
  setEnv(7);
 
  PrintChannel(0, chanA,noiseA);
  PrintChannel(1, chanB,noiseB);
  PrintChannel(2, chanC,noiseC);
  
  
  while(1)  //bucle infinito
  {
    HALT;

    pressKey = GetKeyMatrix(0);      
    if (pressKey!=255)
    {
      if (!keyB0semaphore)
      {
        keyB0semaphore = true;
        //if (pressKey==0b11111110)      //0
        if (pressKey==0b11111101) setEnv(0);     //1
        if (pressKey==0b11111011) setEnv(1);     //2
        if (pressKey==0b11110111) setEnv(2);     //3
        if (pressKey==0b11101111) setEnv(3);     //4
        if (pressKey==0b11011111) setEnv(4);     //5
        if (pressKey==0b10111111) setEnv(5);     //6
        if (pressKey==0b01111111) setEnv(6);     //7
      }
    }else keyB0semaphore=false;
    
    
    pressKey = GetKeyMatrix(1);      
    if (pressKey!=255)
    {
      if (!keyB1semaphore)
      {
        keyB1semaphore = true;
        if (pressKey==0b11111110) setEnv(7);     //8
      }
    }else keyB1semaphore=false;
    
    
    pressKey = GetKeyMatrix(3);      
    if (pressKey!=255)
    {
      if (!keyB3semaphore)
      {
        keyB3semaphore = true;
        if (pressKey==0b11111101){ noiseA=NOT(noiseA);PrintChannel(0, chanA,noiseA);} // D
        if (pressKey==0b11110111){ noiseB=NOT(noiseB);PrintChannel(1, chanB,noiseB);} //F
        if (pressKey==0b11101111){ noiseC=NOT(noiseC);PrintChannel(2, chanC,noiseC);} //G
      }      
    }else keyB3semaphore=false;
    
    
    pressKey = GetKeyMatrix(6);      
    if (pressKey!=255)
    {
      if (!keyB6semaphore)
      {
        keyB6semaphore = true;
        if (pressKey==0b11011111){ chanA=NOT(chanA);PrintChannel(0, chanA,noiseA);} //F1
        if (pressKey==0b10111111){ chanB=NOT(chanB);PrintChannel(1, chanB,noiseB);} //F2
        if (pressKey==0b01111111){ chanC=NOT(chanC);PrintChannel(2, chanC,noiseC);} //F3
      }      
    }else keyB6semaphore=false;

  } //END while


}



void setEnv(char envType)
{
  Locate(0,14+_lastEnv);
  Print(" ");
  _lastEnv = envType;
  
  Locate(0,14+envType);
  Print(">");
  
  switch (envType)
  {
    case 0:      
      PlayEnvelope(ENV_LowerBeat);
      break;
    case 1:      
      PlayEnvelope(ENV_Upper);
      break;
    case 2:      
      PlayEnvelope(ENV_LeftSaw);
      break;
    case 3:      
      PlayEnvelope(ENV_LowerTriangle);
      break;
    case 4:      
      PlayEnvelope(ENV_LowerHold);
      break;
    case 5:      
      PlayEnvelope(ENV_RightSaw);
      break;
    case 6:      
      PlayEnvelope(ENV_UpperHold);
      break;
    case 7:      
      PlayEnvelope(ENV_UpperTriangle);
      break;
    
  
  }
}



void PrintChannel(char chan, boolean stateTone, boolean stateNoise)
{
  char posx = (chan*9)+6;
  
  SetChannel(chan, stateTone, stateNoise);
    
  Locate(posx,10); 
  switch (chan)
  {
    case 1:      
      Print("[F2] ");
      break;
    case 2:      
      Print("[F3] ");
      break;
    default:
      Print("[F1] ");
      break;  
  }
  if (stateTone) Print("On ");
  else Print("Off");
  
  Locate(posx,11); 
  switch (chan)
  {
    case 1:      
      Print("[F]  ");
      break;
    case 2:      
      Print("[G]  ");
      break;
    default:
      Print("[D]  ");
      break;  
  }
  if (stateNoise) Print("On ");
  else Print("Off");

  return;
}