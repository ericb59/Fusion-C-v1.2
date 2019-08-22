#ifndef SCREENW_H
#define SCREENW_H

#include "tile.h"
#include "labelbtn.h"
#include <QCloseEvent>
#include <QLabel>
#include <QDialog>
#include <QObject>
#include <QImage>
#include <QPoint>
#include <QPoint>
#include <QTabWidget>
#include <QDrag>

namespace Ui {
    class CScreenW;
}

class CScreenW : public QDialog
{
    Q_OBJECT
	
	static const int PAL_ALL;

signals:
    void signal_close();

protected:
    void closeEvent( QCloseEvent *pEvent );
    void keyPressEvent(QKeyEvent *e);

public:
    explicit CScreenW( QWidget *parent = 0 );
    ~CScreenW();
	void Initialize();

// -----------------------------------
// Screen:
    // New
	void NewScreen();
    // Load
	bool LoadScreen( QString fileName );
    // Save
    // Save as...
	bool SaveScreen( QString fileName );
    // Export Screen ( CDlgExportScreen )
    bool ExportScreenData( QString fileName, bool hexa, int initX, int initY, int w, int h );
    bool ExportScreenBin( QString fileName, int initX, int initY, int w, int h );
    bool ExportScreenBinScroll( QString fileName, int initX, int initY, int w, int h );
    bool ExportScreenBinPletter( QString fileName, int initX, int initY, int w, int h );
    bool ExportScreenBinPletterScroll( QString fileName, int initX, int initY, int w, int h );


    // Import Screen SC2
    bool ImportSC2( QString fileName );
    bool ExportSC2( QString fileName );
// -----------------------------------
// Palette:
    void UpdatePalette();
    // New
    void NewPalette();
    // Load
    bool LoadPalette( QString fileName );
    // Save
    // Save as...
    bool SavePalette( QString fileName );
    // Export Palette ASM
    bool ExportPaletteASM( QString fileName );
    // Export Palette VDP
    bool ExportPaletteVDP( QString fileName );

// -----------------------------------
// Tiles:
    // New
	void NewTiles();
    // Load
	bool LoadTiles( QString fileName );
    // Save
    // Save as...
    bool SaveTiles( QString fileName );
    // Load from Library
	bool LoadTilesLibrary( QString fileName, int bankOr, int xOr, int yOr, int width, int height, int bankDest, int xDest, int yDest );
    // Export ASM data...
    // Export ASM data Hezadecimal...
	bool ExportTilesData( QString fileName, bool hexa );
    // Export bin data...
	bool ExportTilesBin( QString fileName );
    // Export bin data compressed whith Pletter
	void ExportTilesBinPletter( QString fileName );

// -----------------------------------
// Screen Tools:
    // Fill Screen whith tiles 1 to 255
    void Fill1to255();

// -----------------------------------
// Palette Tools:
    // Change Palette ( changepalette )

// -----------------------------------
// Tiles Tools:
    // Copy
    void CopyTile();
    // Paste
    void PasteTile();
    // Paste colors
    void PasteTileColors();
    // Undo
    void Undo();
    // Reorder colors
    void ReorderColors();
    // Group Tiles
    void GroupTiles();
    // Exchange tiles ( CDlgExchangeTiles )
    void ExchangeTiles( int tile1, int tile2 );

// -----------------------------------
// PNG Tools:
    // Expor Screen PNG File
    bool ExportScreenPNG( QString fileName );
    // Export Map PNG File
    bool ExportMapPNG( QString fileName );
    // Export PNG file
    void ExportTilesPNG( QString fileName );
    // Import PNG file crated whith nMSXtiles
    bool ImportTiles( QString fileName );

// -----------------------------------
// Funciones Públicas:
    int  get_MapWidth();
    int  get_MapHeight();
    void SetMapWidth( int value);
    void SetMapHeight( int value);
    void SetOneBank( bool value );
    bool GetOneBank();
// -----------------------------------

public slots:

// -----------------------------------
// Las acciones del ratón sobre el Screen:
// onClickedLeft( int, int )    -> OnScreenClickLeft( int, int  )
// onReleasedLeft( int, int )   -> OnScreenReleaseLeft( int, int )
// onReleasedRight( int, int )  -> OnScreenReleaseRight( int, int )
// onMouseMoveEvent( int, int ) -> OnScreenMouseMoveEvent( int, int )
    void OnScreenClickLeft( int x, int y );
    void OnScreenReleaseLeft( int x, int y );
    void OnScreenReleaseRight( int x, int y );
    void OnScreenMouseMoveEvent( int x, int y );
//  void UpdateScreen(); Función auxiliar privada
// -----------------------------------

// -----------------------------------
// Las acciones del ratón sobre el MultiScreen: ( m_pFrmMultiScreen )
// ui->m_pGrMapX      -> valueChanged(int) -> OnMapXChanged(int)
// ui->m_pGrMapY      -> valueChanged(int) -> OnMapYChanged(int)
// ui->m_pGrNextX     -> clicked()         -> OnMapNextX()
// ui->m_pGrNextY     -> clicked()         -> OnMapNextY()
// ui->m_pGrPrevX     -> clicked()         -> OnMapPrevX()
// ui->m_pGrPrevY     -> clicked()         -> OnMapPrevY()
// ui->m_pGrMapWidth  -> valueChanged(int) -> OnMapWidthChanged( int)
// ui->m_pGrMapHeight -> valueChanged(int) -> OnMapHeightChanged(int)
// ui->m_pGrOneBank   -> clicked()         -> OnOneBank()
// -----------------------------------
    void OnMapXChanged( int value );
    void OnMapYChanged( int value );
    void OnMapNextX();
    void OnMapPrevX();
    void OnMapNextY();
    void OnMapPrevY();
    void OnMapWidthChanged( int value );
    void OnMapHeightChanged(int value );
    void OnOneBank();         // Conmuta entre tres y un banco de baldosas
//  void SetOneBank( bool value );  Auxiliar y pública
//	bool GetOneBank();              Auxiliar y pública
// -----------------------------------

// -----------------------------------
// Las acciones del ratón sobre los Bancos:
// m_pTabBank    -> currentChanged(int)         -> OnBankChanged( int )
// m_pLblBank[0] -> onClickedLeft(int,int)      -> OnBank0Click( int, int )
// m_pLblBank[1] -> onClickedLeft(int,int)       -> OnBank1Click( int, int )
// m_pLblBank[2] -> onClickedLeft(int,int)       -> OnBank2Click( int, int )
// m_pLblBank[0] -> onClickedRight(int,int)      -> OnBank0ClickRight( int, int )
// m_pLblBank[1] -> onClickedRight(int,int)      -> OnBank1ClickRight( int, int )
// m_pLblBank[2] -> onClickedRight(int,int)      -> OnBank2ClickRight( int, int )
// m_pLblBank[0] -> onDoubleClickedLeft(int,int) -> OnBank0DoubleClick( int, int )
// m_pLblBank[1] -> onDoubleClickedLeft(int,int) -> OnBank1DoubleClick( int, int )
// m_pLblBank[2] -> onDoubleClickedLeft(int,int) -> OnBank2DoubleClick( int, int )
    void OnBankChanged( int index );
    void OnBank0Click( int x, int y);
    void OnBank1Click( int x, int y);
    void OnBank2Click( int x, int y);
//  void OnBankClick( int x, int y, int bank ); Función auxiliar privada
    void OnBank0ClickRight( int x, int y);
    void OnBank1ClickRight( int x, int y);
    void OnBank2ClickRight( int x, int y);
//  void OnBankClickRight( int x, int y, int bank ); Función auxiliar privada
    void OnBank0DoubleClick( int x, int y );
    void OnBank1DoubleClick( int x, int y );
    void OnBank2DoubleClick( int x, int y );
//	void OnBankDoubleClick( int x, int y, int bank ); Función auxiliar privada
//  void UpdateBank( int tile, int bank, bool updateBank = true ); Función auxiliar privada
//  void InitBanks(); Función auxiliar privada
// -----------------------------------

// -----------------------------------
// Las acciones del ratón sobre el Editor de Baldosas ( m_pFrmDesigner )
// m_pLblPalette -> onClickedLeft( int, int ) -> OnPaletteClick( int, int )
// m_pLblPalBack -> onClickedLeft( int, int ) -> OnPalBackClick( int, int )
// m_pLblPalFore -> onClickedLeft( int, int ) -> OnPalForeClick( int, int )
// m_pLblTile    -> onClickedLeft( int, int ) -> OnTileClickLeft( int, int )
// m_pLblTile    -> onClickedRight(int, int ) -> OnTileClickRight( int, int )
// m_pLblColors  -> onClickedLeft( int, int ) -> OnColorsClick( int, int )
// m_pLblColors  -> onClickedRight(int, int ) -> OnColorsChange( int, int )
// ui->m_pBtnFromBank -> clicked() -> OnFromBank()
// ui->m_pBtnToBank   -> clicked() -> OnToBank()
// ui->m_pRadBack     -> clicked() -> OnRadBack()
// ui->m_pRadFore     -> clicked() -> OnRadFore()
// ui->m_pGrTileClear          -> clicked() -> OnTileClear()
// ui->m_pGrTileReorder        -> clicked() -> OnTileReorder()
// ui->m_pGrTileShiftDown      -> clicked() -> OnTileShiftDown()
// ui->m_pGrTileShiftUp        -> clicked() -> OnTileShiftUp()
// ui->m_pGrTileShiftLeft      -> clicked() -> OnTileShiftLeft()
// ui->m_pGrTileShiftRight     -> clicked() -> OnTileShiftRight()
// ui->m_pGrTileFlipHorizontal -> clicked() -> OnTileFlipHorizontal()
// ui->m_pGrTileFlipVertical   -> clicked() -> OnTileFlipVertical()
// ui->m_pGrTileInvertColors   -> clicked() -> OnTileInvertColors()
// ui->m_pGrTileRotate         -> clicked() -> OnTileRotate()
    void OnPaletteClick( int x, int y );
    void OnPalBackClick( int, int );
    void OnPalForeClick( int, int );
    void OnTileClickLeft( int x, int y );
    void OnTileClickRight( int x, int y );
    void OnColorsClick( int x, int y );
    void OnColorsChange( int x, int y ); // Cambia el color de la paleta (Nuevo) changepalette
    void OnFromBank();
    void OnToBank();
    void OnRadBack();
    void OnRadFore();
    void OnTileClear();
    void OnTileReorder();
    void OnTileShiftDown();
    void OnTileShiftUp();
    void OnTileShiftLeft();
    void OnTileShiftRight();
    void OnTileFlipHorizontal();
    void OnTileFlipVertical();
    void OnTileInvertColors();
    void OnTileRotate();
// -----------------------------------

// -----------------------------------
// Diálogo de cambiar colores y un solo banco ( m_pFrmColorsOneBank )
// ui->m_pChangeColors -> clicked() -> OnChangeColors()
    void OnChangeColors();
// -----------------------------------

// -----------------------------------
// Tiles Block of (m_pFrmTilesBlocksOf)
// ui->m_pGrTilesBlockW -> valueChanged(int) -> OnTilesBlockChanged(int)
// ui->m_pGrTilesBlockH -> valueChanged(int) -> OnTilesBlockChanged(int)
   void OnTilesBlockChanged( int );
// -----------------------------------

// -----------------------------------
// m_pFrmBackgroundTiles
//  int  GetBackgroundTile( int y ); // private
// -----------------------------------

public:
    uint COLORS_TABLE_SCREEN[16];
private:
    Ui::CScreenW *ui;

// Funciones privadas:

    // Auxiliares de Screen
    void UpdateScreen();

    // Auxiliares de solts Bancos
    void OnBankClick( int x, int y, int bank );
    void OnBankClickRight( int x, int y, int bank );
    void OnBankDoubleClick( int x, int y, int bank );
    void UpdateBank( int tile, int bank, bool updateBank = true );
    void InitBanks();

    // Auxiliares del Editor de Baldosas
    void SetTileDesign();
	void CopyToTileDesign( int index, int bank );

    // Auxiliares del Diálogo de cambiar colores y un solo banco

    // m_pFrmBackgroundTiles
    int  GetBackgroundTile( int y );    // m_pFrmBackgroundTiles
	
    // Auxiliares Privadas:
	void PaintTile( QImage *pImage, CTile &tile, int posX, int posY, int dotSize );
	void PaintGrid( QImage *pImage, int gridWidth, int gridHeight );
    int  GetColor( unsigned int color, unsigned int palette[] );
    bool DeduceColorPalette( unsigned int color, unsigned int palette[16] );
    void CheckImagePalette( QString fileName, unsigned int palette[] );

// Variables privadas:
    QTabWidget *m_pTabBank;
    QFrame *m_pFrmBank[3];
    CLabelBtn *m_pLblBank[3];
    CLabelBtn *m_pLblPalette;
    CLabelBtn *m_pLblPalBack;
    CLabelBtn *m_pLblPalFore;
    CLabelBtn *m_pLblTile;
    CLabelBtn *m_pLblColors;
    CLabelBtn *m_pLblScreen;

    bool m_Selecting;
    CTile m_CopyTile;
    CTile m_UndoTile;

    QPoint m_SelColor;                  // El color actual seleccionado
    QPoint m_SelTile;                   // La baldosa actual seleccionada

    QPoint m_LastGridValue;
    QList<QVariant> m_Undo;

    QPoint m_LastClickPos;
    CLabelBtn *m_pLblSelection;
    QList< QList<int> > m_ScreenSelection;		//Codis de Tile sel_leccionats
    QList< QList<int> > m_ScreenSelectionCopy;	//Copia de m_ScreenSelection en fer Copiar
    QRect m_SelectionRect;						//Rectangle de sel_leccio
    int m_LastTilesBlockW;
    int m_LastTilesBlockH;
    int m_MouseX;
    int m_MouseY;

    QImage m_CursorImage;
    CLabelBtn *m_pLblCursor[4];
    QDrag *m_pDrag;
    int m_LastBank;
    int m_LastBankX;
    int m_LastBankY;

//  int m_CurrentScreen; // ??? Sin uso

    // El mapa consistente en 64 x 64 habitaciones de 256 * 3 baldosas
    int m_Screen[64][64][256*3];	// Codi de Tile assignat
    unsigned char m_Buffer[256*3*64*64]; //TODO - 1  Un buffer para el mapa
    int m_MapScreenX;               // Las habitación actual dentro del ancho del mapa
    int m_MapScreenY;               // Las habitación actual dentro del alto del mapa
    // ui->m_pGrMapWidth->value();  // El ancho en habitaciones del mapa
    // ui->m_pGrMapHeight->value(); // El alto  en habitaciones del mapa

    // Tres bancos de baldosas de 256 baldosas cada banco
    CTile m_TilesBank[256][3];
    CTile m_TileDesign;         // La baldosa en diseño
    QImage m_BankImages[3];

    // Las etiquetas de columnas y filas...
    QLabel *m_pColsLabels[32];
    QLabel *m_pRowsLabels[24];

};

#endif

// QUITADO:
//	bool ImportPolkaTiles( QString fileName );
//	bool ExportPolkaTiles( QString fileName );
//  bool ImportTiles( QString fileName, QString paletteFileName = "" );
//  bool GenPNGPaletteFile( QString fileName ); // Create PNG Palette file
