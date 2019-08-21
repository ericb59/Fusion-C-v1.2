//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnitMain.h"
#include "UnitPiano.h"

int octave;
int step;
int fill;
bool shift_down;
bool ctrl_down;

//таблички:
//PT3NoteTable_ST
//PT3NoteTable_PT_34_35
//PT3NoteTable_ASM_34_35
//PT3NoteTable_REAL_34_35
//SQT_Table

const AnsiString TableNames[5]={"Soundtracker","Protracker","ASM or PSC","Real","SQ Tracker"};

const int FreqTables[12*8*5]={
	0x0EF8,0x0E10,0x0D60,0x0C80,0x0BD8,0x0B28,0x0A88,0x09F0,0x0960,0x08E0,0x0858,0x07E0,
		0x077C,0x0708,0x06B0,0x0640,0x05EC,0x0594,0x0544,0x04F8,0x04B0,0x0470,0x042C,0x03FD,
		0x03BE,0x0384,0x0358,0x0320,0x02F6,0x02CA,0x02A2,0x027C,0x0258,0x0238,0x0216,0x01F8,
		0x01DF,0x01C2,0x01AC,0x0190,0x017B,0x0165,0x0151,0x013E,0x012C,0x011C,0x010A,0x00FC,
		0x00EF,0x00E1,0x00D6,0x00C8,0x00BD,0x00B2,0x00A8,0x009F,0x0096,0x008E,0x0085,0x007E,
		0x0077,0x0070,0x006B,0x0064,0x005E,0x0059,0x0054,0x004F,0x004B,0x0047,0x0042,0x003F,
		0x003B,0x0038,0x0035,0x0032,0x002F,0x002C,0x002A,0x0027,0x0025,0x0023,0x0021,0x001F,
		0x001D,0x001C,0x001A,0x0019,0x0017,0x0016,0x0015,0x0013,0x0012,0x0011,0x0010,0x000F,
		
		0x0C22,0x0B73,0x0ACF,0x0A33,0x09A1,0x0917,0x0894,0x0819,0x07A4,0x0737,0x06CF,0x066D,
		0x0611,0x05BA,0x0567,0x051A,0x04D0,0x048B,0x044A,0x040C,0x03D2,0x039B,0x0367,0x0337,
		0x0308,0x02DD,0x02B4,0x028D,0x0268,0x0246,0x0225,0x0206,0x01E9,0x01CE,0x01B4,0x019B,
		0x0184,0x016E,0x015A,0x0146,0x0134,0x0123,0x0112,0x0103,0x00F5,0x00E7,0x00DA,0x00CE,
		0x00C2,0x00B7,0x00AD,0x00A3,0x009A,0x0091,0x0089,0x0082,0x007A,0x0073,0x006D,0x0067,
		0x0061,0x005C,0x0056,0x0052,0x004D,0x0049,0x0045,0x0041,0x003D,0x003A,0x0036,0x0033,
		0x0031,0x002E,0x002B,0x0029,0x0027,0x0024,0x0022,0x0020,0x001F,0x001D,0x001B,0x001A,
		0x0018,0x0017,0x0016,0x0014,0x0013,0x0012,0x0011,0x0010,0x000F,0x000E,0x000D,0x000C,
		
		0x0D10,0x0C55,0x0BA4,0x0AFC,0x0A5F,0x09CA,0x093D,0x08B8,0x083B,0x07C5,0x0755,0x06EC,
		0x0688,0x062A,0x05D2,0x057E,0x052F,0x04E5,0x049E,0x045C,0x041D,0x03E2,0x03AB,0x0376,
		0x0344,0x0315,0x02E9,0x02BF,0x0298,0x0272,0x024F,0x022E,0x020F,0x01F1,0x01D5,0x01BB,
		0x01A2,0x018B,0x0174,0x0160,0x014C,0x0139,0x0128,0x0117,0x0107,0x00F9,0x00EB,0x00DD,
		0x00D1,0x00C5,0x00BA,0x00B0,0x00A6,0x009D,0x0094,0x008C,0x0084,0x007C,0x0075,0x006F,
		0x0069,0x0063,0x005D,0x0058,0x0053,0x004E,0x004A,0x0046,0x0042,0x003E,0x003B,0x0037,
		0x0034,0x0031,0x002F,0x002C,0x0029,0x0027,0x0025,0x0023,0x0021,0x001F,0x001D,0x001C,
		0x001A,0x0019,0x0017,0x0016,0x0015,0x0014,0x0012,0x0011,0x0010,0x000F,0x000E,0x000D,
		
		0x0CDA,0x0C22,0x0B73,0x0ACF,0x0A33,0x09A1,0x0917,0x0894,0x0819,0x07A4,0x0737,0x06CF,
		0x066D,0x0611,0x05BA,0x0567,0x051A,0x04D0,0x048B,0x044A,0x040C,0x03D2,0x039B,0x0367,
		0x0337,0x0308,0x02DD,0x02B4,0x028D,0x0268,0x0246,0x0225,0x0206,0x01E9,0x01CE,0x01B4,
		0x019B,0x0184,0x016E,0x015A,0x0146,0x0134,0x0123,0x0112,0x0103,0x00F5,0x00E7,0x00DA,
		0x00CE,0x00C2,0x00B7,0x00AD,0x00A3,0x009A,0x0091,0x0089,0x0082,0x007A,0x0073,0x006D,
		0x0067,0x0061,0x005C,0x0056,0x0052,0x004D,0x0049,0x0045,0x0041,0x003D,0x003A,0x0036,
		0x0033,0x0031,0x002E,0x002B,0x0029,0x0027,0x0024,0x0022,0x0020,0x001F,0x001D,0x001B,
		0x001A,0x0018,0x0017,0x0016,0x0014,0x0013,0x0012,0x0011,0x0010,0x000F,0x000E,0x000D,
		
		0x0d5d,0x0c9c,0x0be7,0x0b3c,0x0a9b,0x0a02,0x0973,0x08eb,0x086b,0x07f2,0x0780,0x0714,
		0x06ae,0x064e,0x05f4,0x059e,0x054f,0x0501,0x04b9,0x0475,0x0435,0x03f9,0x03c0,0x038a,
		0x0357,0x0327,0x02fa,0x02cf,0x02a7,0x0281,0x025d,0x023b,0x021b,0x01fc,0x01e0,0x01c5,
		0x01ac,0x0194,0x017d,0x0168,0x0153,0x0140,0x012e,0x011d,0x010d,0x00fe,0x00f0,0x00e2,
		0x00d6,0x00ca,0x00be,0x00b4,0x00aa,0x00a0,0x0097,0x008f,0x0087,0x007f,0x0078,0x0071,
		0x006b,0x0065,0x005f,0x005a,0x0055,0x0050,0x004c,0x0047,0x0043,0x0040,0x003c,0x0039,
		0x0035,0x0032,0x0030,0x002d,0x002a,0x0028,0x0026,0x0024,0x0022,0x0020,0x001e,0x001c,
		0x001b,0x0019,0x0018,0x0016,0x0015,0x0014,0x0013,0x0012,0x0011,0x0010,0x000f,0x000e
};



void __fastcall TFormPiano::UpdateStatic(void)
{
	StaticText1->Caption="Octave "+IntToStr(octave);
	StaticText2->Caption="Step "+IntToStr(step);
	StaticText3->Caption="Fill "+IntToStr(fill);
}


void __fastcall TFormPiano::HighlightKey(int keyn)
{
	const TColor hlw=clTeal;
	const TColor hlb=clTeal;
	Label1->Font->Color=((keyn==0)?hlw:clBlack);
	Label8->Font->Color=((keyn==1)?hlb:clWhite);
	Label2->Font->Color=((keyn==2)?hlw:clBlack);
	Label9->Font->Color=((keyn==3)?hlb:clWhite);
	Label3->Font->Color=((keyn==4)?hlw:clBlack);
	Label4->Font->Color=((keyn==5)?hlw:clBlack);
	Label10->Font->Color=((keyn==6)?hlb:clWhite);
	Label5->Font->Color=((keyn==7)?hlw:clBlack);
	Label11->Font->Color=((keyn==8)?hlb:clWhite);
	Label6->Font->Color=((keyn==9)?hlw:clBlack);
	Label12->Font->Color=((keyn==10)?hlb:clWhite);
	Label7->Font->Color=((keyn==11)?hlw:clBlack);
}



void __fastcall TFormPiano::EnterNote(int note)
{
	int vol,coct;
	
	coct=octave;
	if(shift_down) coct=octave+1;
	if(ctrl_down) coct=octave-1;
	
	coct-=1;
	if(coct<0) coct=0;
	if(coct>7) coct=7;
	
	if(note>=0)
	{
        if(SpeedButton3->Down)
        {
			vol=UpDown4->Position;
        }
        else
        {
			vol=-1;
		}

		FormMain->EnterFromPiano(FreqTables[coct*12+note+UpDown5->Position*96],SpeedButton1->Down,vol,step,fill);
	}
	
	HighlightKey(note);
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormPiano *FormPiano;
//---------------------------------------------------------------------------
__fastcall TFormPiano::TFormPiano(TComponent* Owner)
: TForm(Owner)
{
}
//---------------------------------------------------------------------------


void __fastcall TFormPiano::FormCreate(TObject *Sender)
{
	octave=4;
	step=1;
	fill=1;
	shift_down=false;
	ctrl_down=false;
	StaticText5->Caption=TableNames[UpDown5->Position];
}
//---------------------------------------------------------------------------
void __fastcall TFormPiano::FormShow(TObject *Sender)
{
	UpdateStatic();
}
//---------------------------------------------------------------------------
void __fastcall TFormPiano::UpDown1ChangingEx(TObject *Sender,
											  bool &AllowChange, short NewValue, TUpDownDirection Direction)
{
	AllowChange=false;
	if(NewValue>1)
	{
		if(octave<8) octave++;
	}
	if(NewValue<1)
	{
		if(octave>1) octave--;
	}
	UpdateStatic();
}
//---------------------------------------------------------------------------
void __fastcall TFormPiano::UpDown2ChangingEx(TObject *Sender,
											  bool &AllowChange, short NewValue, TUpDownDirection Direction)
{
	AllowChange=false;
	if(NewValue>1)
	{
		if(step<256) step++;
	}
	if(NewValue<1)
	{
		if(step>0) step--;
	}
	if(SpeedButton2->Down)
	{
        fill=step;
        if(fill<1) fill=1;
	}
	UpdateStatic();
}
//---------------------------------------------------------------------------
void __fastcall TFormPiano::UpDown3ChangingEx(TObject *Sender,
											  bool &AllowChange, short NewValue, TUpDownDirection Direction)
{
	AllowChange=false;
	if(NewValue>1)
	{
		if(fill<256) fill++;
	}
	if(NewValue<1)
	{
		if(fill>1) fill--;
	}
	UpdateStatic();
}
//---------------------------------------------------------------------------

void __fastcall TFormPiano::FormKeyDown(TObject *Sender, WORD &Key,
										TShiftState Shift)
{
	int note;

	if(Key==VK_OEM_3)//tilde
	{
		FormMain->MViewPianoInputClick(Sender);
		return;
	}

	if(Key>=VK_NUMPAD1&&Key<=VK_NUMPAD8)
	{
		octave=Key-VK_NUMPAD1+1;
		UpdateStatic();
	}
	
	if(Key==VK_SHIFT) shift_down=true;
	if(Key==VK_CONTROL) ctrl_down=true;
	
	note=-1;
	switch(Key)
	{
	case 'Z': note=0; break;
	case 'S': note=1; break;
	case 'X': note=2; break;
	case 'D': note=3; break;
	case 'C': note=4; break;
	case 'V': note=5; break;
	case 'G': note=6; break;
	case 'B': note=7; break;
	case 'H': note=8; break;
	case 'N': note=9; break;
	case 'J': note=10; break;
	case 'M': note=11; break;
	}
	EnterNote(note);
}
//---------------------------------------------------------------------------

void __fastcall TFormPiano::SpeedButton2Click(TObject *Sender)
{
	UpDown3->Enabled=!SpeedButton2->Down;
	if(!UpDown3->Enabled)
	{
		fill=step;
		UpdateStatic();
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormPiano::FormMouseUp(TObject *Sender, TMouseButton Button,
										TShiftState Shift, int X, int Y)
{
	HighlightKey(-1);	
}
//---------------------------------------------------------------------------

void __fastcall TFormPiano::FormKeyUp(TObject *Sender, WORD &Key,
									  TShiftState Shift)
{
	if(Key==VK_SHIFT) shift_down=false;
	if(Key==VK_CONTROL) ctrl_down=false;
	HighlightKey(-1);
}
//---------------------------------------------------------------------------

void __fastcall TFormPiano::FormPaint(TObject *Sender)
{
	ImageList1->Draw(Canvas,8,8,0);
}
//---------------------------------------------------------------------------

void __fastcall TFormPiano::Label1Click(TObject *Sender)
{
	EnterNote(0);
}
//---------------------------------------------------------------------------

void __fastcall TFormPiano::Label2Click(TObject *Sender)
{
	EnterNote(2);
}
//---------------------------------------------------------------------------

void __fastcall TFormPiano::Label8Click(TObject *Sender)
{
	EnterNote(1);
}
//---------------------------------------------------------------------------

void __fastcall TFormPiano::Label9Click(TObject *Sender)
{
	EnterNote(3);	
}
//---------------------------------------------------------------------------

void __fastcall TFormPiano::Label3Click(TObject *Sender)
{
	EnterNote(4);	
}
//---------------------------------------------------------------------------

void __fastcall TFormPiano::Label4Click(TObject *Sender)
{
	EnterNote(5);	
}
//---------------------------------------------------------------------------

void __fastcall TFormPiano::Label10Click(TObject *Sender)
{
	EnterNote(6);	
}
//---------------------------------------------------------------------------

void __fastcall TFormPiano::Label5Click(TObject *Sender)
{
	EnterNote(7);	
}
//---------------------------------------------------------------------------

void __fastcall TFormPiano::Label11Click(TObject *Sender)
{
	EnterNote(8);	
}
//---------------------------------------------------------------------------

void __fastcall TFormPiano::Label6Click(TObject *Sender)
{
	EnterNote(9);	
}
//---------------------------------------------------------------------------

void __fastcall TFormPiano::Label12Click(TObject *Sender)
{
	EnterNote(10);	
}
//---------------------------------------------------------------------------

void __fastcall TFormPiano::Label7Click(TObject *Sender)
{
	EnterNote(11);	
}
//---------------------------------------------------------------------------
void __fastcall TFormPiano::UpDown5Click(TObject *Sender, TUDBtnType Button)
{
	StaticText5->Caption=TableNames[UpDown5->Position];
}
//---------------------------------------------------------------------------

