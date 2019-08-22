#include "dlgexportscreen.h"
#include "ui_exportscreen.h"
#include <QMessageBox>
#include <QFileDialog>

CDlgExportScreen::CDlgExportScreen(QString lastPath, int lastFormat, int lastFileMode, int lastWidth, int lastHeight, int lastX, int lastY )
{
    m_pForm = new Ui_FExportScreen();
    m_pForm->setupUi( this );

    m_LastFormat = lastFormat;
    m_LastFileMode = lastFileMode;
    m_LastWidth = lastWidth;
    m_LastHeight = lastHeight;
    m_LastPath = lastPath;

    connect( m_pForm->m_pGrOk, SIGNAL( clicked()), this, SLOT( OnOk() ) );
    connect( m_pForm->m_pGrCancel, SIGNAL( clicked() ), this, SLOT(reject()));
    connect( m_pForm->m_pGrSelFileName, SIGNAL( clicked() ), this, SLOT( OnSelFileName() ) );
    connect( m_pForm->m_pGrFormat, SIGNAL( currentIndexChanged(int)), this, SLOT( OnFormatChanged( int ) ) );

    m_pForm->m_pGrFormat->addItem( "ASM data" );
    m_pForm->m_pGrFormat->addItem( "ASM data Hexadecimal" );
    m_pForm->m_pGrFormat->addItem( "Bin data" );
    m_pForm->m_pGrFormat->addItem( "Bin data compressed with Pletter" );
    m_pForm->m_pGrFormat->setCurrentIndex( lastFormat );

    m_pForm->m_pGrFilesMode->addItem( "One file by Screen" );
    m_pForm->m_pGrFilesMode->addItem( "One file whole Map" );
    m_pForm->m_pGrFilesMode->setCurrentIndex( lastFileMode );

    m_pForm->m_pGrWidth->setText( QString( "%1" ).arg( lastWidth ) );
    m_pForm->m_pGrHeight->setText( QString( "%1" ).arg(lastHeight ) );
    m_pForm->m_pGrX->setText( QString( "%1" ).arg( lastX ) );
    m_pForm->m_pGrY->setText( QString( "%1" ).arg( lastY ));
    m_pForm->m_pGrFileName->setText( m_LastPath);
}

int CDlgExportScreen::get_X() { return m_X; }
int CDlgExportScreen::get_Y() { return m_Y; }
int CDlgExportScreen::get_Width()  { return m_Width; }
int CDlgExportScreen::get_Height() { return m_Height; }
CDlgExportScreen::TYPE_FORMAT CDlgExportScreen::get_Format() { return m_Format; }
CDlgExportScreen::TYPE_FILE_MODE CDlgExportScreen::get_FileMode() { return m_FileMode; }
QString CDlgExportScreen::get_FileName() { return m_FileName;}


void CDlgExportScreen::OnOk() {
    bool ok;
    bool res = true;


    m_Width = m_pForm->m_pGrWidth->text().toInt( &ok );
    res &= ok;
    if( res ) m_Height = m_pForm->m_pGrHeight->text().toInt( &ok );
    res &= ok;
    if( res ) m_X = m_pForm->m_pGrX->text().toInt( &ok );
    res &= ok;
    if( res ) m_Y = m_pForm->m_pGrY->text().toInt( &ok );

    if( !res ) {
        QMessageBox::critical( this, "Error", "Numeric values expected!" );
        return;
    }

    if( m_pForm->m_pGrFileName->text() == "" ) {
        QMessageBox::critical( this, "Error", "File Name missing!" );
        return;
    }

    m_FileMode = (CDlgExportScreen::TYPE_FILE_MODE)m_pForm->m_pGrFilesMode->currentIndex();
    m_Format = (CDlgExportScreen::TYPE_FORMAT)m_pForm->m_pGrFormat->currentIndex();
    m_FileName = m_pForm->m_pGrFileName->text();

    accept();
}


void CDlgExportScreen::OnSelFileName() {
    m_pForm->m_pGrFileName->setText(
        QFileDialog::getSaveFileName(
            this, "File Name", m_LastPath,
            "Assembler (*.asm);;Binary (*.bin);;Pletter5 (*.plet5);;All files (*.*)"
        )
    );
}


void CDlgExportScreen::OnFormatChanged( int index ) {
    if( index < 2 ) {
        m_pForm->m_pGrFilesMode->setCurrentIndex(0);
        m_pForm->m_pGrFilesMode->setEnabled( false );
    }
    else m_pForm->m_pGrFilesMode->setEnabled( true );
}



