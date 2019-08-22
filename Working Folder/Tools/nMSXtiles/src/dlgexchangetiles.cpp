#include <QMessageBox>
#include "dlgexchangetiles.h"
#include "ui_fdlgexchangetiles.h"


CDlgExchangeTiles::CDlgExchangeTiles(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgExchangeTiles)
{
    ui->setupUi(this);

    for( int i = 0; i < 256; i++ )
    {
        ui->cboTile1->addItem( QString( "%1" ).arg( i ) );
        ui->cboTile2->addItem( QString( "%1" ).arg( i ) );
    }

    connect( ui->cmdOk, SIGNAL( clicked() ), this, SLOT( OnOk() ) );
    connect( ui->cmdCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

}

CDlgExchangeTiles::~CDlgExchangeTiles()
{
    delete ui;
}

int CDlgExchangeTiles::get_Tile1()
{
    return m_Tile1;
}

int CDlgExchangeTiles::get_Tile2()
{
    return m_Tile2;
}

void CDlgExchangeTiles::OnOk()
{
    if( ui->cboTile1->currentText() == ui->cboTile2->currentText())
    {
        QMessageBox::critical( this, "Error", "The tiles must be different" );
        return;
    }

    m_Tile1 = ui->cboTile1->currentText().toInt();
    m_Tile2 = ui->cboTile2->currentText().toInt();

    accept();
}

