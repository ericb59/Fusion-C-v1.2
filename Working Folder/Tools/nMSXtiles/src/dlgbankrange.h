#ifndef DLGBANKRANGE_H
#define DLGBANKRANGE_H

#include <ui_dlgbankrange.h>

class CDlgBankRange : public QDialog, public Ui_FDlgBankRange
{
	Q_OBJECT
private:
	int m_FromTile;
	int m_ToTile;
	int m_BankOption;
public:
	CDlgBankRange();

	int get_FromTile();
	int get_ToTile();
	int get_BankOption();
	
private slots:

	void OnOk();
	void OnCancel();
};

#endif // DLGBANKRANGE_H
