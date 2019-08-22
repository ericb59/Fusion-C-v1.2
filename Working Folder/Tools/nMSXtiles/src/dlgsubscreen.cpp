#include "dlgsubscreen.h"
#include "ui_dlgsubscreen.h"


CDlgSubscreen::CDlgSubscreen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSubscreen)
{
    m_DefX = 0;
    m_DefY = 0;
    m_DefWidth = 32;
    m_DefHeight = 24;

    ui->setupUi(this);

    ui->m_pGrX->setText( QString( "%1" ).arg( m_DefX ));
    ui->m_pGrY->setText( QString( "%1" ).arg( m_DefY ));
    ui->m_pGrWidth->setText( QString( "%1" ).arg( m_DefWidth ));
    ui->m_pGrHeight->setText( QString( "%1" ).arg( m_DefHeight ));

    connect( ui->m_pGrOk, SIGNAL( clicked() ), this, SLOT( OnOk() ) );
    connect( ui->m_pGrCancel, SIGNAL( clicked() ), this, SLOT( OnCancel() ) );

}

CDlgSubscreen::~CDlgSubscreen()
{
    delete ui;
}

int CDlgSubscreen::get_DefX() {
    return m_DefX;
}

int CDlgSubscreen::get_DefY() {
    return m_DefY;
}

int CDlgSubscreen::get_DefWidth() {
    return m_DefWidth;
}

int CDlgSubscreen::get_DefHeight() {
    return m_DefHeight;
}

void CDlgSubscreen::OnOk()
{
    bool ok;

    m_DefX = ui->m_pGrX->text().toInt( &ok );
    if( !ok ) return;

    m_DefY = ui->m_pGrY->text().toInt( &ok );
    if( !ok ) return;

    m_DefWidth = ui->m_pGrWidth->text().toInt( &ok );
    if( !ok ) return;

    m_DefHeight = ui->m_pGrHeight->text().toInt( &ok );
    if( !ok ) return;

    accept();
}

void CDlgSubscreen::OnCancel()
{
    close();
}



