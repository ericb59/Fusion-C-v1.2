#include "changepalette.h"
#include "ui_changepalette.h"
#include "supportfuncs.h"
#include <QMessageBox>

changepalette::changepalette( QWidget *parent, uint pal[], uint index ) :
    QDialog(parent),
    ui(new Ui::changepalette)
{
    ui->setupUi(this);
    if( pal == NULL ) { close(); return; }
    m_pal = pal;
    // this->setWindowTitle( QString( "Change Palette:" ) );
    for( int i = 0; i < 16; i++ ) {
    // Creamos etiquetas:
        QLabel* lab = new QLabel( ui->frame_label );
        lab->setGeometry( i*22+8, 0, 22, 22 );
    // Creamos botones:
        lab->setText( QString::number( i, 16 ).toUpper() );
        m_color_button[i] = new QPushButton( ui->frame_button );
        m_color_button[i]->setGeometry( i*22, 0, 22, 22 );
     // Ponemos color a los botones:
        QPixmap pix( 20, 20 );
        pix.fill( QColor( m_pal[i] ) );
        m_color_button[i]->setIcon( QIcon( pix ) );
     // Conectamos:
        connect( m_color_button[i], &QPushButton::pressed,
            [=]{ on_color_click( int(i) ); }
        );
    }
    m_button = ( index < 16 ) ? int(index) : 0;
    m_color.setRgb( QRgb(m_pal[ m_button ]) );
    setSliders();
    setColor();
}

changepalette::~changepalette()
{
    delete ui;
}

void changepalette::on_slider_R_valueChanged(int value )
{
    ui->value_R->setText( QString( "%1" ).arg( value ) );
    m_color.setRed( CSupportFuncs::Convert_3bit_to_8bit( value ) );
    setColor();
}

void changepalette::on_slider_G_valueChanged(int value)
{
    ui->value_G->setText( QString( "%1" ).arg( value ) );
    m_color.setGreen( CSupportFuncs::Convert_3bit_to_8bit( value ) );
    setColor();
}

void changepalette::on_slider_B_valueChanged(int value)
{
    ui->value_B->setText( QString( "%1" ).arg( value ) );
    m_color.setBlue( CSupportFuncs::Convert_3bit_to_8bit( value ) );
    setColor();
}

void changepalette::on_PaletteMSX1_clicked()
{
    int ret = QMessageBox::question( this, "Confirm",
        "This action restores the original color palette of an MSX1,\n" \
        "do you want continue?",
        QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No
    );
    if( ret == QMessageBox::No ) return;
    if( ret == QMessageBox::Cancel ) return;
    for( int i = 0; i < 16; i++ ) {
        m_pal[i] = CSupportFuncs::CONST_COLORS_TABLE[i];
        QPixmap pix( 20, 20 );
        pix.fill( QColor( m_pal[i] ) );
        m_color_button[i]->setIcon( QIcon( pix ) );
    }
    m_color.setRgb( QRgb( m_pal[ m_button ] ) );
    setSliders();
    setColor();
}

void changepalette::on_buttonClose_clicked()
{
    close();
}

void changepalette::on_color_click( int i ) {
    m_button = i;
    m_color.setRgb( QRgb( m_pal[ m_button ] ) );
    setSliders();
    setColor();
}

void changepalette::setSliders() {
    int value;
    value = CSupportFuncs::Convert_8bit_to_3bit( m_color.red() );
    ui->value_R->setText( QString( "%1" ).arg( value ) );
    ui->slider_R->setValue( value );
    value = CSupportFuncs::Convert_8bit_to_3bit( m_color.green() );
    ui->value_G->setText( QString( "%1" ).arg( value ) );
    ui->slider_G->setValue( value );
    value = CSupportFuncs::Convert_8bit_to_3bit( m_color.blue() );
    ui->value_B->setText( QString( "%1" ).arg( value ) );
    ui->slider_B->setValue( value );
}

void changepalette::setColor() {
    // Cambiamos el color de label_Preview
    QPixmap pixmap(
        ui->label_Preview->geometry().width(),
        ui->label_Preview->geometry().height()
    );
    pixmap.fill( m_color );
    ui->label_Preview->setPixmap( pixmap );
    // Cambiamos el color del Botón activo:
    QPixmap pix( 20, 20 );
    pix.fill( m_color );
    m_color_button[m_button]->setIcon( QIcon( pix ) );
    m_color_button[m_button]->setFocus();
    // Guardamos el valor:
    m_pal[ m_button ]  = (uint32_t)m_color.rgb();
    m_pal[ m_button ] |= 0xFF000000;
    emit signal_change_palette();
}
