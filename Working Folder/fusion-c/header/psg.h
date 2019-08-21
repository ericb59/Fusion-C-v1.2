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
|              Eric Boez &  Fernando Garcia                 |
|                                                           |
| psg.h : play sound and FX wuth PSG                        |
|                                                           |
|                                                           |
|                                                           |
\___________________________________________________________/
*                         (°□°)
* Most original code from : mvac7/303bcn (aka aorante)
* 
*/


// boolean
#ifndef  __AY38910_H__
#define  __AY38910_H__



#ifndef _BOOLEAN
#define _BOOLEAN
  typedef enum {false = 0, true = 1} boolean;
#endif




typedef struct {
  boolean isTone;
  boolean isNoise;  
  unsigned int Tone;
  char Noise;
  unsigned int Period;
  char Shape;
} FX;



//envelope shapes
#define ENV_LowerBeat      0 
#define ENV_Upper          4 
#define ENV_LeftSaw        8
#define ENV_LowerTriangle 10 
#define ENV_LowerHold     11 
#define ENV_RightSaw      12
#define ENV_UpperHold     13
#define ENV_UpperTriangle 14 
 




/* =============================================================================
 SOUND(register, value)

 Function : Write into a register of PSG
 Input    : [char] register number (0 to 13)
            [char] value
 Output   : -
============================================================================= */
void Sound(char reg, char value);



/* =============================================================================
 PlayFX

 Function : Play FX
 Input    : [char] channel (0, 1 or 2)
            [*FX] sound data in FX structure
 Output   : -
============================================================================= */
void SoundFX(char channel, FX *soundat);



/* =============================================================================
 SetChannel

 Function : Enable/disable Tone and Noise channels.
 Input    : [char] channel (0, 1 or 2)
            [boolean] tone state
            [boolean] noise state
 Output   : -
============================================================================= */
void SetChannelA(char channel, boolean isTone, boolean isNoise);



/* =============================================================================
 SilencePSG

 Function : Silence the three PSG channels.
 Input    : -
 Output   : -
============================================================================= */
void SilencePSG();


/* =============================================================================
 GetSound(register) 

 Function : Read PSG register value
 Input    : [char] register number (0 to 13)
 Output   : [char] value 
============================================================================= */
char GetSound(char reg);



/* =============================================================================
 SetTonePeriod(channel, period) 

 Function : Set Tone Period for any channel
 Input    : [char] channel (0, 1 or 2)
            [unsigned int] period (0 - 4095)
 Output   : -
============================================================================= */
void SetTonePeriod(char channel, unsigned int period);



/* =============================================================================
 SetNoisePeriod(period) 

 Function : Set Noise Period
 Input    : [char] Noise period (0 - 31) 
 Output   : - 
============================================================================= */
void SetNoisePeriod(char period);



/* =============================================================================
 SetEnvelopePeriod(period) 

 Function : Set Envelope Period
 Input    : [unsigned int] Envelope period (0 - 65535) 
 Output   : - 
============================================================================= */
void SetEnvelopePeriod(unsigned int period);



/* =============================================================================
 SetVolume(channel, volume) 

 Function : Set volume channel
 Input    : [char] channel (0, 1 or 2)
            [char] volume, 0 to 15 or 16 for activate envelope
 Output   : -
============================================================================= */
void SetVolume(char channel, char volume);



/* =============================================================================
 SetChannel(channel, isTone, isNoise)

 Function : Mixer. Enable/disable Tone and Noise channels.
 Input    : [char] channel (0, 1 or 2)
            [boolean] tone state
            [boolean] noise state
 Output   : -
============================================================================= */
void SetChannel(char channel, boolean isTone, boolean isNoise);



/* =============================================================================
 PlayEnvelope(shape) 

 Function : Set envelope type.
            Plays the sound on channels that have a volume of 16.
 Input    : [char] Envelope shape (0-15) (see envelope shapes definitions)
 Output   : -
============================================================================= */
void PlayEnvelope(char shape);



#endif