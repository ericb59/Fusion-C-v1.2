#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>

#include "cscreenw.h"
#include "spritesw.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent( QCloseEvent *pEvent );

private slots:

    // Menu Project:
    void on_action_Project_New_triggered();
    void on_action_Project_Load_triggered();
    void on_action_Project_Save_triggered();
    void on_action_Project_Save_as_triggered();
    void on_action_Project_Close_triggered();
    void on_action_Project_Quit_triggered();

    // Menu Screen:
    void on_action_Screen_New_triggered();
    void on_action_Screen_Load_triggered();
    void on_action_Screen_Save_triggered();
    void on_action_Screen_Save_as_triggered();
    void on_action_Screen_Export_Screen_triggered();
    void on_action_Screen_Export_SC2_triggered();
    void on_action_Screen_Import_SC2_triggered();

    // Menu Palette:
    void on_action_Palette_New_triggered();
    void on_action_Palette_Load_triggered();
    void on_action_Palette_Save_triggered();
    void on_action_Palette_Save_as_triggered();
    void on_action_Palette_Export_ASM_triggered();
    void on_action_Palette_Export_VDP_triggered();

    // Menu Tiles:
    void on_action_Tiles_New_triggered();
    void on_action_Tiles_Load_triggered();
    void on_action_Tiles_Save_triggered();
    void on_action_Tiles_Save_as_triggered();
    void on_action_Tiles_Load_from_Library_triggered();
    void on_action_Tiles_Export_ASM_data_triggered();
    void on_action_Tiles_Export_ASM_data_Hexadecimal_triggered();
    void on_action_Tiles_Export_bin_data_triggered();
    void on_action_Tiles_Export_bin_data_compressed_whith_Pletter_triggered();

    // Menu Screen Tools:
    void on_action_Screen_Tools_Fill_Screen_whith_tiles_1_to_255_triggered();

    // Menu Palette Tools:
    void on_action_Palette_Tools_Change_Palette_triggered();

    // Menu Tiles Tools:
    void on_action_Tiles_Tools_Copy_triggered();
    void on_action_Tiles_Tools_Paste_triggered();
    void on_action_Tiles_Tools_Paste_colors_triggered();
    void on_action_Tiles_Tools_Undo_triggered();
    void on_action_Tiles_Tools_Reorder_colors_triggered();
    void on_action_Tiles_Tools_Group_Tiles_triggered();
    void on_action_Tiles_Tools_Exchange_tiles_triggered();

    // Menu PNG Tools:
    void on_action_PNG_Tools_Export_PNG_Screen_file_triggered();
    void on_action_PNG_Tools_Export_Map_PNG_file_triggered();
    void on_action_PNG_Tools_Export_PNG_Tiles_file_triggered();
    void on_action_PNG_Tools_Import_PNG_Tiles_file_triggered();

    // Menu Sprites:
    void on_action_Sprites_1_New_triggered();
    void on_action_Sprites_1_Load_triggered();
    void on_action_Sprites_1_Save_triggered();
    void on_action_Sprites_1_Save_as_triggered();
    void on_action_Sprites_1_Close_triggered();
    void on_action_Sprites_1_Export_triggered();

    // About:
    void on_action_About_nMSXtiles_triggered();
    void on_action_About_pentacour_triggered();
    void on_action_About_pipagerardo_triggered();
    void on_action_About_QT_triggered();

private:
    Ui::MainWindow *ui;

    QMdiSubWindow *m_pScreen_sbw;
    QMdiSubWindow *m_pSprites_sbw;
    CScreenW  *m_pScreen;
    CSpritesW *m_pSprites;

    QString m_ProjectFile;
    QString m_ScreenFile;
    QString m_PaletteFile;
    QString m_TilesFile;
    QString m_SpritesFile;
    QString m_LastPathLoadProject;
    QString m_LastPathExport;
    QString m_Version;

    int m_LastFormat;
    int m_LastFilesMode;
    int m_LastWidth;
    int m_LastHeight;
    int m_LastX;
    int m_LastY;

    void InitScreen();
    void QuitScreen();
    void InitSprites();
    void QuitSprites();
    void SaveProject();
    void EnableMenuItems();

};

#endif // MAINWINDOW_H


// QUITADO:
// void on_action_Project_Import_Polka_triggered();
// void on_action_Project_Export_Polka_triggered();
// void on_action_Palette_Create_PNG_triggered();
// void on_action_Tiles_Import_triggered();
