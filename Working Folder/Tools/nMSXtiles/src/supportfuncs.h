#ifndef SUPPORTFUNCS_H
#define SUPPORTFUNCS_H

#include <QImage>
#include <QColor>

class CSupportFuncs
{
public:
    static const unsigned int CONST_COLORS_TABLE[16];
	static void SetPixelRect( QImage *pImage, int x, int y, unsigned int color, int w, int h );
	static void SetBorderImage( QImage *pImage, int x, int y, int w, int h, int color );

    static unsigned int Convert_8bit_to_3bit( unsigned int comp );
    static unsigned int Convert_3bit_to_8bit( unsigned int comp );
    static unsigned int Convert_8bit( unsigned int comp );
    static void Convert_color_MSX2( QColor& q_color );

};

#endif
