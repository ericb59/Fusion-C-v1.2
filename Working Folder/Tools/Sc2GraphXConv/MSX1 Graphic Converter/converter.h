// MSX Graphics Conversion <ith CIEDE2000 Algorythm
// Original routine version by Leandro Correia (2019)
// port to C with SDL2 library By Eric Boez 2019
//
//  Created by Eric Boez on 15/04/2019.
//  Copyright © 2019 Eric Boez. All rights reserved.
//


//#define WINDOWS  /* uncomment this line to compile for windows.*/
# ifdef WINDOWS
# include <direct.h>
# define GETCURRENTDIR _getcwd
# else
# include <unistd.h>
# define GETCURRENTDIR getcwd
# endif

#ifndef converter_h
#define converter_h

# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include "SDL2/SDL.h"
# include <sys/time.h>

# define NAME           " Graphx Converter "
# define WNAME          " Graphx Converter v0.5 - Ericb59 (2019)"
# define VERSION        " 0.5 "
# define DATE           " April 2019 "
# define WINDOW_WIDTH   808                     // Total  window Width
# define WINDOW_HEIGHT  616                     // Total  Window Height
# define IMAGE_WIDTH    256                     // WIDTH of image to convert
# define IMAGE_HEIGHT   192                     // Height of image to convert
# define ORG_X          10                      // X position of Original image
# define ORG_Y          10                      // Y position of Original image
# define NB_PALETTE     6                       // How many Palette available
# define TOLERANCE_MAX  100                     // Maximum level of the Toleance parameter
# define TOLERANCE_MIN  0                       // minimum level of the Toleance parameter
# define DETAIL_MAX     255                     // Maximum level of the Detail parameter
# define DETAIL_MIN     0                       // Minimum level of the Detail parameter
# define STEP_TOLERANCE 20                      // Changing Tolerance value Step in Graphical Interface
# define STEP_DETAIL    20                      // Changing Setail value Step in Graphical Interface


# define Pi 3.141592653589793238462643383279


typedef struct    s_img                    //  Structure to store all needed variables
{
    SDL_Window      *win;
    SDL_Renderer    *ren;
    SDL_Surface     *image_temp;
    SDL_Texture     *texture_temp;
    SDL_Texture     *screen_0;
    SDL_Surface     *mouse_pointer;
    SDL_Surface     *img;
    clock_t         click_clock;
    clock_t         clock;
    unsigned int    Rcol;
    unsigned int    Gcol;
    unsigned int    Bcol;
    unsigned int    tolerance;
    unsigned int    detaillevel;
    unsigned int    RenderImage[192][256];
    unsigned int    msxr[16];
    unsigned int    msxg[16];
    unsigned int    msxb[16];
    unsigned char   UsePalette;
    unsigned char   Nb_colors;
    unsigned char   slot;
    unsigned char   interface;
    char            *savefile;
}   t_img;

char **g_argv;


unsigned char    msxdump[6144*2];

unsigned int Slot_x[9]={
    ORG_X,ORG_X+IMAGE_WIDTH+ORG_X,ORG_X+IMAGE_WIDTH+ORG_X+IMAGE_WIDTH+ORG_X,
    ORG_X,ORG_X+IMAGE_WIDTH+ORG_X,ORG_X+IMAGE_WIDTH+ORG_X+IMAGE_WIDTH+ORG_X,
    ORG_X,ORG_X+IMAGE_WIDTH+ORG_X,ORG_X+IMAGE_WIDTH+ORG_X+IMAGE_WIDTH+ORG_X};
unsigned int Slot_y[9]={
    ORG_Y,ORG_Y,ORG_Y,
    ORG_Y+IMAGE_HEIGHT+ORG_Y,ORG_Y+IMAGE_HEIGHT+ORG_Y,ORG_Y+IMAGE_HEIGHT+ORG_Y,
    ORG_Y+IMAGE_HEIGHT+ORG_Y+IMAGE_HEIGHT+ORG_Y,ORG_Y+IMAGE_HEIGHT+ORG_Y+IMAGE_HEIGHT+ORG_Y,ORG_Y+IMAGE_HEIGHT+ORG_Y+IMAGE_HEIGHT+ORG_Y};

int Palette_msx1[]={
    0x00,0x00,0x00,      // Transparent
    0x01,0x01,0x01,      // Black
    0x3e,0xb8,0x49,      // Medium Green
    0x74,0xd0,0x7d,      // Light Green
    0x59,0x55,0xe0,      // Dark Blue
    0x80,0x76,0xf1,      // Light Blue
    0xb9,0x5e,0x51,      // Dark Red
    0x65,0xdb,0xef,      // Cyan
    0xdb,0x65,0x59,      // Medium Red
    0xff,0x89,0x7d,      // Light Red
    0xcc,0xc3,0x5e,      // Dark Yellow
    0xde,0xd0,0x87,      // Light Yellow
    0x3a,0xa2,0x41,      // Dark Green
    0xb7,0x66,0xb5,      // Magenta
    0xcc,0xcc,0xcc,      // Grey
    0xff,0xff,0xff};     // White

int Palette_msx0[]={
    0,0,0,              // Transparent
    0,0,0,              // Black
    36,219,36,          // Medium Green
    109,255,109,        // Light Green
    36,36,255,          // Dark Blue
    73,109,255,         // Light Blue
    182,36,36,          // Dark Red
    73,219,255,         // Cyan
    255,36,36,          // Medium Red
    255,109,109,        // Light Red
    219,219,36,         // Dark Yellow
    219,219,146,        // Light Yellow
    36,146,36,          // Dark Green
    219,73,182,         // Magenta
    182,182,182,        // Grey
    255,255,255};       // White

int Palette_c64[]={
    0, 0, 0,            // black
    255, 255, 255,      // white
    136, 57, 50,        // red
    103, 182, 189,      // cyan
    139, 63, 150,       // purple
    85, 160, 73,        // green
    64, 49, 141,        // blue
    191, 206, 114,      // yellow
    139, 84, 41,        // orange
    87, 66, 0,          // brown
    184, 105, 98,       // light red
    80, 80, 80,         // dark grey
    120, 120, 120,      // grey
    148, 224, 137,      // light green
    120, 105, 196,      // light blue
    159, 159, 159};     // light grey;

int Palette_spectrum[]={
    0, 0, 0,            // black
    0, 0, 170,          // basic blue
    170, 0, 0,          // basic red
    170, 0, 170,        // basic magenta
    0, 170, 0,          // basic green
    0, 170, 170,        // basic cyan
    170, 170, 0,        // basic yellow
    170, 170, 170,      // basic white
    0, 0, 0,            // black
    0, 0, 255,          // bright blue
    255, 0, 0,          // bright red
    255, 0, 255,        // bright magenta
    0, 255, 0,          // bright green
    0, 255, 255,        // bright cyan
    255, 255, 0,        // bright yellow
    255, 255, 255};     // bright white

int Palette_gameboy[]={
    0x0f,0x38,0x0f,
    0x30,0x62,0x30,
    0x8b,0xac,0x0f,
    0x9b,0xbc,0x0f};

int Palette_BW[]={
    0x00, 0x00, 0x00,   // Black
    0xff, 0xff, 0xff,   // White
    0x01, 0x01,0x01};   // Black

int Palette_msxbw[]={
    0,0,0,              // Transparent
    0,0,0,              // Black
    0,0,0,              // Black
    0,0,0,              // Black
    0,0,0,              // Black
    0,0,0,              // Black
    0,0,0,              // Black
    0,0,0,              // Black
    0,0,0,              // Black
    0,0,0,              // Black
    0,0,0,              // Black
    0,0,0,              // Black
    0,0,0,              // Black
    0,0,0,              // Black
    182,182,182,        // Grey
    255,255,255};       // White

char *Palette_name[]={"MSX Palette 0",
    "MSX Palette 1",
    "C64 Palette",
    "SPECTRUM Palette",
    "GAME BOY Palette",
    "B & W Palette",
    "MSX SC2 B & W"};


void    init_mem(t_img *e);
void    ft_memdel(void **ap);
void    free_SDL(t_img *e);
void    ft_exit(t_img *e, int error, char *text);
int     SDL_init(t_img *e, SDL_Window *win, SDL_Renderer *ren,int width , int height);
void    SDL_put_img(int sx,int sy, int sw, int sh, int dx, int dy, t_img *e, SDL_Surface *surf_src, SDL_Surface *surf_dest);
int     load_image(t_img *e);
void    SDL_render(t_img *e);
void    SDL_pixel_put_to_image(t_img *e, int x, int y, unsigned int color);
Uint32  SDL_getpixel(SDL_Surface *surface, int x, int y);
double  calcdist2000(double r1, double g1, double b1, double r2, double g2, double b2);
void    MSXoutput(t_img *e);
void    ReadPalette(t_img *e);
void    ImageProcess(t_img *e);
void    Do_it (t_img *e);
char    PrintDo(t_img *e);
void    DrawSlot(t_img *e,int Start_x, int Start_y);
void    Info(t_img *e);
SDL_Surface *SDL_loadBMP(Uint32 format,t_img *e);




#endif /* converter_h */
