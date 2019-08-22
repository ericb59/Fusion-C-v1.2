#ifndef CHANGEPALETTE_H
#define CHANGEPALETTE_H

#include <QDialog>
#include <QPushButton>
#include <QColor>

namespace Ui {
class changepalette;
}

class changepalette : public QDialog
{
    Q_OBJECT
signals:
    void signal_change_palette();

public:
    explicit changepalette( QWidget *parent, uint pal[16], uint index );
    ~changepalette();
    void setPalette( uint pal[16], uint index );

private slots:
    void on_slider_R_valueChanged(int value);
    void on_slider_G_valueChanged(int value);
    void on_slider_B_valueChanged(int value);
    void on_PaletteMSX1_clicked();
    void on_buttonClose_clicked();

private:
    Ui::changepalette *ui;
//    uint m_palette[16];
    uint  *m_pal;       // Puntero a la paleta padre
    QColor m_color;     // Color seleccionado
    int m_button;       // Botón seleccionado
    QPushButton* m_color_button[16];
    void on_color_click( int i );
    void setSliders();
    void setColor();

};

#endif // CHANGEPALETTE_H
