#include "cscreenw.h"
#include "ui_cscreenw.h"
#include "mainwindow.h"
#include "changecolors.h"
#include "dlgbankrange.h"
#include "supportfuncs.h"
#include "changepalette.h"
#include <QDebug>
#include <QFile>
#include <QPixmap>
#include <QString>
#include <QMessageBox>

// extern int PletterMain( int dataSize, unsigned char *pData, QString destfilename );

extern bool pletter(
    unsigned char* pData,
    size_t         dataSize,
    const QString& destfile,
    bool           save_length
);

const int CScreenW::PAL_ALL = 100;

void CScreenW::closeEvent( QCloseEvent *pEvent ) {
    emit signal_close();
    pEvent->accept();
}

void CScreenW::keyPressEvent(QKeyEvent *e) {
    if(e->key() != Qt::Key_Escape){
        QDialog::keyPressEvent(e);
    }
}

CScreenW::CScreenW( QWidget *parent ) : QDialog( parent ), ui( new Ui::CScreenW ) {
    ui->setupUi(this);
}

CScreenW::~CScreenW() {
    delete ui;
}

void CScreenW::Initialize() {
	int i;
	int x, y;
	QPalette pal;
	QImage image;

// ------------------------------------------------------------------------------------------------
// Inicia la paleta de Colores:
    for( int i = 0; i < 16; i++ ) COLORS_TABLE_SCREEN[i] = CSupportFuncs::CONST_COLORS_TABLE[i];
    setFixedWidth( 782 );
    setFixedHeight( 670 ); // 722 );
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Inicia el Screen (m_pFrmScreen):
    m_pLblScreen = new CLabelBtn( ui->m_pFrmScreen, 32*16, 24*16 );
    m_pLblScreen->setGeometry( 3, 3, 32*16, 24*16 );
	m_pLblScreen->setMouseTracking( true );
	m_pLblScreen->show();

    m_pLblSelection = new CLabelBtn( ui->m_pFrmScreen, 0, 0 );
    m_pLblSelection->setGeometry( 0, 0, 0, 0 );
	m_pLblSelection->setFrameShape( QFrame::Box );
	m_pLblSelection->setFrameShadow( QFrame::Raised );
	pal = m_pLblSelection->palette();
	pal.setColor( QPalette::Dark, Qt::green );
	m_pLblSelection->setPalette( pal );

    for( x = 0; x < 32; x++ ) {
        m_pColsLabels[x] = new QLabel( QString( "%1" ).arg( x ), this );
        m_pColsLabels[x]->setGeometry( 16*x+26, 5, 16, 16 );
        m_pColsLabels[x]->setAlignment( Qt::AlignCenter );
        m_pColsLabels[x]->setFrameShape(QFrame::Box);
        m_pColsLabels[x]->setFrameShadow(QFrame::Raised);
    }

    for( x = 0; x < 24; x++ ) {
        m_pRowsLabels[x] = new QLabel( QString( "%1" ).arg( x%8 ), this );
        m_pRowsLabels[x]->setGeometry( 6, 16*x+26, 16, 16 );
        m_pRowsLabels[x]->setAlignment( Qt::AlignCenter );
        m_pRowsLabels[x]->setFrameShape(QFrame::Box);
        m_pRowsLabels[x]->setFrameShadow(QFrame::Raised);
    }

	connect( m_pLblScreen, SIGNAL( onClickedLeft( int, int ) ), this, SLOT( OnScreenClickLeft( int, int ) ) );
	connect( m_pLblScreen, SIGNAL( onReleasedLeft( int, int ) ), this, SLOT( OnScreenReleaseLeft( int, int ) ) );
	connect( m_pLblScreen, SIGNAL( onReleasedRight( int, int ) ), this, SLOT( OnScreenReleaseRight( int, int ) ) );
	connect( m_pLblScreen, SIGNAL( onMouseMoveEvent( int, int ) ), this, SLOT( OnScreenMouseMoveEvent( int, int ) ) );
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Inicia el MultiScreen (m_pFrmMultiScreen) :
    m_MapScreenX = 0;
    m_MapScreenY = 0;
    ui->m_pGrMapX->setValue( 0 );
    ui->m_pGrMapY->setValue( 0 );
    // Añadido para poner tope a MultiScreen
    ui->m_pGrMapX->setMaximum( 0 );
    ui->m_pGrMapY->setMaximum( 0 );
    ui->m_pGrOneBank->setChecked( false );
    connect( ui->m_pGrMapX,  SIGNAL( valueChanged(int)), this, SLOT(OnMapXChanged(int)));
    connect( ui->m_pGrMapY,  SIGNAL( valueChanged(int)), this, SLOT(OnMapYChanged(int)));
    connect( ui->m_pGrNextX, SIGNAL( clicked() ), this, SLOT( OnMapNextX() ) );
    connect( ui->m_pGrNextY, SIGNAL( clicked() ), this, SLOT( OnMapNextY() ) );
    connect( ui->m_pGrPrevX, SIGNAL( clicked() ), this, SLOT( OnMapPrevX() ) );
    connect( ui->m_pGrPrevY, SIGNAL( clicked() ), this, SLOT( OnMapPrevY() ) );
    connect( ui->m_pGrMapWidth,  SIGNAL( valueChanged( int ) ), this, SLOT( OnMapWidthChanged( int)));
    connect( ui->m_pGrMapHeight, SIGNAL( valueChanged( int ) ), this, SLOT( OnMapHeightChanged(int)));
    connect( ui->m_pGrOneBank,   SIGNAL( clicked() ), this, SLOT( OnOneBank() ) );
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Inicia los Bancos:
    m_pTabBank = new QTabWidget( this );
    // m_pTabBank->setGeometry( QRect( 5, 485, 596, 184 ) );
    m_pTabBank->setGeometry( QRect( 5, 475, 596, 184 ) );
    for( i = 0; i < 3; i++ ) {
        QWidget *pTab = new QWidget();
		m_pTabBank->addTab( pTab, QString());
		m_pTabBank->setTabText( m_pTabBank->indexOf( pTab), QString( "Bank %1" ).arg( i ) );
		m_pFrmBank[i] = new QFrame( pTab );
		m_pFrmBank[i]->setGeometry( QRect( 5, 10, 591, 194) );
		m_pFrmBank[i]->setFrameShape( QFrame::StyledPanel );
		m_pFrmBank[i]->setFrameShadow( QFrame::Raised );
		m_pLblBank[i] = new CLabelBtn( m_pFrmBank[i], 32*18, 8*18 );
		m_pLblBank[i]->setGeometry( 0, 0, m_pFrmBank[i]->width(), m_pFrmBank[i]->height() );
		m_pLblBank[i]->show();
	}
	
	connect( m_pTabBank, SIGNAL( currentChanged( int ) ), this, SLOT( OnBankChanged( int ) ) );

	connect( m_pLblBank[0], SIGNAL( onClickedLeft( int, int ) ), this, SLOT( OnBank0Click( int, int ) ) );
	connect( m_pLblBank[1], SIGNAL( onClickedLeft( int, int ) ), this, SLOT( OnBank1Click( int, int ) ) );
	connect( m_pLblBank[2], SIGNAL( onClickedLeft( int, int ) ), this, SLOT( OnBank2Click( int, int ) ) );

    connect( m_pLblBank[0], SIGNAL( onClickedRight( int, int ) ), this, SLOT( OnBank0ClickRight( int, int ) ) );
    connect( m_pLblBank[1], SIGNAL( onClickedRight( int, int ) ), this, SLOT( OnBank1ClickRight( int, int ) ) );
    connect( m_pLblBank[2], SIGNAL( onClickedRight( int, int ) ), this, SLOT( OnBank2ClickRight( int, int ) ) );

	m_pLblBank[0]->setMouseTracking( true );
	m_pLblBank[1]->setMouseTracking( true );
	m_pLblBank[2]->setMouseTracking( true );
/*
	connect( m_pLblBank[0], SIGNAL( onReleasedLeft( int, int ) ), this, SLOT( OnBankReleaseLeft( int, int ) ) );
	connect( m_pLblBank[1], SIGNAL( onReleasedLeft( int, int ) ), this, SLOT( OnBankReleaseLeft( int, int ) ) );
	connect( m_pLblBank[2], SIGNAL( onReleasedLeft( int, int ) ), this, SLOT( OnBankReleaseLeft( int, int ) ) );
*/
	connect( m_pLblBank[0], SIGNAL( onDoubleClickedLeft( int, int ) ), this, SLOT( OnBank0DoubleClick( int, int ) ) );
	connect( m_pLblBank[1], SIGNAL( onDoubleClickedLeft( int, int ) ), this, SLOT( OnBank1DoubleClick( int, int ) ) );
	connect( m_pLblBank[2], SIGNAL( onDoubleClickedLeft( int, int ) ), this, SLOT( OnBank2DoubleClick( int, int ) ) );
	
    m_pTabBank->setTabEnabled( 0, true );
    m_pTabBank->setTabEnabled( 1, true );
    m_pTabBank->setTabEnabled( 2, true );

// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// m_pFrmDesigner       Editor de Baldosas
//   m_pFrmColors       Donde se seleccionan los colores de la paleta
//   m_pFrmPalette      Color de fondo y tinta para cada línea de la baldosa
//   m_pFrmTile         La baldosa donde se pinta

    // -----------------------------------------------------------------
    // m_pFrmPalette      Color de fondo y tinta para cada línea de la baldosa
    m_pLblPalette = new CLabelBtn( ui->m_pFrmPalette, 40, 96 );
    m_pLblPalette->setGeometry( 0, 0, ui->m_pFrmPalette->width(), ui->m_pFrmPalette->height() );
	connect( m_pLblPalette, SIGNAL( onClickedLeft( int, int ) ), this, SLOT( OnPaletteClick( int, int ) ) );

    m_pLblPalBack = new CLabelBtn( ui->m_pFrmDesigner, 16, 10 );
    m_pLblPalBack->setGeometry( ui->m_pLblPalBackC->x(), ui->m_pLblPalBackC->y() + 25, 16, 10 );
	
    m_pLblPalFore = new CLabelBtn( ui->m_pFrmDesigner, 16, 10 );
    m_pLblPalFore->setGeometry( ui->m_pLblPalForeC->x(), ui->m_pLblPalForeC->y() + 25, 16, 10 );

	connect( m_pLblPalBack, SIGNAL( onClickedLeft( int, int ) ), this, SLOT( OnPalBackClick( int, int ) ) );
	connect( m_pLblPalFore, SIGNAL( onClickedLeft( int, int ) ), this, SLOT( OnPalForeClick( int, int ) ) );

    // -----------------------------------------------------------------
    // m_pFrmTile         La baldosa donde se pinta
    //  m_pLblTile = new CLabelBtn( ui->m_pFrmTile, 96, 96 );
    m_pLblTile = new CLabelBtnMove( ui->m_pFrmTile, 96, 96 ); //  Añadido para dibujar arrastrando...
    m_pLblTile->setGeometry( 2, 2, ui->m_pFrmTile->width(), ui->m_pFrmTile->height() );
	
	connect( m_pLblTile, SIGNAL( onClickedLeft( int, int ) ), this, SLOT( OnTileClickLeft( int, int ) ) );
	connect( m_pLblTile, SIGNAL( onClickedRight( int, int ) ), this, SLOT( OnTileClickRight( int, int ) ) );

    // -----------------------------------------------------------------
    // m_pFrmColors       Donde se seleccionan los colores de la paleta
    m_pLblColors = new CLabelBtn( ui->m_pFrmColors, 96, 12 );
    m_pLblColors->setGeometry( 3, 3, 12*16, 16 );
	
    // Pone los colores de la paleta a m_pLblColors
	image = QImage( m_pLblColors->width(), m_pLblColors->height(), QImage::Format_RGB32 );
    for( x = 0; x < 16; x++ ) CSupportFuncs::SetPixelRect( &image, x*12, 0, COLORS_TABLE_SCREEN[x], 12, 16 );
	m_pLblColors->put_Image( image );
	
	connect( m_pLblColors, SIGNAL( onClickedLeft( int, int ) ), this, SLOT( OnColorsClick( int, int ) ) );
    connect( m_pLblColors, SIGNAL( onClickedRight( int, int ) ), this, SLOT( OnColorsChange( int, int ) ) );

    connect( ui->m_pBtnFromBank, SIGNAL( clicked() ), this, SLOT( OnFromBank() ) );
    connect( ui->m_pBtnToBank, SIGNAL( clicked() ), this, SLOT( OnToBank() ) );

    connect( ui->m_pRadBack, SIGNAL( clicked() ), this, SLOT( OnRadBack() ) );
    connect( ui->m_pRadFore, SIGNAL( clicked() ), this, SLOT( OnRadFore() ) );
	
    connect( ui->m_pGrTileClear, SIGNAL( clicked() ), this, SLOT( OnTileClear() ) );
    connect( ui->m_pGrTileReorder, SIGNAL( clicked() ), this, SLOT( OnTileReorder() ) );
    connect( ui->m_pGrTileShiftDown, SIGNAL( clicked() ), this, SLOT( OnTileShiftDown() ) );
    connect( ui->m_pGrTileShiftUp, SIGNAL( clicked() ), this, SLOT( OnTileShiftUp() ) );
    connect( ui->m_pGrTileShiftLeft, SIGNAL( clicked() ), this, SLOT( OnTileShiftLeft() ) );
    connect( ui->m_pGrTileShiftRight, SIGNAL( clicked() ), this, SLOT( OnTileShiftRight() ) );
    connect( ui->m_pGrTileFlipHorizontal, SIGNAL( clicked() ), this, SLOT( OnTileFlipHorizontal() ) );
    connect( ui->m_pGrTileFlipVertical, SIGNAL( clicked() ), this, SLOT( OnTileFlipVertical() ) );
    connect( ui->m_pGrTileInvertColors, SIGNAL( clicked() ), this, SLOT( OnTileInvertColors() ) );
    connect( ui->m_pGrTileRotate, SIGNAL( clicked() ), this, SLOT( OnTileRotate() ) );

    ui->m_pRadBack->setChecked( false );
    ui->m_pRadFore->setChecked( true );
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// m_pFrmColorsOneBank
    connect( ui->m_pChangeColors, SIGNAL( clicked() ), this, SLOT( OnChangeColors()));
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Tiles Block of (m_pFrmTilesBlocksOf)
    m_LastTilesBlockW = ui->m_pGrTilesBlockW->text().toLongLong();
    m_LastTilesBlockH = ui->m_pGrTilesBlockH->text().toLongLong();
    connect( ui->m_pGrTilesBlockW, SIGNAL( valueChanged( int)), this, SLOT( OnTilesBlockChanged( int )));
    connect( ui->m_pGrTilesBlockH, SIGNAL( valueChanged( int)), this, SLOT( OnTilesBlockChanged( int )));
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// m_pFrmBackgroundTiles
    ui->m_pFrmBackgroundTiles->setVisible( true ); //TODO -- 1
    for( i = 0; i < 256; i++ ) {
        ui->m_pGrBackgroundTiles0->addItem( QString( "%1" ).arg( i ) );
        ui->m_pGrBackgroundTiles1->addItem( QString( "%1" ).arg( i ) );
        ui->m_pGrBackgroundTiles2->addItem( QString( "%1" ).arg( i ) );
	}
// ------------------------------------------------------------------------------------------------

/*
	m_pLblBankTile = new CLabelBtn( this, 16, 16 );
	m_pLblBankTile->setGeometry( 0, 0, 16, 16 );
	m_pLblBankTile->setPixmap( QPixmap( 16, 16 ) );
//	m_pLblBankTile->setMouseTracking( true );
    m_pLblBankTile->setVisible( false );
//  m_pLblBankTile->show();
*/
/*
	connect( m_pLblBankTile, SIGNAL( onMouseMoveEvent( int, int ) ), this, SLOT( OnBankTileMouseMoveEvent( int, int ) ) );
    connect( m_pLblBankTile, SIGNAL( onReleasedLeft( int, int ) ), this, SLOT(   OnBankTileReleaseLeft( int, int ) ) );
    connect( m_pLblBankTile, SIGNAL( onClickedLeft( int, int ) ), this, SLOT( OnBankTileClickLeft( int, int ) ) );
    connect( m_pLblBankTile, SIGNAL( onDoubleClickedLeft( int, int ) ), this, SLOT( OnBankTileDoubleClickLeft( int, int ) ) );
    m_LastBankTilePos = QPoint( -1, -1 );
*/
//	m_DragDrop = false;


    m_LastBank = m_LastBankX = m_LastBankY = 0;

	m_Selecting = false;
	m_SelectionRect = QRect( 0, 0, 0, 0 );

    m_SelColor = QPoint( 1, 0 );
	m_LastClickPos = QPoint( -1, -1 );
    m_LastGridValue = QPoint( 0, 0 );

    // Esto es el cursor:
    m_CursorImage = QImage( 16, 16, QImage::Format_RGB32 ); //TODO - 1 No membre
    CSupportFuncs::SetPixelRect( &m_CursorImage, 0, 0, COLORS_TABLE_SCREEN[15], 16, 16 );
    m_pLblCursor[0] = new CLabelBtn( ui->m_pFrmScreen, 15, 1 );
    m_pLblCursor[0]->setGeometry( 0, 0, 0, 0 );
    m_pLblCursor[0]->put_Image( m_CursorImage );

    m_pLblCursor[1] = new CLabelBtn( ui->m_pFrmScreen, 1, 15 );
    m_pLblCursor[1]->setGeometry( 0, 0, 0, 0 );
    m_pLblCursor[1]->put_Image( m_CursorImage );

    m_pLblCursor[2] = new CLabelBtn( ui->m_pFrmScreen, 15, 11 );
    m_pLblCursor[2]->setGeometry( 0, 0, 0, 0 );
    m_pLblCursor[2]->put_Image( m_CursorImage );

    m_pLblCursor[3] = new CLabelBtn( ui->m_pFrmScreen, 1, 15 );
    m_pLblCursor[3]->setGeometry( 0, 0, 0, 0 );
    m_pLblCursor[3]->put_Image( m_CursorImage );

    // Inicia las Habitaciones:
    for( int j = 0; j < 64; j++ )
        for( int k = 0; k < 64; k++ )
            for( x = 0; x < 256*3; x++ )
                m_Screen[j][k][x] = 0;

    // Inicia los bancos:
    for( y = 0; y < 3; y++ )
        for( x = 0; x < 256; x++ )
            m_TilesBank[x][y] = CTile();


    setMouseTracking( true );
    SetTileDesign(); // Añadido para actualizar el Diseñador de Baldosas

}

// -------------------------------------------------
// SCREEN:
// -------------------------------------------------
void CScreenW::NewScreen() {
    int i;
    for( i = 0; i < 32*24; i++ )
        m_Screen[m_MapScreenX][m_MapScreenY][i] = 0;
    UpdateScreen();
}

bool CScreenW::LoadScreen( QString fileName ) {
    int i;
    QString line;
    QFile hFile;
    int x, y;
    hFile.setFileName( fileName );
    hFile.open( QIODevice::ReadOnly );
    QTextStream str( &hFile );
    if( hFile.atEnd() ) {
        hFile.close();
        return false;
    }
    while( true ) {
        line = str.readLine();
        if( line.isNull() ) break;
        // Este caso no debería darse... (Estará por compatibilidad)
        if( line == "SCREEN" ) {
            x = 0;
            y = 0;
        } else if( line.indexOf( "SCREEN-" ) >= 0 ) {
            x = line.split( "-" ).at( 1 ).toInt();
            y = line.split( "-" ).at( 2 ).toInt();
        } else break;

        for( i = 0; i < 32*24; i++ ) {
            line = str.readLine();
            m_Screen[x][y][i] = line.toInt();
        }
    }
    if( !line.isNull() && line == "MAP WIDTH-HEIGHT" ) {
        ui->m_pGrMapWidth->setValue( str.readLine().toInt() );
        ui->m_pGrMapHeight->setValue( str.readLine().toInt() );
        ui->m_pGrMapX->setMaximum( ui->m_pGrMapWidth->value() -  1 );
        ui->m_pGrMapY->setMaximum( ui->m_pGrMapHeight->value() - 1 );
    } else {
        ui->m_pGrMapWidth->setValue( 1 );
        ui->m_pGrMapHeight->setValue( 1 );
        ui->m_pGrMapX->setMaximum(0);
        ui->m_pGrMapY->setMaximum( 0 );
    }
    hFile.close();
    UpdateScreen();
    return true;
}

bool CScreenW::SaveScreen( QString fileName ) {
    int i;
    QFile hFile;
    hFile.setFileName( fileName );
    hFile.open( QIODevice::WriteOnly );
    QTextStream str( &hFile );
    for( int y = 0; y < ui->m_pGrMapHeight->value(); y++ ) {
        for( int x = 0; x < ui->m_pGrMapWidth->value(); x++ ) {
            str << "SCREEN-" << x << "-" << y << endl;
            for( i = 0; i < 32*24; i++ )
                str << m_Screen[x][y][i] << endl;
        }
    }
    str << "MAP WIDTH-HEIGHT" << endl;
    str << ui->m_pGrMapWidth->value() << endl;
    str << ui->m_pGrMapHeight->value();
    hFile.flush();
    hFile.close();
    return true;
}

bool CScreenW::ExportScreenData( QString fileName, bool hexa, int initX, int initY, int w, int h ) {
    int addX, addY;
    int x, y;
    int wi, he;
    QFile hFile;
    QString tmp;
    int mapX, mapY;
    hFile.setFileName( fileName );
    hFile.open( QIODevice::WriteOnly | QIODevice::Text );
    QTextStream str( &hFile );
    addX = 1; // ui->m_pGrTilesBlockW->text().toLongLong(); // ???
    addY = 1; // ui->m_pGrTilesBlockH->text().toLongLong(); // ???
    wi = initX + w;
    if( wi > 32 ) wi = 32;
    he = initY + h;
    if( he > 24 ) he = 24;
    for( mapY = 0; mapY < ui->m_pGrMapHeight->value(); mapY++ ) {
        for( mapX = 0; mapX < ui->m_pGrMapWidth->value(); mapX++ ) {
        // Cambiado:
            str << endl << QString( "SCREEN_%1_%2:" ).arg( mapX ).arg( mapY ) << endl;
            str << "\tDB ";
            for( y = initY; y < he; ) {
                for( x = initX; x < wi; ) {
                    if( hexa ) str << QString( "0x%1" ).arg( m_Screen[mapX][mapY][y*32+x], 0, 16 ).rightJustified( 4, ' ' );
                    else str << QString( "%1" ).arg( m_Screen[mapX][mapY][y*32+x] ).rightJustified( 3, ' ' );
                    if( (y*32+x+1)%32 == 0 || x == ( wi - 1 ) ) {
                        str << endl;
                        if( (y*32+x+1) != 256*3 && !( y == (he-1) && x == (wi-1) )) str << "\tDB ";
                    } else if( !( x == (wi - 1) && y == (he - 1) ) ) str <<",";
                    x += addX;
                }
                y += addY;
            }
        }
    }
    str << endl;
    hFile.flush();
    hFile.close();
    return true;
}

bool CScreenW::ExportScreenBin( QString fileName, int initX, int initY, int w, int h ) {
    int x, y;
    int addX, addY;
    int wi, he;
    int mapX, mapY;
    addX = 1; // ui->m_pGrTilesBlockW->text().toLongLong(); // ???
    addY = 1; // ui->m_pGrTilesBlockH->text().toLongLong(); // ???
    wi = initX + w;
    if( wi > 32 ) wi = 32;
    he = initY + h;
    if( he > 24 ) he = 24;
    for( mapY = 0; mapY < ui->m_pGrMapHeight->value(); mapY++ ) {
        for( mapX = 0; mapX < ui->m_pGrMapWidth->value(); mapX++ ) {
            QFile hFile;
            hFile.setFileName( QString( "%1_%2_%3" ).arg( fileName ).arg( mapX ).arg( mapY ) );
            hFile.open( QIODevice::WriteOnly );
            QDataStream str( &hFile );
            for( y = initY; y < he; ) {
                for( x = initX; x < wi; ) {
                    str << (unsigned char)m_Screen[mapX][mapY][y*32+x];
                    x+= addX;
                }
                y+= addY;
            }
            hFile.flush();
            hFile.close();
         }
    }
    return true;
}

bool CScreenW::ExportScreenBinScroll( QString fileName, int initX, int initY, int w, int h ) {
    int s;
    int x, y;
    int wi, he;
    int mapX, mapY;
    QFile hFile;
//    unsigned char buffer[256*3*64*64];
//    addX = m_pGrTilesBlockW->text().toLongLong(); //TODO - 1 No tiene en cuenta m_pGrTilesBlockW / H
//    addY = m_pGrTilesBlockH->text().toLongLong();
    hFile.setFileName( fileName );
    hFile.open( QIODevice::WriteOnly );
    QDataStream str( &hFile );
    s = 0;
    wi = initX + w;
    if( wi > 32 ) wi = 32;
    he = initY + h;
    if( he > 24 ) he = 24;
    y = initY;
    x = initX;
    mapY = 0;
    mapX = 0;
    for( int row = 0; row < h*ui->m_pGrMapHeight->value(); row++ ) {
        mapX = 0;
        for( int col = 0; col < w*ui->m_pGrMapWidth->value(); col++ ) {
            str<<(unsigned char)m_Screen[mapX][mapY][y*32+x];
            x++;
            s++;
            if( x == wi ) {
                x = initX;
                mapX++;
                if( mapX > ui->m_pGrMapWidth->value() ) {
                    Q_ASSERT(  col == (w*ui->m_pGrMapWidth->value() ) );
                }
            }
        }
        y++;
        if( y == he ) {
            y = initY;
            mapY++;
            if( mapY > ui->m_pGrMapHeight->value() ) {
                Q_ASSERT( row == (h*ui->m_pGrMapHeight->value() ) );
            }
        }
    }
    hFile.flush();
    hFile.close();
    return true; //TODO - 2
}

bool CScreenW::ExportScreenBinPletter( QString fileName, int initX, int initY, int w, int h ) {
    int s;
    int x, y;
    int addX, addY;
    int wi, he;
    int mapX, mapY;
    // unsigned char buffer[256*3*12*12];
    addX = 1; // ui->m_pGrTilesBlockW->text().toLongLong();     // ???
    addY = 1; // ui->m_pGrTilesBlockH->text().toLongLong();     // ???
    wi = initX + w;
    if( wi > 32 ) wi = 32;
    he = initY + h;
    if( he > 24 ) he = 24;
    for( mapY = 0; mapY < ui->m_pGrMapHeight->value(); mapY++ ) {
        for( mapX = 0; mapX < ui->m_pGrMapWidth->value(); mapX++ ) {
            s = 0;  // Añadido
            for( y = initY; y < he; ) {
                for( x = initX; x < wi; ) {
                    m_Buffer[s] = (unsigned char)m_Screen[mapX][mapY][y*32+x];
                    x+= addX;
                    s++;
                }
                y+= addY;
            }
            pletter( m_Buffer, s, QString( "%1_%2_%3" ).arg( fileName ).arg( mapX ).arg( mapY ), false );
        }
    }
    return true; // TODO - 2
}

bool CScreenW::ExportScreenBinPletterScroll( QString fileName, int initX, int initY, int w, int h ) {
    int s;
    int x, y;
    int wi, he;
    int mapX, mapY;
//    addX = m_pGrTilesBlockW->text().toLongLong(); //TODO - 1 No tiene en cuenta m_pGrTilesBlockW / H
//    addY = m_pGrTilesBlockH->text().toLongLong();
    s = 0;
    wi = initX + w;
    if( wi > 32 ) wi = 32;
    he = initY + h;
    if( he > 24 ) he = 24;
    y = initY;
    x = initX;
    mapY = 0;
    mapX = 0;
    for( int row = 0; row < h*ui->m_pGrMapHeight->value(); row++ ) {
        mapX = 0;
        for( int col = 0; col < w*ui->m_pGrMapWidth->value(); col++ ) {
            m_Buffer[s] = (unsigned char) m_Screen[mapX][mapY][y*32+x];
            x++;
            s++;
            if( x == wi ) {
                x = initX;
                mapX++;
                if( mapX > ui->m_pGrMapWidth->value() ) {
                    //Q_ASSERT(  col == (w*m_pGrMapWidth->value() ) );
                }
            }
        }
        y++;
        if( y == he ) {
            y = initY;
            mapY++;
            if( mapY > ui->m_pGrMapHeight->value() ) {
                //Q_ASSERT( row == (h*m_pGrMapHeight->value() ) );
            }
        }
    }
    // PletterMain( s, m_Buffer, fileName );
    pletter( m_Buffer, s, fileName, false );
    return true; //TODO - 2
}

bool CScreenW::ImportSC2( QString fileName ) {
    // Para saber si es un banco u tres...
    // ui->m_pGrOneBank->isChecked()
    QMessageBox msgBox;
    msgBox.setIcon( QMessageBox::Warning );
    msgBox.setDefaultButton( QMessageBox::Ok );
    msgBox.setWindowTitle( "SC2 file:" );
    QFile file( fileName );
    if( !file.open( QIODevice::ReadOnly ) ) {
        msgBox.setText( "The file can´t open." );
        msgBox.exec();
        return false;
    }
    if( file.size() < ( 0x37FF + 0x7 ) ) {
        msgBox.setText( "Error of size." );
        msgBox.exec();
        file.close();
        return false;
    }
    QDataStream in( &file );
    quint8 byte = 0;
    in >> byte;
    if( byte != quint8(0xFE) ) {
        msgBox.setText( "Error in header." );
        msgBox.exec();
        file.close();
        return false;
    }
    quint16 initAddress, endAddress, runAddress;
    in >> byte; initAddress  = byte;
    in >> byte; initAddress |= ( quint16( byte ) << 8 );
    in >> byte; endAddress   = byte;
    in >> byte; endAddress  |= ( quint16( byte ) << 8 );
    in >> byte; runAddress   = byte;
    in >> byte; runAddress  |= ( quint16( byte ) << 8 );
    if( ( initAddress != 0 ) && ( endAddress != 0x37ff ) && ( runAddress != 0 ) ) {
        QString msg;
        msg = "The VRAM addresses are not adequate:\n\n";
        msg += QString( "Init address:\t%1\n" ).arg( ushort(initAddress), 4, 16, QChar('0') );
        msg += QString( "End address:\t%1\n" ).arg( ushort( endAddress), 4, 16, QChar('0') );
        msg += QString( "Run address:\t%1\n" ).arg( ushort( runAddress), 4, 16, QChar('0') );
        msg += "\nStandard addresses will be used.";
        msgBox.setText( msg );
        msgBox.exec();
    }
    // 0000H - 07FFH	-->	Pattern generator table 1 -> 256 * 8 = 2048
    // 0800H - 0FFFH	-->	Pattern generator table 2 -> 256 * 8 = 2048
    // 1000H - 17FFH	-->	Pattern generator table 3 -> 256 * 8 = 2048
    // Cargando la tabla de patrones
    for( int bank = 0; bank <   3; ++bank ) {
    for( int tile = 0; tile < 256; ++tile ) {
    for( int row  = 0; row  <   8; ++row  ) {
        in >> byte;
        m_TilesBank[tile][bank].SetRowPattern( row, byte );
    }
    }
    }
    // 1800H - 18FFH	-->	Pattern name table 1 -> 256
    // 1900H - 19FFH	-->	Pattern name table 2 -> 256
    // 1A00H - 1AFFH	-->	Pattern name table 3 -> 256
    // Cargando los nombres;
    for( int nam  = 0; nam < 256*3; ++nam  ) {
        in >> byte;
        m_Screen[m_MapScreenX][m_MapScreenY][nam] = byte;
    }
    // 1B00H - 1B7FH	-->	Sprite attribute table 128 bytes 0x80
    for( int i = 0x0; i < 0x80; ++i  ) in >> byte;
    // 1B80H - 1BAFH	-->	Palette table 48 bytes 0x30
    for( int i = 0x0; i < 0x30; ++i  ) in >> byte;
    // 1BB0H - 1FFFH	--> ???         1104 bytes 0x450
    for( int i = 0x0; i < 0x450; ++i  ) in >> byte;
    // 2000H - 27FFH	-->	Colour table 1 -> 256 * 8 = 2048
    // 2800H - 2FFFH	-->	Colour table 2 -> 256 * 8 = 2048
    // 3000H - 37FFH	-->	Colour table 3 -> 256 * 8 = 2048
    // de 2000H a 37FFH = 1800H -> 6144
    // Cargando la tabla de colores
    for( int bank = 0; bank <   3; ++bank ) {
    for( int tile = 0; tile < 256; ++tile ) {
    for( int row  = 0; row  <   8; ++row  ) {
        in >> byte;
        m_TilesBank[tile][bank].SetRowColor( row, byte );
    }
    }
    }
    if( ui->m_pGrOneBank->isChecked() ) {
        for( int tile = 0; tile < 256; ++tile ) {
            m_TilesBank[tile][1] = m_TilesBank[tile][0];
            m_TilesBank[tile][2] = m_TilesBank[tile][0];
        }
    }
    file.close();
    InitBanks();
    UpdateScreen();
    return true;
}

bool CScreenW::ExportSC2( QString fileName ) {
    // Para saber si es un banco u tres...
    // ui->m_pGrOneBank->isChecked()
    QMessageBox msgBox;
    msgBox.setIcon( QMessageBox::Warning );
    msgBox.setDefaultButton( QMessageBox::Ok );
    msgBox.setWindowTitle( "SC2 file:" );
    QFile file( fileName );
    if( !file.open( QIODevice::WriteOnly ) ) {
        msgBox.setText( "The file can´t open." );
        msgBox.exec();
        return false;
    }
    QDataStream out( &file );
    // Cabecera:
    out << quint8(  0xFE );     // Binary File
    out << quint16( 0x0000 );   // Init Address
//  out << quint16( 0x37FF );   // End  Address
    out << quint8( 0xFF );      // End  Address
    out << quint8( 0x37 );      // End  Address
    out << quint16( 0x0000 );   // Run  Address
    // 0000H - 07FFH	-->	Pattern generator table 1 -> 256 * 8 = 2048
    // 0800H - 0FFFH	-->	Pattern generator table 2 -> 256 * 8 = 2048
    // 1000H - 17FFH	-->	Pattern generator table 3 -> 256 * 8 = 2048
    // Guardando la tabla de patrones
    for( int bank = 0; bank <   3; ++bank ) {
    for( int tile = 0; tile < 256; ++tile ) {
    for( int row  = 0; row  <   8; ++row  ) {
        out << quint8( m_TilesBank[tile][bank].GetRowPattern( row ) );
    }
    }
    }
    // 1800H - 18FFH	-->	Pattern name table 1 -> 256
    // 1900H - 19FFH	-->	Pattern name table 2 -> 256
    // 1A00H - 1AFFH	-->	Pattern name table 3 -> 256
    // Guardando los nombres;
    for( int nam  = 0; nam < 256*3; ++nam  ) {
        out << quint8( m_Screen[m_MapScreenX][m_MapScreenY][nam] );
    }
    // 1B00H - 1B7FH	-->	Sprite attribute table 128 bytes 0x80
    for( int i = 0x0; i < 0x80; ++i  ) out << quint8( 0 );
    // 1B80H - 1BAFH	-->	Palette table 48 bytes 0x30
    for( int i = 0x0; i < 0x30; ++i  ) out << quint8( 0 );
    // 1BB0H - 1FFFH	--> ???         1104 bytes 0x450
    for( int i = 0x0; i < 0x450; ++i  ) out << quint8( 0 );
    // 2000H - 27FFH	-->	Colour table 1 -> 256 * 8 = 2048
    // 2800H - 2FFFH	-->	Colour table 2 -> 256 * 8 = 2048
    // 3000H - 37FFH	-->	Colour table 3 -> 256 * 8 = 2048
    // de 2000H a 37FFH = 1800H -> 6144
    // Guardando la tabla de colores
    for( int bank = 0; bank <   3; ++bank ) {
    for( int tile = 0; tile < 256; ++tile ) {
    for( int row  = 0; row  <   8; ++row  ) {
        out << quint8( m_TilesBank[tile][bank].GetRowColor( row ) );
    }
    }
    }
    file.close();
    return true;
}

// -------------------------------------------------
// PALETTE:
// -------------------------------------------------
void CScreenW::UpdatePalette() {
    QImage image = QImage( m_pLblColors->width(), m_pLblColors->height(), QImage::Format_RGB32 );
    for( int x = 0; x < 16; x++ ) CSupportFuncs::SetPixelRect( &image, x*12, 0, COLORS_TABLE_SCREEN[x], 12, 16 );
    m_pLblColors->put_Image( image );
    SetTileDesign();
    for( int bank = 0; bank < 3; bank++ )  {
        for( int tile = 0; tile < 256; tile++ ) {
            PaintTile( &m_BankImages[bank], m_TilesBank[tile][bank], (tile%32)*18+1, (tile/32)*18+1, 2 );
            m_pLblBank[bank]->put_Image( m_BankImages[bank] );
        }
    }
    UpdateScreen();
}

void CScreenW::NewPalette() {
    // Iniciar Paleta de color:
    for( int i = 0; i < 16; ++i ) {
        COLORS_TABLE_SCREEN[i] = CSupportFuncs::CONST_COLORS_TABLE[i];
    }
    UpdatePalette();
}

bool CScreenW::LoadPalette(QString fileName) {
    QString line;  // QString line = str.readLine();
    QFile hFile;
    hFile.setFileName( fileName );
    hFile.open( QIODevice::ReadOnly );
    QTextStream str( &hFile );
    if( hFile.atEnd() ) {
        hFile.close();
        return false;
    }
    line = str.readLine();
    if( line != "PALETTE" ) {
        hFile.close();
        return false;
    }
    for( int i = 0; i < 16; ++i ) {
        line = str.readLine();
        if( line.isNull() ) {
            hFile.close();
            return false;
        }
        COLORS_TABLE_SCREEN[i] = line.toUInt();
    }
    hFile.close();
    UpdatePalette();
    return true;
}

bool CScreenW::SavePalette(QString fileName) {
    QFile hFile;
    hFile.setFileName( fileName );
    hFile.open( QIODevice::WriteOnly );
    QTextStream str( &hFile );
    str << "PALETTE" << endl;
    for( int i = 0; i < 16; ++i ) {
        str << COLORS_TABLE_SCREEN[i] << endl;
    }
    hFile.flush();
    hFile.close();
    return true;
}

bool CScreenW::ExportPaletteASM( QString fileName ) {
    QFile hFile;
    hFile.setFileName( fileName );
    hFile.open( QIODevice::WriteOnly | QIODevice::Text );
    QTextStream str( &hFile );
    QColor color;
    QString s;
    uint r, g, b;
    str << "PALETTE:" << endl;
    str << "; --------------------------------------------------------" << endl;
    str << ";    [0~15] Decimal [0~7]      Binary VDP      Hexadecimal" << endl;
    str << ";   DB    NN, R, B, G    ; 0RRR0BBB 00000GGG ; 0xAARRGGBB " << endl;
    str << "; --------------------------------------------------------" << endl;
    for( int i = 0; i < 16; ++i ) {
        int j = ( i < 15 ) ? i + 1 : 0 ;
        color.setRgb( QRgb( COLORS_TABLE_SCREEN[j] ) );
        r = CSupportFuncs::Convert_8bit_to_3bit( color.red() );
        g = CSupportFuncs::Convert_8bit_to_3bit( color.green() );
        b = CSupportFuncs::Convert_8bit_to_3bit( color.blue() );
        // Decimal:
        s  = QString( "    DB    %1, " ).arg( j, 2, 10 );
        s += QString( "%1, %2, %3    ; " ).arg( r ).arg( b ).arg( g );
        // Binary:
        s += QString( "0%1" ).arg( r, 3, 2, QChar('0') );
        s += QString( "0%1 " ).arg( b, 3, 2, QChar('0') );
        s += QString( "00000%1 ; " ).arg( g, 3, 2, QChar('0') );
        // Hexadecimal:
        s += QString( "0x%1" ).arg( color.rgb(), 8, 16, QChar( '0' ) ).toUpper();
        // print
        str << s << endl;
    }
    hFile.flush();
    hFile.close();
    return true;
}

bool CScreenW::ExportPaletteVDP( QString fileName ) {
    QFile hFile;
    hFile.setFileName( fileName );
    hFile.open( QIODevice::WriteOnly | QIODevice::Text );
    QTextStream str( &hFile );
    QColor color;
    QString s;
    uint r, g, b;
    str << "; ----------------------------------------------" << endl;
    str << ";    [0~15]         Binary           Hexadecimal" << endl;
    str << "; ColorNN:    0RRR0BBB 00000GGG    ; 0xAARRGGBB " << endl;
    str << "; ----------------------------------------------" << endl;
    for( int i = 0; i < 16; ++i ) {
        color.setRgb( QRgb( COLORS_TABLE_SCREEN[i] ) );
        r = CSupportFuncs::Convert_8bit_to_3bit( color.red() );
        g = CSupportFuncs::Convert_8bit_to_3bit( color.green() );
        b = CSupportFuncs::Convert_8bit_to_3bit( color.blue() );
        s  = QString( "; Color%1:    " ).arg( i, 2, 10, QChar('0') );
        s += QString( "0%1" ).arg( r, 3, 2, QChar('0') );
        s += QString( "0%1 " ).arg( b, 3, 2, QChar('0') );
        s += QString( "00000%1    " ).arg( g, 3, 2, QChar('0') );
        s += QString( "; 0x%1" ).arg( color.rgb(), 8, 16, QChar('0') ).toUpper();
        str << s << endl;
    }
    hFile.flush();
    hFile.close();
    return true;
}

// -------------------------------------------------
// TILES:
// -------------------------------------------------

void CScreenW::NewTiles() {
    int b, i;
    for( b = 0; b < 3; b++ ) {
        for( i = 0; i < 256; i++ )
            m_TilesBank[i][b] = CTile();
    }
    InitBanks();
}

bool CScreenW::LoadTiles( QString fileName ) {
    int i;
    int x, y, b;
    QString line;
    QFile hFile;
    hFile.setFileName( fileName );
    hFile.open( QIODevice::ReadOnly );
    QTextStream str( &hFile );
    if( hFile.atEnd() ) {
        hFile.close();
        return false;
    }
    if( str.readLine() != "TILES" ) {
        hFile.close();
        return false;
    }
    for( b = 0; b < 3; b++ ) {
        line = str.readLine();
        if( line != QString( "BANK%1" ).arg( b ) ) {
            hFile.close();
            return false;
        }
        for( i = 0; i < 256; i++ ) {
            for( y = 0; y < 8; y++ ) m_TilesBank[i][b].SetBackColor( y, str.readLine().toInt() );
            for( y = 0; y < 8; y++ ) m_TilesBank[i][b].SetForeColor( y, str.readLine().toInt() );
            for( y = 0; y < 8; y++ ) {
                for( x = 0; x < 8; x++ ) m_TilesBank[i][b].SetPixel( x, y, str.readLine().toInt() );
            }
        }
    }
    hFile.close();
    InitBanks();
    return true;
}

bool CScreenW::LoadTilesLibrary( QString fileName, int bankOr, int xOr, int yOr, int width, int height, int bankDest, int xDest, int yDest ) {
    int i;
    int vX, vY;
    int x, y, b;
    int dX, dY;
    QString line;
    QFile hFile;
    hFile.setFileName( fileName );
    hFile.open( QIODevice::ReadOnly );
    QTextStream str( &hFile );
    //TODO - 1 Indicar error
    if( hFile.atEnd() ) {
        hFile.close();
        return false;
    }
    if( str.readLine() != "TILES" ) {
        hFile.close();
        return false;
    }
    for( b = 0; b < 3; b++ ) {
        line = str.readLine();
        if( line != QString( "BANK%1" ).arg( b ) ) {
            hFile.close();
            return false;
        }
        for( i = 0; i < 256; i++ ) {
            vX = i % 32;
            vY = i / 32;
            dX = vX - xOr + xDest;
            dY = vY - yOr + yDest;
            if( vX >= xOr && vX < ( xOr + width ) && vY >= yOr && vY < ( yOr + height ) && bankOr == b && dX >= 0 && dX < 32 && dY >=0 && dY < 8 ) {
                qDebug()<<"CScreenW::LoadTilesLibrary dX="<<dX<<" dY="<<dY;
                for( y = 0; y < 8; y++ ) m_TilesBank[dY*32+dX][bankDest].SetBackColor( y, str.readLine().toInt() );
                for( y = 0; y < 8; y++ ) m_TilesBank[dY*32+dX][bankDest].SetForeColor( y, str.readLine().toInt() );
                for( y = 0; y < 8; y++ ) {
                   for( x = 0; x < 8; x++ ) m_TilesBank[dY*32+dX][bankDest].SetPixel( x, y, str.readLine().toInt() );
                }
            } else {
                for( y = 0; y < 8*2; y++ ) str.readLine();
                for( y = 0; y < 8*8; y++ ) str.readLine();
            }
        }
    }
    hFile.close();
    InitBanks();
    return true;
}

bool CScreenW::SaveTiles( QString fileName ) {
    int i, b;
    int x, y;
    QFile hFile;
    hFile.setFileName( fileName );
    hFile.open( QIODevice::WriteOnly );
    QTextStream str( &hFile );
    str << "TILES" << endl;
    for( b = 0; b < 3; b++ ) {
        str << "BANK" << b << endl;
        for( i = 0; i < 256; i++ ) {
            for( x = 0; x < 8; x++ ) str<<m_TilesBank[i][b].GetBgColor(x) << endl;
            for( x = 0; x < 8; x++ ) str<<m_TilesBank[i][b].GetForeColor(x) << endl;
            for( y = 0; y < 8; y++ ) {
                for( x = 0; x < 8; x++ ) str<<m_TilesBank[i][b].GetPixel( x, y ) << endl;
            }
        }
    }
    hFile.flush();
    hFile.close();
    return true;
}

bool CScreenW::ExportTilesData( QString fileName, bool hexa ) {
    int i, j;
    int b;
    QFile hFile;
    hFile.setFileName( fileName );
    hFile.open( QIODevice::WriteOnly | QIODevice::Text );
    QTextStream str( &hFile );
    for( b = 0; b < 3; b++ ) {
        str<<QString( "BANK_PATTERN_%1:").arg( b ) << endl;
        for( j = 0; j < 256; j++ ) {
            str << "\tDB ";
            for( i = 0; i < 8; i++ ) {
                if( hexa ) str<<QString( "0x%1" ).arg( m_TilesBank[j][b].GetRowPattern( i ), 0, 16 );
                else str << m_TilesBank[j][b].GetRowPattern( i );
                if( i != 7 ) str<<", ";
            }
            str <<endl;
        }
    }
    str << endl << endl;
    for( b = 0; b < 3; b++ ) {
        str<<QString( "BANK_COLOR_%1:").arg( b ) << endl;
        for( j = 0; j < 256; j++ ) {
            str << "\tDB ";
            for( i = 0; i < 8; i++ ) {
                if( hexa ) str<<QString( "0x%1" ).arg( m_TilesBank[j][b].GetRowColor( i ), 0, 16 );
                else str << m_TilesBank[j][b].GetRowColor( i );

                if( i != 7 ) str <<", ";
            }
            str << endl;
        }
    }
    hFile.flush();
    hFile.close();
    return true;
}

bool CScreenW::ExportTilesBin( QString fileName ) {
    int i, j;
    int b;
    QFile hFile;
    int nLoops;
    CDlgBankRange dlgBankRange;
    dlgBankRange.exec();
    if( dlgBankRange.result() == 0 ) //TODO - 1 MsgBox
    {
        return false;
    }
    // hFile.setFileName( fileName + QString( ".til" ) );
    hFile.setFileName( fileName + QString( ".chr" ) );
    hFile.open( QIODevice::WriteOnly );
    if( ui->m_pGrOneBank->isChecked() ) nLoops = 1;
    else nLoops = 3;
    QDataStream str( &hFile );
    for( b = 0; b < nLoops; b++ ) {
        for( j = dlgBankRange.get_FromTile(); j <  dlgBankRange.get_ToTile() + 1; j++ ) {
            if( dlgBankRange.get_BankOption() == 0 || dlgBankRange.get_BankOption() == (b + 1) ) {
                for( i = 0; i < 8; i++ ) {

                    str<<(unsigned char)m_TilesBank[j][b].GetRowPattern( i );
                }
            }
        }
    }
    hFile.flush();
    hFile.close();
    // hFile.setFileName( fileName + QString( ".col" ) );
    hFile.setFileName( fileName + QString( ".clr" ) );
    hFile.open( QIODevice::WriteOnly );
    QDataStream strCol( &hFile );
    for( b = 0; b < nLoops; b++ ) {
        for( j = dlgBankRange.get_FromTile(); j < dlgBankRange.get_ToTile() + 1; j++ ) {
            if( dlgBankRange.get_BankOption() == 0 || dlgBankRange.get_BankOption() == (b + 1) ) {
                for( i = 0; i < 8; i++ ) {
                    strCol<<(unsigned char)m_TilesBank[j][b].GetRowColor( i );
                }
            }
        }
    }
    hFile.flush();
    hFile.close();
    return true;
}

void CScreenW::ExportTilesBinPletter( QString fileName ) {
    long sz;
    int i, b, j;
    int nLoops;
    unsigned char buffer[256*3*8];
    CDlgBankRange dlgBankRange;
    dlgBankRange.exec();
    if( dlgBankRange.result() == 0 ) //TODO - 1 MsgBox
    {
        return;
    }
    // 0 -> todos los bancos
    // 1
    // 2
    // 3
    // b -> 0, 1, 2
    if( ui->m_pGrOneBank->isChecked() ) nLoops = 1;
    else nLoops = 3;
    for( b = 0, sz = 0; b < nLoops; b++ ) {
        for( j = dlgBankRange.get_FromTile(); j < dlgBankRange.get_ToTile() + 1; j++ ) {
            if( dlgBankRange.get_BankOption() == 0 || dlgBankRange.get_BankOption() == (b + 1) ) {
                //for( i = 0; i < 8; i++ ) buffer[b*256*8+j*8+i] = m_TilesBank[j][b].GetRowPattern( i );
                for( i = 0; i < 8; i++, sz++ ) buffer[sz] = m_TilesBank[j][b].GetRowPattern( i );
            }
        }
    }
    // PletterMain( sz/*b*256*8*/, buffer, QString( fileName + QString( ".til" ) ) );
    // pletter( buffer, sz/*b*256*8*/, QString( fileName + QString( ".til" ) ), false );
    pletter( buffer, sz, QString( fileName + QString( ".chr" ) ), false );

    for( b = 0, sz = 0; b < nLoops; b++ ) {
        for( j = dlgBankRange.get_FromTile(); j < dlgBankRange.get_ToTile() + 1; j++ ) {
            if( dlgBankRange.get_BankOption() == 0 || dlgBankRange.get_BankOption() == (b + 1) ) {
                for( i = 0; i < 8; i++, sz++ ) buffer[sz] = m_TilesBank[j][b].GetRowColor( i );
            }
        }
    }
    // PletterMain( sz/*b*256*8*/, buffer, QString( fileName + QString( ".col" ) ) );
    // pletter( buffer, sz/*b*256*8*/, QString( fileName + QString( ".col" ) ), false );
    pletter( buffer, sz, QString( fileName + QString( ".clr" ) ), false );
}

// -------------------------------------------------
// SCREEN TOOLS:
// -------------------------------------------------
void CScreenW::Fill1to255() {
    int b, i;
    for( b = 0; b < 3; b++ )
        for( i = 0; i < 32*8; i++ )
            m_Screen[m_MapScreenX][m_MapScreenY][i+b*32*8]=i;
    UpdateScreen();
}

// -------------------------------------------------
// PALETTE TOOLS:
// -------------------------------------------------


// -------------------------------------------------
// TILES TOOLS:
// -------------------------------------------------

void CScreenW::CopyTile() {
    if( m_ScreenSelection.size() ) m_ScreenSelectionCopy = m_ScreenSelection;
    else {
        m_CopyTile = m_TilesBank[m_SelTile.x()][m_SelTile.y()];
        m_ScreenSelectionCopy.clear();
    }
}

void CScreenW::PasteTile() {
    int xI;
    int x, y;
    int i, j;
    QList<int> s;
    if( m_ScreenSelectionCopy.size() ) {
        xI = m_MouseX / 16;
        y = m_MouseY / 16;
        if( m_ScreenSelectionCopy.size() == m_SelectionRect.height() && m_ScreenSelectionCopy.at( 0 ).size() == m_SelectionRect.width()	) {
            for( i = 0; i < m_ScreenSelectionCopy.size(); i++ ) {
                s =  m_ScreenSelectionCopy.at( i );
                if( ( i + y ) > 23 ) break;
                for( j = 0, x = xI; j < s.size(); j++ ) {
                    if( ( j + x ) > 31 ) break;
                    m_Screen[m_MapScreenX][m_MapScreenY][(i+y)*32+j+x] = s.at( j );
                }
            }
        } else {
            for( y = m_SelectionRect.top(), j = 0; y <= m_SelectionRect.bottom(); y++, j++ ) {
                if( j == m_ScreenSelectionCopy.size() ) j = 0;
                s = m_ScreenSelectionCopy.at( j );
                for( x = m_SelectionRect.left(), i = 0; x <= m_SelectionRect.right(); x++, i++ ) {
                    if( i == m_ScreenSelectionCopy.at( j ).size() ) i = 0;
                    m_Screen[m_MapScreenX][m_MapScreenY][y*32+x] = s.at( i );
                }
            }
        }
        UpdateScreen();
    } else {
        if( m_ScreenSelection.size() ) {
            for( y = m_SelectionRect.top(); y <= m_SelectionRect.bottom(); y++ ) {
                for( x = m_SelectionRect.left(); x <= m_SelectionRect.right(); x++ ) {
                    m_Screen[m_MapScreenX][m_MapScreenY][y*32+x] = m_SelTile.x();
                }
            }
            UpdateScreen();
        } else {
            m_TilesBank[m_SelTile.x()][m_SelTile.y()] = m_CopyTile;
            UpdateBank( m_SelTile.x(), m_SelTile.y() );
        }
    }
}

void CScreenW::PasteTileColors() {
    int y;
    for( y = 0; y < 8; y++ ) {
        m_TilesBank[m_SelTile.x()][m_SelTile.y()].SetForeColor( y, m_CopyTile.GetForeColor( y ) );
        m_TilesBank[m_SelTile.x()][m_SelTile.y()].SetBackColor( y, m_CopyTile.GetBgColor( y ) );
    }
    UpdateBank( m_SelTile.x(), m_SelTile.y() );
}

//TODO - 1 class CUndo
void CScreenW::Undo() {
    if( m_Undo.size() == 0 ) return;
    if( m_Undo.at( 0 ) == "TOBANK" ) {
        if( m_Undo.at( 1 ) == "ALLBANKS" ) {
            m_TilesBank[ m_Undo.at( 2 ).toLongLong() ] [ 0 ] = m_UndoTile;
            m_TilesBank[ m_Undo.at( 2 ).toLongLong() ] [ 1 ] = m_UndoTile;
            m_TilesBank[ m_Undo.at( 2 ).toLongLong() ] [ 2 ] = m_UndoTile;
            UpdateBank( m_Undo.at( 2 ).toLongLong(), 0 );
        } else if( m_Undo.at( 1 ) == "ONEBANK" ) {
            m_TilesBank[ m_Undo.at( 2 ).toLongLong() ] [ m_Undo.at( 3 ).toLongLong() ] = m_UndoTile;
            UpdateBank( m_Undo.at( 2 ).toLongLong(), m_Undo.at( 3 ).toLongLong() );
        } else Q_ASSERT( false );
        m_Undo.clear();
    } else if( m_Undo.at( 0 ) == "SCREENCLICK" ) {
        m_Screen[m_MapScreenX][m_MapScreenY][ m_Undo.at( 2 ).toLongLong() *32 + m_Undo.at( 1 ).toLongLong() ] = m_Undo.at( 3 ).toLongLong();
        UpdateScreen();
        m_Undo.clear();
    } else {
        Q_ASSERT( false );
    }
}

void CScreenW::ReorderColors() {
    for( unsigned int b = 0; b < 3 ; b++ ) {
        for( unsigned int x = 0; x < 256; x++ ) {
            m_TilesBank[x][b].ReorderLessFirst();
        }
    }
}

void CScreenW::GroupTiles() {
    int blacked;
    int i, j, b;
    int index;
    CTile groupBank[256];
    for( b = 0; b < 3; b++ ) {
        for( i = 0; i < 256; i++ ) groupBank[b] = CTile();
        for( i = 0, index = 0, blacked = false; i < 256; i++ ) {
            if( !m_TilesBank[i][b].IsBlack() || !blacked ) {
                if( m_TilesBank[i][b].IsBlack() ) blacked = true;
                groupBank[index] = m_TilesBank[i][b];
                for( j = 0; j < 256; j++ ) {
                    for( int mapY = 0; mapY < ui->m_pGrMapHeight->value(); mapY++ ) {
                        for( int mapX = 0; mapX < ui->m_pGrMapWidth->value(); mapX++ ) {
                            if( m_Screen[mapX][mapY][j+256*b] == i ) m_Screen[mapX][mapY][j+256*b] = index;
                        }
                    }

                }
                index++;
            }
        }
        for( i = 0; i < 256; i++ ) m_TilesBank[i][b] = groupBank[i];
    }
    InitBanks();
}

void CScreenW::ExchangeTiles( int tiles1, int tiles2 ) {
    for( int b = 0; b < 3; b++ ) {
        if( !ui->m_pGrOneBank->isChecked() && m_pTabBank->currentIndex() != b ) continue;
        CTile tmp = m_TilesBank[tiles2][b];
        m_TilesBank[tiles2][b] = m_TilesBank[tiles1][b];
        m_TilesBank[tiles1][b] = tmp;
        UpdateBank( tiles1, b );
        UpdateBank( tiles2, b );
        for( int i = 0; i < 32*8; i++ ) {
            if( m_Screen[m_MapScreenX][m_MapScreenY][i+b*32*8] == tiles1 )
                m_Screen[m_MapScreenX][m_MapScreenY][i+b*32*8] = tiles2;
            else if( m_Screen[m_MapScreenX][m_MapScreenY][i+b*32*8] == tiles2 )
                m_Screen[m_MapScreenX][m_MapScreenY][i+b*32*8] = tiles1;
        }
    }
    UpdateScreen();
}

// -------------------------------------------------
// PNG TOOLS
// -------------------------------------------------

bool CScreenW::ExportScreenPNG( QString fileName ) {
    int xT, yT;
    int i, y, x;
    int tileId;
    CTile tile;
    QImage image;
    image = QImage( 256, 192, QImage::Format_RGB32 );
    for( yT = 0, i = -1; yT < 192; yT+=8 ) {
        for( xT = 0; xT < 256; xT+=8 ) {
            i++;
            tileId = m_Screen[m_MapScreenX][m_MapScreenY][i];
            for( y = 0, tile = m_TilesBank[tileId%256][i/256]; y < 8; y++ ) {
                for( x = 0; x < 8; x++ ) {
                    image.setPixel( xT + x, yT + y, COLORS_TABLE_SCREEN[tile.GetPixelColor(x, y)]);

                }
            }
        }
    }
    return image.save( fileName );
}

bool CScreenW::ExportMapPNG( QString fileName ) {
    int xT, yT;
    int i, y, x;
    int tileId;
    CTile tile;
    QImage image;
    image = QImage( 256*ui->m_pGrMapWidth->value(), 192*ui->m_pGrMapHeight->value(), QImage::Format_RGB32 );
    for( int screenY = 0; screenY < ui->m_pGrMapHeight->value(); screenY++ ) {
        for( int screenX = 0; screenX < ui->m_pGrMapWidth->value(); screenX++ ) {
            for( yT = 0, i = -1; yT < 192; yT+=8 ) {
                for( xT = 0; xT < 256; xT+=8 ) {
                    i++;
                    tileId = m_Screen[screenX][screenY][i];
                    for( y = 0, tile = m_TilesBank[tileId%256][i/256]; y < 8; y++ ) {
                        for( x = 0; x < 8; x++ ) {
                            image.setPixel( xT + x + screenX*256, yT + y + screenY*192, COLORS_TABLE_SCREEN[tile.GetPixelColor(x, y)]);

                        }
                    }
                }
            }
        }
    }
    return image.save( fileName );
}

void CScreenW::ExportTilesPNG( QString fileName ) {
    int h;
    int xT, yT;
    int i, y, x;
    CTile tile;
    QImage image;
    if( ui->m_pGrOneBank->isChecked() ) h = 64;
    else h = 64*3;
    image = QImage( 256, h, QImage::Format_RGB32 );
    for( yT = 0, i = -1; yT < h; yT+=8 ) {
        for( xT = 0; xT < 256; xT+=8 ) {
            i++;
            for( y = 0, tile = m_TilesBank[i%256][i/256]; y < 8; y++ ) {
                for( x = 0; x < 8; x++ ) {
                    image.setPixel( xT + x, yT + y, COLORS_TABLE_SCREEN[tile.GetPixelColor(x, y)]);

                }
            }
        }
    }
    image.save( fileName );
}

// bool CScreenW::ImportTiles( QString fileName , QString paletteFileName ) {
bool CScreenW::ImportTiles( QString fileName ) {
    int i;
    int color;
    int x, y, b;
    int fore, back;
    QImage image;
    unsigned int palette[16];
    // Palette used by nMSXtiles
    for( i = 0; i < 16; i++ ) palette[i] = COLORS_TABLE_SCREEN[i];
    CheckImagePalette( fileName, palette );
    /*
    if( paletteFileName == "" ) {
        for( i = 0; i < 16; i++ ) palette[i] = COLORS_TABLE_SCREEN[i];
        CheckImagePalette( fileName, palette );
    } else {
        if( !image.load( paletteFileName ) ) return false;
        if( image.width() < 16  ) return false;
        for( i = 0; i < 16; i++ ) palette[i] = (unsigned int )image.pixel( i, 0 );
    }
    */
    if( !image.load( fileName ) ) return false;
    if( image.width() < 256 || image.height() < 64 ) return false;
    for( b = 0; b < 3; b++ ) {
        if( b > 0 && image.height() <= 64 ) break;
        for( y = 0; y < 8*8; y++ ) {
            for( x = 0; x < 32; x++ ) {
                fore = back = -1;
                for( i = 0; i < 8; i++ ) {
                    color = GetColor( image.pixel( x*8+i, y+b*64 ), palette );
                    if( fore == -1  ) {
                        fore = color;
                        m_TilesBank[(y/8)*32+x][b].SetForeColor( y%8, fore );
                        m_TilesBank[(y/8)*32+x][b].SetPixel( i, y%8, 1 );
                    } else if( fore == color ) {
                        m_TilesBank[(y/8)*32+x][b].SetPixel( i, y%8, 1 );
                    } else if( back == -1 ) {
                        back = color;
                        m_TilesBank[(y/8)*32+x][b].SetBackColor( y%8, back );
                        m_TilesBank[(y/8)*32+x][b].SetPixel( i, y%8, 0 );
                    } else if( back == color ) {
                        m_TilesBank[(y/8)*32+x][b].SetPixel( i, y%8, 0 );
                    } else { //Three colors on a row!
                        m_TilesBank[(y/8)*32+x][b].SetForeColor( y%8, fore );
                    }
                }
            }
        }
    }
    InitBanks();
    return true;
}

// -------------------------------------------------
// FUNCIONES PÚBLICAS:
// -------------------------------------------------

int CScreenW::get_MapWidth() {
    return ui->m_pGrMapWidth->value();
}

int CScreenW::get_MapHeight() {
    return ui->m_pGrMapHeight->value();
}

void CScreenW::SetMapWidth(int value) {
    ui->m_pGrMapWidth->setValue( value );
    ui->m_pGrMapX->setMaximum( value - 1 );
}

void CScreenW::SetMapHeight( int value ) {
    ui->m_pGrMapHeight->setValue( value );
    ui->m_pGrMapY->setMaximum( value - 1);
}

// -------------------------------------------------
// SLOTS PÚBLICOS:
// -------------------------------------------------

// ---------------------------------------------------------------
// Las acciones del ratón sobre el Screen:
// ---------------------------------------------------------------
void CScreenW::OnScreenClickLeft( int x, int y ) {
    m_ScreenSelection.clear();
    m_Selecting = true;
    m_pLblCursor[0]->setVisible( false );
    m_pLblCursor[1]->setVisible( false );
    m_pLblCursor[2]->setVisible( false );
    m_pLblCursor[3]->setVisible( false );
    m_LastClickPos = QPoint( x, y );
}

void CScreenW::OnScreenReleaseLeft( int x, int y ) {
    int tmp;
    int i, j;
    int xI, yI;
    int xE, yE;
    QList<int> r;
    m_Selecting = false;
    m_ScreenSelection.clear();
    //TODO - 1 xI<xE yI < yE
    if( m_LastClickPos.x() >= 0 && ( abs( m_LastClickPos.x() - x ) >= 8 || abs( m_LastClickPos.y() - y ) >= 8 ) ) {
        xI = m_LastClickPos.x() / 16 * 16;
        yI = m_LastClickPos.y() / 16 * 16;
        xE = ( x / 16 * 16 );
        yE = ( y / 16 * 16 );
        if( xI > xE ) {
            tmp = xE;
            xE = xI;
            xI = tmp;
        }
        if( yI > yE ) {
            tmp = yE;
            yE = yI;
            yI = tmp;
        }
        m_pLblSelection->setGeometry( xI + 3, yI + 2, abs( xI - xE ), abs( yI - yE ) );
        m_pLblSelection->show();
        xI = xI / 16;
        xE = xE / 16;
        yI = yI /16;
        yE = yE / 16;
        m_SelectionRect = QRect( xI, yI, xE - xI, yE - yI );
        for( i = yI; i < yE; i++ ) {
            r.clear();
            for( j = xI; j < xE; j++ ) {
                r<<m_Screen[m_MapScreenX][m_MapScreenY][i*32+j];
            }
            m_ScreenSelection<<r;
        }
        return;
    }
    m_pLblSelection->setVisible( false );
    x = x / 16;
    y = y / 16;
    if( ( y / 8 ) != m_SelTile.y() && !ui->m_pGrOneBank->isChecked() ) return;
    m_Undo.clear();
    m_Undo<<"SCREENCLICK"<<x<<y<<m_Screen[m_MapScreenX][m_MapScreenY][y*32+x];
    for( i = 0; i < ui->m_pGrTilesBlockH->text().toLongLong(); i++ ) {
        for( j = 0; j < ui->m_pGrTilesBlockW->text().toLongLong(); j++ ) {
            if( ( m_SelTile.x() + (i*32) + j ) < 256 )
                m_Screen[m_MapScreenX][m_MapScreenY][(y+i)*32+x+j] = m_SelTile.x()+(i*32)+j;
            else Q_ASSERT( false );
        }
    }
    UpdateScreen();
}

// Borra con la baldosa de fondo...
void CScreenW::OnScreenReleaseRight( int x, int y ) {
    m_pLblSelection->setGeometry( 0, 0, 0, 0 );
    m_ScreenSelection.clear();
    x = x / 16;
    y = y / 16;
    m_Undo.clear();
    m_Undo << "SCREENCLICK " << x << y << m_Screen[m_MapScreenX][m_MapScreenY][y*32+x];
    m_Screen[m_MapScreenX][m_MapScreenY][y*32 + x] = GetBackgroundTile( y/8 );
    UpdateScreen();
}

void CScreenW::OnScreenMouseMoveEvent( int x, int y ) {
    QPalette pal;
    int ox, oy;
    if( x < 0 || y < 0 ) return;
    m_MouseX = x;
    m_MouseY = y;
    x /= 16;
    y /= 16;
    if( x > 31 || y >23 ) return;
    if( m_LastGridValue.x() != x || m_LastGridValue.y() != y ) {
        pal = m_pColsLabels[m_LastGridValue.x()]->palette();
        pal.setColor( QPalette::Dark, Qt::gray );
        m_pColsLabels[m_LastGridValue.x()]->setPalette( pal );
        pal.setColor( QPalette::Dark, Qt::green );
        m_pColsLabels[x]->setPalette( pal );
        Q_ASSERT( m_pRowsLabels[m_LastGridValue.y()] );
        pal = m_pRowsLabels[m_LastGridValue.y()]->palette();
        pal.setColor( QPalette::Dark, Qt::gray );
        m_pRowsLabels[m_LastGridValue.y()]->setPalette( pal );
        pal.setColor( QPalette::Dark, Qt::green );
        Q_ASSERT( m_pRowsLabels[y] );
        m_pRowsLabels[y]->setPalette( pal );
        m_LastGridValue = QPoint( x, y );
        ui->m_pLblShowTileBank->setText( QString( "%1" ).arg( m_Screen[m_MapScreenX][m_MapScreenY][ y*32+x] ) );
    }
    if( m_Selecting ) {
        if( m_LastClickPos.x() > m_MouseX) ox = m_MouseX;
        else ox = m_LastClickPos.x();
        if( m_LastClickPos.y() > m_MouseY ) oy = m_MouseY;
        else oy = m_LastClickPos.y();
        m_pLblSelection->setGeometry( ox, oy, abs( m_MouseX - m_LastClickPos.x() ), abs( m_MouseY - m_LastClickPos.y() ) );
        m_pLblSelection->setVisible( true );
    } else {
        m_pLblCursor[0]->setGeometry( x*16+1 + 3,  y*16+1, 14, 1 );
        m_pLblCursor[1]->setGeometry( x*16+20, y*16+1 + 2,  1, 16 );
        m_pLblCursor[2]->setGeometry( x*16+1 + 3,  y*16+21, 14, 1 );
        m_pLblCursor[3]->setGeometry( x*16+1,  y*16+1 + 2,  1, 16 );
        m_pLblCursor[0]->setVisible( true );
        m_pLblCursor[1]->setVisible( true );
        m_pLblCursor[2]->setVisible( true );
        m_pLblCursor[3]->setVisible( true );
    }
}

void CScreenW::UpdateScreen() {
    int i;
    int row, col;
    QImage image;
    image = QImage( 32*16, 24*16, QImage::Format_RGB32 );
    for( i = 0; i < 3; i++ ) {
        for( row = 0; row < 8; row++ ) {
            for( col = 0; col < 32; col++ ) {
                PaintTile( &image, m_TilesBank[ m_Screen[m_MapScreenX][m_MapScreenY][(i*8+row)*32+col] ][ i ], col*16, (i*8+row)*16, 2 );
            }
        }
    }
    m_pLblScreen->put_Image( image );
}

// ---------------------------------------------------------------
// Las acciones del ratón sobre el MultiScreen: ( m_pFrmMultiScreen )
// ---------------------------------------------------------------

void CScreenW::OnMapXChanged( int value ) {
    m_MapScreenX = value;
    InitBanks();
}

void CScreenW::OnMapYChanged( int value ) {
    m_MapScreenY = value;
    InitBanks();
}

void CScreenW::OnMapNextX() {
    ui->m_pGrMapX->setValue( ui->m_pGrMapX->value() + 1 );
}

void CScreenW::OnMapPrevX() {
    ui->m_pGrMapX->setValue( ui->m_pGrMapX->value() - 1 );
}

void CScreenW::OnMapNextY() {
    ui->m_pGrMapY->setValue( ui->m_pGrMapY->value() +1 );
}

void CScreenW::OnMapPrevY() {
    ui->m_pGrMapY->setValue( ui->m_pGrMapY->value() - 1);
}

void CScreenW::OnMapWidthChanged( int value ) {
    ui->m_pGrMapX->setMaximum( value - 1 );
}

void CScreenW::OnMapHeightChanged( int value ) {
    ui->m_pGrMapY->setMaximum( value -1 );
}

void CScreenW::OnOneBank() {
    if( ui->m_pGrOneBank->isChecked() ) {
        QMessageBox msgBox;
        msgBox.setText( "Tiles from Bank 1 and 2 will be replaced by the Tiles of Bank 0." );
        msgBox.setInformativeText( "Are you sure?" );
        msgBox.setStandardButtons( QMessageBox::Ok | QMessageBox::Cancel );
        msgBox.setDefaultButton(QMessageBox::Cancel );
        int ret = msgBox.exec();
        if( ret != QMessageBox::Ok ) {
            ui->m_pGrOneBank->setChecked( false );
            return;
         }
    }
    int i;
    m_pTabBank->setCurrentIndex( 0 );
    for( i = 0; i < 256; i++ ) {
        m_TilesBank[i][1] = m_TilesBank[i][0];
        m_TilesBank[i][2] = m_TilesBank[i][0];
    }
    m_pTabBank->setTabEnabled( 1, !ui->m_pGrOneBank->isChecked() );
    m_pTabBank->setTabEnabled( 2, !ui->m_pGrOneBank->isChecked() );
    ui->m_pGrBackgroundTiles1->setVisible( !ui->m_pGrOneBank->isChecked() );
    ui->m_pGrBackgroundTiles2->setVisible( !ui->m_pGrOneBank->isChecked() );
    ui->m_pGrLblBackgroundTiles1->setVisible( !ui->m_pGrOneBank->isChecked() );
    ui->m_pGrLblBackgroundTiles2->setVisible( !ui->m_pGrOneBank->isChecked() );
    InitBanks();
}

// Función pública:
void CScreenW::SetOneBank( bool value ) {
    ui->m_pGrOneBank->setChecked( value );
    // Añadido para que no se habra el cuadro de diálogo...
    if( !value ) return;
    int i;
    m_pTabBank->setCurrentIndex( 0 );
    for( i = 0; i < 256; i++ ) {
        m_TilesBank[i][1] = m_TilesBank[i][0];
        m_TilesBank[i][2] = m_TilesBank[i][0];
    }
    m_pTabBank->setTabEnabled( 1, !ui->m_pGrOneBank->isChecked() );
    m_pTabBank->setTabEnabled( 2, !ui->m_pGrOneBank->isChecked() );
    ui->m_pGrBackgroundTiles1->setVisible( !ui->m_pGrOneBank->isChecked() );
    ui->m_pGrBackgroundTiles2->setVisible( !ui->m_pGrOneBank->isChecked() );
    ui->m_pGrLblBackgroundTiles1->setVisible( !ui->m_pGrOneBank->isChecked() );
    ui->m_pGrLblBackgroundTiles2->setVisible( !ui->m_pGrOneBank->isChecked() );
    InitBanks();
}

// Función pública:
bool CScreenW::GetOneBank() {
    return ui->m_pGrOneBank->isChecked();
}

// ---------------------------------------------------------------
// Las acciones del ratón sobre los Bancos:
// ---------------------------------------------------------------

void CScreenW::OnBankChanged( int index ) {
    OnBankClick( ( m_SelTile.x() % 32 )*18, ( m_SelTile.x() / 32 )*18, index );
}
void CScreenW::OnBank0Click( int x, int y ) { OnBankClick( x, y, 0 ); }
void CScreenW::OnBank1Click( int x, int y ) { OnBankClick( x, y, 1 ); }
void CScreenW::OnBank2Click( int x, int y ) { OnBankClick( x, y, 2 ); }
void CScreenW::OnBankClick( int x, int y, int bank ) {
    m_LastBank = bank;
    m_LastBankX = x;
    m_LastBankY = y;
    m_pLblSelection->setGeometry( 0, 0, 0, 0 );
    m_ScreenSelection.clear();
    CSupportFuncs::SetBorderImage(
        &m_BankImages[m_SelTile.y()],
        (m_SelTile.x()%32)*18 + 1,
        (m_SelTile.x()/32)*18 + 1,
        18*m_LastTilesBlockW-1,
        18*m_LastTilesBlockH-1,
        0xC0C0C0
    );
    m_pLblBank[m_SelTile.y()]->put_Image( m_BankImages[m_SelTile.y()] );
    m_SelTile = QPoint( (y/18)*32+(x/18), bank );
    CSupportFuncs::SetBorderImage(
        &m_BankImages[bank],
        (m_SelTile.x()%32)*18 + 1,
        (m_SelTile.x()/32)*18 + 1,
        18*ui->m_pGrTilesBlockW->text().toLongLong()-1,
        18*ui->m_pGrTilesBlockH->text().toLongLong()-1,
        0xffff00
    );
    m_LastTilesBlockW = ui->m_pGrTilesBlockW->text().toLongLong();
    m_LastTilesBlockH = ui->m_pGrTilesBlockH->text().toLongLong();
    m_pLblBank[bank]->put_Image( m_BankImages[bank] );//TODO - 1 Serveix?
    ui->m_pLblShowTile->setText( QString( "%1" ).arg( m_SelTile.x() ) );
}

void CScreenW::OnBank0ClickRight( int x, int y ) { OnBankClickRight( x, y, 0 ); }
void CScreenW::OnBank1ClickRight( int x, int y ) { OnBankClickRight( x, y, 1 ); }
void CScreenW::OnBank2ClickRight( int x, int y ) { OnBankClickRight( x, y, 2 ); }
//TODO - 1 No es poden intercanviar entre bancs. Falta el que no ho permeti.
//Intercanvia el darrer
void CScreenW::OnBankClickRight( int x, int y, int bank ) {
    QPoint currentSelTile = QPoint( (y/18)*32+(x/18), bank );
    CTile current = m_TilesBank[currentSelTile.x()][currentSelTile.y()];
    if( ui->m_pGrOneBank->isChecked() ) {
        m_TilesBank[currentSelTile.x()][0] = m_TilesBank[m_SelTile.x()][0];
        m_TilesBank[m_SelTile.x()][0] = current;
        m_TilesBank[currentSelTile.x()][1] = m_TilesBank[currentSelTile.x()][0];
        m_TilesBank[m_SelTile.x()][1] = m_TilesBank[m_SelTile.x()][0];
        m_TilesBank[currentSelTile.x()][2] = m_TilesBank[currentSelTile.x()][0];
        m_TilesBank[m_SelTile.x()][2] = m_TilesBank[m_SelTile.x()][0];
    } else {
        m_TilesBank[currentSelTile.x()][currentSelTile.y()] = m_TilesBank[m_SelTile.x()][m_SelTile.y()];
        m_TilesBank[m_SelTile.x()][m_SelTile.y()] = current;
    }
    for( int mapY = 0; mapY < ui->m_pGrMapHeight->value(); mapY++ ) {
        for( int mapX = 0; mapX < ui->m_pGrMapWidth->value(); mapX++ ) {
            if( ui->m_pGrOneBank->isChecked() ) {
                for( int i = 0; i < 256*3; i++ ) {
                    if( m_Screen[mapX][mapY][i] == currentSelTile.x() ) m_Screen[mapX][mapY][i] = m_SelTile.x();
                    else if( m_Screen[mapX][mapY][i] == m_SelTile.x() ) m_Screen[mapX][mapY][i] = currentSelTile.x();
                }
            } else {
                for( int i = 0; i < 256; i++ ) {

                    if( m_Screen[mapX][mapY][i+bank*256] == currentSelTile.x() ) m_Screen[mapX][mapY][i+bank*256] = m_SelTile.x();
                    else if( m_Screen[mapX][mapY][i+bank*256] == m_SelTile.x() ) m_Screen[mapX][mapY][i+bank*256] = currentSelTile.x();
                }
            }
        }
    }
    UpdateBank( currentSelTile.x(), currentSelTile.y(), false );
    UpdateBank( m_SelTile.x(), m_SelTile.y(), true );
}

void CScreenW::OnBank0DoubleClick( int x, int y ) { OnBankDoubleClick( x, y, 0 ); }
void CScreenW::OnBank1DoubleClick( int x, int y ) {	OnBankDoubleClick( x, y, 1 ); }
void CScreenW::OnBank2DoubleClick( int x, int y ) { OnBankDoubleClick( x, y, 2 ); }
void CScreenW::OnBankDoubleClick( int /*x*/, int /*y*/, int /*bank*/ ) {
    m_pLblSelection->setGeometry( 0, 0, 0, 0 ); //TODO - 1 Class CSelection
    m_ScreenSelection.clear();
    OnFromBank();
}

void CScreenW::UpdateBank( int tile, int bank, bool updateScreen ) {
	PaintTile( &m_BankImages[bank], m_TilesBank[tile][bank], (tile%32)*18+1, (tile/32)*18+1, 2 );
	m_pLblBank[bank]->put_Image( m_BankImages[bank] ); 
	if( updateScreen ) UpdateScreen();
}

void CScreenW::InitBanks() {
    for( int j = 0; j < 3; j++ ) {
		m_BankImages[j] = m_pLblBank[j]->pixmap()->toImage();
		PaintGrid( &m_BankImages[j], 16, 16 );
        for( int i = 0; i < 256; i++ ) {
			PaintTile( &m_BankImages[j], m_TilesBank[i][j], (i%32)*18+1, (i/32)*18+1, 2 );
		}
		m_pLblBank[j]->put_Image( m_BankImages[j] );
	}
	UpdateScreen();
    if     ( m_LastBank == 0 ) OnBank0Click( m_LastBankX, m_LastBankY );
    else if( m_LastBank == 1 ) OnBank1Click( m_LastBankX, m_LastBankY );
    else                       OnBank2Click( m_LastBankX, m_LastBankY );
}

// ---------------------------------------------------------------
// Las acciones del ratón sobre el Editor de Baldosas ( m_pFrmDesigner )
// ---------------------------------------------------------------

void CScreenW::OnPaletteClick( int x, int y ) {
	m_SelColor = QPoint( x/(int)20, y/(int)12 );
    m_pLblPalFore->setFrameShape(QFrame::NoFrame);
    m_pLblPalFore->setFrameShadow(QFrame::Plain);
    m_pLblPalBack->setFrameShape(QFrame::NoFrame);
    m_pLblPalBack->setFrameShadow(QFrame::Plain);
    if( m_SelColor.x() == 0 ) ui->m_pRadBack->click();
    else ui->m_pRadFore->click();
//  SetTileDesign();
}

void CScreenW::OnPalBackClick( int, int ) {
    m_SelColor = QPoint( 0, PAL_ALL );
    m_pLblPalBack->setFrameShape(QFrame::Box);
    m_pLblPalBack->setFrameShadow(QFrame::Raised);
    m_pLblPalFore->setFrameShape(QFrame::NoFrame);
    m_pLblPalFore->setFrameShadow(QFrame::Plain);
    ui->m_pRadBack->click();
//  SetTileDesign();
}

void CScreenW::OnPalForeClick( int, int ) {
    m_SelColor = QPoint( 1, PAL_ALL );
    m_pLblPalFore->setFrameShape(QFrame::Box);
    m_pLblPalFore->setFrameShadow(QFrame::Raised);
    m_pLblPalBack->setFrameShape(QFrame::NoFrame);
    m_pLblPalBack->setFrameShadow(QFrame::Plain);
    ui->m_pRadFore->click();
//  SetTileDesign();
}

void CScreenW::OnTileClickLeft( int x, int y ) {
    m_pLblPalBack->setFrameShape(QFrame::NoFrame);
    m_pLblPalBack->setFrameShadow(QFrame::Plain);
    m_pLblPalFore->setFrameShape(QFrame::NoFrame);
    m_pLblPalFore->setFrameShadow(QFrame::Plain);

    x /= 12;
    y /= 12;
    if( ui->m_pRadBack->isChecked() ) {
        m_TileDesign.SetBackPixel( x, y );
        m_SelColor = QPoint( 0, y );
    } else {
        m_TileDesign.SetForePixel( x, y );
        m_SelColor = QPoint( 1, y );
    }
    SetTileDesign();
}

void CScreenW::OnTileClickRight( int x, int y ) {
    m_pLblPalBack->setFrameShape(QFrame::NoFrame);
    m_pLblPalBack->setFrameShadow(QFrame::Plain);
    m_pLblPalFore->setFrameShape(QFrame::NoFrame);
    m_pLblPalFore->setFrameShadow(QFrame::Plain);

    x /= 12;
    y /= 12;
//  if( !ui->m_pRadBack->isChecked() ) m_TileDesign.SetBackPixel( x, y );
//  else m_TileDesign.SetForePixel( x, y );
    if( !ui->m_pRadBack->isChecked() ) {
         m_TileDesign.SetBackPixel( x, y );
         m_SelColor = QPoint( 0, y );
    } else {
         m_TileDesign.SetForePixel( x, y );
         m_SelColor = QPoint( 1, y );
    }
    SetTileDesign();
}

void CScreenW::OnColorsClick( int x, int /*y*/ ) {
    int c, i;
	c = x / 12;
    if( m_SelColor.x() == 0 )  {
		if( m_SelColor.y() != PAL_ALL )	m_TileDesign.SetBackColor( m_SelColor.y(), c );
        else for( i = 0; i < 8; i++ ) m_TileDesign.SetBackColor( i, c );
    } else  {
		if( m_SelColor.y() != PAL_ALL )	m_TileDesign.SetForeColor( m_SelColor.y(), c );
        else for( i = 0; i < 8; i++ ) m_TileDesign.SetForeColor( i, c );
	}
    SetTileDesign();
}

void CScreenW::OnColorsChange( int x, int /*y*/ ) {
    changepalette* pal = new changepalette( this, COLORS_TABLE_SCREEN, x / 12 );
    QObject::connect( pal, &changepalette::signal_change_palette, this, &CScreenW::UpdatePalette );
    pal->show();
    pal->exec();
}

void CScreenW::OnFromBank() {
	CopyToTileDesign( m_SelTile.x(), m_SelTile.y() );
}

void CScreenW::OnToBank() {
    if( ui->m_pGrOneBank->isChecked() ) {
		m_Undo.clear();
		m_Undo<<"TOBANK"<<"ALLBANKS"<<m_SelTile.x();
        m_UndoTile = m_TilesBank[ m_SelTile.x() ][ 0 ];
		m_TilesBank[ m_SelTile.x() ][ 0 ] = m_TileDesign;
		m_TilesBank[ m_SelTile.x() ][ 1 ] = m_TileDesign;
		m_TilesBank[ m_SelTile.x() ][ 2 ] = m_TileDesign;
		UpdateBank( m_SelTile.x(), 0 );
	} else {
		m_Undo.clear();
		m_Undo<<"TOBANK"<<"ONEBANK"<<m_SelTile.x()<<m_SelTile.y();
		m_UndoTile = m_TilesBank[ m_SelTile.x() ] [ m_SelTile.y() ];
		m_TilesBank[ m_SelTile.x() ][ m_SelTile.y() ] = m_TileDesign;	
		UpdateBank( m_SelTile.x(), m_SelTile.y() );
	}
}

void CScreenW::OnRadBack() {
    if( m_SelColor.y() == PAL_ALL ) {
        m_pLblPalBack->setFrameShape(QFrame::Box);
        m_pLblPalBack->setFrameShadow(QFrame::Raised);
        m_pLblPalFore->setFrameShape(QFrame::NoFrame);
        m_pLblPalFore->setFrameShadow(QFrame::Plain);
    }
    m_SelColor = QPoint( 0, m_SelColor.y() );
    SetTileDesign();
}

void CScreenW::OnRadFore() {
    if( m_SelColor.y() == PAL_ALL ) {
        m_pLblPalBack->setFrameShape(QFrame::NoFrame);
        m_pLblPalBack->setFrameShadow(QFrame::Plain);
        m_pLblPalFore->setFrameShape(QFrame::Box);
        m_pLblPalFore->setFrameShadow(QFrame::Raised);
    }
    m_SelColor = QPoint( 1, m_SelColor.y() );
    SetTileDesign();
}

void CScreenW::OnTileClear() {
    m_TileDesign.Reset();
    SetTileDesign();
}

void CScreenW::OnTileReorder() {
    m_TileDesign.Reorder();
    SetTileDesign();
}

void CScreenW::OnTileShiftDown() {
    m_TileDesign.ShiftDown();
    SetTileDesign();
}

void CScreenW::OnTileShiftUp() {
    m_TileDesign.ShiftUp();
    SetTileDesign();
}

void CScreenW::OnTileShiftLeft() {
    m_TileDesign.ShiftLeft();
    SetTileDesign();
}

void CScreenW::OnTileShiftRight() {
    m_TileDesign.ShiftRight();
    SetTileDesign();
}

void CScreenW::OnTileFlipHorizontal() {
    m_TileDesign.FlipHorizontal();
    SetTileDesign();
}

void CScreenW::OnTileFlipVertical() {
    m_TileDesign.FlipVertical();
    SetTileDesign();
}

void CScreenW::OnTileInvertColors() {
    int i;
    for( i = 0; i < 8; i++ ) m_TileDesign.InvertRow( i );
    SetTileDesign();
}

void CScreenW::OnTileRotate() {
    m_TileDesign.Rotate();
    SetTileDesign();
}

void CScreenW::SetTileDesign() {
    int y;
    QImage image;
    image = QImage( 96, 96, QImage::Format_RGB32 );
    PaintTile( &image, m_TileDesign, 0, 0, 12 );
    m_pLblTile->put_Image( image );
    image = QImage( 42, 98, QImage::Format_RGB32 );
    for( y = 0; y < 8; y++ ) {
        CSupportFuncs::SetPixelRect( &image, 0, y*12, COLORS_TABLE_SCREEN[m_TileDesign.GetBgColor( y )], 20, 12 );
        CSupportFuncs::SetPixelRect( &image, 20, y*12, COLORS_TABLE_SCREEN[m_TileDesign.GetForeColor( y )], 20, 12 );
        CSupportFuncs::SetBorderImage( &image, 0, y*12, 20, 12, 0xC0C0C0 );
        CSupportFuncs::SetBorderImage( &image, 20, y*12, 20, 12, 0xC0C0C0 );
    }
    if( m_SelColor.y() != PAL_ALL ) {
        uint col;
        if( m_SelColor.x() == 0 )
            col = COLORS_TABLE_SCREEN[m_TileDesign.GetBgColor( m_SelColor.y() )];
        else
            col = COLORS_TABLE_SCREEN[m_TileDesign.GetForeColor( m_SelColor.y() )];
        col = ~col;
        col |= 0xFF000000;
        CSupportFuncs::SetBorderImage( &image, m_SelColor.x() * 20 + 3, m_SelColor.y()*12 + 3, 14, 6, col );
    }
    m_pLblPalette->put_Image( image );
}

void CScreenW::CopyToTileDesign( int index, int bank ) {
    m_TileDesign = m_TilesBank[index][bank];
    SetTileDesign();
}

// ---------------------------------------------------------------
// Diálogo de cambiar colores y un solo banco ( m_pFrmColorsOneBank )
// ---------------------------------------------------------------

// Diálogo de intercambio de colores, realmente intercambia las baldosas
void CScreenW::OnChangeColors() {
    CChangeColors *pChangeColors = new CChangeColors( this );
    pChangeColors->exec();
    if( pChangeColors->result() == 0 ) {
        delete pChangeColors;
        return;
    }
    QList<QPoint> changeColors = pChangeColors->get_Changes();
    delete pChangeColors;
    CTile tile;
    QPoint c;
    for( int b = 0; b < 3; b++ ) {
        for( int j = 0; j < 256; j++ ) {
            tile = m_TilesBank[j][b];
            tile.ChangeColors( changeColors );
            m_TilesBank[j][b] = tile;
        }
    }
    InitBanks();
}


// ---------------------------------------------------------------
// Tiles Block of (m_pFrmTilesBlocksOf)
// ---------------------------------------------------------------
void CScreenW::OnTilesBlockChanged( int ) {
    InitBanks();
}

// ---------------------------------------------------------------
// m_pFrmBackgroundTiles
// ---------------------------------------------------------------

//TODO - 1 Deshabilitat. Per mes endavant.
int CScreenW::GetBackgroundTile( int y ) {
    if( ui->m_pGrOneBank->isChecked() ) return ui->m_pGrBackgroundTiles0->currentText().toLongLong();
    switch( y ) {
        case 0: return ui->m_pGrBackgroundTiles0->currentText().toLongLong();
        case 1: return ui->m_pGrBackgroundTiles1->currentText().toLongLong();
        case 2: return ui->m_pGrBackgroundTiles2->currentText().toLongLong();
        default: Q_ASSERT( false ); return 0;
    }
}

// ---------------------------------------------------------------
// Funciones Auxiliares Privadas:
// ---------------------------------------------------------------

void CScreenW::PaintTile( QImage *pImage, CTile &tile, int posX, int posY, int dotSize ) {
	int x, y;
    for( y = 0; y < 8; y++ ) {
        for( x = 0; x < 8; x++ ) {
            CSupportFuncs::SetPixelRect( pImage, posX + x*dotSize, posY + y*dotSize, COLORS_TABLE_SCREEN[tile.GetPixelColor( x, y )], dotSize, dotSize );
		}
	}
}

void CScreenW::PaintGrid( QImage *pImage, int gridWidth, int gridHeight ) {
	int i, j;
    for( i = 1; i < pImage->height(); i+= ( gridHeight+ 2 ) ) {
        for( j = 0; j < pImage->width(); j++ ) {
			pImage->setPixel( j, i-1, 0xC0C0C0 );	
			pImage->setPixel( j, i+gridHeight, 0xC0C0C0 );	
		}
	}
    for( i = 1; i < pImage->width(); i+= ( gridWidth + 2 ) ) {
        for( j = 0; j < pImage->height(); j++ )  {
			pImage->setPixel( i-1, j, 0xC0C0C0 );	
			pImage->setPixel( i+gridWidth, j, 0xC0C0C0 );	
		}
	}
	
}

int CScreenW::GetColor( unsigned int color, unsigned int palette[] ) {
    for( int i = 0; i < 16; i++ ) {
        if( color == palette[i] ) return i;
    } // Color not in palette
    return 0;
}

bool CScreenW::DeduceColorPalette( unsigned int color, unsigned int palette[16] ) {
    int i;
    unsigned int tmp;
    unsigned int paltmp[3];
    if( color >= 9000000 && color <= 17000000 ) { //RED 6, 8, 9
        paltmp[0] = palette[6];
        paltmp[1] = palette[8];
        paltmp[2] = palette[9];
        for( i = 2; i >=0; i-- ) {
            if( color == paltmp[i] ) break;
            if( color > paltmp[i] ) {
                tmp = paltmp[i];
                paltmp[i] = color;
                color = tmp;
                i = 3;
            }
        }
        palette[6] = paltmp[0];
        palette[8] = paltmp[1];
        palette[9] = paltmp[2];
    }
    return true;
}

void CScreenW::CheckImagePalette( QString fileName, unsigned int palette[] ) {
    QImage image;
    int i;
    int x, y;
    if( !image.load( fileName ) ) return;
    for( y = 0; y < image.height(); y++ ) {
        for( x = 0; x < image.width(); x++ )  {
            for( i = 0; i < 16; i++ ) if( image.pixel( x, y ) == palette[i] ) break;

            if( i == 16 ) break;
        }
        if( x < image.width() ) break;
    }
    if( y == image.height() ) return;
    //Generate palette
    for( i = 0; i < 16; i++ ) palette[i] = 65000;//TODO - 1
    for( y = 0; y < image.height(); y++ ) {
        for( x = 0; x < image.width(); x++ )  {
            for( i = 0; i < 16; i++ ) {
                if( image.pixel( x, y ) == palette[i] ) break;
            }
            if( i == 16 ) {
                for( i = 0; i < 16; i++ ) {
                    if( palette[i] == 65000 ) { //TODO - 1
                        palette[i] = image.pixel( x, y );
                        break;
                    }
                }
            }
        }
    }
}



// --------------------------------------------------------------
// COSAS QUITADAS: ( TRASTERO )
// --------------------------------------------------------------

/*
bool CScreenW::ImportPolkaTiles( QString fileName ) {
    //7 bytes cabecera
    //256*8 Patterns
    //256*8 back|fore
    int i, b;
    int x, y;
    int value;
    unsigned char buffer[256*8];
    FILE *myFile;
    myFile = fopen( qPrintable( fileName ), "rb" );
    NewTiles();
    if( fread( (char*)buffer, 7, 1, myFile ) != 7 ) return false;
    fread( (char*)buffer, 2048, 1, myFile );
    for( i = 0; i < 256; i++ ) {
        for( y = 0; y < 8; y++ ) {
            for( x = 0; x < 8; x++ ) {
                value = ( buffer[ i*8 + y ] >> x ) & 0x1;
                m_TilesBank[i][0].SetPixel( x, y, value );
            }
        }
    }
    fread( (char*)buffer, 2048, 1, myFile );
    for( i = 0; i < 256; i++ ) {
        for( y = 0; y < 8; y++ ) {
            m_TilesBank[i][0].SetBackColor( y, ( buffer[i*8 + y] >> 4 ) & 0xF );
            m_TilesBank[i][0].SetForeColor( y, ( buffer[i*8 + y] ) & 0xF );
        }
    }
    for( b = 0; b < 3; b++ )
        for( i = 0; i < 32*8; i++ )
            m_Screen[m_MapScreenX][m_MapScreenY][i+b*32*8]=i;
    fclose( myFile );
    InitBanks();
    return true;
}

bool CScreenW::ExportPolkaTiles( QString fileName ) {
    FILE *pFile;
    int i, y, x;
    unsigned char buffer[256*8];
    if( !( pFile = fopen( qPrintable( fileName ), "wb" ) ) ) return false;
    sprintf( (char*)buffer, "%s", "POLKA" );
    buffer[5] = 1;
    buffer[6] = 0;
    fwrite( buffer, sizeof( unsigned char ), 7, pFile );
    for( i = 0; i < 256; i++ ) {
        for( y = 0; y < 8; y++ ) {
            buffer[ i*8 + y ] = 0;
            for( x = 0; x < 8; x++ ) {
                buffer[ i*8 + y ] |= ( m_TilesBank[i][0].GetPixel( x, y ) << x );
            }
        }
    }
    fwrite( buffer, sizeof( unsigned char ), 2048, pFile );
    for( i = 0; i < 256; i++ ) {
        for( y = 0; y < 8; y++ ) {
            buffer[i*8+y] = ( (m_TilesBank[i][0].GetBgColor(y )<<4 ) | (m_TilesBank[i][0].GetForeColor(y) ) );
        }
    }
    fwrite( buffer, sizeof( unsigned char), 2048, pFile );
    buffer[0] = 0; //Num of Palettes
    fwrite( buffer, sizeof( unsigned char ), 1, pFile );
//	[0][0][0][0]
    //[17][6]
    //[51][7]
    //[23][1][39][3]
    //[97][1][39][6][113][1]
    //[115][3][97][6]
    //[100][6][17][4][101][2][85][5][119][7]
    buffer[ 0] = buffer[1] = buffer[2] = buffer[3] = 0;
    buffer[ 4] = 17;	buffer[ 5] = 6;
    buffer[ 6] = 51;	buffer[ 7] = 7;
    buffer[ 8] = 23;	buffer[ 9] = 1;
    buffer[10] = 39;	buffer[11] = 3;
    buffer[12] = 97;	buffer[13] = 1;
    buffer[14] = 39;	buffer[15] = 6;
    buffer[16] =113;	buffer[17] = 1;
    buffer[18] =115;	buffer[19] = 3;
    buffer[20] = 97;	buffer[21] = 6;
    buffer[22] =100;	buffer[23] = 6;
    buffer[24] = 17;	buffer[25] = 4;
    buffer[26] =101;	buffer[27] = 2;
    buffer[28] = 85;	buffer[29] = 5;
    buffer[30] =119;	buffer[31] = 7;
    fwrite( buffer, sizeof( unsigned char ), 32, pFile );
    for( i = 0; i < 256; i++ ) buffer[i] = 1;
    fwrite( buffer, sizeof( unsigned char ), 256, pFile );
    fclose( pFile );
    return true;
}
*/

/*
bool CScreenW::GenPNGPaletteFile( QString fileName ) {
    QImage image;
    image = QImage( 256, 192, QImage::Format_RGB32 );
    for( int i = 0; i < 16; i++ ) {
        image.setPixel( i, 0, COLORS_TABLE_SCREEN[i]);
    }
    return image.save( fileName );
}
*/

/*
bool CScreenW::ImportTilesPNGAdv( QString fileName ) {
    int x, y, i, b;
    int fore, back;
    unsigned int color;
    unsigned int rawColor;
    QImage image;
    unsigned int palette[16];
    //Llegir fitxer i muntar paleta
    if( !image.load( fileName ) ) return false;
    for( i = 0; i < 16; i++ ) palette[i] = 0;
    for( y = 0; y < image.height(); y++ ) {
        for( x = 0; x < image.width(); x++) {
            color = image.pixel( x, y );
            if( color > 0xff000000 ) color -= 0xff000000; //Treu transparencia si hi ha

            DeduceColorPalette( color, palette );
        }
    }
    if( !image.load( fileName ) ) return false;
    if( image.width() < 256 || image.height() < 64 ) return false;
    for( b = 0; b < 3; b++ ) {
        if( b > 0 && image.height() <= 64 ) break;
        for( y = 0; y < 8*8; y++ ) {
            for( x = 0; x < 32; x++ ) {
                fore = back = -1;
                for( i = 0; i < 8; i++ ) {
                    rawColor = image.pixel( x*8+i, y+b*64 );
                    if( rawColor > 0xff000000 ) rawColor -= 0xff000000; //Treu transparencia si hi ha
                    color = GetColor( rawColor, palette );
                    if( fore == -1  ) {
                        fore = color;
                        m_TilesBank[(y/8)*32+x][b].put_ForeColor( y%8, fore );
                        m_TilesBank[(y/8)*32+x][b].put_Pixel( i, y%8, 1 );
                    } else if( fore == color ) {
                        m_TilesBank[(y/8)*32+x][b].put_Pixel( i, y%8, 1 );
                    } else if( back == -1 ) {
                        back = color;
                        m_TilesBank[(y/8)*32+x][b].put_BgColor( y%8, back );
                        m_TilesBank[(y/8)*32+x][b].put_Pixel( i, y%8, 0 );
                    } else if( back == color ) {
                        m_TilesBank[(y/8)*32+x][b].put_Pixel( i, y%8, 0 );
                    } else //Three colors on a row! {
                        m_TilesBank[(y/8)*32+x][b].put_ForeColor( y%8, fore );
                    }
                }
            }
        }
    }
    InitBanks();
    return true;
}
*/

/*
void CScreenW::OnBankTileClickLeft( int x, int y ) {
    int mX, mY;
//	mX = m_pLblBankTile->x() - m_pTabBank->x() - m_pFrmBank[0]->x() - 4;
//	mY = m_pLblBankTile->y() - m_pTabBank->y() - m_pFrmBank[0]->y() - 25;
    qDebug()<<"CScreenW::OnBankTileClickedLEft currentTab="<<m_pTabBank->currentIndex();
    OnBankClick( mX, mY, m_pTabBank->currentIndex() );
}
void CScreenW::OnBankTileDoubleClickLeft( int , int  ) {
    OnFromBank();
}
void CScreenW::OnBankReleaseLeft( int, int ) {
    m_DragDrop = false;
}
void CScreenW::OnBankTileMouseMoveEvent( int , int  ) {
    int difX;
    int difY;
    int modd;
    difX = m_LastBankTilePos.x() - x;
    difY = m_LastBankTilePos.y() - y;
    m_pLblBankTile->setGeometry( m_pLblBankTile->x() - difX, m_pLblBankTile->y() - difY, m_pLblBankTile->width(), m_pLblBankTile->height() );
    m_LastBankTilePos = QPoint( x + difX, y + difY );
    OnScreenMouseMoveEvent( m_pLblBankTile->x() - m_pFrmScreen->x(), m_pLblBankTile->y() - m_pFrmScreen->y() );
}
void CScreenW::OnBank0MouseMoveEvent( int x, int y ) {
    OnBankMouseMoveEvent( x, y, 0 );
}
void CScreenW::OnBank1MouseMoveEvent( int x, int y ) {
    OnBankMouseMoveEvent( x, y, 1 );
}
void CScreenW::OnBank2MouseMoveEvent( int x, int y ) {
    OnBankMouseMoveEvent( x, y, 2 );
}
void CScreenW::OnBankMouseMoveEvent( int x, int y, int bank ) {
    int mX, mY;
    int bX, bY;
    bX = mX = ( x / 18 ) * 18;
    mX += ( m_pTabBank->x() + m_pFrmBank[bank]->x() + 4 ); //4 Bordes
    bY = mY = ( y / 18 ) * 18;
    mY += ( m_pTabBank->y() + m_pFrmBank[bank]->y() + 25 ); //25 Pestanyes
    m_pLblBankTile->setGeometry( mX, mY, 17, 17 );
    m_pLblBankTile->put_Image( m_BankImages[bank].copy( bX, bY, 17, 17 ) );
    m_pLblBankTile->setVisible( true );
}
*/
