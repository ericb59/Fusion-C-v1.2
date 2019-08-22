#include "dlgbankrange.h"

CDlgBankRange::CDlgBankRange()
{
	setupUi( this );


	m_pCboBank->addItem( "All banks" );
	m_pCboBank->addItem( "Bank 0" );
	m_pCboBank->addItem( "Bank 1" );
	m_pCboBank->addItem( "Bank 2" );

	m_pCboBank->setCurrentIndex( 0 );

	m_pFromTile->setText( "0" );
	m_pToTile->setText( "255" );

	connect( m_pOk, SIGNAL( clicked() ), this, SLOT( OnOk() ) );
	connect( m_pCancel, SIGNAL( clicked() ), this, SLOT( OnCancel() ) );
}


int CDlgBankRange::get_FromTile()
{
	return m_FromTile;
}
	
int CDlgBankRange::get_ToTile()
{
	return m_ToTile;
}

int CDlgBankRange::get_BankOption()
{
	return m_BankOption;
}


void CDlgBankRange::OnOk()
{
	bool ok;
	
    m_FromTile = m_pFromTile->text().toInt( &ok );
    if( !ok ) m_FromTile = 0;
    if( m_FromTile < 0   ) m_FromTile = 0;
    if( m_FromTile > 255 ) m_FromTile = 255;

	m_ToTile = m_pToTile->text().toLongLong( &ok );
    if( !ok ) m_ToTile = 255;
    if( m_ToTile < m_FromTile ) m_ToTile = m_FromTile;
    if( m_ToTile > 255 )  m_ToTile = 255;

	m_BankOption = m_pCboBank->currentIndex();

	accept();
}

void CDlgBankRange::OnCancel()
{
	close();
}


