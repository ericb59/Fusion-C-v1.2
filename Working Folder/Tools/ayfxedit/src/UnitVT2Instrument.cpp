//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnitVT2Instrument.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormVortexExp *FormVortexExp;


void __fastcall TFormVortexExp::Update(void)
{
	AnsiString names[12]={"C-","C#","D-","D#","E-","F-","F#","G-","G#","A-","A#","B-"};

	if(BaseNote<1*12) BaseNote=1*12;
	if(BaseNote>8*12) BaseNote=8*12;

	EditNote->Text=names[BaseNote%12]+IntToStr(BaseNote/12);
}

//---------------------------------------------------------------------------
__fastcall TFormVortexExp::TFormVortexExp(TComponent* Owner)
: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TFormVortexExp::ButtonCancelClick(TObject *Sender)
{
	Confirm=false;
	Close();	
}
//---------------------------------------------------------------------------

void __fastcall TFormVortexExp::ButtonOKClick(TObject *Sender)
{
	Confirm=true;
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TFormVortexExp::FormCreate(TObject *Sender)
{
	Confirm=false;
	BaseNote=5*12;

	Update();
}
//---------------------------------------------------------------------------

void __fastcall TFormVortexExp::ButtonDownSemiClick(TObject *Sender)
{
	--BaseNote;

	Update();
}
//---------------------------------------------------------------------------

void __fastcall TFormVortexExp::ButtonUpSemiClick(TObject *Sender)
{
	++BaseNote;

	Update();
}
//---------------------------------------------------------------------------

void __fastcall TFormVortexExp::ButtonDownOctaveClick(TObject *Sender)
{
	BaseNote-=12;

	Update();
}
//---------------------------------------------------------------------------


void __fastcall TFormVortexExp::ButtonUpOctaveClick(TObject *Sender)
{
	BaseNote+=12;

	Update();
}
//---------------------------------------------------------------------------

