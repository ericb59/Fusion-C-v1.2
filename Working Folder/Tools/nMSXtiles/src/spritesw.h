#ifndef SPRITESW_H
#define SPRITESW_H

#include <ui_fspritesw.h>
#include "labelbtn.h"
#include <QCloseEvent>
#include <QDialog>
#include <QList>
#include <QRadioButton>

class CSpritesSet
{
public:
	CSpritesSet();

	void SetColor( int sprite, int color );
	int GetColor( int sprite );
	
	int GetValue( int sprite, int index );
	void SetValue( int sprite, int index, int value );

private:
	int m_Set[4][256];
	int m_Colors[4];
};

class CSpritesW : public QDialog, public Ui_FSpritesW
{
	Q_OBJECT

signals:
    void signal_close();

public:
    explicit CSpritesW( QWidget *parent = 0 );
    ~CSpritesW();

protected:
    void closeEvent( QCloseEvent *pEvent );

public:
	void Initialize();

	void NewSprites();
	bool LoadSprites( QString fileName );
	bool SaveSprites( QString fileName );
	bool ExportSprites( QString fileName );

public slots:
	void OnSpritesClickLeft( int, int );
	void OnSpritesClickRight( int, int );

	void OnCombos0Click( int );
	void OnCombos1Click( int );
	void OnCombos2Click( int );
	void OnCombos3Click( int );
	void OnCombosClick( int combo, int index );
	void OnFlipHClick();
    void OnFlipVClick();
    void OnShiftDownClick();
    void OnShiftUpClick();

	void OnSetsClick( int );
    void OnRadiosClick( int );

    // void on_Signal_Change_Palette();

private:
    static const int BG_COLOR;

	QComboBox *m_pSprites[4];
	QRadioButton *m_pSelSprite[4];
	CLabelBtn *m_pLblSprites;
	CLabelBtn *m_pLblViews[4];
	QList<CSpritesSet*> m_Sprites;    
    uint COLORS_TABLE_SPRITES[16];

	int m_SelSprite;
	int m_SelSet;

//	void UpdateDesigner();
	void UpdateDesigner( int set );
	void UpdateDesigner( int set, int sprite );
	void UpdateDesigner( int set, int sprite, int index, bool paintIsZ, QImage *pImage = NULL );

    void UpdatePreview();
	void UpdatePreview( int set, int sprite, int index, bool paintIsZ, QImage *pImage = NULL );
};

#endif
