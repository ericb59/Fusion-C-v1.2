#include "changecolors.h"
#include "ui_fchangecolors.h"
#include "supportfuncs.h"

CChangeColors::CChangeColors(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CChangeColors)
{
    ui->setupUi(this);
    p_screen = static_cast<CScreenW*>(parent);

    for( int i = 0; i < 16; i++ ) AssignToCombo( ui->m_pGrChange0, i );
    for( int i = 0; i < 16; i++ ) AssignToCombo( ui->m_pGrChange1, i );
    for( int i = 0; i < 16; i++ ) AssignToCombo( ui->m_pGrChange2, i );
    for( int i = 0; i < 16; i++ ) AssignToCombo( ui->m_pGrChange3, i );
    for( int i = 0; i < 16; i++ ) AssignToCombo( ui->m_pGrChange4, i );
    for( int i = 0; i < 16; i++ ) AssignToCombo( ui->m_pGrChange5, i );
    for( int i = 0; i < 16; i++ ) AssignToCombo( ui->m_pGrChange6, i );
    for( int i = 0; i < 16; i++ ) AssignToCombo( ui->m_pGrChange7, i );
    for( int i = 0; i < 16; i++ ) AssignToCombo( ui->m_pGrChange8, i );
    for( int i = 0; i < 16; i++ ) AssignToCombo( ui->m_pGrChange9, i );
    for( int i = 0; i < 16; i++ ) AssignToCombo( ui->m_pGrChange10, i );
    for( int i = 0; i < 16; i++ ) AssignToCombo( ui->m_pGrChange11, i );
    for( int i = 0; i < 16; i++ ) AssignToCombo( ui->m_pGrChange12, i );
    for( int i = 0; i < 16; i++ ) AssignToCombo( ui->m_pGrChange13, i );
    for( int i = 0; i < 16; i++ ) AssignToCombo( ui->m_pGrChange14, i );
    for( int i = 0; i < 16; i++ ) AssignToCombo( ui->m_pGrChange15, i );

    ui->m_pGrChange0->setCurrentIndex( 0 );
    ui->m_pGrChange1->setCurrentIndex( 1 );
    ui->m_pGrChange2->setCurrentIndex( 2 );
    ui->m_pGrChange3->setCurrentIndex( 3 );
    ui->m_pGrChange4->setCurrentIndex( 4 );
    ui->m_pGrChange5->setCurrentIndex( 5 );
    ui->m_pGrChange6->setCurrentIndex( 6 );
    ui->m_pGrChange7->setCurrentIndex( 7 );
    ui->m_pGrChange8->setCurrentIndex( 8 );
    ui->m_pGrChange9->setCurrentIndex( 9 );
    ui->m_pGrChange10->setCurrentIndex( 10 );
    ui->m_pGrChange11->setCurrentIndex( 11 );
    ui->m_pGrChange12->setCurrentIndex( 12 );
    ui->m_pGrChange13->setCurrentIndex( 13 );
    ui->m_pGrChange14->setCurrentIndex( 14 );
    ui->m_pGrChange15->setCurrentIndex( 15 );

    QPixmap pixmap( 16, 16 );

    pixmap.fill( QColor( p_screen->COLORS_TABLE_SCREEN[0] ) );
    ui->m_pGrColor0->setPixmap( pixmap );
    pixmap.fill( QColor( p_screen->COLORS_TABLE_SCREEN[1] ) );
    ui->m_pGrColor1->setPixmap( pixmap );
    pixmap.fill( QColor( p_screen->COLORS_TABLE_SCREEN[2] ) );
    ui->m_pGrColor2->setPixmap( pixmap );
    pixmap.fill( QColor( p_screen->COLORS_TABLE_SCREEN[3] ) );
    ui->m_pGrColor3->setPixmap( pixmap );
    pixmap.fill( QColor( p_screen->COLORS_TABLE_SCREEN[4] ) );
    ui->m_pGrColor4->setPixmap( pixmap );
    pixmap.fill( QColor( p_screen->COLORS_TABLE_SCREEN[5] ) );
    ui->m_pGrColor5->setPixmap( pixmap );
    pixmap.fill( QColor( p_screen->COLORS_TABLE_SCREEN[6] ) );
    ui->m_pGrColor6->setPixmap( pixmap );
    pixmap.fill( QColor( p_screen->COLORS_TABLE_SCREEN[7] ) );
    ui->m_pGrColor7->setPixmap( pixmap );
    pixmap.fill( QColor( p_screen->COLORS_TABLE_SCREEN[8] ) );
    ui->m_pGrColor8->setPixmap( pixmap );
    pixmap.fill( QColor( p_screen->COLORS_TABLE_SCREEN[9] ) );
    ui->m_pGrColor9->setPixmap( pixmap );
    pixmap.fill( QColor( p_screen->COLORS_TABLE_SCREEN[10] ) );
    ui->m_pGrColor10->setPixmap( pixmap );
    pixmap.fill( QColor( p_screen->COLORS_TABLE_SCREEN[11] ) );
    ui->m_pGrColor11->setPixmap( pixmap );
    pixmap.fill( QColor( p_screen->COLORS_TABLE_SCREEN[12] ) );
    ui->m_pGrColor12->setPixmap( pixmap );
    pixmap.fill( QColor( p_screen->COLORS_TABLE_SCREEN[13] ) );
    ui->m_pGrColor13->setPixmap( pixmap );
    pixmap.fill( QColor( p_screen->COLORS_TABLE_SCREEN[14] ) );
    ui->m_pGrColor14->setPixmap( pixmap );
    pixmap.fill( QColor( p_screen->COLORS_TABLE_SCREEN[15] ) );
    ui->m_pGrColor15->setPixmap( pixmap );

    connect( ui->m_pGrOk, SIGNAL( clicked() ), this, SLOT( OnOk() ) );
    connect( ui->m_pGrCancel, SIGNAL( clicked() ), this, SLOT( OnCancel() ) );

    //TODO -1 to improve when I have more time...
    connect( ui->m_pGrChange0, SIGNAL( currentIndexChanged(int)), this, SLOT( OnColor0Changed( int ) ) );
    connect( ui->m_pGrChange1, SIGNAL( currentIndexChanged(int)), this, SLOT( OnColor1Changed( int ) ) );
    connect( ui->m_pGrChange2, SIGNAL( currentIndexChanged(int)), this, SLOT( OnColor2Changed( int ) ) );
    connect( ui->m_pGrChange3, SIGNAL( currentIndexChanged(int)), this, SLOT( OnColor3Changed( int ) ) );
    connect( ui->m_pGrChange4, SIGNAL( currentIndexChanged(int)), this, SLOT( OnColor4Changed( int ) ) );
    connect( ui->m_pGrChange5, SIGNAL( currentIndexChanged(int)), this, SLOT( OnColor5Changed( int ) ) );
    connect( ui->m_pGrChange6, SIGNAL( currentIndexChanged(int)), this, SLOT( OnColor6Changed( int ) ) );
    connect( ui->m_pGrChange7, SIGNAL( currentIndexChanged(int)), this, SLOT( OnColor7Changed( int ) ) );
    connect( ui->m_pGrChange8, SIGNAL( currentIndexChanged(int)), this, SLOT( OnColor8Changed( int ) ) );
    connect( ui->m_pGrChange9, SIGNAL( currentIndexChanged(int)), this, SLOT( OnColor9Changed( int ) ) );
    connect( ui->m_pGrChange10, SIGNAL( currentIndexChanged(int)), this, SLOT( OnColor10Changed( int ) ) );
    connect( ui->m_pGrChange11, SIGNAL( currentIndexChanged(int)), this, SLOT( OnColor11Changed( int ) ) );
    connect( ui->m_pGrChange12, SIGNAL( currentIndexChanged(int)), this, SLOT( OnColor12Changed( int ) ) );
    connect( ui->m_pGrChange13, SIGNAL( currentIndexChanged(int)), this, SLOT( OnColor13Changed( int ) ) );
    connect( ui->m_pGrChange14, SIGNAL( currentIndexChanged(int)), this, SLOT( OnColor14Changed( int ) ) );
    connect( ui->m_pGrChange15, SIGNAL( currentIndexChanged(int)), this, SLOT( OnColor15Changed( int ) ) );

    m_AutoChanging = 0;

}

CChangeColors::~CChangeColors()
{
    delete ui;
}


QList<QPoint> CChangeColors::get_Changes() {
    return m_Changes;
}

void CChangeColors::AssignToCombo( QComboBox *pCombo, int color ) {
//  QPixmap pixmap;
//  pixmap.load( QString( ":/new/res/res/color%1.png" ).arg( color ) );
//  pCombo->addItem( QIcon( pixmap ), QString( "%1" ).arg( color ) );

    QPixmap pixmap( 16, 16 );
    pixmap.fill( QColor( p_screen->COLORS_TABLE_SCREEN[color] ) );

    pCombo->addItem( QIcon( pixmap ), QString( "%1" ).arg( color ) );
}


void CChangeColors::OnOk() {
    if( ui->m_pGrChange0->currentIndex() != 0 ) m_Changes<<QPoint( 0, ui->m_pGrChange0->currentIndex() );
    if( ui->m_pGrChange1->currentIndex() != 1 ) m_Changes<<QPoint( 1, ui->m_pGrChange1->currentIndex() );
    if( ui->m_pGrChange2->currentIndex() != 2 ) m_Changes<<QPoint( 2, ui->m_pGrChange2->currentIndex() );
    if( ui->m_pGrChange3->currentIndex() != 3 ) m_Changes<<QPoint( 3, ui->m_pGrChange3->currentIndex() );
    if( ui->m_pGrChange4->currentIndex() != 4 ) m_Changes<<QPoint( 4, ui->m_pGrChange4->currentIndex() );
    if( ui->m_pGrChange5->currentIndex() != 5 ) m_Changes<<QPoint( 5, ui->m_pGrChange5->currentIndex() );
    if( ui->m_pGrChange6->currentIndex() != 6 ) m_Changes<<QPoint( 6, ui->m_pGrChange6->currentIndex() );
    if( ui->m_pGrChange7->currentIndex() != 7 ) m_Changes<<QPoint( 7, ui->m_pGrChange7->currentIndex() );
    if( ui->m_pGrChange8->currentIndex() != 8 ) m_Changes<<QPoint( 8, ui->m_pGrChange8->currentIndex() );
    if( ui->m_pGrChange9->currentIndex() != 9 ) m_Changes<<QPoint( 9, ui->m_pGrChange9->currentIndex() );
    if( ui->m_pGrChange10->currentIndex() != 10 ) m_Changes<<QPoint( 10, ui->m_pGrChange10->currentIndex() );
    if( ui->m_pGrChange11->currentIndex() != 11 ) m_Changes<<QPoint( 11, ui->m_pGrChange11->currentIndex() );
    if( ui->m_pGrChange12->currentIndex() != 12 ) m_Changes<<QPoint( 12, ui->m_pGrChange12->currentIndex() );
    if( ui->m_pGrChange13->currentIndex() != 13 ) m_Changes<<QPoint( 13, ui->m_pGrChange13->currentIndex() );
    if( ui->m_pGrChange14->currentIndex() != 14 ) m_Changes<<QPoint( 14, ui->m_pGrChange14->currentIndex() );
    if( ui->m_pGrChange15->currentIndex() != 15 ) m_Changes<<QPoint( 15, ui->m_pGrChange15->currentIndex() );

    accept();
}

void CChangeColors::OnCancel() {
    close();
}

void CChangeColors::ChangeColor( int combo, int index ) {
    switch( index ) {
        case 0: ui->m_pGrChange0->setCurrentIndex( combo ); break;
        case 1: ui->m_pGrChange1->setCurrentIndex( combo ); break;
        case 2: ui->m_pGrChange2->setCurrentIndex( combo ); break;
        case 3: ui->m_pGrChange3->setCurrentIndex( combo ); break;
        case 4: ui->m_pGrChange4->setCurrentIndex( combo ); break;
        case 5: ui->m_pGrChange5->setCurrentIndex( combo ); break;
        case 6: ui->m_pGrChange6->setCurrentIndex( combo ); break;
        case 7: ui->m_pGrChange7->setCurrentIndex( combo ); break;
        case 8: ui->m_pGrChange8->setCurrentIndex( combo ); break;
        case 9: ui->m_pGrChange9->setCurrentIndex( combo ); break;
        case 10: ui->m_pGrChange10->setCurrentIndex( combo ); break;
        case 11: ui->m_pGrChange11->setCurrentIndex( combo ); break;
        case 12: ui->m_pGrChange12->setCurrentIndex( combo ); break;
        case 13: ui->m_pGrChange13->setCurrentIndex( combo ); break;
        case 14: ui->m_pGrChange14->setCurrentIndex( combo ); break;
        case 15: ui->m_pGrChange15->setCurrentIndex( combo ); break;
    }
}

void CChangeColors::OnColor0Changed( int ) {
    ChangeColor( 0, ui->m_pGrChange0->currentIndex());
}

void CChangeColors::OnColor1Changed( int ) {
    ChangeColor( 1, ui->m_pGrChange1->currentIndex());
}
void CChangeColors::OnColor2Changed( int ) {
    ChangeColor( 2, ui->m_pGrChange2->currentIndex());
}
void CChangeColors::OnColor3Changed( int ) {
    ChangeColor( 3, ui->m_pGrChange3->currentIndex());
}
void CChangeColors::OnColor4Changed( int ) {
    ChangeColor( 4, ui->m_pGrChange4->currentIndex());
}
void CChangeColors::OnColor5Changed( int ) {
    ChangeColor( 5, ui->m_pGrChange5->currentIndex());
}
void CChangeColors::OnColor6Changed( int ) {
    ChangeColor( 6, ui->m_pGrChange6->currentIndex());
}
void CChangeColors::OnColor7Changed( int ) {
    ChangeColor( 7, ui->m_pGrChange7->currentIndex());
}
void CChangeColors::OnColor8Changed( int ) {
    ChangeColor( 8, ui->m_pGrChange8->currentIndex());
}
void CChangeColors::OnColor9Changed( int ) {
    ChangeColor( 9, ui->m_pGrChange9->currentIndex());
}
void CChangeColors::OnColor10Changed( int ) {
    ChangeColor( 10, ui->m_pGrChange10->currentIndex());
}
void CChangeColors::OnColor11Changed( int ) {
    ChangeColor( 11, ui->m_pGrChange11->currentIndex());
}
void CChangeColors::OnColor12Changed( int ) {
    ChangeColor( 12, ui->m_pGrChange12->currentIndex());
}
void CChangeColors::OnColor13Changed( int ) {
    ChangeColor( 13, ui->m_pGrChange13->currentIndex());
}
void CChangeColors::OnColor14Changed( int ) {
    ChangeColor( 14, ui->m_pGrChange14->currentIndex());
}

void CChangeColors::OnColor15Changed( int ) {
    ChangeColor( 15, ui->m_pGrChange15->currentIndex());
}
