// MSX Graphics Conversion with CIEDE2000 Algorythm
// Original routine version by Leandro Correia (2019)
// port to C with SDL2 library By Eric Boez 2019
//
//  Created by Eric Boez on 15/04/2019.
//  Copyright © 2019 Eric Boez. All rights reserved.
//
// COMPIL WITH SDL2.0 LIBRARY
//
# include "converter.h"

//**--------------------------------**
//   Read And Use a Specific RGB
//              Palette
//**--------------------------------**
//
void ReadPalette(t_img *e)
{
    unsigned char i;
    int id;
    id=0;
    
    switch (e->UsePalette)
    {
        case 0:
            e->Nb_colors=15;    // Number of colors in MSX0 Palette +1
            break;
        case 1:
            e->Nb_colors=15;    // Number of colors in MSX1 Palette +1
            break;
        case 2:
            e->Nb_colors=15;    // Number of colors in C64 Palette +1
            break;
        case 3:
            e->Nb_colors=15;    // Number of colors in SPECTRUM Palette +1
            break;
        case 4:
            e->Nb_colors=3;    // Number of colors in GAMEBOY Palette +1
            break;
        case 5:
            e->Nb_colors=2;    // Number of colors in B & W Palette +1
            break;
        case 6:
            e->Nb_colors=15;    // Number of colors in MSX B & W Palette +1
            break;

    }
    
    
    // Reads the MSX RGB color values;
    for (i=0; i<=e->Nb_colors; i++)
    {
        switch (e->UsePalette)
        {
            case 0:
                e->msxr[i]=Palette_msx0[id];
                e->msxg[i]=Palette_msx0[id+1];
                e->msxb[i]=Palette_msx0[id+2];
                break;
            case 1:
                e->msxr[i]=Palette_msx1[id];
                e->msxg[i]=Palette_msx1[id+1];
                e->msxb[i]=Palette_msx1[id+2];
                break;
            case 2:
                e->msxr[i]=Palette_c64[id];
                e->msxg[i]=Palette_c64[id+1];
                e->msxb[i]=Palette_c64[id+2];
                break;
            case 3:
                e->msxr[i]=Palette_spectrum[id];
                e->msxg[i]=Palette_spectrum[id+1];
                e->msxb[i]=Palette_spectrum[id+2];
                break;
            case 4:
                e->msxr[i]=Palette_gameboy[id];
                e->msxg[i]=Palette_gameboy[id+1];
                e->msxb[i]=Palette_gameboy[id+2];
                break;
            case 5:
                e->msxr[i]=Palette_BW[id];
                e->msxg[i]=Palette_BW[id+1];
                e->msxb[i]=Palette_BW[id+2];
                break;
            case 6:
                e->msxr[i]=Palette_msxbw[id];
                e->msxg[i]=Palette_msxbw[id+1];
                e->msxb[i]=Palette_msxbw[id+2];
                break;

        }
        id=id+3;
    }
}

//**--------------------------------**
//      Free Memory
//
//**--------------------------------**
//
void    ft_memdel(void **ap)
{
    if (ap && *ap)
    {
        free(*ap);
        *ap = NULL;
    }
}

//**--------------------------------**
//      Free SDL surfaces & render
//
//**--------------------------------**
//
void free_SDL(t_img *e)
{
    
    SDL_DestroyTexture(e->screen_0);
    SDL_FreeSurface(e->img);
    SDL_DestroyRenderer(e->ren);
    SDL_DestroyWindow(e->win);
    SDL_Quit();
}

//**--------------------------------**
//      Keyboard control hook
//
//**--------------------------------**
//
int   key_hook(t_img *e)
{
    const Uint8 *keystates = SDL_GetKeyboardState(NULL);
    
    if (e->clock > e->click_clock)
    {
        
        if (keystates[SDL_SCANCODE_ESCAPE])                                     // Exit/Stop
        {
            e->click_clock=clock()+(CLOCKS_PER_SEC);
            ft_exit(e,0,"\nSee you !");
        }
        
        if (keystates[SDL_SCANCODE_S])                  // New slot image
        {
            e->click_clock=clock()+(CLOCKS_PER_SEC/10);
            e->slot++;
            if (e->slot>=9)
                e->slot=1;
            DrawSlot(e, Slot_x[e->slot], Slot_y[e->slot]);
        }
        
        if (keystates[SDL_SCANCODE_1]  )                // Tolerance --
        {
            e->click_clock=clock()+(CLOCKS_PER_SEC/10);
            e->tolerance=e->tolerance-STEP_TOLERANCE;
            if (e->tolerance<=TOLERANCE_MIN)
                e->tolerance=TOLERANCE_MIN;
            PrintDo(e);
            Do_it(e);

        }
        
        if (keystates[SDL_SCANCODE_2] )                // Tolerance ++
        {
            e->click_clock=clock()+(CLOCKS_PER_SEC/10);
            e->tolerance=e->tolerance+STEP_TOLERANCE;
            if (e->tolerance>=TOLERANCE_MAX)
                e->tolerance=TOLERANCE_MAX;
            PrintDo(e);
            Do_it(e);

           
        }
        
        if (keystates[SDL_SCANCODE_9] )                // Detal level  --
        {
            e->click_clock=clock()+(CLOCKS_PER_SEC/10);
            e->detaillevel=e->detaillevel+STEP_DETAIL;
            if (e->detaillevel>=DETAIL_MAX)
                e->detaillevel=DETAIL_MAX;
            PrintDo(e);
            Do_it(e);
        }
        
        if (keystates[SDL_SCANCODE_8] )                // Detail Level --
        {
            e->click_clock=clock()+(CLOCKS_PER_SEC/10);
            e->detaillevel=e->detaillevel-STEP_DETAIL;
            if (e->detaillevel<=DETAIL_MIN)
                e->detaillevel=DETAIL_MIN;
            PrintDo(e);
            Do_it(e);
           
        }
        
        if (keystates[SDL_SCANCODE_P] )                // Use Next Palette
        {
            e->click_clock=clock()+(CLOCKS_PER_SEC/10);
            e->UsePalette++;
            if (e->UsePalette>NB_PALETTE)
                e->UsePalette=0;
            PrintDo(e);
            Do_it(e);
         
        }
    }
    return(0);
}



///**--------------------------------**
//       True Exit of the Program
//          with error keycodes
//**--------------------------------**
//
void ft_exit(t_img *e, int error, char *text)
{
    switch (error) {
        case 0:
            puts("\nEnding  process...");
            break;
            
        case 1:
            puts("\nError N°1 Opening file.\n");
            break;
            
        case 2:
            puts("Error n°2 Loading error.\n");
            break;
            
        case 3:
            puts("Error n°3 Ouch ! .\n");
            break;
            
        case 4:
            puts("Error n°4 Malloc error\n");
            break;
            
        case 5:
            puts("Error n°5 Init Error\n");
            break;
        case 6:
            puts("Parameters missing");
            break;
        default:
            break;
    }
    
    puts(text);
    puts("\n");
    free_SDL(e);
    SDL_Quit();
    
    ft_memdel((void **)&e);
    exit(EXIT_SUCCESS);
}

//**--------------------------------**
//      Initialisation of the SDL
// Window, render & surface creation
//**--------------------------------**
//
int SDL_init(t_img *e, SDL_Window *win, SDL_Renderer *ren,int width , int height)
{
    /* Initialisation de la SDL. Si ça se passe mal, on quitte */
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        fprintf(stderr,"SDL2 initialisation Error\n");
        return (EXIT_FAILURE);
    }
    
    /* Création de la fenêtre  */
    Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_MOUSE_FOCUS;
    e->win = SDL_CreateWindow(WNAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,width, height, flags);
    if(NULL == e->win)
    {
        fprintf(stderr, "SDL_CreateWindow Error : %s", SDL_GetError());
        return (EXIT_FAILURE);
    }
    
    /* Création du renderer */
    e->ren = SDL_CreateRenderer(e->win, -1, SDL_RENDERER_ACCELERATED);
    if(NULL == e->ren)
    {
        fprintf(stderr, "SDL_CreateRenderer Error: %s", SDL_GetError());
        return (EXIT_FAILURE);
    }
    
    /* Black back ground */
    SDL_RenderClear(e->ren);
    SDL_SetRenderDrawColor(e->ren, 0, 0, 0, 255);
    
    //Initialisation of Main surfaces
    e->img = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGB888);
    
    // Initialisation of Texture Screen 0 & screen 1
    e->screen_0 = SDL_CreateTexture(e->ren, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    return(0);
}

//**--------------------------------**
//        Loading BMP image file
// & convert surface to requested color mode
//**--------------------------------**
//
SDL_Surface *SDL_loadBMP(Uint32 format,t_img *e)
{
    SDL_Surface *tmp, *s;
    tmp = SDL_LoadBMP(g_argv[1]);   // Load Image passed as argument
    if(NULL == tmp)
    {
        fprintf(stderr, "SDL_LoadBMP Error: %s", SDL_GetError());
        return (NULL);
    }
    s = SDL_ConvertSurfaceFormat(tmp, format, 0);
    SDL_FreeSurface(tmp);
    if (NULL == s)
    {
        fprintf(stderr, "SDL_ConvertSurfaceFormat Error : %s", SDL_GetError());
        ft_exit(e,2,"\n BMP conversion error");
    }
    return (s);
}

//**--------------------------------**
//   SDL Copy image to image process
//              surfaces
//**--------------------------------**
//
void SDL_put_img(int sx,int sy, int sw, int sh, int dx, int dy, t_img *e, SDL_Surface *surf_src, SDL_Surface *surf_dest)
{
    SDL_Rect dest = { dx, dy, 0, 0};
    SDL_Rect src = { sx , sy , sw , sh};
    SDL_BlitSurface(surf_src , &src , surf_dest , &dest);
}

//**--------------------------------**
//      Load Original image
//           to main surface
//**--------------------------------**
//
int load_image(t_img *e)
{

        if (!(e->image_temp = SDL_loadBMP(SDL_PIXELFORMAT_RGB888,e)))
            ft_exit(e,2,"Original image problem");
        SDL_put_img(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, ORG_X, ORG_Y, e, e->image_temp, e->img);
        SDL_FreeSurface(e->image_temp);
    
    return(1);
}

//**--------------------------------**
//    Mem variables initialisation
//
//**--------------------------------**
//
void init_mem(t_img *e)
{
    if (SDL_init(e,e->win,e->ren,WINDOW_WIDTH,WINDOW_HEIGHT) !=0)           // SDL2 Initialisation
        ft_exit(e,5,"SDL2 Init Problem !");
}

//**--------------------------------**
//    Plot a Pixel to SDL Image
//
//**--------------------------------**
//
void    SDL_pixel_put_to_image(t_img *e, int x, int y, unsigned int color)
{
    if (x < WINDOW_WIDTH && y < WINDOW_HEIGHT && x >= 0 && y >= 0){
        Uint32 *pixels = NULL;
        pixels = e->img->pixels;
        //SDL_LockSurface(e->img);
        pixels[x + y*WINDOW_WIDTH] = color;
        pixels=NULL;
    }
}

//**--------------------------------**
//    SDL2 Rendering Full Image
//
//**--------------------------------**
//
void SDL_render(t_img *e)
{
    
    ///////////// Rendering Main surface to  Texture screen_0 ////////////////////
    
    SDL_SetRenderTarget(e->ren, e->screen_0);
    
    e->texture_temp = SDL_CreateTextureFromSurface(e->ren, e->img);
    
    SDL_Rect dest = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    SDL_Rect src = { 0 , 0 , WINDOW_WIDTH , WINDOW_HEIGHT};
    SDL_RenderCopy(e->ren, e->texture_temp, &src, &dest);
    SDL_DestroyTexture(e->texture_temp);

    
    SDL_SetRenderTarget(e->ren, NULL);
    SDL_RenderCopy(e->ren, e->screen_0,NULL,NULL);
    SDL_RenderPresent(e->ren);
}

//**--------------------------------**
//    SDL2 Get Pixel Data from Image
//
//**--------------------------------**
//
Uint32 SDL_getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    
    switch(bpp) {
        case 1:
            return *p;
            
        case 2:
            return *(Uint16 *)p;
            
        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
            
        case 4:
            return *(Uint32 *)p;
            
        default:
            return 0;       /* shouldn't happen, but avoids warnings */
    }
}

//**--------------------------------**
//    CIEDE2000 Calcul routine
//
//**--------------------------------**
//
double calcdist2000(double r1, double g1, double b1, double r2, double g2, double b2)
{
    
    if (r1==r2 && g1==g2 && b1==b2)
    {
        return(3.0);
    }
    
    // Convert two RGB color values into Lab and uses the CIEDE2000 formula to calculate the distance between them.
    // This function first converts RGBs to XYZ and then to Lab.
    
    // This is not optimized, but I did my best to make it readable. In some rare cases there are some weird colors,
    // so MAYBE there's a small bug in the implementation.
    // The RGB to Lab conversion in here could easily be substituted by a giant RGB to Lab lookup table,
    // consuming much more memory, but gaining A LOT in speed.
    
    //	Converting RGB values into XYZ
    
    
    double r=r1/255.0;
    double g=g1/255.0;
    double b=b1/255.0;
    
    if (r > 0.04045)
    {
        r=pow(((r+0.055)/1.055),2.4);
    }
    else
    {
        r=r/12.92;
    }
    
    if (g > 0.04045)
    {
        g=pow(((g+0.055)/1.055),2.4);
    }
    else
    {
        g=g/12.92;
    }
    
    if (b > 0.04045)
    {
        b=pow(((b+0.055)/1.055),2.4);
    }
    else
    {
        b=b/12.92;
    }
    
    r=r*100.0;
    g=g*100.0;
    b=b*100.0;
    
    // Observer. = 2°, Illuminant = D65
    double x=r*0.4124 + g*0.3576 + b*0.1805;
    double y=r*0.2126 + g*0.7152 + b*0.0722;
    double z=r*0.0193 + g*0.1192 + b*0.9505;
    
    x=x/95.047;   //Observer= 2°, Illuminant= D65
    y=y/100.000;
    z=z/108.883;
    
    if (x > 0.008856)
    {
        x=pow(x , (1.0/3.0));
    }
    else
    {
        x = (7.787 * x ) + ( 16.0 / 116.0 );
    }
    
    if (y > 0.008856)
    {
        y=pow(y , (1.0/3.0 ));
    }
    else
    {
        y = (7.787 * y ) + ( 16.0 / 116.0 );
    }
    
    if (z > 0.008856)
    {
        z=pow(z , (1.0/3.0));
    }
    else
    {
        z = (7.787 * z ) + ( 16.0 / 116.0 );
    }
    
    double l1 = ( 116.0 * y ) - 16.0;
    double a1 = 500.0 * (x-y);
          b1 = 200.0 * (y-z);
    
    
    r=r2/255.0;
    g=g2/255.0;
    b=b2/255.0;
    
    if (r > 0.04045)
    {
        r=pow(((r+0.055)/1.055),2.4);
    }
    else
    {
        r=r/12.92;
    }
    
    if (g > 0.04045)
    {
        g=pow(((g+0.055)/1.055),2.4);
    }
    else
    {
        g=g/12.92;
    }
    
    if (b > 0.04045)
    {
        b=pow(((b+0.055)/1.055),2.4);
    }
    else
    {
        b=b/12.92;
    }
    
    r=r*100.0;
    g=g*100.0;
    b=b*100.0;
    
    //Observer. = 2°, Illuminant = D65
    x=r*0.4124 + g*0.3576 + b*0.1805;
    y=r*0.2126 + g*0.7152 + b*0.0722;
    z=r*0.0193 + g*0.1192 + b*0.9505;
    
    
    x=x/95.047;   //Observer= 2°, Illuminant= D65
    y=y/100.000;
    z=z/108.883;
    
    if (x > 0.008856)
    {
        x=pow(x, (1.0/3.0));
    }
    else
    {
        x = (7.787 * x ) + ( 16.0 / 116.0 );
    }
    
    if (y > 0.008856)
    {
        y=pow(y , (1.0/3.0));
    }
    else
    {
        y = (7.787 * y ) + ( 16.0 / 116.0 );
    }
    
    if (z > 0.008856)
    {
        z=pow(z , (1.0/3.0));
    }
    else
    {
        z = (7.787 * z ) + ( 16.0 / 116.0 );
    }
    
    //	Converts XYZ to Lab...
    
    double l2 = (116.0 * y) - 16.0;
    double a2 = 500.0 * (x-y);
    b2 = 200.0 * (y-z);
    
    // ...and then calculates distance between Lab colors, using the CIEDE2000 formula.
    
    double dl=l2-l1;
    double hl=l1+dl*0.5;
    double sqb1=b1*b1;
    double sqb2=b2*b2;
    double c1=sqrtf(a1*a1+sqb1);
    double c2=sqrtf(a2*a2+sqb2);
    double hc7=pow(((c1+c2)*0.5),7.0);
    double trc=sqrtf(hc7/(hc7+6103515625.0));
    double t2=1.5-trc*0.5;
    double ap1=a1*t2;
    double ap2=a2*t2;
           c1=sqrtf(ap1*ap1+sqb1);
           c2=sqrtf(ap2*ap2+sqb2);
    double dc=c2-c1;
    double hc=c1+dc*0.5;
          hc7=pow(hc,7.0);
          trc=sqrtf(hc7/(hc7+6103515625.0));
    double h1=atan2(b1,ap1);
    
    if (h1<0)
    {
        h1=h1+Pi*2.0;
    }
    double h2=atan2(b2,ap2);
    
    if (h2<0)
    {
        h2=h2+Pi*2.0;
    }
    
    double hdiff=h2-h1;
    double hh=h1+h2;
    if (fabs(hdiff)>Pi)
    {
        hh=hh+Pi*2;
        if (h2<=h1)
        {
            hdiff=hdiff+Pi*2.0;
        }

    }
    else
    {
        hdiff=hdiff-Pi*2.0;
    }
    
    hh=hh*0.5;
    t2=1.0-0.17*cos(hh-Pi/6.0)+0.24*cos(hh*2.0);
    t2=t2+0.32*cos(hh*3.0+Pi/30.0);
    t2=t2-0.2*cos(hh*4.0-Pi*63.0/180.0);
    double dh=2.0*sqrtf(c1*c2)*sin(hdiff*0.5);
    double sqhl=(hl-50.0)*(hl-50.0);
    double fl=dl/(1.0+(0.015*sqhl/sqrtf(20.0+sqhl)));
    double fc=dc/(hc*0.045+1.0);
    double fh=dh/(t2*hc*0.015+1.0);
    double dt=30*exp(-(pow(36.0*hh-55.0*Pi,2.0))/(25.0*Pi*Pi));
    r=0-2.0*trc*sin(2.0*dt*Pi/180.0);
    
    return sqrtf(fl*fl+fc*fc+fh*fh+r*fc*fh);
}

//**--------------------------------**
//   Image processing Routine
//
//**--------------------------------**
//
void ImageProcess(t_img *e)
{
    int x,y,j,i;
    Uint8 r,g,b,a;
    
    
    int *Final_color;
    unsigned char buf[8];
    
    int octetr[8],octetg[8],octetb[8];
    double octetdetail[8];
    int octetfinal[8], octetvalue[8];
    int toner[5],toneg[5],toneb[5];
    double distcolor[5];
    double detail[IMAGE_WIDTH][IMAGE_HEIGHT];        //Detail map
    int imagedata[IMAGE_WIDTH][IMAGE_HEIGHT];        //Luminosity data of original image
    int cor,cor1,cor2,cor3,dif1,dif2,corfinal,corfinal2,id,bestdistance,dist,finaldist,finaldista,finaldistb;
    int bestcor1=0;
    int bestcor2=0;
    unsigned char byte=0;
    unsigned char colbyte=0;
    unsigned int bytepos=0;
    
    y=0;
    x=0;
    id=0;

    ReadPalette(e);
    
    //Reads all luminosity values
    for (j=0; j<=IMAGE_HEIGHT-1;j++)
    {
        for (i=0;i<=IMAGE_WIDTH-1;i++)
        {
            SDL_GetRGBA(SDL_getpixel(e->img, i, j), e->img->format, &r, &g, &b, &a);
            imagedata[i][j]=(r+g+b)/3;
        }
    }
    
    if (e->detaillevel <IMAGE_WIDTH-1)
    {
        for (j=1; j<=IMAGE_HEIGHT-1;j++)
        {
            for (i=1;i<=IMAGE_WIDTH-2;i++)
            {
                cor=imagedata[i-1][j];
                cor2=imagedata[i][j];
                cor3=imagedata[i+1][j];
                dif1=abs(cor-cor2);
                dif2=abs(cor2-cor3);
                if (dif1 > dif2)
                {
                    corfinal=dif1;
                }
                else
                {
                    corfinal=dif2;
                }
                cor=imagedata[i][j-1];
                cor3=imagedata[i][j+1];
                dif1=abs(cor-cor2);
                dif2=abs(cor2-cor3);
                if (dif1 > dif2)
                {
                    corfinal2=dif1;
                }
                else
                {
                    corfinal2=dif2;
                }
                corfinal=(corfinal+corfinal2)>>1; // Shr 1
                corfinal=corfinal;
                detail[i][j]=corfinal;
            }
        }
        
        for (i=0; i<=IMAGE_WIDTH-1; i++)
        {
            detail[i][0]=0;
            detail[i][IMAGE_HEIGHT-1]=0;
        }
        for (i=0; i<=IMAGE_HEIGHT-1; i++)
        {
            detail[0][i]=0;
            detail[IMAGE_WIDTH-1][i]=0;
        }
        
        for (j=0; j<=IMAGE_HEIGHT-1;j++)
        {
            for (i=0;i<=IMAGE_WIDTH-1;i++)
            {
                if (detail[i][j]<1)
                {
                    detail[i][j]=1;
                }
                detail[i][j]=(detail[i][j]/e->detaillevel)+1;
            }
        }
    }
    else
    {
        for (j=0; j<=IMAGE_HEIGHT-1;j++)
        {
            for (i=0;i<=IMAGE_WIDTH-1;i++)
            {
                detail[i][j]=1;
            }
        }
    }
    
    ///////////////////////
    while (y<IMAGE_HEIGHT)
    {
        bestdistance=99999999;
        for (i=0;i<=7;i++)
        {
            // Get the RGB values of 8 pixels of the original image
            SDL_GetRGBA(SDL_getpixel(e->img, ORG_X+x+i, ORG_Y+y), e->img->format, &r, &g, &b,&a);
            octetr[i]=r;
            octetg[i]=g;
            octetb[i]=b;
            octetdetail[i]=detail[x+i][y];
        }
        
        // Brute force starts. Programs tests all 15 x 15 MSX color combinations. For each pixel octet it'll have
        // to compare the original pixel colors with three different colors:
        // two MSX colors and a mixed RGB of both. If this RGB mixed is chosen it'll later be substituted by dithering.
        for (cor1=1;cor1<= e->Nb_colors;cor1++)
        {
            for (cor2=cor1;cor2<= e->Nb_colors;cor2++)
            {
                dist=0;
                
                // If KeyHit(1) Then End
                
                // First MSX color of the octet
                toner[0]=e->msxr[cor1];
                toneg[0]=e->msxg[cor1];
                toneb[0]=e->msxb[cor1];
                
                // Second MSX color of the octet
                toner[2]=e->msxr[cor2];
                toneg[2]=e->msxg[cor2];
                toneb[2]=e->msxb[cor2];
                
                
                // A mix of both MSX colors RGB values. Since MSX cannot mix colors,
                // later if this color is chosen it'll be substituted by a 2x2 dithering pattern.
                toner[1]=(e->msxr[cor1]+e->msxr[cor2])/2;
                toneg[1]=(e->msxg[cor1]+e->msxg[cor2])/2;
                toneb[1]=(e->msxb[cor1]+e->msxb[cor2])/2;
                
                // if colors are not too distant according to the tolerance parameter, octect will be dithered.
                if (calcdist2000(e->msxr[cor1],e->msxg[cor1],e->msxb[cor1],e->msxr[cor2],e->msxg[cor2],e->msxb[cor2]) <= e->tolerance)
                {
                    // dithered
                    for (i=0;i<=7;i++)
                    {
                        for (j=0;j<=2;j++)
                        {
                            distcolor[j]=(calcdist2000(toner[j],toneg[j],toneb[j],octetr[i],octetg[i],octetb[i]))*octetdetail[i];
                        }
                        //finaldist=distcolor(0):octetvalue(i)=0
                        finaldist=distcolor[0];
                        octetvalue[i]=0;
                        for (j=1;j<=2;j++)
                        {
                            if (distcolor[j]<finaldist)
                            {
                                //finaldist=distcolor(j):octetvalue(i)=j
                                finaldist=distcolor[j];
                                octetvalue[i]=j;
                            }
                        }
                        
                        dist=dist+finaldist;
                        if (dist > bestdistance)
                        {
                            break;
                        }
                    }
                }
                else
                {
                    // not dithered
                    for (i=0;i<=7;i++)
                    {
                        finaldista=(calcdist2000(toner[0],toneg[0],toneb[0],octetr[i],octetg[i],octetb[i]))*octetdetail[i];
                        finaldistb=(calcdist2000(toner[2],toneg[2],toneb[2],octetr[i],octetg[i],octetb[i]))*octetdetail[i];
                        
                        if (finaldista < finaldistb)
                        {
                            octetvalue[i]=0;
                            finaldist=finaldista;
                        }
                        else
                        {
                            octetvalue[i]=2;
                            finaldist=finaldistb;
                        }
                        dist=dist+finaldist;
                        if (dist > bestdistance)
                        {
                            break;
                        }
                    }
                }
                
                if (dist < bestdistance)
                {
                    //bestdistance=dist:bestcor1=cor1:bestcor2=cor2
                    bestdistance=dist;
                    bestcor1=cor1;
                    bestcor2=cor2;
                    for (i=0;i<=7;i++)
                    {
                        octetfinal[i]=octetvalue[i];
                    }
                }
                if (bestdistance==0)
                {
                    break;
                }
            }
            
            
            
            if (bestdistance==0)
            {
                break;
            }
        }
        byte=0;
        for (i=0;i<=7;i++)
        {
            switch (octetfinal[i])
            {
                case 0:
                    e->Rcol=e->msxr[bestcor1];
                    e->Gcol=e->msxg[bestcor1];
                    e->Bcol=e->msxb[bestcor1];
                    
                    break;
                case 1:
                    if (y % 2 == i % 2)
                    {
                        
                        e->Rcol=e->msxr[bestcor2];
                        e->Gcol=e->msxg[bestcor2];
                        e->Bcol=e->msxb[bestcor2];
                        byte =  byte+pow(2, 7-i);
                        
                    }
                    else
                    {
                        
                        e->Rcol=e->msxr[bestcor1];
                        e->Gcol=e->msxg[bestcor1];
                        e->Bcol=e->msxb[bestcor1];
                        
                    }
                    break;
                    
                case 2:
                    
                    e->Rcol=e->msxr[bestcor2];
                    e->Gcol=e->msxg[bestcor2];
                    e->Bcol=e->msxb[bestcor2];
                    byte =  byte+pow(2, 7-i);
                    break;
            }
            
            buf[0] = e->Bcol;
            buf[1] = e->Gcol;
            buf[2] = e->Rcol;
            buf[3] = 0xff;
            
            Final_color = (int *)buf;
            e->RenderImage[y][x+i]=*Final_color;
        }
        y=y+1;
        if (y % 8==0)
        {
            y=y-8;
            x=x+8;
        }
        if (x>IMAGE_WIDTH-1){
            x=0;
            y=y+8;
        }
        // Put Data in MSX Dump for saving file
        colbyte=bestcor2*16+bestcor1;
        msxdump[bytepos]=byte;
        msxdump[bytepos+6144]=colbyte;
        bytepos++;
    }
}

//**--------------------------------**
//    Saving Computed image
//     To MSX Screen2 Bin Format
//**--------------------------------**
//

void MSXoutput(t_img *e)
{
    int i,j;
    unsigned char MsxHeader[7]= {0xFE,0x00,0x00,0xFF,0x37,0x00,0x00};
    
    
    FILE *f = fopen(g_argv[2], "wb");
    if (f==NULL)
        ft_exit(e,1,"Writing output ile problem!");
    
    for (i=0; i<7; i++) {
        fputc(MsxHeader[i], f);
    }

    for (i=0;i<=6143;i++)
    {
        fputc(msxdump[i], f);
    }

    for (j=0;j<=2;j++)
    {
        for (i=0;i<=255;i++)
        {
            fputc((char)i, f);
        }
    }
    
    for (i=0;i<=1279;i++)
    {
        fputc(0, f);
    }

    for (i=0;i<=6143;i++)
    {
        fputc(msxdump[i+6144], f);
    }
    fclose(f);
}

//**--------------------------------**
//   Draw Computed Image on Screen
//
//**--------------------------------**
//

void DrawRenderImage(t_img *e,int Start_x, int Start_y)
{
    int x,y;

    for (y=0;y<=IMAGE_HEIGHT-1;y++)
    {
        for (x=0; x<=IMAGE_WIDTH-1; x++) {
            SDL_pixel_put_to_image(e, Start_x+x, Start_y+y, e->RenderImage[y][x]);
        }
    }
}

void DrawSlot(t_img *e,int Start_x, int Start_y)
{
    int x,y;
    
    for (y=0;y<=IMAGE_HEIGHT-1;y++)
    {
        for (x=0; x<=IMAGE_WIDTH-1; x++) {
            SDL_pixel_put_to_image(e, Start_x+x, Start_y+y, 0xFFFFFFFF);
        }
    }
    SDL_render(e);
}

char PrintDo(t_img *e)
{
    printf("\n-> Computing ... Using ");
    printf("%s",Palette_name[e->UsePalette]);
    return(1);
}


void Do_it (t_img *e)
{
    ImageProcess(e);
    
    DrawRenderImage(e, Slot_x[e->slot], Slot_y[e->slot]);
    

    SDL_render(e);
     printf("\n-> Interface : %d",e->interface);
     printf("\n-> Color Tolerance : %d",e->tolerance);
     printf("\n-> Detail Level : %d",e->detaillevel);
     printf("\n________________________");
    
    MSXoutput(e);
}
void Info(t_img *e)
{
    printf("\n "); printf(NAME); printf(VERSION);printf(DATE);
    printf("\n is converting 256 x 192 pixels BMP Images \n to a fixed color palette using CIEDE2000 Algorythm");
    printf("\n Original convesion Routine by Leandro Correia");
    printf("\n This tool was coded in C, with SDL2 Graphic Library\n by Eric Boez 2019 - BZ Prod Game Studio");
    printf("\n For MSX1 Screen2 mode with 16 colors palette support.");
    printf("\n This tool is provided as is, with no guaranty\n Under the Creative Commons CC BY_SA 4.0 license");
}
void Usage(t_img *e)
{
    char i;
    printf("\n GraphxConverter usage : \n graphxconv <Input file> <output file> [-i<n> -p<n> -t<n> -d<n> -c -h] \n");
    
    printf("\n<input file> : path and name of the BMP file to convert");
    printf("\n<output file> : path and name of the saved file");
    printf("\n-i<n> : if n=0, quit Graphic Interfac once conversion is done. (ex: -i0)");
    printf("\n-p<n> : Palette number to use (ex : -p3)");
    printf("\n-t<n> : Color Tolerance. <n> must be between 0 and 100 (ex: -t90)");
    printf("\n-d<n> : Details Level. <n> must be between 0 and 255 (ex: -d30)");
    printf("\n-c : Show Copyright Informations");
    printf("\n-h : the help text you are reading at the moment !");
    printf("\n----------------");
    printf("\nList of palettes:");
    for(i=0;i<=NB_PALETTE;i++)
    {
        printf("\n%d : %s",i,Palette_name[i]);
    }
    printf("\n----------------");
    printf("\nList of Keys in Graphic Interface:");
    printf("\n1 : Color tolerance -");
    printf("\n2 : Color tolerance +");
    printf("\n8 : Detail level -");
    printf("\n9 : Detail Level +");
    printf("\nP : use Next Palette");
    printf("\nS : New Slot image");
    printf("\nESC : Quit program");
    
    ft_exit(e,6,"");
}


//**--------------------------------**
//              Main
//
//**--------------------------------**
//

int main(int argc, char *argv[]) {
    
    char C_err=0;
    
    t_img *e = NULL;
    SDL_Event ev;
    
    printf("\n");
    printf(NAME);
    printf("\n");
    
    if (!(e = (t_img*)calloc(1,sizeof(t_img))))                             // Structure Init
        ft_exit(e,4,"Malloc error: ->e");
    
    init_mem(e);
        e->click_clock=clock()+(CLOCKS_PER_SEC/10);
    
    
    //Default parameters
    e->tolerance=100;
    e->detaillevel=32;
    e->UsePalette=0;
    e->slot=1;
    e->interface=1;
    e->click_clock=e->clock;

    
    g_argv = argv;
    ////////////////////// Parameters Checking

    if (argc<3)
    {
        printf("\n ERROR : Missing Input file and/or Output file name. \nType the command line with -h for help\n");
        C_err=1;
    }
    else
    {
        ++argv;
        --argc;
        ++argv;
        --argc;
    }

    while ((argc > 1) && (argv[1][0] == '-'))
    {
        
        switch (argv[1][1])
        {
            case 'i':
                e->interface=atoi(&argv[1][2]);
                break;
                
            case 'p':
                e->UsePalette=atoi(&argv[1][2]);
                if (e->UsePalette>NB_PALETTE)
                    e->UsePalette=NB_PALETTE;
                break;
                
            case 't':
                 e->tolerance=atoi(&argv[1][2]);
                 if (e->tolerance>TOLERANCE_MAX || e->tolerance<TOLERANCE_MIN)
                 {
                     printf("\n ERROR : Wrong Color Tolerance parameter\n");
                     Usage(e);
                 }
                break;
            case 'd':
                e->detaillevel=atoi(&argv[1][2]);
                if (e->detaillevel>DETAIL_MAX || e->detaillevel<DETAIL_MIN)
                {
                    printf("\n ERROR : Wrong Details Level parameter\n");
                    Usage(e);
                }
                break;
            case 'c':
                C_err=1;
                Info(e);
                ft_exit(e,0,"\nSee you !");
                break;
            case 'h':
                Usage(e);
                ft_exit(e,0,"");
                break;
            default:
                printf("\nWrong Argument: %s\n", argv[1]);
                Usage(e);
        }
        
        ++argv;
        --argc;
        if (C_err==1)
             ft_exit(e,0,"\n");
    }

    ////////////////////////////////////
    
    load_image(e);
    
    SDL_render(e);
    
    SDL_LockSurface(e->img);
    SDL_SetRenderTarget(e->ren, NULL);

    PrintDo(e);
    Do_it(e);
    
    if (e->interface==0)
        ft_exit(e,0,"\nSee you !");
    
    while (1) // infinite loop
    {
        SDL_render(e);
        e->clock=clock();
        SDL_PumpEvents();
        //if (ev.type==SDL_KEYDOWN)
            key_hook(e);
        
        if (SDL_PollEvent(&ev) )                        //  events in the queue
        {
            if(ev.window.event == SDL_WINDOWEVENT_CLOSE)
                ft_exit(e,0,"\nSee you !");
        }

    }
}
