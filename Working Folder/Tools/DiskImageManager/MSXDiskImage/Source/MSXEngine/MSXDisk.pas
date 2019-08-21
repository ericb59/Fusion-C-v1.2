unit MSXDisk;

{$MODE Delphi}

interface

uses
  Math, Classes, DateUtils, SysUtils, FileUtil;

const
  mfaReadOnly = $01;
  mfaHidden = $02;
  mfaSystem = $04;
  mfaLabel = $08;
  mfaDirectory = $10;
  mfaArchive = $20;

  MSXPathDelim = '\';

type
  TMSXDiskResult = (
    mdrDone = 0, mdrDiskFull, mdrDirectoryFull, mdrDirectoryExists,
    mdrDirectoryNotEmpty, mdrFileNotFound
  );
  TMSXDiskSortType = (
    mstNone = -1, mstNameExt, mstExtName, mstSize, mstDateTime
  );

  TTimeStamp = packed record
  private
    function GetSecond: Integer;
    procedure SetSecond(ASecond: Integer);
    function GetMinute: Integer;
    procedure SetMinute(AMinute: Integer);
    function GetHour: Integer;
    procedure SetHour(AHour: Integer);
    function GetTime: TDateTime;
    procedure SetTime(ATime: TDateTime);
  public
    Value: Word;
    property Time: TDateTime read GetTime write SetTime;
    property Hour: Integer read GetHour write SetHour;
    property Minute: Integer read GetMinute write SetMinute;
    property Second: Integer read GetSecond write SetSecond;
    procedure SetValue(AValue: Word);
  end;

  TDateStamp = packed record
  private
    function GetYear: Integer;
    procedure SetYear(AYear: Integer);
    function GetMonth: Integer;
    procedure SetMonth(AMonth: Integer);
    function GetDay: Integer;
    procedure SetDay(ADay: Integer);
    function GetDate: TDateTime;
    procedure SetDate(ADate: TDateTime);
  public
    Value: Word;
    property Date: TDateTime read GetDate write SetDate;
    property Day: Integer read GetDay write SetDay;
    property Month: Integer read GetMonth write SetMonth;
    property Year: Integer read GetYear write SetYear;
    procedure SetValue(AValue: Word);
  end;

  PDirEntry = ^TDirEntry;

  { TDirEntry }

  TDirEntry = packed record
  private
    function GetCluster: Cardinal;
    procedure SetCluster(ACluster: Cardinal);
    function GetFileName: string;
    procedure SetFileName(AName: string);
    function GetCreateDateTime: TDateTime;
    procedure SetCreateDateTime(ADateTime: TDateTime);
    function GetModifyDateTime: TDateTime;
    procedure SetModifyDateTime(ADateTime: TDateTime);
  public
    Name: array[0..7] of Char;
    Ext: array[0..2] of Char;
    Attr: Byte;
    Reserved: Byte;
    CreateTimeMSec: Byte;
    CreateTime: TTimeStamp;
    CreateDate: TDateStamp;
    AccessDate: TDateStamp;
    ClusterHigh: Word;
    ModifyTime: TTimeStamp;
    ModifyDate: TDateStamp;
    ClusterLow: Word;
    Size: Cardinal;
    property Cluster: Cardinal read GetCluster write SetCluster;
    property FileName: string read GetFileName write SetFileName;
    property CreateDateTime: TDateTime read GetCreateDateTime write SetCreateDateTime;
    property ModifyDateTime: TDateTime read GetModifyDateTime write SetModifyDateTime;
//    constructor Create(NewEntry: Boolean);
    procedure Clear(NewEntry: Boolean = False);
    function Existed: Boolean;
    function Deleted: Boolean;
    function AttrString: string;
    function IsDirectory: Boolean;
    function IsLabel: Boolean;
  end;
  PDirEntryArray = ^TDirEntryArray;
  TDirEntryArray = array[0..0] of TDirEntry;
  TArrayOfDirEntry = array of TDirEntry;

  { TDirEntryList }

  TDirEntryList = class(TList)
  private
    FSortType: TMSXDiskSortType;
    FDescOrdered: Boolean;
    function GetDirs(AIndex: Integer): PDirEntry;
    procedure SetDescOrdered(AValue: Boolean);
    procedure SetSortType(AValue: TMSXDiskSortType);
    procedure Notify(Ptr: Pointer; Action: TListNotification); override;
  public
    constructor Create;
    destructor Destroy; override;
    function Add(ADirEntry: TDirEntry): Integer;
    function AddDir: PDirEntry;
    procedure Sort;
    property SortType: TMSXDiskSortType read FSortType write SetSortType;
    property DescOrdered: Boolean read FDescOrdered write SetDescOrdered;
    property Dirs[AIndex: Integer]: PDirEntry read GetDirs;
  end;

  PBootRecord = ^TBootRecord;
  TBootRecord = packed record
  public
    JumpCode: array[0..2] of Byte;    // $00
    OemName: array[0..7] of Char;     // $03
    BytesPerSector: Word;             // $0B
    SectorsPerCluster: Byte;          // $0D
    ReservedSectorCount: Word;        // $0E
    FatTableCount: Byte;              // $10
    MaxRootEntry: Word;               // $11
    TotalSectorCount: Word;           // $13
    MediaType: Byte;                  // $15, $F8 = Fixed Disk, $F0 = Removable Disk
    SectorsPerFat: Word;              // $16
    SectorsPerTrack: Word;            // $18
    TotalHeadCount: Word;             // $1A
    BootCode: array[0..483] of Byte;  // $1C..$1FF, Or information to replace disk
//    PartitionSectorOffset: Cardinal; // Floppy not allow
//    TotalSectorCount32: Cardinal;    // Floppy not allow
//    DriveNumber_Bios13H: Byte;
//    Reserved: Byte;
//    ExtendedBootSignature: Byte;
//    VolumeSerialNumber: Cardinal;
//    VolumeLabelName: array[0..10] of Byte;
//    FileSystemType: array[0..7] of Byte;
//    BootCode: array[0..447] of Byte;  // Or information to replace disk
//    Signature: Word;
  end;

  { TMSXDisk }

  TMSXDisk = class
  private
    FFileName, FDirectory: string;
    FBootRecord: PBootRecord;
//    FDirTable: PDirEntryArray;
    FFatOffset, FDirOffset, FRootDirOffset, FDataOffset: Integer;
    FClusterCount, FDirCount, FRootDirCount, FDirIndex: Integer;
    FSectorSize, FClusterSize, FSectorsPerCluster: Integer;
    FReservedCount, FFatTableCount, FSectorsPerFat: Integer;
    FSectorCount, FFatSize: Integer;
    FLastError: TMSXDiskResult;
    function FindEmptyFat(AIndex: Integer = 2): Integer;
    procedure DeleteFat(AIndex: Integer);
    function GetClusterOffset(AIndex: Integer): Integer;
    function GetClusterNumber(AClusterOffset: Integer): Integer;
    function GetFatSize(ACluster: Integer): Integer;
  protected
    function GetFatTable(ACluster: Integer): Integer;
    procedure SetFatTable(ACluster: Integer; AValue: Integer);
    function GetDirTable(AIndex: Integer): TDirEntry;
    procedure SetDirTable(AIndex: Integer; AItem: TDirEntry);
    function GetFileCount: Integer;
    function GetFileUsage: Integer;
    function GetDiskUsage: Integer;
    function GetDiskFree: Integer;
    procedure SetDirectory(ADirectory: string);
  public
    Buffer: array[0..737279] of Byte;
    constructor Create;
    destructor Destroy; override;
    procedure Clear;
    procedure Changed;
    procedure ResetDir;
    procedure Assign(Source: TMSXDisk);
    function FetchDir(IncludeDeleted: Boolean = False): TDirEntry;
    function ExtractToFile(ADestName, AFileName: string): Boolean;
    function ExtractToStream(stream: TStream; fname: string): Boolean;
    function ExtractToMem(fname: string): TMemoryStream;
    function IncludePathDelim(ADirName: string): string;
    function ExcludePathDelim(ADirName: string): string;
    function Add(AFileName: string): Integer; overload;
    function Add(AList: TStrings): Integer; overload;
    function Delete(fname: string): TMSXDiskResult;
    function IndexOf(fname: string): Integer;
    function IndexOfSubDir(ADirName: string): Integer;
    function Lookup(fname: string): TDirEntry;
    function LookupSubDir(ADirName: string): TDirEntry;
    function FileNameExists(AFileName: string): Boolean;
    function SubDirExists(ADirName: string): Boolean;
    function GetSubDirFileCount(ADirName: string): Integer;
    function CreateSubDir(ADirName: string): TMSXDiskResult;
    function ChangeDir(ADir: string): Boolean;
    procedure GetCurrentDirList(AList: TDirEntryList);
    function LoadFromStream(AStream: TStream; AFileName: string = ''): Boolean;
    function LoadFromFile: Boolean; overload;
    function LoadFromFile(AFileName: string): Boolean; overload;
    procedure SaveToFile; overload;
    procedure SaveToFile(AFileName: string); overload;
    class function ShortFileName(AName: string): string;
    property BootRecord: PBootRecord read FBootRecord;
    property FatTable[ACluster: Integer]: Integer read GetFatTable write SetFatTable;
    property DirTable[AIndex: Integer]: TDirEntry read GetDirTable write SetDirTable;
    property DirCount: Integer read FDirCount;
    property FileName: string read FFileName write FFileName;
    property Directory: string read FDirectory write SetDirectory;
    property FileCount: Integer read GetFileCount;
    property FileUsage: Integer read GetFileUsage;
    property DiskUsage: Integer read GetDiskUsage;
    property DiskFree: Integer read GetDiskFree;
    property LastError: TMSXDiskResult read FLastError;
  end;

implementation

uses
  XSStream;

function CompareNameExt(PEntry1, PEntry2: Pointer): Integer;
var
  pdir1, pdir2: PDirEntry;
begin
  pdir1 := PEntry1;
  pdir2 := PEntry2;
  Result := 0;
  if pdir1.IsDirectory then
  begin
    if not pdir2.IsDirectory then Result := -1
  end
  else if pdir2.IsDirectory then Result := 1;
  if Result = 0 then
  begin
    Result := CompareText(pdir1.Name, pdir2.Name);
    if Result = 0 then Result := CompareText(pdir1.Ext, pdir2.Ext);
  end;
end;

function CompareExtName(PEntry1, PEntry2: Pointer): Integer;
var
  pdir1, pdir2: PDirEntry;
begin
  pdir1 := PEntry1;
  pdir2 := PEntry2;
  Result := 0;
  if pdir1.IsDirectory then
  begin
    if not pdir2.IsDirectory then Result := -1
  end
  else if pdir2.IsDirectory then Result := 1;
  if Result = 0 then
  begin
    if pdir1.IsDirectory then
    begin
      Result := CompareText(pdir1.Name, pdir2.Name);
      if Result = 0 then Result := CompareText(pdir1.Ext, pdir2.Ext);
    end
    else
    begin
      Result := CompareText(pdir1.Ext, pdir2.Ext);
      if Result = 0 then Result := CompareText(pdir1.Name, pdir2.Name);
    end;
  end;
end;

function CompareSize(PEntry1, PEntry2: Pointer): Integer;
var
  pdir1, pdir2: PDirEntry;
begin
  pdir1 := PEntry1;
  pdir2 := PEntry2;
  Result := 0;
  if pdir1.IsDirectory then
  begin
    if not pdir2.IsDirectory then Result := -1
  end
  else if pdir2.IsDirectory then Result := 1;
  if Result = 0 then
  begin
    Result := pdir1.Size - pdir2.Size;
    if Result = 0 then Result := CompareText(pdir1.Name, pdir2.Name);
    if Result = 0 then Result := CompareText(pdir1.Ext, pdir2.Ext);
  end;
end;

function CompareDateTime(PEntry1, PEntry2: Pointer): Integer;
var
  pdir1, pdir2: PDirEntry;
begin
  pdir1 := PEntry1;
  pdir2 := PEntry2;
  Result := 0;
  if pdir1.IsDirectory then
  begin
    if not pdir2.IsDirectory then Result := -1
  end
  else if pdir2.IsDirectory then Result := 1;
  if Result = 0 then
  begin
    if not pdir1.IsDirectory then
    begin
      Result := pdir1.ModifyDate.Value - pdir2.ModifyDate.Value;
      if Result = 0 then Result := pdir1.ModifyTime.Value - pdir2.ModifyTime.Value;
    end;
    if Result = 0 then Result := CompareText(pdir1.Name, pdir2.Name);
    if Result = 0 then Result := CompareText(pdir1.Ext, pdir2.Ext);
  end;
end;

function DescCompareNameExt(PEntry1, PEntry2: Pointer): Integer;
var
  pdir1, pdir2: PDirEntry;
begin
  pdir1 := PEntry1;
  pdir2 := PEntry2;
  Result := 0;
  if pdir1.IsDirectory then
  begin
    if not pdir2.IsDirectory then Result := -1
  end
  else if pdir2.IsDirectory then Result := 1;
  if Result = 0 then
  begin
    if pdir1.IsDirectory then
      Result := CompareText(pdir1.Name, pdir2.Name)
    else Result := CompareText(pdir2.Name, pdir1.Name);
    if Result = 0 then Result := CompareText(pdir1.Ext, pdir2.Ext);
  end;
end;

function DescCompareExtName(PEntry1, PEntry2: Pointer): Integer;
var
  pdir1, pdir2: PDirEntry;
begin
  pdir1 := PEntry1;
  pdir2 := PEntry2;
  Result := 0;
  if pdir1.IsDirectory then
  begin
    if not pdir2.IsDirectory then Result := -1
  end
  else if pdir2.IsDirectory then Result := 1;
  if Result = 0 then
  begin
    if pdir1.IsDirectory then
    begin
      Result := CompareText(pdir1.Name, pdir1.Name);
      if Result = 0 then Result := CompareText(pdir1.Name, pdir2.Name);
    end
    else
    begin
      Result := CompareText(pdir2.Ext, pdir1.Ext);
      if Result = 0 then Result := CompareText(pdir1.Name, pdir2.Name);
    end;
  end;
end;

function DescCompareSize(PEntry1, PEntry2: Pointer): Integer;
var
  pdir1, pdir2: PDirEntry;
begin
  pdir1 := PEntry1;
  pdir2 := PEntry2;
  Result := 0;
  if pdir1.IsDirectory then
  begin
    if not pdir2.IsDirectory then Result := -1
  end
  else if pdir2.IsDirectory then Result := 1;
  if Result = 0 then
  begin
    Result := pdir2.Size - pdir1.Size;
    if Result = 0 then Result := CompareText(pdir1.Name, pdir2.Name);
    if Result = 0 then Result := CompareText(pdir1.Ext, pdir2.Ext);
  end;
end;

function DescCompareDateTime(PEntry1, PEntry2: Pointer): Integer;
var
  pdir1, pdir2: PDirEntry;
begin
  pdir1 := PEntry1;
  pdir2 := PEntry2;
  Result := 0;
  if pdir1.IsDirectory then
  begin
    if not pdir2.IsDirectory then Result := -1
  end
  else if pdir2.IsDirectory then Result := 1;
  if Result = 0 then
  begin
    if not pdir1.IsDirectory then
    begin
      Result := pdir2.ModifyDate.Value - pdir1.ModifyDate.Value;
      if Result = 0 then Result := pdir2.ModifyTime.Value - pdir1.ModifyTime.Value;
    end;
    if Result = 0 then Result := CompareText(pdir1.Name, pdir2.Name);
    if Result = 0 then Result := CompareText(pdir1.Ext, pdir2.Ext);
  end;
end;

{ TDirEntryList }

function TDirEntryList.GetDirs(AIndex: Integer): PDirEntry;
begin
  Result := PDirEntry(Items[AIndex]);
end;

procedure TDirEntryList.SetDescOrdered(AValue: Boolean);
begin
  if FDescOrdered = AValue then Exit;
  FDescOrdered := AValue;
end;

procedure TDirEntryList.SetSortType(AValue: TMSXDiskSortType);
begin
  if FSortType = AValue then Exit;
  FSortType := AValue;
end;

constructor TDirEntryList.Create;
begin
  inherited Create;
  FSortType := mstNone;
  FDescOrdered := False;
end;

destructor TDirEntryList.Destroy;
begin
  inherited Destroy;
end;

function TDirEntryList.Add(ADirEntry: TDirEntry): Integer;
var
  pdir: PDirEntry;
begin
  New(pdir);
  pdir^ := ADirEntry;
  Result := inherited Add(pdir);
end;

function TDirEntryList.AddDir: PDirEntry;
begin
  New(Result);
  inherited Add(Result);
end;

procedure TDirEntryList.Sort;
begin
  if FDescOrdered then
  case FSortType of
    mstNameExt: inherited Sort(DescCompareNameExt);
    mstExtName: inherited Sort(DescCompareExtName);
    mstSize: inherited Sort(DescCompareSize);
    mstDateTime: inherited Sort(DescCompareDateTime);
  end
  else
  case FSortType of
    mstNameExt: inherited Sort(CompareNameExt);
    mstExtName: inherited Sort(CompareExtName);
    mstSize: inherited Sort(CompareSize);
    mstDateTime: inherited Sort(CompareDateTime);
  end;
end;

procedure TDirEntryList.Notify(Ptr: Pointer; Action: TListNotification);
begin
  case Action of
    lnExtracted, lnDeleted: Dispose(PDirEntry(Ptr));
  end;
end;

// TTimeStamp (hhhh hmmm mmms ssss)

procedure TTimeStamp.SetValue(AValue: Word);
begin
  Value := AValue;
end;

function TTimeStamp.GetSecond: Integer;
begin
  Result := (Value and $001F) shl 1;
end;

procedure TTimeStamp.SetSecond(ASecond: Integer);
begin
  Value := Value and $FFE0 or ((ASecond shr 1) and $001F);
end;

function TTimeStamp.GetMinute: Integer;
begin
  Result := (Value shr 5) and $003F;
end;

procedure TTimeStamp.SetMinute(AMinute: Integer);
begin
  Value := Value and $F81F or ((AMinute and $003F) shl 5);
end;

function TTimeStamp.GetHour: Integer;
begin
  Result := (Value shr 11) and $001F;
end;

procedure TTimeStamp.SetHour(AHour: Integer);
begin
  Value := Value and $07FF or ((AHour and $001F) shl 11);
end;

function TTimeStamp.GetTime: TDateTime;
begin
  Result := EncodeTime(Hour, Minute, Second, 0);
end;

procedure TTimeStamp.SetTime(ATime: TDateTime);
var
  h, m, s, u: Word;
begin
  DecodeTime(ATime, h, m, s, u);
  Hour := h;
  Minute := m;
  Second := s;
end;

// TDateStamp (yyyy yyym mmmd dddd)
procedure TDateStamp.SetValue(AValue: Word);
begin
  Value := AValue;
end;

function TDateStamp.GetYear: Integer;
begin
  Result := ((Value shr 9) and $007F) + 1980;
end;

procedure TDateStamp.SetYear(AYear: Integer);
begin
  Value := (Value and $01FF) or (((AYear - 1980) and $007F) shl 9);
end;

function TDateStamp.GetMonth: Integer;
begin
  Result := (Value shr 5) and $000F;
end;

procedure TDateStamp.SetMonth(AMonth: Integer);
begin
  Value := (Value and $FE1F) or ((AMonth and $000F) shl 5);
end;

function TDateStamp.GetDay: Integer;
begin
  Result := Value and $001F;
end;

procedure TDateStamp.SetDay(ADay: Integer);
begin
  Value := (Value and $FFE0) or (ADay and $001F);
end;

function TDateStamp.GetDate: TDateTime;
begin
  Result := EncodeDate(Year, Month, Day);
end;

procedure TDateStamp.SetDate(ADate: TDateTime);
var
  y, m, d: Word;
begin
  DecodeDate(ADate, y, m, d);
  Year := y;
  Month := m;
  Day := d;
end;

// TFileStamp
//constructor TDirEntry.Create(NewEntry: Boolean);
//begin
//  Clear(NewEntry);
//end;

function TDirEntry.AttrString: string;
const
  AttrText = 'advshr';
var
  n: Integer;
begin
  Result := '';
  for n := 0 to 5 do
  begin
    if Attr and (1 shl n) <> 0 then
      Result := Result + AttrText[6 - n]
    else Result := Result + '-';
  end;
end;

procedure TDirEntry.Clear(NewEntry: Boolean = False);
var
  ntime: TDateTime;
begin
  if NewEntry then
  begin
    Name := #$E5'       ';
    Ext := '   ';
    Attr := 0;
    Reserved := 0;
    ntime := Now;
    ModifyDateTime := ntime;
    CreateDate.Value := 0;
    CreateTime.Value := 0;
    AccessDate.Value := 0;
//    CreateDateTime := ntime;
//    AccessDate.Date := ntime;
    Cluster := 0;
    Size := 0;
  end
  else FillChar(Self, SizeOf(Self), 0);
end;

function TDirEntry.GetCluster: Cardinal;
begin
  Result := (ClusterHigh shl 16) or ClusterLow;
end;

procedure TDirEntry.SetCluster(ACluster: Cardinal);
begin
  ClusterHigh := ACluster shr 16;
  ClusterLow := ACluster and $FFFF;
end;

function TDirEntry.GetFileName: string;
begin
  Result := TrimRight(Ext);
  if Result <> '' then Result := '.' + Result;
  Result := TrimRight(Name) + Result;
end;

procedure TDirEntry.SetFileName(AName: string);
var
  se: string;
begin
  AName := UpperCase(ExtractFileName(AName));
  se := ExtractFileExt(AName);
  Delete(se, 1, 1);
  AName := ChangeFileExt(AName, '');
  FillChar(Name, 8, ' ');
  FillChar(Ext, 3, ' ');
  Move(AName[1], Name, Min(Length(AName), 8));
  Move(se[1], Ext, Min(Length(se), 3));
end;

function TDirEntry.GetCreateDateTime: TDateTime;
begin
  with CreateDate, CreateTime do
  begin
    Result := EncodeDateTime(Year, Month, Day, Hour, Minute, Second, (CreateTimeMSec * 10) div 2);
  end;
end;

procedure TDirEntry.SetCreateDateTime(ADateTime: TDateTime);
var
  h, m, s, u: Word;
begin
  CreateDate.Date := ADateTime;
  DecodeTime(ADateTime, h, m, s, u);
  CreateTimeMSec := (u * 2) div 10;
  with CreateTime do
  begin
    Hour := h;
    Minute := m;
    Second := s;
  end;
end;

function TDirEntry.GetModifyDateTime: TDateTime;
begin
  with ModifyDate, ModifyTime do
  begin
    Result := EncodeDateTime(Year, Month, Day, Hour, Minute, Second, 0);
  end;
end;

function TDirEntry.IsDirectory: Boolean;
begin
  Result := (Attr and mfaDirectory) <> 0;
end;

function TDirEntry.IsLabel: Boolean;
begin
  Result := (Attr and mfaLabel) <> 0;
end;

procedure TDirEntry.SetModifyDateTime(ADateTime: TDateTime);
begin
  ModifyDate.Date := ADateTime;
  ModifyTime.Time := ADateTime;
end;

function TDirEntry.Existed: Boolean;
begin
  Result := Name[0] <> #0;
end;

function TDirEntry.Deleted: Boolean;
begin
  Result := Name[0] = #$E5;
end;

// TMSXDisk
constructor TMSXDisk.Create;
begin
  FBootRecord := @Buffer[0];
  FSectorSize := 512;
  FClusterSize := 1024;
  FSectorsPerCluster := 2;
  FDirIndex := 0;
  FFileName := '';
  FDirectory := MSXPathDelim;
  FLastError := mdrDone;
  Clear;
end;

destructor TMSXDisk.Destroy;
begin
  inherited Destroy;
end;

function TMSXDisk.SubDirExists(ADirName: string): Boolean;
begin
  Result := IndexOfSubDir(ADirName) >= 0;
end;

function TMSXDisk.GetSubDirFileCount(ADirName: string): Integer;
var
  dir: TDirEntry;
  n: Integer;
begin
  Result := 0;
  if ChangeDir(ADirName) then
  begin
    n := -1;
    repeat
      Inc(n);
      dir := DirTable[n];
      if (dir.FileName <> '.') and (dir.FileName <> '..')
        and not dir.Deleted and dir.Existed then Inc(Result);
    until not dir.Existed;
    ChangeDir('..');
  end;
end;

function TMSXDisk.CreateSubDir(ADirName: string): TMSXDiskResult;
var
  ndir, nfat, n, fsize, coffset: Integer;
  dir, subdir: TDirEntry;
  fs: TFileStream;
begin
  if FileNameExists(ADirName) then
    Result := mdrDirectoryExists
  else
  begin
    ndir := -1;
    repeat
      Inc(ndir);
      dir := DirTable[ndir];
    until not dir.Existed or dir.Deleted;
    if ndir < FDirCount then
    begin
      dir.Clear(True);
      dir.FileName := ADirName;
      dir.Attr := mfaDirectory;
    end;
    if not dir.Existed then
      Result := mdrDirectoryFull
    else
    begin
      nfat := FindEmptyFat(2);
      if nfat < 0 then
        Result := mdrDiskFull
      else
      begin
        dir.Cluster := nfat;
        coffset := GetClusterOffset(nfat);
        subdir.Clear(True);
        subdir.Attr := mfaDirectory;
        subdir.Name := '.       ';
        subdir.Cluster := nfat;
        Move(subdir, Buffer[coffset], 32);
        Inc(coffset, 32);
        subdir.Name := '..      ';
        subdir.Cluster := GetClusterNumber(FDirOffset);
        Move(subdir, Buffer[coffset], 32);
        Inc(coffset, 32);
        subdir.Attr := 0;
        subdir.Name := #$E5'       ';
        subdir.Cluster := 0;
        fsize := FClusterSize - 64;
        while fsize > 0 do
        begin
          Move(subdir, Buffer[coffset], 32);
          Inc(coffset, 32);
          Dec(fsize, 32);
        end;
        DirTable[ndir] := dir;
        FatTable[nfat] := $FFF;
        Result := mdrDone;
      end;
    end
  end;
  FLastError := Result;
end;

procedure TMSXDisk.Clear;
const
  MSXBootCode: array[0..243] of Byte = ($00,$00,$D0,$ED,
    $53,$58,$C0,$32,$C2,$C0,$36,$55,$23,$36,$C0,$31,$1F,$F5,$11,$9D,
    $C0,$0E,$0F,$CD,$7D,$F3,$3C,$28,$28,$11,$00,$01,$0E,$1A,$CD,$7D,
    $F3,$21,$01,$00,$22,$AB,$C0,$21,$00,$3F,$11,$9D,$C0,$0E,$27,$CD,
    $7D,$F3,$C3,$00,$01,$57,$C0,$CD,$00,$00,$79,$E6,$FE,$FE,$02,$20,
    $07,$3A,$C2,$C0,$A7,$CA,$22,$40,$11,$77,$C0,$0E,$09,$CD,$7D,$F3,
    $0E,$07,$CD,$7D,$F3,$18,$B4,$42,$6F,$6F,$74,$20,$65,$72,$72,$6F,
    $72,$0D,$0A,$50,$72,$65,$73,$73,$20,$61,$6E,$79,$20,$6B,$65,$79,
    $20,$66,$6F,$72,$20,$72,$65,$74,$72,$79,$0D,$0A,$24,$00,$4D,$53,
    $58,$44,$4F,$53,$20,$20,$53,$59,$53,$00,$00,$00,$00,$00,$00,$00,
    $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,
    $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$F3,$2A,
    $51,$F3,$11,$00,$01,$19,$01,$00,$01,$11,$00,$C1,$ED,$B0,$3A,$EE,
    $C0,$47,$11,$EF,$C0,$21,$00,$00,$CD,$51,$52,$F3,$76,$C9,$18,$64,
    $3A,$AF,$80,$F9,$CA,$6D,$48,$D3,$A5,$0C,$8C,$2F,$9C,$CB,$E9,$89,
    $D2,$00,$32,$26,$40,$94,$61,$19,$20,$E6,$80,$6D,$8A,$00,$00,$00
  );
  MSXBootCode2: array[0..163] of Byte = ($00,$00,$18,$10,
    $56,$4F,$4C,$5F,$49,$44,$00,$1A,$11,$04,$5D,$00,$00,$00,$00,$00,
    $D0,$ED,$53,$6A,$C0,$32,$72,$C0,$36,$67,$23,$36,$C0,$31,$1F,$F5,
    $11,$AB,$C0,$0E,$0F,$CD,$7D,$F3,$3C,$28,$26,$11,$00,$01,$0E,$1A,
    $CD,$7D,$F3,$21,$01,$00,$22,$B9,$C0,$21,$00,$3F,$11,$AB,$C0,$0E,
    $27,$CD,$7D,$F3,$C3,$00,$01,$69,$C0,$CD,$00,$00,$79,$E6,$FE,$D6,
    $02,$F6,$00,$CA,$22,$40,$11,$85,$C0,$0E,$09,$CD,$7D,$F3,$0E,$07,
    $CD,$7D,$F3,$18,$B8,$42,$6F,$6F,$74,$20,$65,$72,$72,$6F,$72,$0D,
    $0A,$50,$72,$65,$73,$73,$20,$61,$6E,$79,$20,$6B,$65,$79,$20,$66,
    $6F,$72,$20,$72,$65,$74,$72,$79,$0D,$0A,$24,$00,$4D,$53,$58,$44,
    $4F,$53,$20,$20,$53,$59,$53,$00,$00,$00,$00,$00,$00,$00,$00,$00
  );

begin
  FillChar(Buffer[0], $E5, SizeOf(Buffer));
  with FBootRecord^ do
  begin
    JumpCode[0] := $EB;
    JumpCode[1] := $FE;
    JumpCode[2] := $90;
    OemName := 'MSX_04  ';
    BytesPerSector := 512;
    SectorsPerCluster := 2;
    ReservedSectorCount := 1;
    FatTableCount := 2;
    MaxRootEntry := 112;
    TotalSectorCount := 1440;
    MediaType := $F9;
    SectorsPerFat := 3;
    SectorsPerTrack := 9;
    TotalHeadCount := 2;
    Move(MSXBootCode2, BootCode, SizeOf(MSXBootCode2));
  end;
  Changed;
  FatTable[0] := $FF9;
  FatTable[1] := $FFF;
end;

function TMSXDisk.GetFatSize(ACluster: Integer): Integer;
begin
  Result := 0;
  repeat
    ACluster := GetFatTable(ACluster);
    if ACluster <> 0 then Inc(Result, FClusterSize);
  until (ACluster = 0) or (ACluster = $FFF) or (ACluster >= FClusterCount);
end;

function TMSXDisk.GetFatTable(ACluster: Integer): Integer;
var
  p: Integer;
begin
  Result := $FFF;
  if (ACluster >= 0) and (ACluster < FClusterCount) then
  begin
    p := FFatOffset + (ACluster shr 1) * 3;
    if (ACluster and 1) > 0 then
      Result := (Buffer[p + 2] shl 4) or (Buffer[p + 1] shr 4)
    else Result := Buffer[p] or ((Buffer[p + 1] and 15) shl 8);
  end;
end;

procedure TMSXDisk.SetFatTable(ACluster: Integer; AValue: Integer);
var
  p: Integer;
begin
  if (ACluster >= 0) and (ACluster < FClusterCount) then
  begin
    p := FFatOffset + (ACluster shr 1) * 3;
    if (ACluster and 1) > 0 then
    begin
      Buffer[p + 1] := (Buffer[p + 1] and $0F) or ((AValue and 15) shl 4);
      Buffer[p + 2] := (AValue shr 4) and $FF;
    end
    else
    begin
      Buffer[p] := AValue and $FF;
      Buffer[p + 1] := (Buffer[p + 1] and $F0) or ((AValue shr 8) and 15);
    end;
    if FFatTableCount > 1 then Move(Buffer[FFatOffset], Buffer[FFatOffset + FFatSize], FFatSize);
  end;
end;

procedure TMSXDisk.SetDirectory(ADirectory: string);
begin
  ChangeDir(ADirectory);
end;

function TMSXDisk.GetDiskFree: Integer;
var
  n: Integer;
begin
  Result := 0;
  for n := 0 to FClusterCount - 1 do
  begin
    if FatTable[n] = 0 then Inc(Result, FClusterSize);
  end;
end;

function TMSXDisk.GetDiskUsage: Integer;
var
  n: Integer;
begin
  Result := 0;
  for n := 0 to FClusterCount - 1 do
  begin
    if FatTable[n] <> 0 then Inc(Result, FClusterSize);
  end;
end;

function TMSXDisk.GetFileCount: Integer;
var
  dir: TDirEntry;
  n: Integer;
begin
  n := 0;
  Result := 0;
  repeat
    dir := DirTable[n];
    if dir.Existed then
      if not dir.Deleted and ((dir.Attr and (mfaDirectory or mfaLabel)) = 0) then Inc(Result);
    Inc(n);
  until not dir.Existed or (n >= FDirCount);
end;

function TMSXDisk.GetFileUsage: Integer;
var
  dir: TDirEntry;
  n: Integer;
begin
  n := 0;
  Result := 0;
  repeat
    dir := DirTable[n];
    if dir.Existed then
      if not dir.Deleted and ((dir.Attr and (mfaDirectory or mfaLabel)) = 0) then Inc(Result, dir.Size);
    Inc(n);
  until not dir.Existed or (n >= FDirCount);
end;

function TMSXDisk.GetDirTable(AIndex: Integer): TDirEntry;
var
  pdir: PDirEntryArray;
  nfat: Integer;
begin
  if (AIndex >= 0) and (AIndex < FDirCount) then
    if FDirOffset = FRootDirOffset then
    begin
      pdir := @Buffer[FRootDirOffset];
      Result := pdir[AIndex]
    end
    else
    begin
      nfat := GetClusterNumber(FDirOffset);
      while AIndex >= 32 do
      begin
        nfat := FatTable[nfat];
        Dec(AIndex, 32);
      end;
      pdir := @Buffer[GetClusterOffset(nfat)];
      Result := pdir[AIndex];
    end
  else Result.Clear(False);
end;

function TMSXDisk.FileNameExists(AFileName: string): Boolean;
begin
  Result := Lookup(AFileName).Existed;
end;

function TMSXDisk.FindEmptyFat(AIndex: Integer = 2): Integer;
begin
  Result := -1;
  while (AIndex < FClusterCount) and (Result < 0) do
  begin
    if FatTable[AIndex] = 0 then Result := AIndex;
    Inc(AIndex);
  end;
end;

procedure TMSXDisk.DeleteFat(AIndex: Integer);
var
  n: Integer;
begin
  while (AIndex > 1) and (AIndex < FClusterCount) do
  begin
    n := FatTable[AIndex];
    if (n < $FF0) or (n > $FF7) then FatTable[AIndex] := 0;
    AIndex := n;
  end;
end;

function TMSXDisk.GetClusterOffset(AIndex: Integer): Integer;
begin
  Result := FDataOffset + (AIndex - 2) * FClusterSize;
end;

function TMSXDisk.GetClusterNumber(AClusterOffset: Integer): Integer;
begin
  if AClusterOffset <> FRootDirOffset then
    Result := ((AClusterOffset - FDataOffset) div FClusterSize) + 2
  else Result := 0;
end;

class function TMSXDisk.ShortFileName(AName: string): string;
var
  se: string;
begin
  AName := UpperCase(ExtractFileName(AName));
  se := TrimRight(Copy(ExtractFileExt(AName), 1, 4));
  AName := TrimRight(Copy(ChangeFileExt(AName, ''), 1, 8));
  Result := AName + se;
end;

// Result = number of file(s) to add (0 or 1)
// LastError = error result
function TMSXDisk.Add(AFileName: string): Integer;
var
  ndir, nfat, n, fsize, coffset: Integer;
  dir, subdir: TDirEntry;
  fs: TFileStream;
begin
  Result := 0;
  fs := TFileStream.Create(AFileName, fmOpenRead);
  try
    fsize := fs.Size;
    AFileName := ShortFileName(AFileName);
    ndir := IndexOf(AFileName);
    if ndir < 0 then
    begin
      ndir := -1;
      repeat
        Inc(ndir);
        dir := DirTable[ndir];
      until not dir.Existed or dir.Deleted;
      if ndir < FDirCount then
      begin
        dir.Clear(True);
        dir.FileName := AFileName;
      end
      else if FDirOffset <> FRootDirOffset then
      begin
        nfat := FindEmptyFat(2);
        if nfat >= 0 then
        begin
          ndir := 32;
          n := GetClusterNumber(FDirOffset);
          while (n < FClusterCount) and (FatTable[n] <> $FFF) and (FatTable[n] <> 0) do
          begin
            n := FatTable[n];
            Inc(ndir, FClusterSize div 32);
          end;
          if n < FClusterCount then
          begin
            FatTable[n] := nfat;
            FatTable[nfat] := $FFF;
            coffset := GetClusterOffset(nfat);
            subdir.Clear(True);
            n := FClusterSize;
            while n > 0 do
            begin
              Move(subdir, Buffer[coffset], 32);
              Inc(coffset, 32);
              Dec(n, 32);
            end;
            dir := DirTable[ndir];
            dir.FileName := AFileName;
            FDirCount := ndir + 32;
          end;
        end;
      end;
    end
    else
    begin
      dir := DirTable[ndir];
      DeleteFat(dir.ClusterLow);
      dir.Cluster := 0;
      dir.Size := 0;
      dir.ModifyDateTime := Now;
    end;
    if dir.Existed then
    begin
      nfat := FindEmptyFat(2);
      dir.Cluster := nfat;
//      DirTable[ndir] := dir;
      while (fsize > 0) and (nfat >= 0) do
      begin
        n := Min(fsize, FClusterSize);
        coffset := GetClusterOffset(nfat);
        fs.Read(Buffer[coffset], n);
        dir.Size := dir.Size + Cardinal(Max(0, n));
//          DirTable[ndir] := dir;
        fsize := fsize - n;
        if fsize > 0 then
          n := FindEmptyFat(nfat + 1)
        else n := -1;
        if n < 0 then
          FatTable[nfat] := $FFF
        else FatTable[nfat] := n;
        nfat := n;
      end;
      if fsize <= 0 then
      begin
        DirTable[ndir] := dir;
        FLastError := mdrDone;
        Result := 1;
      end
      else
      begin
        DeleteFat(dir.Cluster);
        FLastError := mdrDiskFull;
      end;
    end
    else FLastError := mdrDirectoryFull;
  finally
    fs.Free;
  end;
end;

// Result = number of file(s) to add
// LastError = error result
function TMSXDisk.Add(AList: TStrings): Integer;
begin
  Result := 0;
  FLastError := mdrDone;
  while (Result < Alist.Count) and (FLastError = mdrDone) do
  begin
    Inc(Result, Add(AList[Result]));
  end;
end;

procedure TMSXDisk.Assign(Source: TMSXDisk);
begin
  raise Exception.Create('Can not assign TMSXDisk for now.');
end;

procedure TMSXDisk.Changed;
begin
  with FBootRecord^ do
  begin
//    FReservedCount := ReservedSectorCount;
//    FFatTableCount := FatTableCount;
//    FSectorsPerFat := SectorsPerFat;
//    FDirCount := MaxRootEntry;
//    FSectorSize := BytesPerSector;
//    FSectorsPerCluster := SectorsPerCluster;
//    FSectorCount := TotalSectorCount;

    FReservedCount := 1;
    FFatTableCount := 2;
    FRootDirCount := 112;
    FDirCount := FRootDirCount;
    FSectorSize := 512;
    FSectorsPerCluster := 2;
    case MediaType of
      $F8:
      begin
        FSectorCount := 720;
        FSectorsPerFat := 2;
      end
    else
      FSectorCount := 1440;
      FSectorsPerFat := 3;
    end;

    FClusterSize := FSectorSize * FSectorsPerCluster;
    FFatOffset := FSectorSize * FReservedCount;
    FFatSize := FSectorsPerFat * FSectorSize;
    FRootDirOffset := FFatOffset + FFatTableCount * FFatSize;
    FDirOffset := FRootDirOffset;
    FDataOffset := FDirOffset + ((FDirCount * 32 + FSectorSize - 1) div FSectorSize) * FSectorSize;
    FClusterCount := ((FSectorCount * FSectorSize) - FDataOffset) div FClusterSize + 2;
//    FDirTable := @Buffer[FDirOffset];
    FDirectory := MSXPathDelim;
  end;
  FDirIndex := 0;
end;

function TMSXDisk.ChangeDir(ADir: string): Boolean;
var
  sdir: string;
  dir: TDirEntry;
begin
  if ADir = MSXPathDelim then
  begin
    FDirOffset := FRootDirOffset;
    FDirCount := FRootDirCount;
//    FDirTable := @Buffer[FDirOffset];
    FDirectory := ADir;
    Result := True;
  end
  else
  begin
    ADir := UpperCase(ExcludePathDelim(ADir));
    sdir := ExtractFilePath(ADir);
    if sdir <> '' then
    begin
      Result := ChangeDir(sdir);
      if not Result then Exit;
    end;
    dir := LookupSubDir(ExtractFileName(ADir));
    Result := dir.Existed;
    if Result then
    begin
      if dir.ClusterLow = 0 then
      begin
        FDirOffset := FRootDirOffset;
        FDirCount := FRootDirCount;
      end
      else
      begin
        FDirOffset := GetClusterOffset(dir.ClusterLow);
        FDirCount := GetFatSize(dir.ClusterLow) div 32;
      end;
//      FDirTable := @Buffer[FDirOffset];
      if dir.FileName = '..' then
        FDirectory := ExtractFilePath(ExcludePathDelim(FDirectory))
      else if dir.FileName <> '.' then FDirectory := IncludePathDelim(FDirectory) + dir.FileName + MSXPathDelim;
    end;
  end;
end;

procedure TMSXDisk.GetCurrentDirList(AList: TDirEntryList);
var
  dir: TDirEntry;
  ndir: Integer;
begin
  AList.Clear;
  ndir := 0;
  dir := DirTable[ndir];
  while dir.Existed do
  begin
    if not dir.Deleted then AList.Add(dir);
    Inc(ndir);
    dir := DirTable[ndir];
  end;
end;

procedure TMSXDisk.ResetDir;
begin
  FDirIndex := 0;
end;

function TMSXDisk.FetchDir(IncludeDeleted: Boolean): TDirEntry;
begin
  repeat
    Result := DirTable[FDirIndex];
    Inc(FDirIndex);
  until not Result.Deleted or not Result.Existed or IncludeDeleted;
end;

function TMSXDisk.ExtractToFile(ADestName, AFileName: string): Boolean;
var
  fs: TFileStream;
begin
  fs := TFileStream.Create(ADestName, fmCreate);
  try
    Result := ExtractToStream(fs, AFileName);
  finally
    fs.Free;
  end;
end;

function TMSXDisk.ExtractToMem(fname: string): TMemoryStream;
begin
  Result := TMemoryStream.Create;
  ExtractToStream(Result, fname);
  Result.Seek(0, soBeginning);
end;

function TMSXDisk.IncludePathDelim(ADirName: string): string;
var
  n: Integer;
begin
  n := Length(ADirName);
  if (n = 0) or (ADirName[n] <> MSXPathDelim) then
    Result := ADirName + MSXPathDelim
  else Result := ADirName;
end;

function TMSXDisk.ExcludePathDelim(ADirName: string): string;
var
  n: Integer;
begin
  n := Length(ADirName);
  if (n > 0) and (ADirName[n] = MSXPathDelim) then Dec(n);
  Result := Copy(ADirName, 1, n);
end;

function TMSXDisk.ExtractToStream(stream: TStream; fname: string): Boolean;
var
  dir: TDirEntry;
  ns, nc, nw, csize: Integer;
begin
  fname := UpperCase(ExtractFileName(fname));
  dir := Lookup(fname);
  if dir.Existed and (dir.FileName = fname) then
  begin
//    csize := FBootRecord.BytesPerSector * FBootRecord.SectorsPerCluster;
    csize := 512 * 2;
    ns := dir.Size;
    nc := dir.ClusterLow;
    while (ns > 0) and (FatTable[nc] <> 0) and (nc < FClusterCount) do
    begin
      nw := Min(ns, csize);
      stream.Write(Buffer[FDataOffset + (nc - 2) * csize], nw);
      ns := ns - nw;
      nc := FatTable[nc];
    end;
    Result := ns = 0;
  end
  else Result := False;
end;

function TMSXDisk.Delete(fname: string): TMSXDiskResult;
var
  n: Integer;
  dir: TDirEntry;
begin
  n := IndexOf(fname);
  if n < 0 then
    Result := mdrFileNotFound
  else
  begin
    if GetSubDirFileCount(fname) <> 0 then
      Result := mdrDirectoryNotEmpty
    else
    begin
      dir := DirTable[n];
      DeleteFat(dir.ClusterLow);
      dir.Name[0] := Chr($E5);
      DirTable[n] := dir;
      Result := mdrDone;
    end;
  end;
end;

function TMSXDisk.IndexOf(fname: string): Integer;
var
  dir: TDirEntry;
  n: Integer;
begin
  fname := UpperCase(ExtractFileName(fname));
  Result := -1;
  n := 0;
  dir := DirTable[n];
  while dir.Existed and (Result < 0) do
  begin
    if dir.FileName = fname then Result := n;
    Inc(n);
    dir := DirTable[n];
  end;
end;

function TMSXDisk.IndexOfSubDir(ADirName: string): Integer;
var
  dir: TDirEntry;
  n: Integer;
begin
  ADirName := UpperCase(ExtractFileName(ADirName));
  Result := -1;
  n := 0;
  dir := DirTable[n];
  while dir.Existed and (Result < 0) do
  begin
    if (dir.FileName = ADirName) and ((dir.Attr and mfaDirectory) <> 0) then Result := n;
    Inc(n);
    dir := DirTable[n];
  end;
end;

function TMSXDisk.Lookup(fname: string): TDirEntry;
var
  n: Integer;
begin
  n := IndexOf(fname);
  if n >= 0 then
    Result := DirTable[n]
  else Result.Clear;
end;

function TMSXDisk.LookupSubDir(ADirName: string): TDirEntry;
var
  n: Integer;
begin
  n := IndexOfSubDir(ADirName);
  if n >= 0 then
    Result := DirTable[n]
  else Result.Clear;
end;

function TMSXDisk.LoadFromFile(AFileName: string): Boolean;
var
  fs: TStream;
begin
  Clear;
  Result:= FileExistsUTF8(AFileName);
  if Result then
  begin
    FFileName := AFileName;
    fs := TFileStream.Create(AFileName, fmOpenRead);
    if UpperCase(ExtractFileExt(AFileName)) = '.XSA' then
    begin
      fs := TXSStream.Create(fs)
    end;
    try
      Result := LoadFromStream(fs);
    finally
      fs.Free;
    end;
  end;
end;

function TMSXDisk.LoadFromFile: Boolean;
begin
  Result := LoadFromFile(FFileName);
end;

function TMSXDisk.LoadFromStream(AStream: TStream; AFileName: string): Boolean;
begin
  Clear;
  AStream.Read(Buffer[0], SizeOf(Buffer));
  if (FBootRecord.JumpCode[0] = $EB)
    and ((FBootRecord.MediaType = $F8)
    or (FBootRecord.MediaType = $F9)) then
  begin
    Changed;
    if AFileName <> '' then FFileName := AFileName;
    Result := True;
  end
  else Result := False;
end;

procedure TMSXDisk.SaveToFile;
begin
  SaveToFile(FFileName);
end;

procedure TMSXDisk.SaveToFile(AFileName: string);
var
  fs: TStream;
begin
  FFileName := AFileName;
  fs := TFileStream.Create(AFileName, fmCreate);
  if UpperCase(ExtractFileExt(AFileName)) = '.XSA' then
  begin
    fs := TXSStream.Create(fs, AFileName)
  end;
  try
    fs.Write(Buffer[0], SizeOf(Buffer));
  finally
    fs.Free;
  end;
end;

procedure TMSXDisk.SetDirTable(AIndex: Integer; AItem: TDirEntry);
var
  pdir: PDirEntryArray;
  nfat: Integer;
begin
  if (AIndex >= 0) and (AIndex < FDirCount) then
  begin
    if FDirOffset = FRootDirOffset then
    begin
      pdir := @Buffer[FRootDirOffset];
      pdir[AIndex] := AItem
    end
    else
    begin
      nfat := GetClusterNumber(FDirOffset);
      while AIndex >= 32 do
      begin
        nfat := FatTable[nfat];
        Dec(AIndex, 32);
      end;
      pdir := @Buffer[GetClusterOffset(nfat)];
      pdir[AIndex] := AItem;
    end
  end;
end;

end.
