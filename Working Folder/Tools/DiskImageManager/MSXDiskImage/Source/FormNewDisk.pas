unit FormNewDisk;

{$MODE Delphi}

interface

uses
  LCLIntf, LCLType, LMessages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls;

type

  { TNewDiskForm }

  TNewDiskForm = class(TForm)
    TitleLabel: TLabel;
    NameEdit: TEdit;
    CreateButton: TButton;
    AbortButton: TButton;
    procedure CreateButtonClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormShow(Sender: TObject);
  private
    function GetFileName: string;
    { Private declarations }
  public
    IsDirectory: Boolean;
    property FileName: string read GetFileName;
    { Public declarations }
  end;

var
  NewDiskForm: TNewDiskForm;

implementation

{$R *.lfm}

procedure TNewDiskForm.CreateButtonClick(Sender: TObject);
begin
  if Trim(NameEdit.Text) = '' then
    if IsDirectory then
      ShowMessage('Please enter directory name')
    else ShowMessage('Please enter DSK name')
  else ModalResult := mrOK;
end;

procedure TNewDiskForm.FormCreate(Sender: TObject);
begin
  IsDirectory := False;
end;

procedure TNewDiskForm.FormShow(Sender: TObject);
begin
  if IsDirectory then
  begin
    Caption := 'Create directory';
    TitleLabel.Caption := 'Enter new directory name';
  end;
end;

function TNewDiskForm.GetFileName: string;
begin
  if IsDirectory then
    Result := Trim(NameEdit.Text)
  else Result := Trim(NameEdit.Text) + '.dsk';
end;

end.
