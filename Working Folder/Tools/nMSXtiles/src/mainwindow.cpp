#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "changepalette.h"
#include "supportfuncs.h"
#include "dlgexportscreen.h"
#include "fdlgloadbiblio.h"
#include "dlgexchangetiles.h"
#include <QSettings>
#include <QDockWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :   
    QMainWindow( parent ),
    ui( new Ui::MainWindow )
{
    ui->setupUi(this);

    m_Version = QString( "%1.%2.%3" ).arg( VER_NMSXTILES_MAJOR ).arg( VER_NMSXTILES_MINOR ).arg( VER_NMSXTILES_PATCH );
    QSettings settings( "Pentacour", "nMSXTiles" );
    m_LastPathLoadProject = settings.value( "LoadProject" ).toString();
    m_LastPathExport = settings.value( "Export" ).toString();
    m_LastFormat = settings.value( "Format" ).toInt();
    m_LastFilesMode = settings.value( "FilesMode" ).toInt();
    m_LastWidth = settings.value( "Width" ).toInt();
    m_LastHeight = settings.value( "Height" ).toInt();
    m_LastX = settings.value( "X").toInt();
    m_LastY = settings.value( "Y" ).toInt();

    if( m_LastPathLoadProject == "" ) {
        m_LastPathLoadProject = QDir::currentPath();
    }

    // Ocultamos la barra de herramientas y la de estado.
    ui->mainToolBar->hide();
    ui->statusBar->hide();

    setCentralWidget( ui->mdiArea );
    ui->mdiArea->setAttribute( Qt::WA_DeleteOnClose );

    m_pScreen_sbw = NULL;
    m_pSprites_sbw = NULL;
    m_pScreen = NULL;
    m_pSprites = NULL;

    resize( 800, 600 );

    m_ProjectFile = "";
    m_ScreenFile = "";
    m_PaletteFile = "";
    m_TilesFile = "";

    setWindowTitle( QString( "nMSXtiles - v%1" ).arg( m_Version ) );

    EnableMenuItems();

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::closeEvent( QCloseEvent *pEvent ) {
    on_action_Project_Quit_triggered();
    pEvent->accept();
//	pEvent->accept();
//	pEvent->ignore();
}


// ----------------------------------------------
//                   SLOTS
// ----------------------------------------------

// ----------------------------------------------
//                 MENU PROJECT
// ----------------------------------------------

void MainWindow::on_action_Project_New_triggered() {

    if( m_pScreen &&
        QMessageBox::question( this,
        "Confirm", "Save Project?",
        QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes
     ) on_action_Project_Save_as_triggered();

    InitScreen();

    m_pScreen->NewTiles();
    m_pScreen->NewScreen();
    m_pScreen->NewPalette();

    m_ProjectFile = "";
    m_ScreenFile = "";
    m_PaletteFile = "";
    m_TilesFile = "";

    EnableMenuItems();

}

void MainWindow::on_action_Project_Load_triggered() {
    int i;
    QString relPath;
    QFile hFile;
    QString fileName;

    on_action_Project_Close_triggered();

    fileName = QFileDialog::getOpenFileName( this,
        "Project - Load", m_LastPathLoadProject,
        "nMSXtiles Project (*.prj);;All files (*.*)"
    );
    if( fileName == "" ) return;

    m_LastPathLoadProject = fileName.left( fileName.lastIndexOf( "/" ) );

    m_ProjectFile = fileName;

    hFile.setFileName( m_ProjectFile );
    hFile.open( QIODevice::ReadOnly );

    QTextStream str( &hFile );

    if( hFile.atEnd() ) {
        hFile.close();
        QMessageBox::critical( this, "Error", "Unrecognized format file" );
        return;
    }

    if( str.readLine() != "PROJECT" ) {
        hFile.close();
        QMessageBox::critical( this, "Error", "Unrecognized format file" );
        return;
    }

    // CARGAMOS SCREEN:
    if( str.readLine() != "SCREEN" ) {
        hFile.close();
        QMessageBox::critical( this, "Error", "Unrecognized format file" );
        return;
    }
    i = m_ProjectFile.lastIndexOf( "/" );
    relPath = m_ProjectFile.left( i );
    m_ScreenFile = str.readLine();
    if( m_ScreenFile.lastIndexOf( "/" ) == -1 ) m_ScreenFile = relPath + QString( "/" ) + m_ScreenFile;
    InitScreen();
    m_pScreen->LoadScreen( m_ScreenFile );

    // CARGAMOS TILES:
    if( str.readLine() != "TILES" ) {
        hFile.close();
        QMessageBox::critical( this, "Error", "Unrecognized format file" );
        return;
    }
    m_TilesFile = str.readLine();

    if( m_TilesFile.lastIndexOf( "/" ) == -1 ) m_TilesFile = relPath + QString( "/" ) + m_TilesFile;

    m_pScreen->LoadTiles( m_TilesFile );

    if( str.readLine() == "ONEBANK" ) m_pScreen->SetOneBank( true  );
    else                              m_pScreen->SetOneBank( false );

    // CARGAMOS PALETTE (ES OPCIONAL)
    if( str.readLine() == "PALETTE" ) {
        m_PaletteFile = str.readLine();
        if( m_PaletteFile.lastIndexOf( "/" ) == -1 ) m_PaletteFile = relPath + QString( "/" ) + m_PaletteFile;
        m_pScreen->LoadPalette( m_PaletteFile );
    } else m_pScreen->NewPalette();

    // Fin y cierre:
    hFile.close();
    EnableMenuItems();

}

void MainWindow::on_action_Project_Save_triggered()
{
    if( m_ProjectFile == "" ) on_action_Project_Save_as_triggered();
    else SaveProject();
}

void MainWindow::on_action_Project_Save_as_triggered()
{
    QString fileName;

    if( m_ScreenFile == "" ) on_action_Screen_Save_as_triggered();
    else m_pScreen->SaveScreen( m_ScreenFile );

    if( m_TilesFile == "" ) on_action_Tiles_Save_as_triggered();
    else m_pScreen->SaveTiles( m_TilesFile );

    if( m_PaletteFile == "" ) on_action_Palette_Save_as_triggered();
    else m_pScreen->SavePalette( m_PaletteFile );

    if( m_ScreenFile == "" || m_TilesFile == "" ) {
        QMessageBox::question( this, "Project not saved!", "Warning" );
        return;
    }

    m_ProjectFile = QFileDialog::getSaveFileName( this,
        "Project - Save as", m_LastPathLoadProject,
        "nMSXtiles Project (*.prj);;All files (*.*)"
    );

    // qDebug() << "MainWindow::on_action_Project_Save_As_triggered() ProjectFile="<<m_ProjectFile;

    m_LastPathLoadProject = m_ProjectFile.left( m_ProjectFile.lastIndexOf( "/" ) );

    if( m_ProjectFile > "" ) SaveProject();
    else QMessageBox::question( this, "Project not saved!", "Warning" );

}

void MainWindow::on_action_Project_Close_triggered()
{
    if( m_pScreen &&
        QMessageBox::question( this,
            "Confirm", "Save Project?",
            QMessageBox::Yes, QMessageBox::No
        ) == QMessageBox::Yes
    ) on_action_Project_Save_triggered();
    QuitScreen();
    EnableMenuItems();
}

void MainWindow::on_action_Project_Quit_triggered() {
    on_action_Sprites_1_Close_triggered();
    on_action_Project_Close_triggered();
    QSettings settings( "Pentacour", "nMSXTiles" );
    settings.setValue( "LoadProject", m_LastPathLoadProject ) ;
    settings.setValue( "Export", m_LastPathExport );
    settings.setValue( "Format", m_LastFormat );
    settings.setValue( "FilesMode", m_LastFilesMode );
    settings.setValue( "Width", m_LastWidth );
    settings.setValue( "Height", m_LastHeight );
    settings.setValue( "X", m_LastX);
    settings.setValue( "Y", m_LastY );
    close();
}

// ----------------------------------------------
//                MENU SCREEN
// ----------------------------------------------

void MainWindow::on_action_Screen_New_triggered() {
    on_action_Screen_Save_triggered();
    m_pScreen->NewScreen();
}

void MainWindow::on_action_Screen_Load_triggered() {
    QFile hFile;
    QString line;
    QString fileName;
    fileName = QFileDialog::getOpenFileName( this,
        "Screen - Load", m_LastPathLoadProject,
        "nMSXtiles Screen (*.scr);;All files (*.*)"
    );
    if( fileName == "" ) return;
    m_LastPathLoadProject = fileName.left( fileName.lastIndexOf( "/" ) );
    m_ScreenFile = fileName;
    m_pScreen->LoadScreen( m_ScreenFile );
}

void MainWindow::on_action_Screen_Save_triggered()
{
    if( m_ScreenFile == "" ) on_action_Screen_Save_as_triggered();
    else m_pScreen->SaveScreen( m_ScreenFile );
}

void MainWindow::on_action_Screen_Save_as_triggered() {
    QString fileName;
    fileName = QFileDialog::getSaveFileName( this,
        "Screen - Save as", m_LastPathLoadProject,
        "nMSXtiles Screen (*.scr);;All files (*.*)"
    );
    if( fileName > "" ) {
        m_ScreenFile = fileName;
        m_pScreen->SaveScreen( m_ScreenFile );
        m_LastPathLoadProject = fileName.left( fileName.lastIndexOf( "/" ) );
    }
}

void MainWindow::on_action_Screen_Export_Screen_triggered() {
    QString fileName;
    CDlgExportScreen *pExport;

    //pExport = new CDlgExportScreen(m_LastPathExport, this);
    //int lastFormat, int lastFilesMode, int lastWidth, int lastHeight, int lastX, int lastY, QWidget *pParent )
    pExport = new CDlgExportScreen(m_LastPathExport, m_LastFormat, m_LastFilesMode, m_LastWidth, m_LastHeight, m_LastX, m_LastY);
    if( pExport->exec() == QDialog::Accepted ) {

        m_LastPathExport = pExport->get_FileName().left( fileName.lastIndexOf( "/" ) );
        m_LastFormat = pExport->get_Format();
        m_LastFilesMode = pExport->get_FileMode();
        m_LastWidth = pExport->get_Width();
        m_LastHeight = pExport->get_Height();
        m_LastX = pExport->get_X();
        m_LastY = pExport->get_Y();

        switch( pExport->get_Format() ) {
        case CDlgExportScreen::TYPE_ASM:
            m_pScreen->ExportScreenData( pExport->get_FileName(), false, pExport->get_X(), pExport->get_Y(), pExport->get_Width(), pExport->get_Height() );
            break;
        case CDlgExportScreen::TYPE_ASM_HEXA:
            m_pScreen->ExportScreenData( pExport->get_FileName(), true, pExport->get_X(), pExport->get_Y(), pExport->get_Width(), pExport->get_Height() );
            break;
        case CDlgExportScreen::TYPE_BIN:
            if( pExport->get_FileMode() == CDlgExportScreen::TYPE_ONE_WHOLE_MAP ) m_pScreen->ExportScreenBinScroll( pExport->get_FileName(), pExport->get_X(), pExport->get_Y(), pExport->get_Width(), pExport->get_Height() );
            else m_pScreen->ExportScreenBin( pExport->get_FileName(), pExport->get_X(), pExport->get_Y(), pExport->get_Width(), pExport->get_Height() );
            break;
        case CDlgExportScreen::TYPE_BIN_PLETTER:
            if( pExport->get_FileMode() == CDlgExportScreen::TYPE_ONE_WHOLE_MAP ){
               m_pScreen->ExportScreenBinPletterScroll( pExport->get_FileName(), pExport->get_X(), pExport->get_Y(), pExport->get_Width(), pExport->get_Height() );
            }
            else {
               m_pScreen->ExportScreenBinPletter( pExport->get_FileName(), pExport->get_X(), pExport->get_Y(), pExport->get_Width(), pExport->get_Height() );
            }
            break;

        }

    }
}

void MainWindow::on_action_Screen_Import_SC2_triggered() {
    QString fileName;
    fileName = QFileDialog::getOpenFileName( this,
        "Screen - Import SC2", m_LastPathLoadProject,
        "MSX screen 2 image (*.sc2);;All files (*.*)"
    );
    if( fileName == "" ) return;
    m_LastPathLoadProject = fileName.left( fileName.lastIndexOf( "/" ) );
    m_pScreen->ImportSC2( fileName );
}

void MainWindow::on_action_Screen_Export_SC2_triggered() {
    QString fileName = QFileDialog::getSaveFileName( this,
        "Screen - Export SC2", m_LastPathLoadProject,
        "MSX screen 2 image (*.sc2);;All files (*.*)"
    );
    if( fileName == "" ) return;
    m_LastPathLoadProject = fileName.left( fileName.lastIndexOf( "/" ) );
    m_pScreen->ExportSC2( fileName );
}

// ----------------------------------------------
//                 MENU PALETTE
// ----------------------------------------------
void MainWindow::on_action_Palette_New_triggered()
{
    if( !m_pScreen ) return;
    int ret = QMessageBox::question( this, "Confirm",
        "This action restores the original color palette of an MSX1,\n" \
        "do you want to save the current palette?",
        QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No
    );
    if( ret == QMessageBox::Yes ) on_action_Palette_Save_as_triggered();
    if( ret == QMessageBox::Cancel ) return;
    m_pScreen->NewPalette();
}

void MainWindow::on_action_Palette_Load_triggered()
{
//    QFile hFile;
//    QString line;
    QString fileName;
    fileName = QFileDialog::getOpenFileName( this,
        "Palette - Load", m_LastPathLoadProject,
        "nMSXtiles Palette (*.pal);;All files (*.*)"
    );
    if( fileName == "" ) return;
    m_LastPathLoadProject = fileName.left( fileName.lastIndexOf( "/" ) );
    m_PaletteFile = fileName;
    m_pScreen->LoadPalette( m_PaletteFile );
}

void MainWindow::on_action_Palette_Save_triggered()
{
    if( m_PaletteFile == "" ) on_action_Palette_Save_as_triggered();
    else m_pScreen->SavePalette( m_PaletteFile );
}

void MainWindow::on_action_Palette_Save_as_triggered()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName( this,
        "Palette - Save as", m_LastPathLoadProject,
        "nMSXtiles Palette (*.pal);;All files (*.*)" );
    if( fileName > "" ) {
        m_PaletteFile = fileName;
        m_pScreen->SavePalette( m_PaletteFile );
        m_LastPathLoadProject = fileName.left( fileName.lastIndexOf( "/" ) );
    }
}

void MainWindow::on_action_Palette_Export_ASM_triggered()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName( this,
        "Palette - Create ASM Palette file as", m_LastPathLoadProject,
        "Assembler (*.asm);;Text (*.txt);;All files (*.*)"
    );
    if( fileName > "" ) {
        m_LastPathLoadProject = fileName.left( fileName.lastIndexOf( "/" ));
        m_pScreen->ExportPaletteASM( fileName );
    }
}

void MainWindow::on_action_Palette_Export_VDP_triggered()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName( this,
        "Palette - Create VDP Palette file as", m_LastPathLoadProject,
        "Text (*.txt);;All files (*.*)"
    );
    if( fileName > "" ) {
        m_LastPathLoadProject = fileName.left( fileName.lastIndexOf( "/" ));
        m_pScreen->ExportPaletteVDP( fileName );
    }
}

// ----------------------------------------------
//                 MENU TILES
// ----------------------------------------------
void MainWindow::on_action_Tiles_New_triggered()
{
    on_action_Sprites_1_Save_triggered();
    m_pScreen->NewTiles();
}

void MainWindow::on_action_Tiles_Load_triggered()
{
    QFile hFile;
    QString line;
    QString fileName;
    fileName = QFileDialog::getOpenFileName( this,
        "Tiles - Load", m_LastPathLoadProject,
        "nMSXtiles Tiles (*.til);;All files (*.*)"
    );
    if( fileName == "" ) return;
    m_LastPathLoadProject = fileName.left( fileName.lastIndexOf( "/" ) );
    m_TilesFile = fileName;
    m_pScreen->LoadTiles( m_TilesFile );
}

void MainWindow::on_action_Tiles_Save_triggered()
{
    if( m_TilesFile == "" ) on_action_Tiles_Save_as_triggered();
    else m_pScreen->SaveTiles( m_TilesFile );
}

void MainWindow::on_action_Tiles_Save_as_triggered()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName( this,
        "Tiles - Save as",  m_LastPathLoadProject,
        "nMSXtiles Tiles (*.til);;All files (*.*)"
    );
    if( fileName > "" ) {
        m_TilesFile = fileName;
        m_pScreen->SaveTiles( m_TilesFile );
        m_LastPathLoadProject = fileName.left( fileName.lastIndexOf( "/" ) );
    }
}

void MainWindow::on_action_Tiles_Load_from_Library_triggered()
{
    FDlgLoadBiblio *pDlg;
    pDlg = new FDlgLoadBiblio( m_LastPathLoadProject );
    if( !pDlg->exec() ) return;
    if( pDlg->get_FileName() == "" ) return;
    m_LastPathLoadProject = pDlg->get_FileName().left( pDlg->get_FileName().lastIndexOf( "/" ) );
    m_pScreen->LoadTilesLibrary( pDlg->get_FileName(), pDlg->get_BankOr(), pDlg->get_XOr(), pDlg->get_YOr(), pDlg->get_Widht(), pDlg->get_Height(), pDlg->get_BankDest(), pDlg->get_XDest(), pDlg->get_YDest() );
}

void MainWindow::on_action_Tiles_Export_ASM_data_triggered()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName( this,
        "Tiles - Export to ASM as", m_LastPathExport,
        "Assembler (*.asm);;Text (*.txt);;All files (*.*)"
    );
    if( fileName > "" ) {
        m_LastPathExport = fileName.left( fileName.lastIndexOf( "/" ) );
        m_pScreen->ExportTilesData( fileName, false );
    }
}

void MainWindow::on_action_Tiles_Export_ASM_data_Hexadecimal_triggered()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName( this,
        "Tiles - Export to ASM as", m_LastPathExport,
        "Assembler (*.asm);;Text (*.txt);;All files (*.*)"
    );
    if( fileName > "" ) {
        m_LastPathExport = fileName.left( fileName.lastIndexOf( "/" ) );
        m_pScreen->ExportTilesData( fileName, true );
    }
}

void MainWindow::on_action_Tiles_Export_bin_data_triggered()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName( this,
        "Tiles - Export to bin data as", m_LastPathExport,
        "Binary (*.bin);;Text (*.txt);;All files (*.*)"
    );
    if( fileName > "" ) {
        m_LastPathExport = fileName.left( fileName.lastIndexOf( "/" ) );
        m_pScreen->ExportTilesBin( fileName );
    }
}

void MainWindow::on_action_Tiles_Export_bin_data_compressed_whith_Pletter_triggered()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName( this,
        "Tiles - Export to bin data compressed with Pletter as", m_LastPathExport,
        "Pletter5 (*.plet5);;All files (*.*)"
    );
    if( fileName > "" ) {
        m_LastPathExport = fileName.left( fileName.lastIndexOf( "/" ) );
        m_pScreen->ExportTilesBinPletter( fileName );
    }
}

/*
// NO ACTIVA ESTA FUNCION:
void CMainWindow::OnImportTilesPNGAdv()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName( this, "Tiles - Import PNG file withour palette", m_LastPath, "Image (*.png);;All files (*.*)"  );
    if( fileName == ""  )
    {
        QMessageBox::critical( this, "Error", "No file selected!" );
        return;
    }
    m_LastPath = fileName.left( fileName.lastIndexOf( "/" ) );
    if( !m_pScreen->ImportTilesPNGAdv( fileName ) )
    {
        QMessageBox::critical( this, "Error", "Not supported format file" );
    }
}
*/

// ----------------------------------------------
//             MENU SCREEN TOOLS
// ----------------------------------------------
void MainWindow::on_action_Screen_Tools_Fill_Screen_whith_tiles_1_to_255_triggered() {
    if( QMessageBox::question( this,
        "Confirm", "Fill Screen with tiles from 1 to 255",
        QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes
    )
    {
        m_pScreen->Fill1to255();
    }
}

// ----------------------------------------------
//             MENU PALETTE TOOLS
// ----------------------------------------------

void MainWindow::on_action_Palette_Tools_Change_Palette_triggered()
{
    changepalette* pal = new changepalette( m_pScreen, m_pScreen->COLORS_TABLE_SCREEN, 0 );
    QObject::connect( pal, &changepalette::signal_change_palette, m_pScreen, &CScreenW::UpdatePalette );
    pal->show();
    pal->exec();
}

// ----------------------------------------------
//             MENU TILES TOOLS
// ----------------------------------------------

void MainWindow::on_action_Tiles_Tools_Copy_triggered() {
    m_pScreen->CopyTile();
}

void MainWindow::on_action_Tiles_Tools_Paste_triggered() {
    m_pScreen->PasteTile();
}

void MainWindow::on_action_Tiles_Tools_Paste_colors_triggered() {
    m_pScreen->PasteTileColors();
}

void MainWindow::on_action_Tiles_Tools_Undo_triggered() {
    m_pScreen->Undo();
}

void MainWindow::on_action_Tiles_Tools_Reorder_colors_triggered() {
    m_pScreen->ReorderColors();
}

void MainWindow::on_action_Tiles_Tools_Group_Tiles_triggered() {
    m_pScreen->GroupTiles();
}

void MainWindow::on_action_Tiles_Tools_Exchange_tiles_triggered() {
    CDlgExchangeTiles dlg( NULL );
    dlg.exec();
    if( dlg.result() == 0 ) return;
    m_pScreen->ExchangeTiles( dlg.get_Tile1(), dlg.get_Tile2() );
}

// ----------------------------------------------
//                MENU PNG TOOLS
// ----------------------------------------------
// void MainWindow::on_action_Screen_Export_Screen_PNG_File_triggered() {
void MainWindow::on_action_PNG_Tools_Export_PNG_Screen_file_triggered() {
    QString fileName;
    fileName = QFileDialog::getSaveFileName( this,
        "Screen - Export PNG file as", m_LastPathExport,
        "Images (*.png);;All files (*.*)"
    );
    if( fileName > "" ) {
        m_LastPathExport = fileName.left( fileName.lastIndexOf( "/" ) );
        m_pScreen->ExportScreenPNG( fileName );
    }
}

// void MainWindow::on_action_Screen_Export_Map_PNG_File_triggered() {
void MainWindow::on_action_PNG_Tools_Export_Map_PNG_file_triggered() {
    QString fileName;
    fileName = QFileDialog::getSaveFileName( this, "Map - Export PNG file as", m_LastPathExport );
    if( fileName > "" ) {
        m_LastPathExport = fileName.left( fileName.lastIndexOf( "/" ) );
        m_pScreen->ExportMapPNG( fileName );
    }
}

// void MainWindow::on_action_Tiles_Export_PNG_file_triggered() {
void MainWindow::on_action_PNG_Tools_Export_PNG_Tiles_file_triggered() {
    QString fileName;
    fileName = QFileDialog::getSaveFileName( this,
        "Tiles - Export PNG as", m_LastPathLoadProject,
        "Images (*.png);;All files(*.*)"
    );
    if( fileName > "" ) {
        m_LastPathLoadProject = fileName.left( fileName.lastIndexOf( "/" ));
        m_pScreen->ExportTilesPNG( fileName );
    }
}

void MainWindow::on_action_PNG_Tools_Import_PNG_Tiles_file_triggered() {
    QString fileName;
    fileName = QFileDialog::getOpenFileName( this,
        "Tiles - Import PNG file created with nMSXtiles", m_LastPathLoadProject,
        "Image (*.png);;All files (*.*)"
    );
    if( fileName == ""  ) {
        QMessageBox::critical( this, "Error", "No file selected!" );
        return;
    }
    m_LastPathLoadProject = fileName.left( fileName.lastIndexOf( "/" ) );
    if( !m_pScreen->ImportTiles( fileName ) ) {
        QMessageBox::critical( this, "Error", "Not supported format file" );
    }
    on_action_Tiles_Tools_Reorder_colors_triggered();
}

// ----------------------------------------------
//                MENU SPRITES 1
// ----------------------------------------------

void MainWindow::on_action_Sprites_1_New_triggered() {
    if( m_pSprites &&
        QMessageBox::question( this, "Confirm", "Save Sprites?",
        QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes
    ) on_action_Sprites_1_Save_as_triggered(); // OnSaveAsSprites();
    InitSprites();
    m_pSprites->NewSprites();
    m_SpritesFile = "";
    EnableMenuItems();
}

void MainWindow::on_action_Sprites_1_Load_triggered() {
    QFile hFile;
    QString line;
    QString fileName;
    if( m_pSprites &&
        QMessageBox::question( this, "Confirm", "Save Sprites?",
        QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes
    ) on_action_Sprites_1_Save_as_triggered();

    fileName = QFileDialog::getOpenFileName( this,
        "Sprite - Load", m_LastPathLoadProject,
        "nMSXtiles Sprite (*.spt);;All files (*.*)"
    );
    if( fileName == "" ) return;
    m_LastPathLoadProject = fileName.left( fileName.lastIndexOf( "/" ) );
    m_SpritesFile = fileName;
    InitSprites();
    m_pSprites->LoadSprites( m_SpritesFile );
    EnableMenuItems();
}

void MainWindow::on_action_Sprites_1_Save_triggered() {
    if( m_SpritesFile == "" ) on_action_Screen_Save_as_triggered();
    else m_pSprites->SaveSprites( m_SpritesFile );
}

void MainWindow::on_action_Sprites_1_Save_as_triggered() {
    QString fileName;
    fileName = QFileDialog::getSaveFileName( this,
        "Sprites - Save as", m_LastPathLoadProject,
        "nMSXtiles Sprite (*.spt);;All files (*.*)"
    );
    if( fileName > "" ) {
        m_LastPathLoadProject = fileName.left( fileName.lastIndexOf( "/" ) );
        m_SpritesFile = fileName;
        m_pSprites->SaveSprites( m_SpritesFile );
    }
}

void MainWindow::on_action_Sprites_1_Close_triggered() {
    if( m_pSprites &&
        QMessageBox::question( this, "Confirm", "Save Sprites?",
        QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes
    ) on_action_Screen_Save_triggered();
    QuitSprites();
    EnableMenuItems();
}

void MainWindow::on_action_Sprites_1_Export_triggered() {
    QString fileName;
    fileName = QFileDialog::getSaveFileName( this,
        "Sprites - Export as", m_LastPathExport
    );
    if( fileName > "" ) {
        m_LastPathExport = fileName.left( fileName.lastIndexOf( "/" ) );
        m_pSprites->ExportSprites( fileName );
    }
}

// ----------------------------------------------
//                MENU SPRITES 2
// ----------------------------------------------

// ----------------------------------------------
//                MENU ABOUT
// ----------------------------------------------
void MainWindow::on_action_About_nMSXtiles_triggered() {
    QString s;
    s = QString(
        "Screen, tiles and sprites editor for MSX screen 2 and 4.\n\nVersion %1\n\n"
    ).arg( m_Version );
    s += "https://github.com/pipagerardo/nMSXtiles\n";
    s += "http://pentacour.com/";
    QMessageBox::about( this, "nMSXtiles", s );
}

void MainWindow::on_action_About_pentacour_triggered() {
    QMessageBox msgBox;
    msgBox.setWindowTitle( "pentacour:" );
//  msgBox.setWindowIcon( QIcon(QString::fromUtf8(":res/pentacour.png") ) );
    msgBox.setIconPixmap( QPixmap(QString::fromUtf8(":res/pentacour.png") ) );
    msgBox.setText(
    QString(
        "Ramon de las Heras Gomiz\n" \
        "Original author of nMSXtiles"
    )
    );
    msgBox.setInformativeText("http://pentacour.com/");
    msgBox.setStandardButtons( QMessageBox::Ok );
    msgBox.setDefaultButton( QMessageBox::Ok );
    msgBox.exec();
}

void MainWindow::on_action_About_pipagerardo_triggered() {
    QMessageBox msgBox;
    msgBox.setWindowTitle( "pipagerardo:" );
//  msgBox.setWindowIcon( QIcon(QString::fromUtf8(":res/pipagerardo.png") ) );
    msgBox.setIconPixmap( QPixmap(QString::fromUtf8(":res/pipagerardo.png") ) );
    msgBox.setText(
    QString(
        "Programmer who is currently developing nMSXtiles\n\n" \
        "Other projects:\n" \
        "  https://sites.google.com/view/sjasmpg/sjasmpg\n" \
        "  https://sites.google.com/view/png2msx\n" \
        "  https://sites.google.com/view/temptations"
    )
    );
    msgBox.setInformativeText("pipagerardo@gmail.com");
    msgBox.setStandardButtons( QMessageBox::Ok );
    msgBox.setDefaultButton( QMessageBox::Ok );
    msgBox.exec();
}

void MainWindow::on_action_About_QT_triggered() {
    QMessageBox::aboutQt( this, "Qt" );
}

// ----------------------------------------------
// PRIVATE FUNCTIONS
// ----------------------------------------------
void MainWindow::InitScreen() {
    QuitScreen();
    m_pScreen = new CScreenW( this );
    m_pScreen->Initialize();
    m_pScreen_sbw = ui->mdiArea->addSubWindow( m_pScreen, Qt::Dialog );
    QObject::connect( m_pScreen, &CScreenW::signal_close, this, &MainWindow::on_action_Project_Close_triggered );
    m_pScreen->show();
}

void MainWindow::QuitScreen() {
    if( m_pScreen ) {
        delete m_pScreen;
        if( m_pScreen_sbw ) ui->mdiArea->setActiveSubWindow( m_pScreen_sbw );
        ui->mdiArea->closeActiveSubWindow();
        m_pScreen_sbw = NULL;
        m_pScreen = NULL;
    }
}

void MainWindow::InitSprites() {
    QuitSprites();
    m_pSprites= new CSpritesW();
    m_pSprites->Initialize();
    m_pSprites_sbw = ui->mdiArea->addSubWindow( m_pSprites, Qt::Dialog ); // Qt::Widget );
    QObject::connect( m_pSprites, &CSpritesW::signal_close, this, &MainWindow::on_action_Sprites_1_Close_triggered );
    m_pSprites->show();
}

void MainWindow::QuitSprites() {
    if( m_pSprites ) {
        // m_pSprites->Close();
        delete m_pSprites;
        if( m_pSprites_sbw ) ui->mdiArea->setActiveSubWindow( m_pSprites_sbw );
        ui->mdiArea->closeActiveSubWindow();
        m_pSprites_sbw = NULL;
        m_pSprites = NULL;
     }
}

void MainWindow::SaveProject()
{
    int i;
    QFile hFile;
    QString projectFile;
    QString screenFile;
    QString paletteFile;
    QString tilesFile;

    if( m_ProjectFile == "" ) {
        on_action_Project_Save_as_triggered();
    } else {
        on_action_Screen_Save_triggered();
        on_action_Palette_Save_triggered();
        on_action_Tiles_Save_triggered();

        i = m_ScreenFile.lastIndexOf( "/" );
        if( i == -1 ) screenFile = m_ScreenFile;
        else screenFile = m_ScreenFile.left( i );

        i = m_PaletteFile.lastIndexOf( "/" );
        if( i == -1 ) paletteFile = m_PaletteFile;
        else paletteFile = m_PaletteFile.left( i );

        i = m_TilesFile.lastIndexOf( "/" );
        if( i == -1 ) tilesFile = m_TilesFile;
        else tilesFile = m_TilesFile.left( i );

        i = m_ProjectFile.lastIndexOf( "/" );
        if( i == -1 ) projectFile = m_ProjectFile;
        else projectFile = m_ProjectFile.left( i );

        if( screenFile == projectFile && tilesFile == projectFile ) {
            if( i == -1 ) {
                projectFile = m_ProjectFile;
                screenFile  = m_ScreenFile;
                paletteFile = m_PaletteFile;
                tilesFile   = m_TilesFile;
            } else {
                projectFile = m_ProjectFile.right( m_ProjectFile.size() - m_ProjectFile.lastIndexOf( "/" ) - 1 );
                screenFile  = m_ScreenFile.right( m_ScreenFile.size() - m_ScreenFile.lastIndexOf( "/" ) - 1 );
                paletteFile = m_PaletteFile.right( m_PaletteFile.size() - m_PaletteFile.lastIndexOf( "/" ) - 1 );
                tilesFile   = m_TilesFile.right( m_TilesFile.size() - m_TilesFile.lastIndexOf( "/" ) - 1 );
            }
        } else {
            projectFile = m_ProjectFile;
            screenFile = m_ScreenFile;
            paletteFile = m_PaletteFile;
            tilesFile = m_TilesFile;
        }

        hFile.setFileName( m_ProjectFile );
        hFile.open( QIODevice::WriteOnly );

        QTextStream str( &hFile );

        str << "PROJECT" << endl;

        str << "SCREEN" << endl;
        str << screenFile << endl;

        str << "TILES" << endl;
        str << tilesFile << endl;
        if( m_pScreen->GetOneBank() ) str << "ONEBANK" << endl;
        else str << "ALLBANKS" << endl;

        str << "PALETTE" << endl;
        str << paletteFile << endl;

        str << "END" << endl;
        hFile.flush();
        hFile.close();
    }

}

void MainWindow::EnableMenuItems() {

// Project:
    ui->action_Project_New->setEnabled( m_pScreen == NULL );
    ui->action_Project_Load->setEnabled( true );
    ui->action_Project_Save->setEnabled( m_pScreen != NULL );
    ui->action_Project_Save_as->setEnabled( m_pScreen != NULL );
    ui->action_Project_Close->setEnabled( m_pScreen != NULL );

// Screen:
    ui->action_Screen_New->setEnabled( m_pScreen!= NULL );
    ui->action_Screen_Load->setEnabled( m_pScreen != NULL );
    ui->action_Screen_Save->setEnabled( m_pScreen != NULL );
    ui->action_Screen_Save_as->setEnabled( m_pScreen != NULL );
    ui->action_Screen_Export_Screen->setEnabled( m_pScreen != NULL );
    ui->action_Screen_Import_SC2->setEnabled( m_pScreen != NULL );
    ui->action_Screen_Export_SC2->setEnabled( m_pScreen != NULL );

// Palette:
    ui->action_Palette_New->setEnabled( m_pScreen != NULL );
    ui->action_Palette_Load->setEnabled( m_pScreen != NULL );
    ui->action_Palette_Save->setEnabled( m_pScreen != NULL );
    ui->action_Palette_Save_as->setEnabled( m_pScreen != NULL );
    ui->action_Palette_Export_ASM->setEnabled( m_pScreen != NULL );
    ui->action_Palette_Export_VDP->setEnabled( m_pScreen != NULL );

// Tiles:
    ui->action_Tiles_New->setEnabled( m_pScreen != NULL );
    ui->action_Tiles_Load->setEnabled( m_pScreen != NULL );
    ui->action_Tiles_Save->setEnabled( m_pScreen != NULL );
    ui->action_Tiles_Save_as->setEnabled( m_pScreen != NULL );
    ui->action_Tiles_Load_from_Library->setEnabled( m_pScreen != NULL );
    ui->action_Tiles_Export_ASM_data->setEnabled( m_pScreen != NULL );
    ui->action_Tiles_Export_ASM_data_Hexadecimal->setEnabled( m_pScreen != NULL );
    ui->action_Tiles_Export_bin_data->setEnabled( m_pScreen != NULL );
    ui->action_Tiles_Export_bin_data_compressed_whith_Pletter->setEnabled( m_pScreen != NULL );

// Screen Tools:
    ui->action_Screen_Tools_Fill_Screen_whith_tiles_1_to_255->setEnabled( m_pScreen != NULL );

// Palette Tools:
    ui->action_Palette_Tools_Change_Palette->setEnabled( m_pScreen != NULL );

// Tiles Tools:
    ui->action_Tiles_Tools_Copy->setEnabled( m_pScreen != NULL );
    ui->action_Tiles_Tools_Paste->setEnabled( m_pScreen != NULL );
    ui->action_Tiles_Tools_Paste_colors->setEnabled( m_pScreen != NULL );
    ui->action_Tiles_Tools_Undo->setEnabled( m_pScreen != NULL );
    ui->action_Tiles_Tools_Reorder_colors->setEnabled( m_pScreen != NULL );
    ui->action_Tiles_Tools_Group_Tiles->setEnabled( m_pScreen != NULL );
    ui->action_Tiles_Tools_Exchange_tiles->setEnabled( m_pScreen != NULL );

// PNG Tools:
    ui->action_PNG_Tools_Export_PNG_Screen_file->setEnabled( m_pScreen != NULL );
    ui->action_PNG_Tools_Export_Map_PNG_file->setEnabled( m_pScreen != NULL );
    ui->action_PNG_Tools_Export_PNG_Tiles_file->setEnabled( m_pScreen != NULL );
    ui->action_PNG_Tools_Import_PNG_Tiles_file->setEnabled( m_pScreen != NULL );

// Sprites 1:
    ui->action_Sprites_1_New->setEnabled( m_pSprites == NULL );
    ui->action_Sprites_1_Load->setEnabled( true );
    ui->action_Sprites_1_Save->setEnabled( m_pSprites != NULL );
    ui->action_Sprites_1_Save_as->setEnabled( m_pSprites != NULL );
    ui->action_Sprites_1_Close->setEnabled( m_pSprites != NULL );
    ui->action_Sprites_1_Export->setEnabled( m_pSprites != NULL );

// Sprites 2:
    ui->action_Sprites_2_New->setEnabled( false );
    ui->action_Sprites_2_Load->setEnabled( false );
    ui->action_Sprites_2_Save->setEnabled( false );
    ui->action_Sprites_2_Save_as->setEnabled( false );
    ui->action_Sprites_2_Close->setEnabled( false );
    ui->action_Sprites_2_Export->setEnabled( false );
}

// --------------------------------------------------------------
// COSAS QUITADAS: ( TRASTERO )
// --------------------------------------------------------------

/*
//  ui->action_Project_Import_Polka->setEnabled( true );
//  ui->action_Project_Export_Polka->setEnabled( m_pScreen != NULL );
void MainWindow::on_action_Project_Import_Polka_triggered() {
    QString relPath;
    QFile hFile;
    QString fileName;

    on_action_Project_Close_triggered();

    fileName = QFileDialog::getOpenFileName( this,
        "Import Polka! project", m_LastPathLoadProject,
        "Polka! project (*.ppr);;All files (*.*)"
    );

    if( fileName == "" ) return;

    m_LastPathLoadProject = fileName.left( fileName.lastIndexOf( "/" ) );

    InitScreen();

//  m_pScreen->ImportPolkaScreen( fileName );
    m_pScreen->ImportPolkaTiles( fileName );

    EnableMenuItems();

}
void MainWindow::on_action_Project_Export_Polka_triggered() {
    QString fileName;

    fileName = QFileDialog::getSaveFileName( this,
        "Export Polka! project", m_LastPathExport,
        "Polka! project (*.ppr);;All files (*.*)"
    );

    if( fileName > "" )
    {
        m_LastPathExport = fileName.left( fileName.lastIndexOf( "/" ) );
        m_pScreen->ExportPolkaTiles( fileName );
    }

}
*/

/*
// ui->action_Palette_Create_PNG->setEnabled( m_pScreen != NULL );
void MainWindow::on_action_Palette_Create_PNG_triggered() {
    QString fileName;
    fileName = QFileDialog::getSaveFileName( this,
        "Palette - Create PNG Palette file as", m_LastPathLoadProject,
        "Images (*.png);;All files (*.*)"
    );
    if( fileName > "" ) {
        m_LastPathLoadProject = fileName.left( fileName.lastIndexOf( "/" ));
        m_pScreen->GenPNGPaletteFile( fileName );
    }
}
*/

/*
// ui->action_Tiles_Import->setEnabled( m_pScreen != NULL );
void MainWindow::on_action_Tiles_Import_triggered() {
    QString fileName;
    QString fileNamePalette;
    QMessageBox::information( this, "", "First you will be prompted for an Image with Palette (see manual). Then you will be prompted for the Tiles Image" );
        fileNamePalette = QFileDialog::getOpenFileName( this,
        "Tiles - Import Image with Palette", m_LastPathLoadProject,
        "Palette (*.png);;All files (*.*)"
    );
     if( fileNamePalette == "" ) {
        QMessageBox::critical( this, "Error", "No palette, no image!" );
        return;
    }
    m_LastPathLoadProject = fileNamePalette.left( fileNamePalette.lastIndexOf( "/" ) );
    fileName = QFileDialog::getOpenFileName( this,
        "Tiles - Import Image with Tiles", m_LastPathLoadProject,
        "Image (*.png);;All files (*.*)"
    );
    if( fileName == ""  ) {
        QMessageBox::critical( this, "Error", "No file selected!" );
        return;
    }
    m_LastPathLoadProject = fileName.left( fileName.lastIndexOf( "/" ) );
    if( !m_pScreen->ImportTiles( fileName, fileNamePalette ) ) {
        QMessageBox::critical( this, "Error", "Not supported format file" );
    }
}
*/
