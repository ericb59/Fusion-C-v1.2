unit XSStream;

{$MODE Delphi}

interface

uses
  SysUtils, Classes, Dialogs;

//****************************************************************
//* some usefull macro's, etc
//****************************************************************

const
  logtblsize = 4;
  tblsize = 1 shl logtblsize;

type
  PBytes = ^TBytes;
  TBytes = array[0..0] of Byte;
  TArrayOfPBytes = array of PBytes;
  TArraysOfPByte = array of TArrayOfPBytes;
  PPBytes = ^TPBytes;
  TPBytes = array[0..0] of PBytes;

  PHufNodes = ^THufNodes;
  THufNode = record
    weight: SmallInt;
    child1, child2: PHufNodes;
  end;
  THufNodes = array[0..0] of THufNode;

  THufCode = record
    nrbits: SmallInt;
    bitcode: Word;
  end;

  TIndexInfo = record
    firstptr: Word;
    nrptrs: Word;
  end;
  TArrayOfIndexInfo = array of TIndexInfo;
  PIndexInfos = ^TIndexInfos;
  TIndexInfos = array[0..0] of TIndexInfo;

  TXSCore = class
  protected
    updhufcnt: Word;
    cpdist: array[0..tblsize] of Word;
    cpdbmask: array[0..tblsize - 1] of Word;
    tblsizes: array[0..tblsize - 1] of Word;
    huftbl: array[0..2 * tblsize - 2] of THufNode;
    procedure mkhuftbl;
    procedure inithufinfo;
  end;

  TXSDecode = class(TXSCore)
  private
    FStream: TStream;
    FOrgName: string;

    inbuf: array of Byte;
    inbufpos: PBytes;              // pos in input buffer
    inbufcnt: Word;               // #elements in the input buffer

    outbuf: array of Byte;           // the output buffer
    outbufpos: PBytes;        // pos in output buffer
    outbufcnt: Word;              // #elements in the output buffer

    readbufpos: PBytes;
    readbufcnt: Word;             // #elements in the output buffer

    FOrgSize: Cardinal;           // lengte ongecrunchte file
    FPackedSize: Cardinal;  // lengte gecrunchte file

    nrread, nrwrite: Cardinal;             // #gelezen bytes

    bitflg: Byte;                 // flag with the bits
    bitcnt: Byte;                 // #resterende bits
    CodeLength: Byte;
    FEOF: Boolean;

    function chkheader: Integer;    // controleer de fileheader
    procedure charout(ch: Byte);    // put a character in the output stream
    function rdstrlen: Word;        // read string length
    function rdstrpos: Word;        // read string pos
    function charin: Byte;          // read a char
    function bitin: Byte;           // read a bit
    function Read(var AData: Byte): Longint;
  public
    constructor Create(AStream: TStream);
    destructor Destroy; override;
    property OrgSize: Cardinal read FOrgSize;
    property PackedSize: Cardinal read FPackedSize;
    property EOF: Boolean read FEOF;
  end;

  TXSEncode = class(TXSCore)
  private
    FStream: TStream;
    FOrgName: string;
    FState: Integer;

    bytecnt: Word;     //* #bytes in the byte output buffer */
    bytebuf: array of Byte;    //* the byte + bitflg output buffer  */
    bitflgcnt: Word;   //* the current bitflg pos in buffer */
    bitflg: Byte;      //* the current bitflg               */
    setflg: Byte;      //* the bit which has to be set      */
    nrwritten: Integer;
    lookahead_cnt: Word;

    hufcodtbl: array[0..tblsize - 1] of THufCode;

    mwindow: array of Byte;              //* the (wrap around) moving window */

    eofflg: Boolean;              //* end of file flag for input file */
    nrread: Integer;

    strbufptrs: TArraysOfPByte;   //* buffer with ptrs to strptrs     */
    indexinfo: TArrayOfIndexInfo;       //* firstptr and nrptrs             */
    winpos, freepos, replace_cnt, strlen, strpos: Word;                //* first free pos in moving window */

    procedure strout(str: PBytes; strlen: Byte; strpos: Word); //* send a string to the output           */
    procedure charout(ch: Byte); //* send a char to the output             */
    procedure cnvtohuffcodes(nodeptr: PHufNodes; bitcode: Word; nrbits: SmallInt); //* zet de boom om in huffman codes  */
    function findmsb(n: Byte): Byte; //* vind hoogste bit van een getal        */
    function bitglen: Cardinal; //* get length of a bitstream file*/
    procedure bitspos(newpos: Cardinal);  //* go to a position in a bitstrem file   */
    procedure bitout(n: Byte);
    procedure byteout(n: Byte);  //* send a byte to the bitstream          */
    procedure flushout; //* flush the bytebuffer                  */
    procedure fileout(n: Byte); //* send a byte to the output file        */

    function nrindexptrs: Word;
    function indexpmsk: Word;
    function index(ch, offs: Byte): PPBytes;
    function offsindex(chindex: PPBytes; offs: Byte): PPBytes;

    function inglen: Integer;                 //* get length of input file              */
    procedure init_index;                     //* initialize the index information      */
    procedure DeleteString(strpos: Word);     //* delete a string from the indexptrs */
    procedure fAddString(winpos: Word);       //* start pos of the string to add */
    function AddString(winpos: Word; var strposptr: Word): Word; //* add str with srch for best match      */

    procedure mkheader(name: string);                     //* maak de header                        */
    procedure savelens(orglen, destlen: Cardinal);        //* save the org len and dest len         */
  public
    constructor Create(AStream: TStream; AOrgName: string);
    destructor Destroy; override;
    procedure Write(AData: Byte);
  end;

  TXSStream = class(TStream)
  private
    FDecoder: TXSDecode;
    FEncoder: TXSEncode;
    FWrited: Boolean;
  protected
    function GetOrgName: string;
    function GetSize: Int64; override;
    procedure SetSize(NewSize: Longint); override;
    procedure SetSize(const NewSize: Int64); override;
  public
    constructor Create(const AStreamToDecode: TStream); overload;
    constructor Create(const AStreamToEncode: TStream; AOriginalName: string); overload;
    destructor Destroy; override;
    function Read(var Buffer; Count: Longint): Longint; override;
    function Write(const Buffer; Count: Longint): Longint; override;
    function Seek(const Offset: Int64; Origin: TSeekOrigin): Int64; override;
    property OrgName: string read GetOrgName;
  end;

implementation

const
  slwinsnrbits = 13;
  maxstrlen = 254;

  maxhufcnt = 127;

  slwinsize = 1 shl slwinsnrbits;
  outbufsize = slwinsize + 4096;
  inbufsize = 32768;

  obufsize = 32678;
  ibufsize = 64512;

  slwinmask = slwinsize - 1;

  lognrindexptrs = 8;

{$IFDEF bits5}
  cpdext: array[0..31] of Word = ( // Extra bits for distance codes
    0,  0,  0,  0,  1,  1,  2,  2,  2,  2,  3,
    3,  3,  4,  4,  4,  5,  5,  5,  5,  5,  6,
    6,  6,  6,  7,  7,  8, 10, 11, 11, 11 );
{$ELSE}
  cpdext: array[0..15] of Word = ( // Extra bits for distance codes
    0,  0,  0,  0,  1,  2,  3,  4,
    5,  6,  7,  8,  9, 10, 11, 12 );
{$ENDIF}

{ TXSCore }

//***************************************************************
// initialize the huffman info tables                           *
//***************************************************************
procedure TXSCore.inithufinfo;
var
  offs, count: Word;
begin
  offs := 1;
  for count := 0 to tblsize - 1 do
  begin
    cpdist[count] := offs;
    cpdbmask[count] := 1 shl cpdext[count];
    Inc(offs, cpdbmask[count]);
  end;
  cpdist[tblsize] := offs;
//  Writeln(Format('eind offset: %u, eindcount %u', [offs, count]));

  for count := 0 to tblsize - 1 do
  begin
    tblsizes[count] := 0; // reset the table counters
    huftbl[count].child1 := nil; // mark the leave nodes
  end;
  mkhuftbl; // make the huffman table
end;

//****************************************************************
//* maak de huffman codeer informatie				                     *
//****************************************************************
procedure TXSCore.mkhuftbl;
var
  count: Word;
  hufpos: PHufNodes;
  l1pos, l2pos: PHufNodes;
  tempw: SmallInt;
begin
// Initialize the huffman tree
  hufpos := @huftbl[0];
  for count := 0 to tblsize - 1 do
  begin
    tblsizes[count] := tblsizes[count] shr 1;
    hufpos[0].weight := 1 + tblsizes[count];
    Inc(hufpos);
  end;
  for count := tblsize to 2 * tblsize - 2 do
  begin
    hufpos[0].weight := -1;
    Inc(hufpos);
  end;

// Place the nodes in the correct manner in the tree
  while (huftbl[2 * tblsize - 2].weight = -1) do
  begin
    hufpos := @huftbl[0];
    while hufpos[0].weight = 0 do Inc(hufpos);
    l1pos := hufpos;
    Inc(hufpos);
    while hufpos[0].weight = 0 do Inc(hufpos);
    if (hufpos[0].weight < l1pos[0].weight) then
    begin
      l2pos := l1pos;
      l1pos := hufpos;
      Inc(hufpos);
    end
    else
    begin
      l2pos := hufpos;
      Inc(hufpos);
    end;
    tempw := hufpos[0].weight;
    while (tempw <> -1) do
    begin
      if tempw <> 0 then
      begin
        if (tempw < l1pos[0].weight) then
        begin
          l2pos := l1pos;
          l1pos := hufpos;
        end
        else if (tempw < l2pos[0].weight) then l2pos := hufpos;
      end;
      Inc(hufpos);
      tempw := hufpos[0].weight;
    end;
    hufpos[0].weight := l1pos[0].weight + l2pos[0].weight;
    hufpos[0].child1 := @l1pos[0];
    hufpos[0].child2 := @l2pos[0];
    l1pos[0].weight := 0;
    l2pos[0].weight := 0;
  end;
  updhufcnt := maxhufcnt;
end;

{ TXSDecode }

//****************************************************************
// controleer de fileheader
//****************************************************************
function TXSDecode.chkheader: Integer;
var
  buf: string[4];
  temp: Byte;
  n: Integer;
begin
  Result := -1;
  FOrgName := '';
  buf :=  #0#0#0#0;
  if FStream.Read(buf[1], 4) <> 4 then Exit;

  if buf <> 'PCK'#8 then Exit;

  if FStream.Read(FOrgSize, 4) <> 4 then Exit;
  if FStream.Read(FPackedSize, 4) <> 4 then Exit;

  temp := 0;
  n := FStream.Read(temp, 1);
  while (n > 0) and (temp <> 0) do
  begin
    FOrgName := FOrgName + Chr(temp);
    n := FStream.Read(temp, 1);
  end;

  if n > 0 then Result := 0;
end;

constructor TXSDecode.Create(AStream: TStream);
begin
  FStream := AStream;
  SetLength(outbuf, outbufsize);
  SetLength(inbuf, inbufsize);

  inithufinfo; // initialize the cpdist tables
  inbufcnt := 0;       // er zit nog niks in de input buffer  */
  nrread   := 0;       // nog niks ingelezen                  */
  nrwrite  := 0;

  outbufcnt := 0;      // er zit nog niks in de output buffer */
  outbufpos := @outbuf[0]; // dadelijk vooraan in laden           */

  readbufcnt := 0;
  readbufpos := @outbuf[0];

  bitcnt := 0;         // nog geen bits gelezen               */
  if chkheader = -1 then raise Exception.Create('Invalid header in XSA file.');
  CodeLength := 0;
  FEOF := False;
end;

//****************************************************************/
//* Put the next character in the input buffer.                  */
//* Take care that minimal 'slwinsize' chars are before the      */
//* current position.                                            */
//****************************************************************/
procedure TXSDecode.charout(ch: Byte);
begin
  outbufpos[0] := ch;
  Inc(outbufpos);
  Inc(outbufcnt);
end;

//****************************************************************/
//* read string length						*/
//****************************************************************/
function TXSDecode.rdstrlen: Word;
var
  len: Byte;
  nrbits: Byte;
begin
  if bitin = 0 then
    Result := 2
  else if bitin = 0 then
    Result := 3
  else if bitin = 0 then
    Result := 4
  else
  begin
    nrbits := 2;
    while (nrbits < 7) and (bitin <> 0) do Inc(nrbits);

    len := 1;
    while nrbits <> 0 do
    begin
      Dec(nrbits);
      len := (len shl 1) or bitin;
    end;

    Result := len + 1;
  end;
end;

//***************************************************************/
// read string pos						*/
//***************************************************************/
function TXSDecode.rdstrpos: Word;
var
  nrbits: Byte;
  cpdindex: Byte;
  strpos: Word;
  hufpos: PHufNodes;
  strposmsb, strposlsb: Byte;
begin
  hufpos := @huftbl[0];
  Inc(hufpos, 2 * tblsize - 2);

  while Assigned(hufpos[0].child1) do
  begin
    if bitin <> 0 then
    begin
      hufpos := hufpos[0].child2;
    end
    else
    begin
      hufpos := hufpos[0].child1;
    end;
  end;

  cpdindex := (Cardinal(hufpos) - Cardinal(@huftbl[0])) div SizeOf(THufNode);
  Inc(tblsizes[cpdindex]);

  if cpdbmask[cpdindex] >= 256 then
  begin
    strposlsb := charin;  // lees LSB van strpos */
    strposmsb := 0;
    nrbits := cpdext[cpdindex] - 8;
    while nrbits <> 0 do
    begin
      Dec(nrbits);
      strposmsb := strposmsb shl 1;
      strposmsb := strposmsb or bitin;
    end;
    strpos := strposlsb or (strposmsb shl 8);
  end
  else
  begin
    strpos := 0;
    nrbits := cpdext[cpdindex];
    while (nrbits <> 0) do
    begin
      Dec(nrbits);
      strpos := strpos shl 1;
      strpos := strpos or bitin;
    end;
  end;
  if updhufcnt = 0 then
    mkhuftbl // make the huffman table */
  else Dec(updhufcnt);
  Result := strpos + cpdist[cpdindex];
end;

//****************************************************************/
//* read a bit from the input file				*/
//****************************************************************/
function TXSDecode.bitin: Byte;
var
  temp: Byte;
begin
  if bitcnt = 0 then
  begin
    bitflg := charin;   //* lees de bitflg in */
    bitcnt := 8;        //* nog 8 bits te verwerken */
  end;

  temp := bitflg and 1;
  Dec(bitcnt);  //* weer 1 bit verwerkt */
  bitflg := bitflg shr 1;

  Result := temp;
end;

//****************************************************************/
//* Get the next character from the input buffer.                */
//****************************************************************/
function TXSDecode.charin: Byte;
begin
  if inbufcnt = 0 then
  begin
    inbufpos := @inbuf[0];
    inbufcnt := FStream.Read(inbufpos[0], inbufsize);
    if (inbufcnt = 0) then
      raise Exception.Create('Unexpected end of file')
    else
    begin
      nrread := nrread + inbufcnt;
//      Write(Format(#13'nr bytes read: %6lu', [nrread]));
      Dec(inbufcnt);
    end;
  end
  else Dec(inbufcnt);
  Result := inbufpos[0];
  Inc(inbufpos);
end;

destructor TXSDecode.Destroy;
begin
  SetLength(inbuf, 0);
  SetLength(outbuf, 0);
  FStream.Free;
  inherited;
end;

function TXSDecode.Read(var AData: Byte): Longint;
var
  spos: Word;
begin
  Result := 0;
  if (readbufcnt >= outbufcnt - slwinsize) and (CodeLength <> maxstrlen + 1) then
  begin
    Move(readbufpos[0], outbuf[0], outbufcnt - readbufcnt);
    outbufcnt := outbufcnt - readbufcnt;
    outbufpos := @outbuf[0];
    Inc(outbufpos, outbufcnt);
    readbufpos := @outbuf[0];
    readbufcnt := 0;
    repeat
      case bitin of
        0: charout(charin);
        1:
        begin
          CodeLength := rdstrlen;
          if CodeLength <> (maxstrlen + 1) then
          begin
            spos := rdstrpos;
            while CodeLength > 0 do
            begin
              Dec(CodeLength);
              charout(outbufpos[-spos]);
            end;
            CodeLength := 0;
          end;
        end;
      end;
    until (CodeLength = maxstrlen + 1) or (outbufcnt > slwinsize);
  end;
  if readbufcnt < outbufcnt - slwinsize then
  begin
    AData := readbufpos[0];
    Inc(readbufpos);
    Inc(readbufcnt);
    Result := 1;
  end
  else
  begin
    if CodeLength = maxstrlen + 1 then
    begin
      if readbufcnt < outbufcnt then
      begin
        AData := readbufpos[0];
        Inc(readbufpos);
        Inc(readbufcnt);
        Result := 1;
      end
      else FEOF := True;
    end;
  end;
  Inc(nrwrite, Result);
end;

{ TXSEncode }

//***************************************************************/
// stuur een string code naar de uitvoer			*/
// opm: als strlen == maxstrlen+1, hoeft de strpos niet		*/
//      uitgevoerd te worden want dat is de EOF code		*/
//***************************************************************/
procedure TXSEncode.strout(str: PBytes; strlen: Byte; strpos: Word);
var
  mskflg, temp: Byte;
  masklong: Word;
  count: Byte;
begin
  if strlen = 1 then
    charout(str[0])
  else
  begin
    // 1 ==> is strcode: strlen, strpos */
    bitout(1);

    Dec(strlen);
    if strlen <= 3 then
    begin
      // org len bitcode */
      // 4       110     */
      // 3       10      */
      // 2       0       */
      Dec(strlen);
      while strlen <> 0 do
      begin
        bitout(1);
        Dec(strlen);
      end;
      bitout(0);
    end
    else
    begin
      // org len bitcode                */
      //   >4    111x..xy lencode       */
      // met x..xy = 1..10, z.d.d.      */
      //      #(1..1) = strlnrbits-2-1, */
      //     lencode = strlen-1 zonder  */
      //      het hoogste bit           */
      bitout(1);
      bitout(1);
      bitout(1);

      mskflg := findmsb(strlen);
      temp := mskflg shr 2;
      temp := temp shr 1;
      while temp <> 0 do
      begin
      	bitout(1);
        temp := temp shr 1;
      end;
      if strlen < 128 then bitout(0);
	// terminating 0 alleen bij 0..4 bits   */
	// 5 bits is per definitie 128 en hoger */

      // output de laagste bits van strlen      */
      mskflg := mskflg shr  1;
      while mskflg <> 0 do
      begin
      	bitout(strlen and mskflg);
        mskflg := mskflg shr  1;
      end;
    end;

    if strlen <> maxstrlen then
    begin
      // Zet nu nog de strpos achter de code aan. */
      // Gebruik een variabele lengte notatie     */
      // waarbij een kleine strpos minder bits    */
      // gebruikt dan een grote strpos.           */
      count := 0;
      while strpos >= cpdist[count + 1] do Inc(count);
      Inc(tblsizes[count]);
      masklong := 1 shl (hufcodtbl[count].nrbits - 1);
      while masklong <> 0 do
      begin
      	bitout(hufcodtbl[count].bitcode and masklong);
      	masklong := masklong shr 1;
      end;
      Dec(strpos, cpdist[count]);
      if cpdbmask[count] >= 256 then
      begin
      	byteout(strpos);
      	strpos := strpos shr 8;
      	temp := cpdbmask[count] shr 8;
      end
      else temp := cpdbmask[count];
      temp := temp shr 1;
      while temp <> 0 do
      begin
        bitout(strpos and temp);
        temp := temp shr 1;
      end;
    end;
    if updhufcnt = 0 then
    begin
      mkhuftbl; // make the huffman table */
      cnvtohuffcodes(@huftbl[2 * tblsize - 2], 0, 0); // conv into bitstream info */
    end
    else Dec(updhufcnt);
  end;
end;

procedure TXSEncode.Write(AData: Byte);
begin
  if FState = 0 then
  begin
    mwindow[lookahead_cnt] := AData;
    mwindow[slwinsize + lookahead_cnt] := AData;
    Inc(lookahead_cnt);
    if lookahead_cnt >= maxstrlen then
    begin
      init_index;    //* init the index for the first string */
      freepos := 1;
      winpos := 0;     // start at position 0           */
      strlen := 0;     // start with a string of size 0 */
      FState := 1;
    end;
  end;
  if FState > 0 then
  begin
    if FState > 1 then
    begin
      mwindow[(winpos + maxstrlen) and slwinmask] := AData; // store new char */
      if (winpos + maxstrlen) >= slwinsize then
        mwindow[winpos + maxstrlen] := AData;
      winpos := (winpos + 1) and slwinmask;
      if FState = 2 then
        strlen := AddString(winpos, strpos)
      else fAddString(winpos);
      if freepos < slwinsize then Inc(freepos);
    end;
    if (FState = 1) or (FState = 2) then
    begin
      if strlen <= 1 then
      begin
        charout(mwindow[winpos]);
        replace_cnt := 1;
      end
      else
      begin
        if strlen > lookahead_cnt then
          strlen := lookahead_cnt;   // strlen must be <= lookahead_cnt */
        replace_cnt := strlen;
        strout(@mwindow[strpos], replace_cnt, (winpos - strpos) and slwinmask);
      end;
    end;
    DeleteString((winpos + maxstrlen) and slwinmask);
    Dec(replace_cnt);
    if replace_cnt <> 0 then
      FState := 3
    else FState := 2;
  end;
  Inc(nrread);
end;

//****************************************************************/
//* stuur een karakter naar de uitvoer				*/
//****************************************************************/
procedure TXSEncode.charout(ch: Byte);
begin
  //* 0 ==> is een char: character */
  bitout(0);
  byteout(ch);
end;

//****************************************************************/
//* convert to huffman codes					*/
//****************************************************************/
procedure TXSEncode.cnvtohuffcodes(nodeptr: PHufNodes; bitcode: Word; nrbits: SmallInt);
var
  node: Word;
begin
  if not Assigned(nodeptr[0].child1) then
  begin
    node := (Cardinal(@nodeptr[0]) - Cardinal(@huftbl[0])) div SizeOf(THufNode);
    hufcodtbl[node].nrbits := nrbits;
    hufcodtbl[node].bitcode := bitcode;
  end
  else
  begin
    bitcode := bitcode shl 1;
    Inc(nrbits);
    cnvtohuffcodes(nodeptr[0].child1, bitcode, nrbits);
    cnvtohuffcodes(nodeptr[0].child2, 1 + bitcode, nrbits);
  end;
end;

constructor TXSEncode.Create(AStream: TStream; AOrgName: string);
var
  cnt: Word;
begin
  FStream := AStream;
  FOrgName := ExtractFileName(AOrgName);

  SetLength(mwindow, slwinsize + maxstrlen);
  SetLength(strbufptrs, 256);
  for cnt := 0 to 255 do
    SetLength(strbufptrs[cnt], nrindexptrs);
  SetLength(indexinfo, 256);

  eofflg := False;   //* end of file has not been reached yet */
  nrread   := 0;     //* er is nog niks ingelezen             */

  SetLength(bytebuf, obufsize);
  nrwritten := 0; // nothing written yet */
  bitflg := 0;    // reset the bitflag                            */
  bitflgcnt := 0; // bitflag can be placed in beginning of buffer */
  bytecnt := 1;   // first byte can be placed at addr 1           */
  setflg := 1;
  inithufinfo; // initialize the cpdist tables */
  cnvtohuffcodes(@huftbl[2 * tblsize - 2], 0, 0); // conv into bitstream info */

  mkheader(FOrgName);  // maak de header      */

  lookahead_cnt := 0;
  FState := 0;
end;

//****************************************************************/
//* vind het hoogste bit van een getal				*/
//****************************************************************/
function TXSEncode.findmsb(n: Byte): Byte;
var
  mskflg: Byte;
begin
  mskflg := 128;
  while ((n and mskflg) = 0) and (mskflg <> 0) do mskflg := mskflg shr 1;
  Result := mskflg;
end;

//****************************************************************/
//* get length of the bitstream file				*/
//****************************************************************/
function TXSEncode.bitglen: Cardinal;
begin
  Result := nrwritten;
end;

//****************************************************************/
//* go to a position in the bitstream file			*/
//****************************************************************/
procedure TXSEncode.bitspos(newpos: Cardinal);
begin
  FStream.Seek(newpos, soBeginning);
end;

//****************************************************************/
//* Send a byte to the output					*/
//****************************************************************/
procedure TXSEncode.byteout(n: Byte);
begin
  bytebuf[bytecnt] := n;
  Inc(bytecnt);
end;

//****************************************************************/
//* Send a bit to the file					*/
//****************************************************************/
procedure TXSEncode.bitout(n: Byte);
begin
  if setflg = 0 then
  begin
    if bytecnt >= (obufsize - 9) then
      flushout                          //* flush the bit and byte buffer        */
    else bytebuf[bitflgcnt] := bitflg;  //* store the bitflag */
    bitflgcnt := bytecnt;               //* bitflag may be placed at current pos */
    Inc(bytecnt);
    bitflg := 0;                        //* reset the bitflag                    */
    setflg := 1;
  end;
  if n <> 0 then bitflg := bitflg or setflg;
  setflg := setflg shl 1;
end;

//****************************************************************/
//* flush the byte & bitbuffer					*/
//****************************************************************/
procedure TXSEncode.flushout;
begin
  bytebuf[bitflgcnt] := bitflg; //* store the last bitflag */
  FStream.Write(bytebuf[0], bytecnt);
  Inc(nrwritten, bytecnt);
  bytecnt := 0;
end;

//****************************************************************/
//* stuur 1 byte naar de uitvoer file				*/
//****************************************************************/
procedure TXSEncode.fileout(n: Byte);
begin
  FStream.Write(n, 1);
  Inc(nrwritten);
end;

function TXSEncode.nrindexptrs: Word;
begin
  Result := 1 shl lognrindexptrs;
end;

function TXSEncode.indexpmsk: Word;
begin
  Result := nrindexptrs - 1;
end;

function TXSEncode.index(ch, offs: Byte): PPBytes;
begin
  Result := @(strbufptrs[ch][offs and indexpmsk]);
end;

function TXSEncode.offsindex(chindex: PPBytes; offs: Byte): PPBytes;
begin
  Result := @chindex[offs and indexpmsk];
end;

//****************************************************************/
//* Get length of the input file					*/
//****************************************************************/
function TXSEncode.inglen: Integer;
begin
  Result := nrread;
end;

//****************************************************************/
//* add a string to the index without searching for a best match	*/
//****************************************************************/
procedure TXSEncode.fAddString(winpos: Word); //* start pos of the string to add */
var
  indexinfoptr: PIndexInfos;
  winposptr: PBytes;
  ch: Byte;
begin
  winposptr := @mwindow[winpos];
  ch := winposptr[0];
  indexinfoptr := @indexinfo[ch];
  index(ch, indexinfoptr[0].firstptr + indexinfoptr[0].nrptrs)[0] := winposptr;
  Inc(indexinfoptr[0].nrptrs);
end;

//****************************************************************/
//* delete a string from the index information			*/
//****************************************************************/
procedure TXSEncode.DeleteString(strpos: Word);
var
  indexinfoptr: PIndexInfos;
begin
  if strpos >= freepos then
    Exit; //* this string is not in the moving window */

  indexinfoptr := @indexinfo[mwindow[strpos]];
  Dec(indexinfoptr[0].nrptrs);
  Inc(indexinfoptr[0].firstptr);
end;

destructor TXSEncode.Destroy;
var
  cnt: Word;
begin
  case FState of
    0:
    begin
      init_index;    //* init the index for the first string */
      freepos := 1;
      if lookahead_cnt <> 0 then
      begin
        winpos := 0;
        strlen := 0;
        FState := 1;
      end;
    end;
    1, 2, 3:
    begin
      if (FState = 1) or (FState = 3) then
      begin
        if FState = 3 then
        begin
          Dec(lookahead_cnt);  // one character less in the buffer */
          winpos := (winpos + 1) and slwinmask;
          fAddString(winpos);
          if freepos < slwinsize then	Inc(freepos);
        end;

        if FState = 1 then
        begin
          if strlen <= 1 then
          begin
            charout(mwindow[winpos]);
            replace_cnt := 1;
          end
          else
          begin
            if strlen > lookahead_cnt then
              strlen := lookahead_cnt;   // strlen must be <= lookahead_cnt */
            replace_cnt := strlen;
            strout(@mwindow[strpos], replace_cnt, (winpos - strpos) and slwinmask);
          end;
        end;

        Dec(replace_cnt);
        while replace_cnt <> 0 do
        begin
          DeleteString((winpos + maxstrlen) and slwinmask);
          Dec(lookahead_cnt);  // one character less in the buffer */
          winpos := (winpos + 1) and slwinmask;
          fAddString(winpos);
          if freepos < slwinsize then	Inc(freepos);
          Dec(replace_cnt);
        end;
        DeleteString((winpos + maxstrlen) and slwinmask);
      end;

      Dec(lookahead_cnt);
      while lookahead_cnt <> 0 do
      begin
        winpos := (winpos + 1) and slwinmask;
        strlen := AddString(winpos, strpos);
        if freepos < slwinsize then Inc(freepos);
        if strlen <= 1 then
        begin
          charout(mwindow[winpos]);
          replace_cnt := 1;
        end
        else
        begin
          if strlen > lookahead_cnt then
            strlen := lookahead_cnt;   // strlen must be <= lookahead_cnt */
          replace_cnt := strlen;
          strout(@mwindow[strpos], replace_cnt, (winpos - strpos) and slwinmask);
        end;
        Dec(replace_cnt);
        while replace_cnt <> 0 do
        begin
          DeleteString((winpos + maxstrlen) and slwinmask);
          Dec(lookahead_cnt);
          winpos := (winpos + 1) and slwinmask;
          fAddString(winpos);
          if freepos < slwinsize then	Inc(freepos);
          Dec(replace_cnt);
        end;
        DeleteString((winpos + maxstrlen) and slwinmask);
        Dec(lookahead_cnt);
      end;
    end;
  end;
  strout(@mwindow[0], maxstrlen + 1, 0); //* sluit file af */

  flushout;
  savelens(inglen, bitglen); // put the lens in the header */
  SetLength(bytebuf, 0);  //* free the bit & byte buffer  */

  SetLength(mwindow, 0);
  for cnt := 0 to 255 do SetLength(strbufptrs[cnt], 0);
  Setlength(strbufptrs, 0);
  SetLength(indexinfo, 0);

  FStream.Free;
  inherited;
end;

//***************************************************************/
// initialize the index information				*/
//***************************************************************/
procedure TXSEncode.init_index;
var
  n: Integer;
begin
  // reset all ptrs and counters */
  for n := 0 to Length(indexinfo) - 1 do
  begin
    indexinfo[n].firstptr := 0;
    indexinfo[n].nrptrs := 0;
  end;

  // init. index info for first string */
  Inc(indexinfo[mwindow[0]].nrptrs);
  index(mwindow[0], 0)[0] := @mwindow[0];
end;

//***************************************************************/
// Add a string to the index, search for largest match		*/
// Note: on similar match lengths, the smallest stringpos will	*/
//       be choosen to obtain short string codes		*/
//***************************************************************/
function TXSEncode.AddString(winpos: Word; var strposptr: Word): Word;
// start pos of the string to add */
// ptr to the variable holding the strpos on match */
var
  indexcnt: Word;         // index die nu bekeken wordt */
  match_length: Word;
  firstptr: Word;
  count: Word;
  str1, str2: PBytes;
  winposptr: PBytes;
  chindex: PPBytes;
  indexinfoptr: PIndexInfos;
begin
  winposptr := @mwindow[winpos];
  indexinfoptr := @indexinfo[winposptr[0]];
  firstptr := indexinfoptr[0].firstptr;
  chindex := @strbufptrs[winposptr[0]][0];
  match_length := 0;
  indexcnt := indexinfoptr[0].nrptrs;
  if indexcnt > nrindexptrs then indexcnt := nrindexptrs;
  while indexcnt <> 0 do
  begin
    Dec(indexcnt);
    str1 := winposptr;
    str2 := offsindex(@chindex[0], firstptr + indexcnt)[0];
    count := maxstrlen - 1;
    Inc(str1);
    Inc(str2);
    Dec(count);
    while (str1[0] = str2[0]) and (count <> 0) do
    begin
      Inc(str1);
      Inc(str2);
      Dec(count);
    end;
    Inc(Count);
    if (maxstrlen - count) > match_length then
    begin
      // Found a longer match */
      match_length := maxstrlen - count;
      strposptr := Cardinal(@str2[0]) - Cardinal(@mwindow[0]) - match_length;
      if count = 0 then
      begin
    	  Inc(strposptr);
      	Break; // Found an exact match. No further search needed */
      end;
    end;
  end;
  offsindex(@chindex[0], firstptr + indexinfoptr[0].nrptrs)[0] := winposptr;
  Inc(indexinfoptr[0].nrptrs);
  Result := match_length;
end;

//****************************************************************/
//* maak de header aan						*/
//****************************************************************/
procedure TXSEncode.mkheader(name: string);
var
  temp: Word;
begin
  fileout(Ord('P'));
  fileout(Ord('C'));
  fileout(Ord('K'));
  fileout(8);  //* LZ77 met codering 8 */

  //* reserve space for org and dest length */
  for temp := 0 to 7 do fileout(0);

  for temp := 1 to Length(name) do fileout(Ord(name[temp]));
  fileout(0);
end;

//****************************************************************/
//* put the org len and the dest len in the header		*/
//****************************************************************/
procedure TXSEncode.savelens(orglen, destlen: Cardinal);
var
  cnt: Word;
begin
  bitspos(4);
  for cnt := 0 to 3 do
  begin
    fileout(orglen);
    orglen := orglen shr 8;
  end;
  for cnt := 0 to 3 do
  begin
    fileout(destlen);
    destlen := destlen shr 8;
  end;
end;

{ TXSStream }

constructor TXSStream.Create(const AStreamToDecode: TStream);
begin
  FWrited := False;
  FEncoder := nil;
  FDecoder := TXSDecode.Create(AStreamToDecode);
end;

constructor TXSStream.Create(const AStreamToEncode: TStream; AOriginalName: string);
begin
  FWrited := True;
  FDecoder := nil;
  FEncoder := TXSEncode.Create(AStreamToEncode, AOriginalname);
end;

destructor TXSStream.Destroy;
begin
  if FWrited then
    FEncoder.Free
  else FDecoder.Free;
  inherited;
end;

function TXSStream.GetOrgName: string;
begin
  if FWrited then
    Result := FEncoder.FOrgName
  else Result := FDecoder.FOrgName;
end;

function TXSStream.GetSize: Int64;
begin
  if FWrited then raise Exception.Create('Can not get size on XSA encoder mode.');
  Result := FDecoder.FOrgSize;
end;

function TXSStream.Read(var Buffer; Count: Integer): Longint;
var
  bufpos: PBytes;
begin
  if FWrited then raise Exception.Create('Can not read data on XSA encoder mode.');
  Result := 0;
  bufpos := @Buffer;
  while (Count > 0) and not FDecoder.FEOF do
  begin
    Inc(Result, FDecoder.Read(bufpos[0]));
    Inc(bufpos);
    Dec(Count);
  end;
end;

function TXSStream.Seek(const Offset: Int64; Origin: TSeekOrigin): Int64;
var
  n: Int64;
  ch: Byte;
begin
  if FWrited then raise Exception.Create('Can not seek offset on XSA encoder mode.');
  case Origin of
    soEnd: n := FDecoder.OrgSize - Offset;
    soBeginning: n := Offset - FDecoder.nrwrite;
  else
    n := Offset;
  end;
  while (n > 0) and not FDecoder.FEOF do
  begin
    FDecoder.Read(ch);
    Dec(n);
  end;
  Result := FDecoder.nrwrite;
end;

procedure TXSStream.SetSize(NewSize: Integer);
begin
  raise Exception.Create('Can not set size on XSA file format.');
end;

procedure TXSStream.SetSize(const NewSize: Int64);
begin
  raise Exception.Create('Can not set size on XSA file format.');
end;

function TXSStream.Write(const Buffer; Count: Integer): Longint;
var
  bufpos: PBytes;
begin
  if not FWrited then raise Exception.Create('Can not write data on XSA decoder mode.');
  Result := 0;
  bufpos := @Buffer;
  while Result < Count do
  begin
    FEncoder.Write(bufpos[0]);
    Inc(bufpos);
    Inc(Result);
  end;
end;

end.
