//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnitAYChannelSelect.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormAyChn *FormAyChn;

extern int import_chan;
extern bool import_ok;

//---------------------------------------------------------------------------
__fastcall TFormAyChn::TFormAyChn(TComponent* Owner)
: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TFormAyChn::Button1Click(TObject *Sender)
{
	import_ok=true;
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TFormAyChn::FormCreate(TObject *Sender)
{
	import_chan=-1;
}
//---------------------------------------------------------------------------

void __fastcall TFormAyChn::RadioButton1Click(TObject *Sender)
{
	import_chan=-1;	
}
//---------------------------------------------------------------------------
void __fastcall TFormAyChn::RadioButton2Click(TObject *Sender)
{
	import_chan=0;	
}
//---------------------------------------------------------------------------
void __fastcall TFormAyChn::RadioButton3Click(TObject *Sender)
{
	import_chan=1;	
}
//---------------------------------------------------------------------------
void __fastcall TFormAyChn::RadioButton4Click(TObject *Sender)
{
	import_chan=2;	
}
//---------------------------------------------------------------------------
void __fastcall TFormAyChn::Button2Click(TObject *Sender)
{
	import_ok=false;
	Close();
}
//---------------------------------------------------------------------------

