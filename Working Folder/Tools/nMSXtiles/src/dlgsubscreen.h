#ifndef DLGSUBSCREEN_H
#define DLGSUBSCREEN_H

#include <QDialog>

namespace Ui {
class CDlgSubscreen;
}

class CDlgSubscreen : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgSubscreen(QWidget *parent = 0);
    ~CDlgSubscreen();

    int get_DefX();
    int get_DefY();
    int get_DefWidth();
    int get_DefHeight();
    
private:
    Ui::CDlgSubscreen *ui;
    int m_DefX;
    int m_DefY;
    int m_DefWidth;
    int m_DefHeight;

private slots:
    void OnOk();
    void OnCancel();
};

#endif // DLGSUBSCREEN_H
