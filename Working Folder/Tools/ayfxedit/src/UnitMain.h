//---------------------------------------------------------------------------

#ifndef UnitMainH
#define UnitMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <Buttons.hpp>
#include <Dialogs.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published:	// IDE-managed Components
	TMainMenu *MainMenu;
	TMenuItem *File1;
        TMenuItem *MFileExit;
        TMenuItem *MFileEffectClear;
	TMenuItem *N1;
        TOpenDialog *OpenDialogEffect;
        TSaveDialog *SaveDialogEffect;
        TMenuItem *MFileEffectLoad;
        TMenuItem *MFileEffectSave;
        TOpenDialog *OpenDialogImport;
        TMenuItem *MImportWAV;
	TTimer *Timer1;
	TMenuItem *Import1;
        TMenuItem *MImportPSG;
        TMenuItem *MFileLoadBank;
        TMenuItem *MFileSaveBank;
	TMenuItem *N2;
	TMenuItem *Edit1;
        TMenuItem *MEditSelectAll;
        TMenuItem *MEditInverseSelection;
        TMenuItem *MEditUnselectAll;
        TMenuItem *MEditDelete;
	TMenuItem *N4;
	TMenuItem *View1;
        TMenuItem *MViewLinearPeriod;
        TMenuItem *MViewLogPeriod;
        TMenuItem *MEditCut;
        TMenuItem *MEditCopy;
        TMenuItem *MEditPaste;
        TMenuItem *MFileNewBank;
        TMenuItem *MImportVTX;
	TPanel *Panel1;
        TSpeedButton *SpeedButtonPlay;
        TSpeedButton *SpeedButtonStop;
        TSpeedButton *SpeedButtonPrev;
        TSpeedButton *SpeedButtonNext;
        TEdit *EditEffectNum;
        TEdit *EditEffectName;
        TMenuItem *MFileMultiLoad;
        TOpenDialog *OpenDialogMulti;
        TSaveDialog *SaveDialogBank;
        TOpenDialog *OpenDialogBank;
	TMenuItem *Bank1;
        TMenuItem *MBankInsert;
        TMenuItem *MBankDelete;
        TMenuItem *MFileSaveBankNnm;
        TSpeedButton *SpeedButtonAdd;
        TSpeedButton *SpeedButtonDel;
        TMenuItem *MBankAdd;
	TMenuItem *N3;
        TMenuItem *MFileMultiSave;
        TSaveDialog *SaveDialogMulti;
        TSpeedButton *SpeedButtonFirst;
        TSpeedButton *SpeedButtonLast;
        TSpeedButton *SpeedButtonPiano;
	TMenuItem *N5;
        TMenuItem *MViewPianoInput;
        TMenuItem *MImportVGM;
	TMenuItem *Export1;
        TMenuItem *MExportVT2;
        TMenuItem *MExportWav;
        TSaveDialog *SaveDialogExport;
	TMenuItem *MExportCSV;
	TMenuItem *N6;
	TMenuItem *MExportCurrent;
	TMenuItem *MExportAll;
	void __fastcall MFileExitClick(TObject *Sender);
	void __fastcall FormPaint(TObject *Sender);
	void __fastcall FormMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall MFileEffectClearClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall SpeedButtonPlayClick(TObject *Sender);
	void __fastcall SpeedButtonStopClick(TObject *Sender);
	void __fastcall MFileEffectLoadClick(TObject *Sender);
	void __fastcall MFileEffectSaveClick(TObject *Sender);
	void __fastcall FormMouseWheelDown(TObject *Sender, TShiftState Shift,
          TPoint &MousePos, bool &Handled);
	void __fastcall FormMouseWheelUp(TObject *Sender, TShiftState Shift,
          TPoint &MousePos, bool &Handled);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall FormCanResize(TObject *Sender, int &NewWidth,
          int &NewHeight, bool &Resize);
	void __fastcall MImportWAVClick(TObject *Sender);
	void __fastcall FormMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall MImportPSGClick(TObject *Sender);
	void __fastcall MEditUnselectAllClick(TObject *Sender);
	void __fastcall MEditSelectAllClick(TObject *Sender);
	void __fastcall MEditInverseSelectionClick(TObject *Sender);
	void __fastcall MEditDeleteClick(TObject *Sender);
	void __fastcall MViewLinearPeriodClick(TObject *Sender);
	void __fastcall MViewLogPeriodClick(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall FormDeactivate(TObject *Sender);
	void __fastcall MEditCopyClick(TObject *Sender);
	void __fastcall MEditCutClick(TObject *Sender);
	void __fastcall MEditPasteClick(TObject *Sender);
	void __fastcall MImportVTXClick(TObject *Sender);
	void __fastcall MFileNewBankClick(TObject *Sender);
	void __fastcall SpeedButtonPrevClick(TObject *Sender);
	void __fastcall SpeedButtonNextClick(TObject *Sender);
	void __fastcall MFileMultiLoadClick(TObject *Sender);
	void __fastcall MFileSaveBankClick(TObject *Sender);
	void __fastcall MFileLoadBankClick(TObject *Sender);
	void __fastcall MFileSaveBankNnmClick(TObject *Sender);
	void __fastcall EditEffectNameKeyPress(TObject *Sender, char &Key);
	void __fastcall EditEffectNameClick(TObject *Sender);
	void __fastcall EditEffectNameChange(TObject *Sender);
	void __fastcall MBankAddClick(TObject *Sender);
	void __fastcall SpeedButtonAddClick(TObject *Sender);
	void __fastcall SpeedButtonDelClick(TObject *Sender);
	void __fastcall MBankDeleteClick(TObject *Sender);
	void __fastcall MBankInsertClick(TObject *Sender);
	void __fastcall MFileMultiSaveClick(TObject *Sender);
	void __fastcall SpeedButtonFirstClick(TObject *Sender);
	void __fastcall SpeedButtonLastClick(TObject *Sender);
	void __fastcall MViewPianoInputClick(TObject *Sender);
	void __fastcall SpeedButtonPianoClick(TObject *Sender);
	void __fastcall MImportVGMClick(TObject *Sender);
	void __fastcall MExportVT2Click(TObject *Sender);
        void __fastcall MExportWavClick(TObject *Sender);
	void __fastcall MExportCSVClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TFormMain(TComponent* Owner);
	__fastcall void DrawEdit(bool);
	__fastcall void TitleUpdate(void);
	__fastcall void HoldCheck(int,int,bool);
	__fastcall void BankUpdateCount(void);
	__fastcall void EffectSwitchEnable(void);
	__fastcall void EnterFromPiano(int,bool,int,int,int);
	__fastcall AnsiString GetEffectFileName(int);
	__fastcall bool MultiSaveExport(int);
	__fastcall void ColumnAdjust(int);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif
 