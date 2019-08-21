//---------------------------------------------------------------------------

#ifndef UnitSNChannelSelectH
#define UnitSNChannelSelectH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TFormSnChn : public TForm
{
__published:	// IDE-managed Components
	TRadioButton *RadioButton1;
	TRadioButton *RadioButton2;
	TRadioButton *RadioButton3;
	TRadioButton *RadioButton4;
	TRadioButton *RadioButton5;
	TCheckBox *CheckBox1;
	TButton *Button1;
	TButton *Button2;
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall RadioButton1Click(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TFormSnChn(TComponent* Owner);
        void __fastcall UpdateVGMSet(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormSnChn *FormSnChn;
//---------------------------------------------------------------------------
#endif
