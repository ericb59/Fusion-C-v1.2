//---------------------------------------------------------------------------

#ifndef UnitVT2InstrumentH
#define UnitVT2InstrumentH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TFormVortexExp : public TForm
{
__published:	// IDE-managed Components
	TButton *ButtonOK;
	TButton *ButtonCancel;
	TGroupBox *GroupBoxBaseNote;
	TEdit *EditNote;
	TButton *ButtonDownSemi;
	TButton *ButtonDownOctave;
	TButton *ButtonUpSemi;
	TButton *ButtonUpOctave;
	void __fastcall ButtonCancelClick(TObject *Sender);
	void __fastcall ButtonOKClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall ButtonDownOctaveClick(TObject *Sender);
	void __fastcall ButtonDownSemiClick(TObject *Sender);
	void __fastcall ButtonUpSemiClick(TObject *Sender);
	void __fastcall ButtonUpOctaveClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TFormVortexExp(TComponent* Owner);

	void __fastcall Update(void);

	bool Confirm;
	int BaseNote;
};
//---------------------------------------------------------------------------
extern PACKAGE TFormVortexExp *FormVortexExp;
//---------------------------------------------------------------------------
#endif
