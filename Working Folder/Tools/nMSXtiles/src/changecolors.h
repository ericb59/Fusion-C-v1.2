#ifndef CHANGECOLORS_H
#define CHANGECOLORS_H

#include "cscreenw.h"
#include <QDialog>

class QComboBox;

namespace Ui {
class CChangeColors;
}

class CChangeColors : public QDialog
{
    Q_OBJECT
    
public:
    explicit CChangeColors(QWidget *parent = 0);
    ~CChangeColors();

    QList<QPoint> get_Changes();

    
private:
    Ui::CChangeColors *ui;
    CScreenW* p_screen;
    QList<QPoint> m_Changes;
    int m_AutoChanging;

    void AssignToCombo( QComboBox *pCombo, int color );
    void ChangeColor( int combo, int index );

private slots:
    void OnOk();
    void OnCancel();
    void OnColor0Changed( int );
    void OnColor1Changed( int );
    void OnColor2Changed( int );
    void OnColor3Changed( int );
    void OnColor4Changed( int );
    void OnColor5Changed( int );
    void OnColor6Changed( int );
    void OnColor7Changed( int );
    void OnColor8Changed( int );
    void OnColor9Changed( int );
    void OnColor10Changed( int );
    void OnColor11Changed( int );
    void OnColor12Changed( int );
    void OnColor13Changed( int );
    void OnColor14Changed( int );
    void OnColor15Changed( int );
};

#endif // CHANGECOLORS_H

