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
| ayfx_player.h : play sound FX from AyFX Edit software     |
|                                                           |
|                                                           |
|                                                           |
\___________________________________________________________/
*                         (°□°)
* Original AYFX Player code by : Jorge Torres Chacón
* Many thanks to him.
*/

//#include <stdlib.h>


#ifndef _AYFX_H
#define _AYFX_H


#define JIFFY (*(char *)0xfc9e)
/* Defines para la lectura de los flag bytes */
#define MASK_VOLUME 0xF             /* Bit 0..3 */
#define MASK_DISABLE_TONE 0x10      /* Bit 4 */
#define MASK_CHANGE_TONE 0x20       /* Bit 5 */
#define MASK_CHANGE_NOISE 0x40      /* Bit 6 */
/* Bit 7 -> Disable NOISE, se puede hacer con bitshifting. Va al reves*/

/* Defines para escritura de registros del PSG */
#define TONE_REG_0 (unsigned char)0       /* Cada canal usa 2 registros de tono */ 
#define TONE_REG_1 (unsigned char)1       /* Los del canal A, son 0 y 1, sumar 2*i para los canales B y C */
#define NOISE_REG (unsigned char)6        /* Registro de noise, comun a los 3 canales */
#define MIXER_REG (unsigned char)7        /* Registro mixer (disable tone, noise), comun a los 3 canales */
#define VOLUME_REG_0 (unsigned char)8     /* Registro de volumen base canal A, sumar i para los canales B y C */
#define MASK_NEW_VOL (unsigned char)0x3   /* Mascara para cambiar solo los primeros 4 bits */

/* Defines para escritura en el registro 7 (Mixer), se aplican mascaras 
 B5,B4,B3 -> Noise disable C,B,A. Para B y C se obtienen por bit shiffting sobre A
 B2,B1,B0 -> Tone disable C,B,A. Para B y C se obtienen por bit shiffting sobre A */
#define DISABLE_TONE_A 254
#define DISABLE_TONE_B 253
#define DISABLE_TONE_C 251
#define DISABLE_NOISE_A 247
#define DISABLE_NOISE_B 239
#define DISABLE_NOISE_C 223

#define FALSE 0
#define TRUE 1

typedef struct {
  int position; /* Puntero al flag byte en curso. Si es =0, no esta tocando nada */
} Afx;


/*#define SIZEAFB 645*/
static unsigned char *afbdata;
static Afx afx[3];
static char *noises;                    /* Clasificamos los SFX por aquellos que tienen noise y los que no */

 void InitFX(void);	/* Inicia el driver */
 char PlayFX(unsigned char nfx) ;	/* Carga en un canal libre el sfx cuyo numero dentro del afb se le pasa */
 void UpdateFX(void);	/* Actualiza los SFX en curso, se debe invocar el bucle principal */ 
 void FreeFX(void);	/* Libera la memoria ocupada por los SFX */
 char TestFX(void);	/* Chequea si hay SFX en ejecucion */


// Initialisation of SFX
void InitFX(void) 
{
  unsigned char i;
  
  InitPSG();/* Inicializa el PSG a sus valores por defecto */
  for(i=0; i<3; i++) {
    afx[i].position = 0;
  }
}

/* Clasificamos los efectos por los que tienen y los que no tienen ruido */
static void setnoises(void) 
{
  unsigned char nfx,flagbyte;
  int highbyte, position;
  char exit;

  noises = MMalloc((int)afbdata[0]+1);
  for(nfx=1;nfx<=afbdata[0];nfx++) {
    noises[nfx]=FALSE;
    highbyte = ((int)(afbdata[(nfx*2)] & 0xFF)) << 8;
    position = (int)((afbdata[(nfx*2)-1]) | highbyte)+(nfx*2);
    exit=FALSE;
    while(exit==FALSE) {
      if(afbdata[position] == 0xD0 && afbdata[position+1] == 0x20) {
        exit = TRUE;
        continue;
      }
      flagbyte = afbdata[position];
      if((flagbyte>>7)==0) {
        noises[nfx]=TRUE;
        exit=TRUE;
        continue;
      }
      if((flagbyte & MASK_CHANGE_TONE) == MASK_CHANGE_TONE) {
        position = position + 2;
      }
      if((flagbyte & MASK_CHANGE_NOISE) == MASK_CHANGE_NOISE) {
        position++;
      }
      position++;
    }
  }
}


/* Inicia el flag byte del SFX en el canal de turno 
 Los SFX con ruido siempre irán al canal 1 */
char PlayFX(unsigned char nfx)  /* Numero a sfx */
{
  int highbyte, position;

  if(nfx<=afbdata[0]) {
    highbyte = ((int)(afbdata[(nfx*2)] & 0xFF)) << 8;
    position = (int)((afbdata[(nfx*2)-1]) | highbyte)+(nfx*2);
    if(noises[nfx]==TRUE) {
      afx[0].position=position;
      return(0);
    }
    if (afx[1].position==0) {
      afx[1].position=position;
    } else if (afx[2].position==0) {
      afx[2].position=position;
    } else {
      afx[0].position=position;
    }
  }
  return(0);
}

char Reg7Patch(char content)
{
  char varf;
  varf=0;

  varf=(content | 0b10000000);
  content=(varf & 0b10111111);

  return(content);

}
/* Funcion que actualiza el SFX de turno en el ciclo en curso */
static char playsnd(unsigned char i) 
{
  unsigned char volume, flagbyte, disable_tn;
  unsigned char reg7;
  reg7=0;

  if(afbdata[afx[i].position] == 0xD0 && afbdata[afx[i].position+1] == 0x20) {
    afx[i].position=0;
    return (FALSE); /* Fin de SFX, mutar canal */
  }
  flagbyte = afbdata[afx[i].position];
  /* Volume read */
  volume = flagbyte & MASK_VOLUME;
  PSGwrite(VOLUME_REG_0+i,(PSGRead(VOLUME_REG_0+i) & !MASK_NEW_VOL) | volume);
  /* Change tone check: cargamos 2 bytes en registros de tono */
  if((flagbyte & MASK_CHANGE_TONE) == MASK_CHANGE_TONE) {
    afx[i].position++;
    PSGwrite((TONE_REG_0)+(i*2), afbdata[afx[i].position]);
    afx[i].position++;
    PSGwrite((TONE_REG_1)+(i*2), afbdata[afx[i].position]);
  }
  /* Change noise check: cargamos 1 byte en registro de ruido */
  if((flagbyte & MASK_CHANGE_NOISE) == MASK_CHANGE_NOISE) {
    afx[i].position++;
    PSGwrite(NOISE_REG, afbdata[afx[i].position]);
  }
  /* Disable tone check */
  disable_tn = (flagbyte & MASK_DISABLE_TONE) >> 3;
  switch (i)
  {
    case 0:
      reg7=(PSGRead(MIXER_REG) & DISABLE_TONE_A) | (disable_tn << i);
      //PSGwrite(MIXER_REG,reg7);
    break;
    
    case 1:
      reg7=(PSGRead(MIXER_REG) & DISABLE_TONE_B) | (disable_tn << i);
     // PSGwrite(MIXER_REG,reg7);
    break;

    case 2:
      reg7=(PSGRead(MIXER_REG) & DISABLE_TONE_C) | (disable_tn << i);
       
    break;
  }

  PSGwrite(MIXER_REG,Reg7Patch(reg7));
  /*
  if(i==(unsigned char)0) {
    PSGwrite(MIXER_REG, (PSGRead(MIXER_REG) & DISABLE_TONE_A) | (disable_tn << i));
  } else if(i==(unsigned char)1) {
    PSGwrite(MIXER_REG, (PSGRead(MIXER_REG) & DISABLE_TONE_B) | (disable_tn << i));
  } else if(i=(unsigned char)2) {
    PSGwrite(MIXER_REG, (PSGRead(MIXER_REG) & DISABLE_TONE_C) | (disable_tn << i));
  }
 */
  /* Disable noise check */
  disable_tn = flagbyte >> 7;
  switch (i)
  {
    case 0:
      reg7=(PSGRead(MIXER_REG) & DISABLE_NOISE_A) | (disable_tn << (i+3));
     // PSGwrite(MIXER_REG, reg7);
    break;

    case 1:
      reg7=(PSGRead(MIXER_REG) & DISABLE_NOISE_B) | (disable_tn << (i+3));
      //PSGwrite(MIXER_REG, reg7);
    break;

    case 2:
      reg7=(PSGRead(MIXER_REG) & DISABLE_NOISE_C) | (disable_tn << (i+3));
      
    break;
  }

  PSGwrite(MIXER_REG, Reg7Patch(reg7));
/*
  if(i==(unsigned char)0) {
    PSGwrite(MIXER_REG, (PSGRead(MIXER_REG) & DISABLE_NOISE_A) | (disable_tn << (i+3)));
  } else if(i==(unsigned char)1) {
    PSGwrite(MIXER_REG, (PSGRead(MIXER_REG) & DISABLE_NOISE_B) | (disable_tn << (i+3)));
  } else if(i=(unsigned char)2) {
    PSGwrite(MIXER_REG, (PSGRead(MIXER_REG) & DISABLE_NOISE_C) | (disable_tn << (i+3)));
  }
*/


  afx[i].position++;
  return (TRUE);
}

/* En cada ciclo actualiza los SFX que están en ejecución */
void UpdateFX(void) 
{
  unsigned char i;

  for(i=0;i<3;i++) {
    if(afx[i].position!=0) {
      if(playsnd(i)==FALSE) {
        /* Mute PSGwrite channel */
        PSGwrite(VOLUME_REG_0+i, PSGRead(VOLUME_REG_0+i) & 0xF0);
      }
    }
  }
}

/* Chequea si hay SFX en ejecucion */
char TestFX(void) {
  if(afx[0].position!=0 || afx[1].position!=0 || afx[2].position!=0) {
    return (TRUE);
  } else {
    return (FALSE);
  }
}

/* Libera la memoria ocupada por los objetos del driver */
void FreeFX(void) {
  free(afbdata);
  free(noises);
}


#endif
