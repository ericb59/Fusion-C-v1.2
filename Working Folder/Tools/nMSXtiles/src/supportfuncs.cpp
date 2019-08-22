#include <supportfuncs.h>

const uint CSupportFuncs::CONST_COLORS_TABLE[16] =
{
/*
    0xff000000, 0xff000000, 0xff21C842, 0xff5EDC78,
    0xff5455ED, 0xff7D76FC, 0xffD4524D, 0xff42EBF5,
    0xffFC5554, 0xffFF7978, 0xffD4C154, 0xffE6CE80,
    0xff21B03B, 0xffC95BBA, 0xffCCCCCC, 0xffFFFFFF
*/
    0xFF000000, 0xFF000000, 0xFF21C842, 0xFF5EDC78,
    0xFF5455ED, 0xFF7D76FC, 0xFFD4524D, 0xFF42EBF5,
    0xFFFC5554, 0xFFFF7978, 0xFFD4C154, 0xFFE6CE80,
    0xFF21B03B, 0xFFC95BBA, 0xFFCCCCCC, 0xFFFFFFFF
};

void CSupportFuncs::SetPixelRect( QImage *pImage, int x, int y, unsigned int color, int w, int h )
{
	int r, s;
	QRgb *pPixel;
	
	
	for( s = y; s < y+h; s++ )
	{
		for( r = x, pPixel = ( (QRgb*)pImage->scanLine( s ) + x  ) ; r < x+w; r++, pPixel++ )
		{
			*pPixel = color;
		}
	}

}

void CSupportFuncs::SetBorderImage( QImage *pImage, int x, int y, int w, int h, int color )
{
	int i;
	

	for( i = x - 1; i < x + w + 1; i++ )
	{
		if( i < 0 || i >= pImage->width() || ( y - 1 ) < 0 || ( y + h ) >= pImage->height() ) continue;

        pImage->setPixel( i, y - 1, color );
        pImage->setPixel( i, y + h, color );
	}
	
	for( i = y -1; i < y + h+ 1; i++ )
	{
		if( ( x -1 ) < 0 || ( x - 1 ) >= pImage->width() || i < 0 || i >= pImage->height() ) continue;

        pImage->setPixel( x - 1, i, color );
        pImage->setPixel( x + w, i, color );
	}

	
}

unsigned int CSupportFuncs::Convert_8bit_to_3bit( unsigned int comp ) {
    if( comp <  21 ) return 0;
    if( comp <  60 ) return 1;
    if( comp <  99 ) return 2;
    if( comp < 134 ) return 3;
    if( comp < 169 ) return 4;
    if( comp < 203 ) return 5;
    if( comp < 237 ) return 6;
    return 7;
}

unsigned int CSupportFuncs::Convert_3bit_to_8bit( unsigned int comp ) {
    switch( comp ) {
        case 0:  return 0;
        case 1:  return 42;
        case 2:  return 80;
        case 3:  return 117;
        case 4:  return 151;
        case 5:  return 186;
        case 6:  return 220;
        default: return 255;
    }
}

unsigned int CSupportFuncs::Convert_8bit( unsigned int comp ) {
    if( comp <  21 ) return 0;
    if( comp <  60 ) return 42;
    if( comp <  99 ) return 80;
    if( comp < 134 ) return 117;
    if( comp < 169 ) return 151;
    if( comp < 203 ) return 186;
    if( comp < 237 ) return 220;
    return 255;
}

void CSupportFuncs::Convert_color_MSX2( QColor& q_color ) {
    q_color.setRed(   Convert_8bit( q_color.red()   ) );
    q_color.setGreen( Convert_8bit( q_color.green() ) );
    q_color.setBlue(  Convert_8bit( q_color.blue()  ) );
    q_color.setAlpha( 255 );
}
