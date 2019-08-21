unit FormMain;

{$MODE Delphi}

interface

uses
  MSXDisk,
  LCLIntf, LCLType, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, FileCtrl, Grids, ComCtrls, EditBtn, ShellCtrls, FileUtil;

const
  GridColText: array[0..4] of string = (
    'Name', 'Ext', 'Size', 'Date', 'Attr'
  );

type

  { TMainForm }

  TMainForm = class(TForm)
    MakeDirButton: TButton;
    MainFileBox: TFileListBox;
    ImageList1: TImageList;
    MainGrid: TStringGrid;
    OpenDialog1: TOpenDialog;
    AddButton: TButton;
    ConvertButton: TButton;
    DeleteButton: TButton;
    MainStatus: TStatusBar;
    CreateDiskButton: TButton;
    MainDirTree: TShellTreeView;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure MainDirTreeChange(Sender: TObject; Node: TTreeNode);
    procedure MainDirTreeCustomDrawItem(Sender: TCustomTreeView;
      Node: TTreeNode; State: TCustomDrawState; var DefaultDraw: Boolean);
    procedure MainFileBoxClick(Sender: TObject);
    procedure MainGridDblClick(Sender: TObject);
    procedure MainGridHeaderClick(Sender: TObject; IsColumn: Boolean;
      Index: Integer);
    procedure MainGridKeyDown(Sender: TObject; var Key: Word;
      Shift: TShiftState);
    procedure AddButtonClick(Sender: TObject);
    procedure ConvertButtonClick(Sender: TObject);
    procedure DeleteButtonClick(Sender: TObject);
    procedure CreateDiskButtonClick(Sender: TObject);
    procedure MakeDirButtonClick(Sender: TObject);
  private
    DiskImage: TMSXDisk;
    FSortColumn: Integer;
    FSortType: TMSXDiskSortType;
    FDescOrdered: Boolean;
    function GetFullName(ANode: TTreeNode): string;
    function GetSelectFileName(ARow: Integer = -1): string;
    procedure UpdateGrid;
    procedure UpdateColName;
    { Private declarations }
  public
    { Public declarations }
  end;

var
  MainForm: TMainForm;

implementation

{$R *.lfm}

uses
  FormNewDisk;

procedure TMainForm.AddButtonClick(Sender: TObject);
var
  err: TMSXDiskResult;
  sl: TStringList;
begin
  if OpenDialog1.Execute then
  begin
    sl := TStringList.Create;
    try
      sl.AddStrings(OpenDialog1.Files);
      sl.Sort;
      DiskImage.Add(sl);
    finally
      sl.Free;
    end;
    err := DiskImage.LastError;
    DiskImage.SaveToFile;
    MainFileBox.Click;
    case err of
      mdrDiskFull: MessageDlg('Disk Image Full', mtError, [mbOk], 0);
      mdrDirectoryFull: MessageDlg('Disk Image Directory Full', mtError, [mbOk], 0);
    end;
  end;
end;

procedure TMainForm.ConvertButtonClick(Sender: TObject);
var
  fname, ext: string;
begin
  ext := UpperCase(ExtractFileExt(DiskImage.FileName));
  if ext = '.XSA' then
  begin
    fname := ChangeFileExt(DiskImage.FileName, '.DSK');
    if FileExistsUTF8(fname) then
    if MessageDlg(DiskImage.FileName, 'Overwrite file exists?', mtConfirmation, [mbYes, mbNo], 0, mbNo) = mrNo then
      Exit;
    DiskImage.LoadFromFile;
    DiskImage.SaveToFile(fname);
    DiskImage.FileName := '';
    MainFileBox.UpdateFileList;
    MainFileBox.ItemIndex := MainFileBox.Items.IndexOf(ExtractFileName(fname));
    MainFileBox.Click;
  end
  else if ext = '.DSK' then
  begin
    fname := ChangeFileExt(DiskImage.FileName, '.XSA');
    if FileExistsUTF8(fname) then
    if MessageDlg(DiskImage.FileName, 'Overwrite file exists?', mtConfirmation, [mbYes, mbNo], 0, mbNo) = mrNo then
      Exit;
    DiskImage.LoadFromFile;
    DiskImage.SaveToFile(fname);
    DiskImage.FileName := '';
    MainFileBox.UpdateFileList;
    MainFileBox.ItemIndex := MainFileBox.Items.IndexOf(ExtractFileName(fname));
    MainFileBox.Click;
  end;
end;

procedure TMainForm.CreateDiskButtonClick(Sender: TObject);
var
  aform: TNewDiskForm;
  dsk: TMSXDisk;
  s: string;
begin
  aform := TNewDiskForm.Create(nil);
  try
    aform.Left := Left + (Width - aform.Width) div 2;
    aform.Top := Top + (Height - aform.Height) div 2;
    if aform.ShowModal = mrOK then
    begin
      s := IncludeTrailingPathDelimiter(MainFileBox.Directory) + aform.FileName;
      if FileExistsUTF8(s) then
      begin
        if MessageDlg(s, 'Are you sure to overwrite existed DSK file', mtConfirmation, [mbYes, mbNo], 0, mbNo) <> mrYes then s := '';
      end;
      if s <> '' then
      begin
        dsk := TMSXDisk.Create;
        dsk.SaveToFile(s);
        DiskImage.FileName := '';
        MainFileBox.UpdateFileList;
        MainFileBox.ItemIndex := MainFileBox.Items.IndexOf(ExtractFileName(s));
        MainFileBox.Click;
      end;
    end;
  finally
    aform.Free;
  end;
end;

procedure TMainForm.MakeDirButtonClick(Sender: TObject);
var
  aform: TNewDiskForm;
  s: string;
begin
  if DiskImage.FileName <> '' then
  begin
    aform := TNewDiskForm.Create(nil);
    try
      aform.Left := Left + (Width - aform.Width) div 2;
      aform.Top := Top + (Height - aform.Height) div 2;
      aform.IsDirectory := True;
      if aform.ShowModal = mrOK then
      begin
        s := aform.FileName;
        if DiskImage.FileNameExists(s) then
          ShowMessage('Directory exists')
        else
        begin
          case DiskImage.CreateSubDir(s) of
            mdrDirectoryExists: ShowMessage('Directory exists');
            mdrDirectoryFull: ShowMessage('Directory full');
            mdrDiskFull: ShowMessage('Disk full');
          else
            DiskImage.SaveToFile;
            MainFileBox.UpdateFileList;
            MainFileBox.ItemIndex := MainFileBox.Items.IndexOf(ExtractFileName(DiskImage.FileName));
            MainFileBox.Click;
          end;
        end;
      end;
    finally
      aform.Free;
    end;
  end;
end;

function TMainForm.GetFullName(ANode: TTreeNode): string;
begin
  Result := '';
  repeat
    Result := IncludeTrailingPathDelimiter(ANode.Text) + Result;
    ANode := ANode.Parent;
  until not Assigned(ANode);
end;

procedure TMainForm.DeleteButtonClick(Sender: TObject);
var
  fname: string;
  n: Integer;
begin
  if MessageDlg('Delete file(s)', 'Are you sure to delete selected file?', mtConfirmation, [mbYes, mbNo], 0, mbNo) = mrYes then
  begin
    for n := MainGrid.Selection.Top to MainGrid.Selection.Bottom do
    begin
      fname := GetSelectFileName(n);
      if fname <> '' then
      case DiskImage.Delete(fname) of
        mdrFileNotFound: ShowMessage(fname + ' file not found');
        mdrDirectoryNotEmpty: ShowMessage(fname + ' directory not empty');
      end;
    end;
    DiskImage.SaveToFile;
    MainFileBox.UpdateFileList;
    MainFileBox.ItemIndex := MainFileBox.Items.IndexOf(ExtractFileName(DiskImage.FileName));
    MainFileBox.Click;
  end;
end;

procedure TMainForm.FormCreate(Sender: TObject);
var
  n: Integer;
begin
  FSortColumn := 0;
  FSortType := mstNone;
  MainDirTree.Path := GetCurrentDir;
  UpdateColName;
  DiskImage := TMSXDisk.Create;
end;

procedure TMainForm.FormDestroy(Sender: TObject);
begin
  DiskImage.Free;
end;

procedure TMainForm.MainDirTreeChange(Sender: TObject; Node: TTreeNode);
begin
  DiskImage.FileName := '';
  MainFileBox.Directory := MainDirTree.Path;
  MainFileBox.Click;
end;

procedure TMainForm.MainDirTreeCustomDrawItem(Sender: TCustomTreeView;
  Node: TTreeNode; State: TCustomDrawState; var DefaultDraw: Boolean);
begin
  if Node.Expanded then
    Node.ImageIndex := 3
  else Node.ImageIndex := 1;
  Node.SelectedIndex := 3;
end;

procedure TMainForm.MainFileBoxClick(Sender: TObject);
var
  olddir, ext: string;
begin
  ext := UpperCase(ExtractFileExt(MainFileBox.FileName));
  if (ext = '.DSK') or (ext = '.XSA') then
  begin
    if ext = '.DSK' then
      ConvertButton.Caption := 'Convert to XSA'
    else ConvertButton.Caption := 'Convert to DSK';
    if DiskImage.FileName <> '' then
      olddir := DiskImage.Directory
    else olddir := MSXPathDelim;
    DiskImage.LoadFromFile(MainFileBox.FileName);
    DiskImage.ChangeDir(olddir);
    MainStatus.Panels[0].Text := ExtractFileName(DiskImage.FileName);
    MainStatus.Panels[1].Text := 'File Usage : ' + IntToStr(DiskImage.FileUsage) + ' byte(s)';
    MainStatus.Panels[2].Text := IntToStr(DiskImage.FileCount) + ' file(s)';
    MainStatus.Panels[3].Text := 'Disk Free : ' + IntToStr(DiskImage.DiskFree) + ' byte(s)';
  end
  else
  begin
    DiskImage.FileName := '';
    DiskImage.Clear;
    MainStatus.Panels[0].Text := '';
    MainStatus.Panels[1].Text := '';
    MainStatus.Panels[2].Text := '';
    MainStatus.Panels[3].Text := '';
  end;
  UpdateGrid;
end;

function TMainForm.GetSelectFileName(ARow: Integer): string;
begin
  if ARow < 0 then ARow := MainGrid.Row;
  Result := TrimRight(MainGrid.Cells[1, ARow]);
  if Result <> '' then Result := '.' + Result;
  Result := TrimRight(MainGrid.Cells[0, ARow]) + Result;
  if Copy(Result, 1, 1) = '[' then Delete(Result, 1, 1);
  if Copy(Result, Length(Result), 1) = ']' then Delete(Result, Length(Result), 1);
end;

procedure TMainForm.UpdateGrid;
var
  name, ext, size: string;
  dirs: TDirEntryList;
  pdir: PDirEntry;
  n: Integer;
begin
  MainGrid.RowCount := 2;
  MainGrid.Rows[1].Text := '';
  if DiskImage.FileName <> '' then
  begin
    dirs := TDirEntryList.Create;
    try
      DiskImage.GetCurrentDirList(dirs);
      dirs.SortType := FSortType;
      dirs.DescOrdered := FDescOrdered;
      dirs.Sort;
      for n := 1 to dirs.Count do
      begin
        pdir := dirs[n - 1];
        MainGrid.RowCount := n + 1;
        if pdir.IsDirectory then
        begin
          name := '[' + TrimRight(pdir.Name);
          ext := TrimRight(pdir.Ext);
          if ext <> '' then name := name + '.' + ext;
          name := name + ']';
          ext := '';
          size := '';
        end
        else if pdir.IsLabel then
        begin
          name := '' + TrimRight(pdir.Name);
          ext := TrimRight(pdir.Ext);
          if ext <> '' then name := name + '.' + ext;
          name := name + '';
          ext := '';
          size := '(LABEL)';
        end
        else
        begin
          name := pdir.Name;
          ext := pdir.Ext;
          size := IntToStr(pdir.Size);
        end;
        MainGrid.Cells[0, n] := UpperCase(name);
        MainGrid.Cells[1, n] := UpperCase(ext);
        MainGrid.Cells[2, n] := size;
        MainGrid.Cells[3, n] := Format('%2.2d/%2.2d/%4.4d', [pdir.ModifyDate.Day, pdir.ModifyDate.Month, pdir.ModifyDate.Year]);
        MainGrid.Cells[4, n] := pdir.AttrString;
      end;
    finally
      dirs.Free;
    end;
  end;
end;

procedure TMainForm.UpdateColName;
var
  n: Integer;
  s: string;
begin
  for n := 0 to 4 do
  begin
    s := GridColText[n];
    if Ord(FSortType) = n then
    begin
      if FDescOrdered then
        s := s + ' ▼'
      else s := s + ' ▲';
    end;
    MainGrid.Cells[n, 0] := s;
  end;
end;

procedure TMainForm.MainGridDblClick(Sender: TObject);
var
  fs: TFileStream;
  oldrow: Integer;
  olddir, fname: string;
  entry: TDirEntry;
  p: TPoint;
begin
  p := MainGrid.MouseToCell(Mouse.CursorPos);
  if p.y = 0 then Exit;
  fname := GetSelectFileName;
  if fname = '' then Exit;
  entry := DiskImage.Lookup(fname);
  if entry.IsDirectory then
  begin
    DiskImage.ChangeDir(fname);
    UpdateGrid;
    MainStatus.Panels[1].Text := 'File Usage : ' + IntToStr(DiskImage.FileUsage) + ' byte(s)';
    MainStatus.Panels[2].Text := IntToStr(DiskImage.FileCount) + ' file(s)';
  end
  else if not entry.IsLabel then
  begin
    fname := MainFileBox.Directory + PathDelim + fname;
    if FileExistsUTF8(fname) then if MessageDlg(fname, 'Overwrite file exists?', mtConfirmation, [mbYes, mbNo], 0, mbNo) = mrNo then
    begin
      Exit;
    end;
    fs := TFileStream.Create(fname, fmCreate);
    try
      DiskImage.ExtractToStream(fs, fname);
    finally
      fs.Free;
    end;
    oldrow := MainGrid.Row;
    olddir := DiskImage.Directory;
    MainFileBox.UpdateFileList;
    MainFileBox.ItemIndex := MainFileBox.Items.IndexOf(ExtractFileName(DiskImage.FileName));
    MainFileBox.Click;
    DiskImage.ChangeDir(olddir);
    UpdateGrid;
    MainGrid.Row := oldrow;
  end;
end;

procedure TMainForm.MainGridHeaderClick(Sender: TObject; IsColumn: Boolean;
  Index: Integer);
begin
  if not IsColumn then Exit;
  if Index = 4 then Index := -1;
  if Ord(FSortType) <> Index then
  begin
    FSortType := TMSXDiskSortType(Index);
    FDescOrdered := False;
  end
  else FDescOrdered := not FDescOrdered;
  UpdateColName;
  MainFileBox.Click;
end;

procedure TMainForm.MainGridKeyDown(Sender: TObject; var Key: Word;
  Shift: TShiftState);
begin
  case Key of
    VK_DELETE: DeleteButton.Click;
  end;
end;

end.
