//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnitSNChannelSelect.h"

extern int vgm_chan;
extern bool vgm_noise;
extern bool import_ok;

void __fastcall TFormSnChn::UpdateVGMSet(void)
{
	vgm_noise=CheckBox1->Checked;
	if(RadioButton1->Checked) vgm_chan=-1;
	if(RadioButton2->Checked) vgm_chan=0;
	if(RadioButton3->Checked) vgm_chan=1;
	if(RadioButton4->Checked) vgm_chan=2;
	if(RadioButton5->Checked) vgm_chan=3;
	if(vgm_chan==3)
	{
		CheckBox1->Enabled=false;
	}
	else
	{
		CheckBox1->Enabled=true;
	}
}



//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormSnChn *FormSnChn;
//---------------------------------------------------------------------------
__fastcall TFormSnChn::TFormSnChn(TComponent* Owner)
: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TFormSnChn::Button1Click(TObject *Sender)
{
	import_ok=true;
	Close();	
}
//---------------------------------------------------------------------------

void __fastcall TFormSnChn::Button2Click(TObject *Sender)
{
	import_ok=false;
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TFormSnChn::RadioButton1Click(TObject *Sender)
{
	UpdateVGMSet();
}
//---------------------------------------------------------------------------
void __fastcall TFormSnChn::FormCreate(TObject *Sender)
{
	UpdateVGMSet();
}
//---------------------------------------------------------------------------

