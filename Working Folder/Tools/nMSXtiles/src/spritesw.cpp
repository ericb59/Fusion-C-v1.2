#include "spritesw.h"
#include "supportfuncs.h"
#include <QPixmap>
#include <QImage>
#include <QFile>
#include <QDebug>

CSpritesSet::CSpritesSet()
{
	int i;
	int sprite;


	for( sprite = 0; sprite < 4; sprite++ )	
	{
		for( i = 0; i < 256; i++ ) m_Set[sprite][i] = 0;
		m_Colors[sprite] = 0;
	}
}

void CSpritesSet::SetColor( int sprite, int color )
{
	m_Colors[sprite] = color;
}

int CSpritesSet::GetColor( int sprite )
{
	return m_Colors[sprite];
}

int CSpritesSet::GetValue( int sprite, int index )
{
	return m_Set[sprite][index];
}

void CSpritesSet::SetValue( int sprite, int index, int value )
{
	int i;
	
	
	for( i = 0; i < 4; i++ )
	{
		if( i == sprite ) m_Set[i][index] = value;
		else 
		{
			if( value )	m_Set[i][index] = 0;
		}
	}
}


const int CSpritesW::BG_COLOR = 0xffd8dbde;

// CSpritesW::CSpritesW()
CSpritesW::CSpritesW( QWidget *parent ) :
    QDialog( parent )
{
    setupUi( this );
    setFixedWidth( 354 );
    setFixedHeight( 373 );
    for( int i = 0; i < 16; i++ ) COLORS_TABLE_SPRITES[i] = CSupportFuncs::CONST_COLORS_TABLE[i];
}

CSpritesW::~CSpritesW() {
    // Close();
    // this->close();
    while( m_Sprites.size() )
    {
        delete m_Sprites.front();
        m_Sprites.removeFirst();
    }
    // delete ui;
}

void CSpritesW::closeEvent( QCloseEvent *pEvent )
{
    emit signal_close();
    pEvent->accept();
    // Close();
    /*
    if( m_Close ) pEvent->accept();
    else pEvent->ignore();
    */
}

void CSpritesW::Initialize()
{
	int i, j;
	QPixmap pixmap;
	QImage image;

	for( i = 0; i < 4; i++ )
	{
		m_pSprites[i] = new QComboBox( m_pFrmDesigner );
		m_pSprites[i]->setGeometry( 10 + i*80, 30, 69, 22 );	

		for( j = 0; j < 16; j++ ) 
		{
			image = QImage( 60, 22, QImage::Format_RGB32 );
            CSupportFuncs::SetPixelRect( &image, 0, 0, COLORS_TABLE_SPRITES[j], 60, 22 );
			pixmap = QPixmap::fromImage( image );
			m_pSprites[i]->addItem( QIcon( pixmap ), QString( "" ) );
		}

	}

	connect( m_pSprites[0], SIGNAL( currentIndexChanged( int ) ), this, SLOT( OnCombos0Click( int ) ) );
	connect( m_pSprites[1], SIGNAL( currentIndexChanged( int ) ), this, SLOT( OnCombos1Click( int ) ) );
	connect( m_pSprites[2], SIGNAL( currentIndexChanged( int ) ), this, SLOT( OnCombos2Click( int ) ) );
	connect( m_pSprites[3], SIGNAL( currentIndexChanged( int ) ), this, SLOT( OnCombos3Click( int ) ) );
	connect( m_pGrFlipH,    SIGNAL( clicked() ), this, SLOT( OnFlipHClick() ) );
    connect( m_pGrFlipV,    SIGNAL( clicked() ), this, SLOT( OnFlipVClick() ) );
    connect( m_pGrShiftDown, SIGNAL( clicked() ), this, SLOT( OnShiftDownClick() ) );
    connect( m_pGrShiftUp, SIGNAL( clicked() ), this, SLOT( OnShiftUpClick() ) );

	for( i = 1; i < 5; i++ ) m_pGrSets->addItem( QString( "Sprites Set %1" ).arg( i ) );
	connect( m_pGrSets, SIGNAL( currentIndexChanged( int ) ), this, SLOT( OnSetsClick( int ) ) );
	
	for( i = 0; i < 4; i++ )
	{
		m_pSelSprite[i] = new QRadioButton( QString( "Plane %1" ).arg( i ), m_pFrmDesigner );
		m_pSelSprite[i]->setGeometry( 20 + i*80, 10, 61, 16 );
        connect( m_pSelSprite[i], &QRadioButton::clicked, [=] { OnRadiosClick(i); });
	}

	image = QImage( 16*2, 16*2, QImage::Format_RGB32 );
	CSupportFuncs::SetPixelRect( &image, 0, 0, BG_COLOR, 16*2, 16*2 );
	for( i = 0; i < 4; i++ )
	{
		m_pLblViews[i] = new CLabelBtn( m_pFrmPreview, 16*2, 16*2 );
		m_pLblViews[i]->setGeometry( i*(16*2+5) + 5, 5, 16*2, 16*2 );
		m_pLblViews[i]->put_Image( image ); 
	}
	
// m_pLblSprites = new CLabelBtn( m_pFrmSprites, 96*2, 96*2 );
// Añadido para pintar arrastrando...
    m_pLblSprites = new CLabelBtnMove( m_pFrmSprites, 96*2, 96*2 );
	m_pLblSprites->setGeometry( 2, 2, 96*2, 96*2 );
	
	connect( m_pLblSprites, SIGNAL( onClickedLeft( int, int ) ), this, SLOT( OnSpritesClickLeft( int, int ) ) );
	connect( m_pLblSprites, SIGNAL( onClickedRight( int, int ) ), this, SLOT( OnSpritesClickRight( int, int ) ) );

		
	m_SelSet = 0;
	m_SelSprite = 0;
	
}
//AQUI vaciar la lista al cerrar ventana
void CSpritesW::NewSprites()
{
	int i;

	for( i = 0; i < 4; i++ ) m_Sprites.append( new CSpritesSet() );
	
	OnSetsClick( 0 );
}

bool CSpritesW::LoadSprites( QString fileName )
{
	int i;
	int s, t;
	QString line;
	QFile hFile;
	
	
	for( i = 0; i < 4; i++ ) m_Sprites.append( new CSpritesSet() );
	
	hFile.setFileName( fileName );
	hFile.open( QIODevice::ReadOnly );
	QTextStream str( &hFile );

	if( hFile.atEnd() ) 
	{
		hFile.close();
		return false;
	}
	
	if( str.readLine() != "SPRITES" )
	{
		hFile.close();
		return false;
	}

	for( s = 0; s < 4; s++ )
	{
		line = str.readLine();
		
		if( line != QString( "SET%1" ).arg( s ) )
		{
			hFile.close();
			return false;
		}

		for( t = 0; t < 4; t++ )
		{
			if( str.readLine() != QString( "SPRITE%1" ).arg( t ) )
			{
				hFile.close();
				return false;
			}
			
			m_Sprites.at( s )->SetColor( t, str.readLine().toInt() );
			
			for( i = 0; i < 256; i++ ) m_Sprites.at( s )->SetValue( t, i, str.readLine().toInt() );
		}
	}

	hFile.close();

	for( i = 3; i >=0; i-- ) OnSetsClick( i );

	return true;

}

bool CSpritesW::SaveSprites( QString fileName )
{
	int i, s, t;
	QFile hFile;
	

	hFile.setFileName( fileName );
	hFile.open( QIODevice::WriteOnly );

	QTextStream str( &hFile );
	
	str<<"SPRITES"<<endl;
	
	for( s = 0; s < 4; s++ )
	{
		str<<"SET"<<s<<endl;
		
		for( t = 0; t < 4; t++ )
		{
			str<<"SPRITE"<<t<<endl;
			str<<m_Sprites.at( s )->GetColor( t )<<endl;

			for( i = 0; i < 16*16; i++ ) str<<m_Sprites.at( s )->GetValue( t, i )<<endl;
		}
	}
	
	hFile.flush();
	hFile.close();
	
	return true;
}

bool CSpritesW::ExportSprites( QString fileName )
{
	int index;
	int value;
	int s, t;
	int x, y, j;
	QFile hFile;
	

	hFile.setFileName( fileName );
	hFile.open( QIODevice::WriteOnly );

	QTextStream str( &hFile );
	
	for( s = 0; s < 4; s++ )
	{
		str<<"****SET"<<(s+1)<<"****"<<endl;
		
		for( t = 0; t < 4; t++ )
		{
			str<<"SPRITE"<<t<<endl;
			str<<"Color: "<<m_Sprites.at( s )->GetColor( t )<<endl;
			
			for( j = 0; j < 4; j++ )
			{
				switch( j )
				{
					case 0: index = 0; break;
					case 1: index = 128; break;
					case 2: index = 8; break;
					case 3: index = 136; break;
					default: Q_ASSERT( false ); return false;
				}

				str<<"db ";
				
				for( y = 0; y < 8; y++, index+=8 )
				{
					for( x = 7, value = 0; x >= 0; x--, index++ )
					{
						value = value | ( m_Sprites.at( s )->GetValue( t, index ) << x );
					}
					str<<value;
					if( y != 7 ) str<<", ";
				}
				str<<endl;
 			}

		}
	}
	
	hFile.flush();
	hFile.close();
	
	return true;

}

void CSpritesW::OnSpritesClickLeft( int x, int y )
{
	m_Sprites.at( m_SelSet )->SetValue( m_SelSprite, (y/12)*16 + (x/12), 1 );
	
	UpdateDesigner( m_SelSet, m_SelSprite, (y/12)*16 + (x/12), false );
	
	UpdatePreview( m_SelSet, m_SelSprite, (y/12)*16 + (x/12), false );
}

void CSpritesW::OnSpritesClickRight( int x, int y )
{
	if( m_Sprites.at( m_SelSet )->GetValue( m_SelSprite, (y/12)*16 + (x/12) ) == 0 ) return;
	
	m_Sprites.at( m_SelSet )->SetValue( m_SelSprite, (y/12)*16 + (x/12), 0 );

	UpdateDesigner( m_SelSet, m_SelSprite, (y/12)*16 + (x/12), true );
	
	UpdatePreview( m_SelSet, m_SelSprite, (y/12)*16 + (x/12 ), true );
}

void CSpritesW::OnCombos0Click( int index )
{
	OnCombosClick( 0, index );
}

void CSpritesW::OnCombos1Click( int index )
{
	OnCombosClick( 1, index );
}

void CSpritesW::OnCombos2Click( int index )
{
	OnCombosClick( 2, index );
}

void CSpritesW::OnCombos3Click( int index )
{
	OnCombosClick( 3, index );
}

void CSpritesW::OnCombosClick( int combo, int index )
{
	m_Sprites.at( m_SelSet )->SetColor( combo, index );
	m_pSelSprite[combo]->click();

}

void CSpritesW::OnRadiosClick( int index )
{
	m_SelSprite = index;
	UpdateDesigner( m_SelSet, m_SelSprite );
}

void CSpritesW::OnSetsClick( int index )
{
	int i;
		
	m_SelSet = index;
	
	for( i = 0; i < 4; i++ ) m_pSprites[i]->setCurrentIndex( m_Sprites.at( m_SelSet )->GetColor( i ) );
	
	UpdateDesigner( m_SelSet );
	m_pSelSprite[0]->click();
}

void CSpritesW::UpdateDesigner( int set )
{
	int i;
	int sprite;
	QImage image;
	
	
	image = QImage( 96*2, 96*2, QImage::Format_RGB32 );
	CSupportFuncs::SetPixelRect( &image, 0, 0, BG_COLOR, 96*2, 96*2 );
	
	m_pLblSprites->put_Image( image ); 
	
	for( i = 0; i < 4; i++ ) m_pSprites[i]->setCurrentIndex( m_Sprites.at( m_SelSet )->GetColor( i ) );
	
	for( sprite = 0; sprite < 4; sprite++ ) UpdateDesigner( set, sprite );
}

void CSpritesW::UpdateDesigner( int set, int sprite )
{
	int i;
	QImage image;

	
    image = m_pLblSprites->pixmap()->toImage();

    CSupportFuncs::SetPixelRect( &image, 0, 0, BG_COLOR, 96*2, 96*2 );
    //

    for( i = 0; i < 256; i++ ) UpdateDesigner( set, sprite, i, false, &image );
	
	m_pLblSprites->put_Image( image );
	
	image = m_pLblViews[set]->pixmap()->toImage();
	
	for( i = 0; i < 256; i++ ) UpdatePreview( set, sprite, i, false, &image );
	
	m_pLblViews[set]->put_Image( image );
}

void CSpritesW::UpdateDesigner( int set, int sprite, int index, bool paintIsZ, QImage *pImage )
{
	QImage image;
	QImage *pTmp;
	unsigned int color;
	
	
	if( !pImage )
	{
		image = m_pLblSprites->pixmap()->toImage();
		pTmp = &image;
	}
	else pTmp = pImage;
	
    if( paintIsZ ) color = BG_COLOR; // COLORS_TABLE_SPRITES[ 0 ];
    else if( m_Sprites.at( set )->GetValue( sprite, index ) == 1 ) color = COLORS_TABLE_SPRITES[ m_Sprites.at( set )->GetColor( sprite ) ];
	else return;
	
	CSupportFuncs::SetPixelRect( pTmp, (index%16)*12, (index/16)*12, color, 12, 12 );
	
	if( !pImage ) m_pLblSprites->put_Image( *pTmp );
}

void CSpritesW::UpdatePreview()
{
    QImage image;
    unsigned int color;


//    image = m_pLblViews[0]->pixmap()->toImage();
/*
    if( paintIsZ ) color = BG_COLOR; // COLORS_TABLE_SPRITES[0];
    else if( m_Sprites.at( set )->GetValue( sprite, index ) == 1 ) color = COLORS_TABLE_SPRITES[ m_Sprites.at( set )->GetColor( sprite ) ];
    else return;

    CSupportFuncs::SetPixelRect( pTmp, (index%16)*2, (index/16)*2, color, 2, 2 );
    */
    image = QImage( 96*2, 96*2, QImage::Format_RGB32 );
    //CSupportFuncs::SetPixelRect( &image, 0, 0, BG_COLOR, 96*2, 96*2 );
    for( int i = 0; i < 4; i++ )
    {
        for( int y = 0; y < 16; y++ )
        {
            for( int x = 0; x < 16; x++ )
            {
                color = COLORS_TABLE_SPRITES[ m_Sprites.at( 0 )->GetColor( i ) ];
                CSupportFuncs::SetPixelRect( &image, x, y, color, 2, 2 );
            }
        }
    }

    //m_Sprites.at( m_SelSet )->GetValue( m_SelSprite, (y/12)*16 + (x/12) )

    m_pLblViews[0]->put_Image( image );
}

void CSpritesW::UpdatePreview( int set, int sprite, int index, bool paintIsZ, QImage *pImage )
{
	QImage image;
	QImage *pTmp;
	unsigned int color;
	

	if( !pImage )
	{
		image = m_pLblViews[set]->pixmap()->toImage();
		pTmp = &image;
	}
	else pTmp = pImage;	
	
    if( paintIsZ ) color = BG_COLOR; // COLORS_TABLE_SPRITES[0];
    else if( m_Sprites.at( set )->GetValue( sprite, index ) == 1 ) color = COLORS_TABLE_SPRITES[ m_Sprites.at( set )->GetColor( sprite ) ];
	else return;
	
	CSupportFuncs::SetPixelRect( pTmp, (index%16)*2, (index/16)*2, color, 2, 2 );
	
	if( !pImage ) m_pLblViews[set]->put_Image( *pTmp );
}

void CSpritesW::OnFlipHClick()
{
	int i;
	int x, y;
	int values[16];
	
	
	for( i = 0; i < 4; i++ )
	{
		for( y = 0; y < 16; y++ )
		{
			for( x = 0; x < 16; x++ )
			{
				values[ x ] = m_Sprites.at( m_SelSet )->GetValue( i, y*16 + x );
			}
			for( x = 0; x < 16; x++ )
			{
				m_Sprites.at( m_SelSet )->SetValue( i, y*16 + x, values[15 - x] );
			}
		}
	}

    OnSetsClick( m_SelSet );
}

void CSpritesW::OnFlipVClick()
{
    int i;
    int x, y;
    int values[16];


    for( i = 0; i < 4; i++ )
    {
        for( x = 0; x < 16; x++ )
        {
            for( y = 0; y < 16; y++ )
            {
                values[ y ] = m_Sprites.at( m_SelSet )->GetValue( i, y*16 + x );
            }
            for( y = 0; y < 16; y++ )
            {
                m_Sprites.at( m_SelSet )->SetValue( i, (15-y)*16 + x, values[y] );
            }
        }
    }

    OnSetsClick( m_SelSet );
}

void CSpritesW::OnShiftDownClick()
{
    int i;
    int x, y;
    // int values[16];


    for( i = 0; i < 4; i++ )
    {
        for( y = 15; y > 0; y-- )
        {
            for( x = 0; x < 16; x++ )
            {
                m_Sprites.at( m_SelSet )->SetValue( i, y*16 + x, m_Sprites.at( m_SelSet )->GetValue( i, (y-1)*16 + x));
            }
        }
    }

    for( x = 0; x <16; x++ )
    {
        m_Sprites.at( m_SelSet )->SetValue( i, x, 0 );
    }

    OnSetsClick( m_SelSet );
}

void CSpritesW::OnShiftUpClick()
{
    int i;
    int x, y;
    // int values[16];


    for( i = 0; i < 4; i++ )
    {
        for( y = 0; y < 16; y++ )
        {
            for( x = 0; x < 16; x++ )
            {
                m_Sprites.at( m_SelSet )->SetValue( i, y*16 + x, m_Sprites.at( m_SelSet )->GetValue( i, (y+1)*16 + x));
            }
        }
    }

    for( x = 0; x <16; x++ )
    {
        m_Sprites.at( m_SelSet )->SetValue( i, 15*16+x, 0 );
    }

    OnSetsClick( m_SelSet );
}
