#include "fdlgloadbiblio.h"
#include <QFileDialog>

FDlgLoadBiblio::FDlgLoadBiblio( QString lastPath )
{
    int i;


    m_LastPath = lastPath;

    m_DlgLoad.setupUi( this );
    m_DlgLoad.m_pGrBancOr->addItem( "Bank 1" );
    m_DlgLoad.m_pGrBancOr->addItem( "Bank 2" );
    m_DlgLoad.m_pGrBancOr->addItem( "Bank 3" );
    m_DlgLoad.m_pGrBancDest->addItem( "Bank 1");
    m_DlgLoad.m_pGrBancDest->addItem( "Bank 2");
    m_DlgLoad.m_pGrBancDest->addItem( "Bank 3");

    for( i = 0; i < 32; i++ )
    {
        m_DlgLoad.m_pGrWidth->addItem( QString( "%1" ).arg( i + 1 ) );
        m_DlgLoad.m_pGrXOr->addItem( QString( "%1" ).arg( i  ) );
        m_DlgLoad.m_pGrXDest->addItem( QString( "%1" ).arg( i ) );
    }

    for( i = 0; i < 8; i++ )
    {
        m_DlgLoad.m_pGrHeight->addItem( QString( "%1" ).arg( i + 1 ));
        m_DlgLoad.m_pGrYOr->addItem( QString( "%1" ).arg( i ));
        m_DlgLoad.m_pGrYDest->addItem( QString( "%1" ).arg( i ));
    }

    connect( m_DlgLoad.m_pGrFile, SIGNAL( clicked() ), this, SLOT( OnLibraryFile() ) );
    connect( m_DlgLoad.m_pGrOk, SIGNAL( clicked() ), this, SLOT(accept()));
    connect( m_DlgLoad.m_pGrCancel, SIGNAL( clicked() ), this, SLOT(reject()));
}

QString FDlgLoadBiblio::get_FileName()
{
    return m_DlgLoad.m_pGrFileName->text();
}

int FDlgLoadBiblio::get_XOr()
{
    return m_DlgLoad.m_pGrXOr->currentText().toLongLong();
}

int FDlgLoadBiblio::get_YOr()
{
    return m_DlgLoad.m_pGrYOr->currentText().toLongLong();
}

int FDlgLoadBiblio::get_XDest()
{
    return m_DlgLoad.m_pGrXDest->currentText().toLongLong();
}

int FDlgLoadBiblio::get_YDest()
{
    return m_DlgLoad.m_pGrYDest->currentText().toLongLong();
}

int FDlgLoadBiblio::get_Widht()
{
    return m_DlgLoad.m_pGrWidth->currentText().toLongLong();
}

int FDlgLoadBiblio::get_Height()
{
    return m_DlgLoad.m_pGrHeight->currentText().toLongLong();
}

int FDlgLoadBiblio::get_BankOr()
{
    return m_DlgLoad.m_pGrBancOr->currentIndex();
}

int FDlgLoadBiblio::get_BankDest()
{
    return m_DlgLoad.m_pGrBancDest->currentIndex();
}

void FDlgLoadBiblio::OnLibraryFile()
{
    QString fileName;


    fileName = QFileDialog::getOpenFileName( this, "Tiles Library - Load", m_LastPath, "nMSXtiles Tiles *.til (*.til);;All files (*.*)" );

    if( fileName == "" ) return;

    m_LastPath = fileName.left( fileName.lastIndexOf( "/" ) );

    m_DlgLoad.m_pGrFileName->setText( fileName );

}
