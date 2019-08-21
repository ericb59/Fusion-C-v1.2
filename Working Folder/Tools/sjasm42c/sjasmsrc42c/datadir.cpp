/*

  Sjasm Z80 Assembler version 0.42

  Copyright 2011 Sjoerd Mastijn

  This software is provided 'as-is', without any express or implied warranty.
  In no event will the authors be held liable for any damages arising from the
  use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it freely,
  subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not claim
     that you wrote the original software. If you use this software in a product,
     an acknowledgment in the product documentation would be appreciated but is
     not required.

  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.

  3. This notice may not be removed or altered from any source distribution.

*/

// datadir.cpp

#include "sjasm.h"

byte map[256];
byte smap[256];

void getbytes(string &line, Data &e, bool madd, bool usemap, bool canval, bool dc, bool dz) {
  int val,add=0,len=0,glabelnotfound=0;
  string ol;
  unsigned char *m=smap;
  if (usemap) m=map;
  if (madd) {
    if (ParseExpression(line,add)) check8(add);
    else { error("Offset expected",ERRREP); add=0; }
    glabelnotfound+=labelnotfound;
  }
  for(;;) {
    skipblanks(line);
    if (line.empty()) { error("Expression expected"); break; }
    if (line[0]=='\'') {
      ol=line; len=0;
      for(;;) {
        line.erase(0,1);
        if (line.empty()) { error("Unexpected end of string"); break; }
        if (line[0]=='\'') { line.erase(0,1); break; }
        e.push(m[(line[0]+add)&255]); ++len;
      }
      if (len==1 && canval) {
        if (ParseExpression(ol,val)) { check8(val); e[e.size()-1]=m[(val+add)&255]; }
        else { error("Syntax error",ERRREP); }
        glabelnotfound+=labelnotfound;
        line=ol;
      }
    } else if (line[0]=='"') {
      len=0; ol=line;
      line.erase(0,1);
      do {
        if (line.empty() || line[0]=='"') { error("Unexpected end of string"); break; }
        getcharconstchar(line,val); check8(val); e.push(m[(val+add)&255]); ++len;
      } while (line[0]!='"');
      line.erase(0,1);
      if (len==1 && canval) {
        if (ParseExpression(ol,val)) { check8(val); e[e.size()-1]=m[(val+add)&255]; }
        else { error("Syntax error",ERRREP); }
        glabelnotfound+=labelnotfound;
        line=ol;
      }
    } else {
      if (!canval) error("String expected");
      if (ParseExpression(line,val)) { check8(val); e.push(m[(val+add)&255]); }
      else { error("Syntax error",ERRREP); }
      glabelnotfound+=labelnotfound;
    }
    if (dz) e.push(0);
    if (dc)
      if (!len) error("Empty string not allowed");
      else  e[e.size()-1]=e[e.size()-1]|128;
    if (!comma(line)) break;
  }
  labelnotfound=glabelnotfound;
  checkjunk(line);
}

void piDB(string line,Data &e) {
  getbytes(line,e,false,false,true,false,false);
}

void piDC(string line,Data &e) {
  getbytes(line,e,false,false,false,true,false);
}

void piDD(string line,Data &e) {
  int val;
  do {
    if (ParseExpression(line,val)) {
      for (int i=0;i!=4;++i) { e.push((byte)val); val>>=8; }
    } else { error("Syntax error"); return; }
  } while (comma(line));
  checkjunk(line);
}

void piDT(string line,Data &e) {
  int val;
  do {
    if (ParseExpression(line,val)) {
      check24(val); for (int i=0;i!=3;++i) { e.push((byte)val); val>>=8; }
    } else { error("Syntax error"); return; }
  } while (comma(line));
  checkjunk(line);
}

void piDW(string line,Data &e) {
  int val;
  do {
    if (ParseExpression(line,val)) {
      check16(val); e.push((byte)val); e.push((byte)(val>>8));
    } else { error("Syntax error"); return; }
  } while (comma(line));
  checkjunk(line);
}

void piDZ(string line,Data &e) {
  getbytes(line,e,false,false,false,false,true);
}

void piABYTE(string line,Data &e) {
  getbytes(line,e,true,false,true,false,false);
}

void piABYTEC(string line,Data &e) {
  getbytes(line,e,true,false,false,true,false);
}

void piABYTEZ(string line,Data &e) {
  getbytes(line,e,true,false,false,false,true);
}

void piASC(string line,Data &e) {
  getbytes(line,e,false,true,true,false,false);
}

void piASCC(string line,Data &e) {
  getbytes(line,e,false,true,false,true,false);
}

void piASCZ(string line,Data &e) {
  getbytes(line,e,false,true,false,false,true);
}

void piASCMAP(string line, Data &) {
  int first,last;
  if (!ParseExpression(line,first)) { error("Character(range) expected"); return; }
  if (need(line,"..")) {
    if (!ParseExpression(line,last)) { error("Character(range) expected"); return; }
  } else last=first;
  if (first!=last) check8u(last); check8u(first);
  if (first>last) { error("Illegal range");  }
  ++last;
  if (!needcomma(line)) return;
  if (need(line,"=>")) {
    int a,b;
    if (!ParseExpression(line,b)) { error("Base expected"); return; }
    if (comma(line)) {
      if (!ParseExpression(line,a)) { error("Increment expected"); return; }
    } else a=1;
    for (int i=first; i!=last; ++i) { check8(b); map[i]=(byte)b; b+=a; }
  } else {
    int oadres=adres;
    for (int i=first; i!=last; ++i) {
      string l=line;
      int e;
      adres=i; // terrible hack :)
      if (!ParseExpression(l,e)) break;
      check8(e);  map[i]=(byte)e;
    }
    adres=oadres;
  }
  again=1; //checkjunk(line);
}

void piASCMAPRESET(string line, Data &) {
  for (int i=0; i!=256; ++i) map[i]=(byte)i;
  again=1; checkjunk(line);
}

void piASCMAPCLEAR(string line, Data &) {
  for (unsigned i=0; i!=256; ++i) map[i]=0;
  again=1; checkjunk(line);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdDB(string &line, RawSource *rs) {
  rs->GetSource().push_back(new Mnemonic(line,piDB));
  rs->GetSource().back()->Process();
}

void pdDC(string &line, RawSource *rs) {
  rs->GetSource().push_back(new Mnemonic(line,piDC));
  rs->GetSource().back()->Process();
}

void pdDD(string &line, RawSource *rs) {
  rs->GetSource().push_back(new Mnemonic(line,piDD));
  rs->GetSource().back()->Process();
}

void pdDS(string &line, RawSource *rs) {
  rs->GetSource().push_back(new piDS(line));
  rs->GetSource().back()->Process();
}

void pdDT(string &line, RawSource *rs) {
  rs->GetSource().push_back(new Mnemonic(line,piDT));
  rs->GetSource().back()->Process();
}

void pdDW(string &line, RawSource *rs) {
  rs->GetSource().push_back(new Mnemonic(line,piDW));
  rs->GetSource().back()->Process();
}

void pdDZ(string &line, RawSource *rs) {
  rs->GetSource().push_back(new Mnemonic(line,piDZ));
  rs->GetSource().back()->Process();
}

void pdABYTE(string &line, RawSource *rs) {
  rs->GetSource().push_back(new Mnemonic(line,piABYTE));
  rs->GetSource().back()->Process();
}

void pdABYTEC(string &line, RawSource *rs) {
  rs->GetSource().push_back(new Mnemonic(line,piABYTEC));
  rs->GetSource().back()->Process();
}

void pdABYTEZ(string &line, RawSource *rs) {
  rs->GetSource().push_back(new Mnemonic(line,piABYTEZ));
  rs->GetSource().back()->Process();
}

void pdASC(string &line, RawSource *rs) {
  rs->GetSource().push_back(new Mnemonic(line,piASC));
  rs->GetSource().back()->Process();
}

void pdASCC(string &line, RawSource *rs) {
  rs->GetSource().push_back(new Mnemonic(line,piASCC));
  rs->GetSource().back()->Process();
}

void pdASCZ(string &line, RawSource *rs) {
  rs->GetSource().push_back(new Mnemonic(line,piASCZ));
  rs->GetSource().back()->Process();
}

void pdASCMAP(string &line, RawSource *rs) {
  rs->GetSource().push_back(new Mnemonic(line,piASCMAP));
  rs->GetSource().back()->Process();
}

void pdASCMAPRESET(string &line, RawSource *rs) {
  rs->GetSource().push_back(new Mnemonic(line,piASCMAPRESET));
  rs->GetSource().back()->Process();
}

void pdASCMAPCLEAR(string &line, RawSource *rs) {
  rs->GetSource().push_back(new Mnemonic(line,piASCMAPCLEAR));
  rs->GetSource().back()->Process();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FunctionTable<pFun,RawSource> datafuntab;

void initPidata() {
  FunctionEntry<pFun> funs[] = {
    "byte",pdDB,
    "word",pdDW,
    "dword",pdDD,
    "db",pdDB,
    "dc",pdDC,
    "dd",pdDD,
    "ds",pdDS,
    "dt",pdDT,
    "dw",pdDW,
    "dz",pdDZ,
    "abyte",pdABYTE,
    "abytec",pdABYTEC,
    "abytez",pdABYTEZ,
    "block",pdDS,
    "asc",pdASC,
    "ascc",pdASCC,
    "ascz",pdASCZ,
    "ascmap",pdASCMAP,
    "ascmap.reset",pdASCMAPRESET,
    "ascmap.clear",pdASCMAPCLEAR,
    "defb",pdDB,
    "defw",pdDW,
    "defs",pdDS,
    "defd",pdDD,
    "defm",pdDB
  };
  datafuntab.init(funs, sizeof funs/sizeof funs[0]);
  for (int i=0; i!=256; ++i) smap[i]=map[i]=(byte)i;
}

//eof
