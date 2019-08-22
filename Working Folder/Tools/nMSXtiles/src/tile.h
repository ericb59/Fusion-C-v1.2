#ifndef TILE_H
#define TILE_H

#include <QList>
#include <QPoint>

class CTile {
public:
    CTile();
    void Reset();

// Getters:
    // int x, y con valores de 0 a 7 como es lógico.
    // color de 0 a 15
    int GetForeColor( int y );          // m_ForeColors[y];
    int GetBgColor( int y );            // m_BgColors[y];
    // Ojo! color patrón [ 0 = m_BgColors[y] | 1 = m_ForeColors[y] ]
    int GetPixel( int x, int y );       // m_Pixels[x][y];
    int GetPixelColor( int x, int y );  // Retorna el color de 0 a 15 de un pixel.
    int GetRowPattern( int row );   // Retorna el patrón de bits de un fila de 0 a 7
    int GetRowColor( int row );     // Retorna los colores [tinta4bits|fondo4bits] de un fila de 0 a 7

// Setters:
    // int x, y con valores de 0 a 7 como es lógico.
    // color de 0 a 15
    void SetForeColor(  int y, int color );     // m_ForeColors[y] = color;
    void SetBackColor(  int y, int color );     // m_BgColors[y] = color;
    // Ojo! color patrón [ 0 = m_BgColors[y] | 1 = m_ForeColors[y] ]
    void SetPixel( int x, int y, int color );   // m_Pixels[x][y] = color;
    void SetForePixel( int x, int y );      // m_Pixels[x][y] = 1;
    void SetBackPixel( int x, int y );      // m_Pixels[x][y] = 0;
    void SetRowPattern( int row, quint8 value );
    void SetRowColor(  int row, quint8 color );

// Others:
	void Reorder();
    void ReorderLessFirst();
	void Rotate();
	void InvertRow( int y );
	bool IsBlack();
	void ShiftUp();
	void ShiftDown();
	void ShiftLeft();
	void ShiftRight();
	void FlipHorizontal();
	void FlipVertical(); 
    void ChangeColors( QList<QPoint> changes );

// Operators:
    CTile& operator = ( const CTile& copy );

private:
    // int x, y con valores de 0 a 7 como es lógico.
    int m_ForeColors[8];    // Color de tinta de 0 a 15
    int m_BgColors[8];      // Color de fondo de 0 a 15
    int m_Pixels[8][8];     // Patrón de pixels sólo [ 0 | 1 ]
};

#endif


// Al loro cambios:
//
//  int get_ForeColor( int y );
//  int GetForeColor( int y );     <--
//
//  int get_BgColor( int y );
//  int GetBgColor( int y );        <--
//
//  int get_Pixel( int x, int y );
//  int GetPixel( int x, int y );   <--
//
//  void put_Pixel( int x, int y, int color );
//  void SetPixel( int x, int y, int color );   <--
//
// void put_ForeColor( int y, int color );
// void SetForeColor(  int y, int color ); <--
//
//  void put_BgColor(   int y, int color );
//  void SetBackColor(  int y, int color ); <--
//
//  void put_Pixel( int x, int y, int color );
//  void SetPixel( int x, int y, int color );   <--

