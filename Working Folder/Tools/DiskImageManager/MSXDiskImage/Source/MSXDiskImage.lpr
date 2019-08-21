program MSXDiskImage;

{$MODE Delphi}

uses
  Forms, Interfaces,
  FormMain in 'FormMain.pas' {MainForm},
  MSXDisk in '..\..\MSXEngine\MSXDisk.pas',
  XSStream in '..\..\MSXEngine\XSStream.pas',
  FormNewDisk in 'FormNewDisk.pas' {NewDiskForm};

{$R *.res}

begin
  Application.Initialize;
  Application.Title := 'MSX Disk Image';
  Application.CreateForm(TMainForm, MainForm);
  Application.Run;
end.
