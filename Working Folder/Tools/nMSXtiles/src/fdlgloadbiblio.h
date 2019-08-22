#ifndef FDLGLOADBIBLIO_H
#define FDLGLOADBIBLIO_H

#include <QDialog>
#include "ui_dlgloadbiblio.h"

class FDlgLoadBiblio : public QDialog
{
    Q_OBJECT

public:
    FDlgLoadBiblio( QString lastPath );

    QString get_FileName();
    int get_XOr();
    int get_YOr();
    int get_XDest();
    int get_YDest();
    int get_Widht();
    int get_Height();
    int get_BankOr();
    int get_BankDest();

private:
    Ui_DlgLoadBiblio m_DlgLoad;
    QString m_LastPath;

private slots:
    void OnLibraryFile();
};

#endif // FDLGLOADBIBLIO_H
