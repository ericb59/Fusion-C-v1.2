#ifndef DLGEXCHANGETILES_H
#define DLGEXCHANGETILES_H

#include <QDialog>

namespace Ui {
class CDlgExchangeTiles;
}

class CDlgExchangeTiles : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgExchangeTiles(QWidget *parent = 0);
    ~CDlgExchangeTiles();


    int get_Tile1();
    int get_Tile2();

private:
    Ui::CDlgExchangeTiles *ui;
    int m_Tile1;
    int m_Tile2;

private slots:
    void OnOk();

};

#endif // DLGEXCHANGETILES_H
