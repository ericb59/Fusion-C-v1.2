#ifndef LABELBTN_H
#define LABELBTN_H

#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QImage>


class CLabelBtn :  public QLabel
{
	Q_OBJECT
public:
	CLabelBtn( QWidget *pParent, int w, int h );
	
	void put_Image( const QImage &image );
	QImage& get_Image();

	void SetPixel( int x, int y, uint color );

protected:
    QImage m_Image;
	void mousePressEvent( QMouseEvent* );  
	void mouseReleaseEvent( QMouseEvent* );
	void mouseDoubleClickEvent( QMouseEvent* );
	void mouseMoveEvent( QMouseEvent *pEvent );
	
signals:
	void onClickedLeft( int x, int y );
	void onClickedRight( int x, int y );
	void onDoubleClickedLeft( int x, int y );
	void onMouseMoveEvent( int x, int y );
	void onReleasedLeft( int x, int y );
	void onReleasedRight( int x, int y );
};

// Esta clase sirve para pintar con el boton pulsado...
class CLabelBtnMove : public CLabelBtn {
public:
    CLabelBtnMove( QWidget *pParent, int w, int h )
        : CLabelBtn( pParent, w, h ) {}
protected:
    void mouseMoveEvent( QMouseEvent *pEvent );
};

#endif
