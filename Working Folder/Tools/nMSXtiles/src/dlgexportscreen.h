#ifndef DLGEXPORTSCREEN_H
#define DLGEXPORTSCREEN_H

#include "ui_exportscreen.h"
#include <QWidget>
#include <QDialog>


class CDlgExportScreen : public QDialog
{
    Q_OBJECT
public:
    enum TYPE_FORMAT {
        TYPE_ASM = 0, TYPE_ASM_HEXA, TYPE_BIN, TYPE_BIN_PLETTER
    };

    enum TYPE_FILE_MODE {
        TYPE_ONE_BY_SCREEN = 0, TYPE_ONE_WHOLE_MAP
    };

    CDlgExportScreen(QString lastPath, int lastFormat, int lastFileMode, int lastWidth, int lastHeight, int lastX, int lastY);


    int get_X();
    int get_Y();
    int get_Width();
    int get_Height();
    TYPE_FORMAT get_Format();
    TYPE_FILE_MODE get_FileMode();
    QString get_FileName();


private:
    Ui_FExportScreen *m_pForm;
    int m_X;
    int m_Y;
    int m_Width;
    int m_Height;
    TYPE_FORMAT m_Format;
    TYPE_FILE_MODE m_FileMode;
    QString m_FileName;
    QString m_LastPath;
    int m_LastFileMode;
    int m_LastFormat;
    int m_LastWidth;
    int m_LastHeight;

private slots:
    void OnOk();
    void OnSelFileName();
    void OnFormatChanged( int index );

};

#endif // DLGEXPORTSCREEN_H
