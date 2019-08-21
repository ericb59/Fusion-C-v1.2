//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("UnitMain.cpp", FormMain);
USEFORM("UnitAYChannelSelect.cpp", FormAyChn);
USEFORM("UnitPiano.cpp", FormPiano);
USEFORM("UnitSNChannelSelect.cpp", FormSnChn);
USEFORM("UnitVT2Instrument.cpp", FormVortexExp);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		Application->Title = "AY Sound FX Editor";
		Application->CreateForm(__classid(TFormMain), &FormMain);
		Application->CreateForm(__classid(TFormAyChn), &FormAyChn);
		Application->CreateForm(__classid(TFormPiano), &FormPiano);
		Application->CreateForm(__classid(TFormSnChn), &FormSnChn);
		Application->CreateForm(__classid(TFormVortexExp), &FormVortexExp);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
