//---------------------------------------------------------------------------

#ifndef UnitPianoH
#define UnitPianoH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <ComCtrls.hpp>
#include <Mask.hpp>
#include <Buttons.hpp>
#include <ImgList.hpp>
//---------------------------------------------------------------------------
class TFormPiano : public TForm
{
__published:	// IDE-managed Components
	TUpDown *UpDown1;
	TStaticText *StaticText1;
	TUpDown *UpDown2;
	TStaticText *StaticText2;
	TUpDown *UpDown3;
	TStaticText *StaticText3;
	TSpeedButton *SpeedButton1;
	TSpeedButton *SpeedButton2;
	TSpeedButton *SpeedButton3;
	TStaticText *StaticText4;
	TUpDown *UpDown4;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *Label11;
	TLabel *Label12;
	TImageList *ImageList1;
	TStaticText *StaticText5;
	TUpDown *UpDown5;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall UpDown1ChangingEx(TObject *Sender, bool &AllowChange,
          short NewValue, TUpDownDirection Direction);
	void __fastcall UpDown2ChangingEx(TObject *Sender, bool &AllowChange,
          short NewValue, TUpDownDirection Direction);
	void __fastcall UpDown3ChangingEx(TObject *Sender, bool &AllowChange,
          short NewValue, TUpDownDirection Direction);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall SpeedButton2Click(TObject *Sender);
	void __fastcall FormMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall FormPaint(TObject *Sender);
	void __fastcall Label1Click(TObject *Sender);
	void __fastcall Label2Click(TObject *Sender);
	void __fastcall Label8Click(TObject *Sender);
	void __fastcall Label9Click(TObject *Sender);
	void __fastcall Label3Click(TObject *Sender);
	void __fastcall Label4Click(TObject *Sender);
	void __fastcall Label10Click(TObject *Sender);
	void __fastcall Label5Click(TObject *Sender);
	void __fastcall Label11Click(TObject *Sender);
	void __fastcall Label6Click(TObject *Sender);
	void __fastcall Label12Click(TObject *Sender);
	void __fastcall Label7Click(TObject *Sender);
	void __fastcall UpDown5Click(TObject *Sender, TUDBtnType Button);
private:	// User declarations
public:		// User declarations
	__fastcall TFormPiano(TComponent* Owner);
        __fastcall void UpdateStatic(void);
        __fastcall void HighlightKey(int);
        __fastcall void EnterNote(int);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormPiano *FormPiano;
//---------------------------------------------------------------------------
#endif
