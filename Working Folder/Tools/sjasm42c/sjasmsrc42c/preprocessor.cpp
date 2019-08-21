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

// preprocessor.cpp

#include "sjasm.h"

DefineTable deftab;
DefineArgTable defargtab;
MacNumTable macnumtab;
MacroTable mactab;
StructTable structtab;

Stop stop;

stack<ifinfo> ifstack;

FunctionTable<pFun,RawSource> group1iftab;
FunctionTable<pFun,RawSource> group2iftab;
FunctionTable<pFun,RawSource> group1tab;
FunctionTable<pFun,RawSource> group2tab;

StringList macronl,macroel;
StringList whilenl,whileel;
StringList repeatnl,repeatel;
StringList structurenl,structureel;

string getbuildindefine(string d, string &) {
  if (d[d.size()-1]!='_') return "";
  if (d=="__LINE__") return tostr(curlin);
  if (d=="__MODULE__") if (modlabp.empty()) return " "; else return modlabp;
  if (d=="__VERSION__") return VERSION;
  if (d=="__FILE__") return listopt._filename;
  if (d=="__OUTPUT__") return destfilename;
  if (d=="__DATE__") return startdate;
  if (d=="__TIME__") return starttime;
  if (d.substr(0,5)=="__ARG" && d.substr(6)=="__") {
    unsigned char a=*d.substr(5,1).c_str()-'0';
    if (a<10 && a) return cmdparameter[a];
  }
  return "";
}

string getmacrofun(string id, string &s) {
  string ns=id+s;
  if (mactab.emit(ns,0,true)) { s=ns; return stop._retval; }
  return "";
}

Find frepdef("qwertyuioplkjhgfdsazxcvbnmQWERTYUIOPLKJHGFDSAZXCVBNM_'\"@");
Find fmacfun("qwertyuioplkjhgfdsazxcvbnmQWERTYUIOPLKJHGFDSAZXCVBNM_");

string ReplaceDefines(string s) {
  string nid,nr,res("");
  int pos;
  for(;;) {
    pos=frepdef.find(s);
    if (pos) res+=s.substr(0,pos);
    if (pos==string::npos) break;
    s=s.substr(pos);
    if (s[0]=='_' || isalpha(s[0])) {
      nid=getid(s);
      nr=defargtab.getrepl(nid);
      if (nr.empty()) nr=deftab.getrepl(nid,s);
      if (nr.empty() && nid[0]=='_') nr=getbuildindefine(nid,s);
      if (nr.empty()) nr=nid;
      res+=nr;
    } else if (s[0]=='\'' || s[0]=='"') {
      pos=getstringlength(s);
      res+=s.substr(0,pos); s=s.substr(pos);
    } else {
      if (s[0]!='@') error1("ReplaceDefines",ERRINTERNAL);
      int skip=-1;
      while (cneed(s,'@')) ++skip;
      if (s[0]=='\'' || s[0]=='"') {
        pos=getstringlength(s);
        res+=s[0]+ReplaceDefines(s.substr(1,pos-2))+s[pos-1];
        s=s.substr(pos);
      } else if (isdigit(s[0])) {
        int max,num,rot;
        if (!getConstant(s,num)) error1("ReplaceDefines",ERRINTERNAL);
        nr=macnumtab.getrepl(0,skip);
        if (nr.empty()) { error1("Use of @num not allowed outside macro"); continue; }
        if (num) {
          if (!getConstant(nr,max)) error1("ReplaceDefines",ERRINTERNAL);
          if (num>max) nr="";
          else {
            nr=macnumtab.getrepl(ROTATE,skip);
            if (nr.empty()) error1("ReplaceDefines",ERRINTERNAL);
            if (!getConstant(nr,rot)) error1("ReplaceDefines",ERRINTERNAL);
            num-=1; num=(num+rot)%max; num+=1;
            nr=macnumtab.getrepl(num,skip);
            if (nr.empty()) error1("ReplaceDefines",ERRINTERNAL);
          }
        }
        res+=nr;
      } else if (cneed(s,'#')) {
        nr=macnumtab.getrepl(ITERATION,skip);
        if (nr.empty()) { nr="0"; error1("Use of @# not allowed outside loops"); }
        res+=nr;
      } else if (cneed(s,'*')) {
        res+=tostr(unieknummer++);
      } else {
        ++skip; while (skip--) res+='@';
      }
    }
  }

  s=res;
  res.clear();
  nr.clear();
  for(;;) {
    pos=fmacfun.find(s);
    if (pos) res+=s.substr(0,pos);
    if (pos==string::npos) break;
    s=s.substr(pos);
    nid=getid(s);
    if (s[0]=='(') nr=getmacrofun(nid,s);
    if (nr.empty()) res+=nid;
    else { res+=nr; nr.clear(); }
  }

  string r;
  for(;;) {
    pos=(int)res.find_first_of("'\">");
    if (pos) r+=res.substr(0,pos);
    if (pos==string::npos) break;
    res=res.substr(pos);
    if (res[0]=='\'' || res[0]=='"') {
      pos=getstringlength(res);
      r+=res.substr(0,pos); res=res.substr(pos);
    } else if (res[0]=='>' && res[1]=='<') {
      pos=(int)r.find_last_not_of(' ');
      if (pos!=string::npos) r.erase(pos+1);
      res=res.substr(2); skipblanks(res);
    } else {
      r+='>'; res.erase(0,1);
    }
  }
  return r;
}

int pregetdef(string &line,string &n, string &r, StringList &a) {
  a.clear();
  if ((n=getid(line)).empty()) { error1("Illegal define name"); return 0; }
  if (line[0]=='(') {
    line.erase(0,1);
    while(1) {
      a.push_back(getid(line));
      if (sbcneed(line,')')) break;
      if (!needcomma(line)) break;
    }
  }
  skipblanks(line);
  r=line; line.clear();
  return 1;
}

void pushif(bool c) {
  ifinfo ii;
  ii._curlin=curlin; ii._listcurlin=listcurlin;
  ii._filename=listopt._filename;
  ii._else=false;
  if (ifstack.empty()) {
    ii._active=c; ii._level=1;  ii._nest=1;
  } else { 
    ii._nest=ifstack.top()._nest+1; ii._level=ifstack.top()._level;
    if (ifstack.top()._active) { 
      ii._active=c;  ++ii._level;
    } else {
      ii._active=false;
    }
  }
  ii._beenactive=ii._active;
  ifstack.push(ii);
}

void elseif(bool c) {
  if (ifstack.empty()) { error1("ELSEIF without IF"); return; }
  if (ifstack.top()._else) { error1("ELSEIF within ELSE"); return; }
  if (ifstack.top()._beenactive) ifstack.top()._active=false;
  else ifstack.top()._active=ifstack.top()._beenactive=c;
}

void preELSE(string &, RawSource *) {
  if (ifstack.empty()) { error1("ELSE without IF"); return; }
  if (ifstack.top()._else) { error1("ELSE within ELSE"); return; }
  ifstack.top()._else=true;
  if (ifstack.top()._level==ifstack.top()._nest) ifstack.top()._active=!ifstack.top()._beenactive;
}

void preENDIF(string &, RawSource *) {
  if (ifstack.empty()) {
    error1("ENDIF without IF");
    return;
  }
  ifstack.pop();
}

void ifIF(string &line, void(*push)(bool)) {
  int val;
  if (!ParseExpression(line,val)) { error1("Expression expected"); return; }
  if (labelnotfound) error1("Forward reference");
  (*push)(val!=0);
}

void ifIFDEF(string &line, void(*push)(bool)) {
  string d=getid(line);
  string p=defargtab.getrepl(d);
  if (!p.empty()) d=p;
  (*push)(deftab.exists(d,-1)!=0);
}

void ifIFNDEF(string &line, void(*push)(bool)) {
  string d=getid(line);
  string p=defargtab.getrepl(d);
  if (!p.empty()) d=p;
  (*push)(deftab.exists(d,-1)==0);
}

void ifIFIDN(string &line, void(*push)(bool)) {
  string een,twee;
  een=trim(getargument(line)); needcomma(line); twee=trim(getargument(line));
  (*push)(een==twee);
}

void ifIFIDNI(string &line, void(*push)(bool)) {
  string een,twee;
  een=tolower(trim(getargument(line))); needcomma(line); twee=tolower(trim(getargument(line)));
  (*push)(een==twee);
}

void ifIFDIF(string &line, void(*push)(bool)) {
  string een,twee;
  een=trim(getargument(line)); needcomma(line); twee=trim(getargument(line));
  (*push)(een!=twee);
}

void ifIFDIFI(string &line, void(*push)(bool)) {
  string een,twee;
  een=tolower(trim(getargument(line))); needcomma(line); twee=tolower(trim(getargument(line)));
  (*push)(een!=twee);
}

void ifIFB(string &line, void(*push)(bool)) {
  skipblanks(line); (*push)(line.empty()); line.clear();
}

void ifIFID(string &line, void(*push)(bool)) {
  string d=getid(line); (*push)(!d.empty()); if (d.empty()) line.clear();
}

void ifIFSTR(string &line, void(*push)(bool)) {
  skipblanks(line);
  if (line[0]=='"' || line[0]=='\'') {
    string e; getstring(line,e);
    (*push)(true);
  } else {
    line.clear();
    (*push)(false);
  }
}

void ifIFNUM(string &line, void(*push)(bool)) {
  skipblanks(line); 
  if (isdigit(line[0]) || (line[0]=='#' && isalnum(line[1])) || (line[0]=='$' && isalnum(line[1])) || line[0]=='%' && isdigit(line[1])) { 
    int nval; getConstant(line,nval);
    (*push)(true);
  } else {
    line.clear();
    (*push)(false);
  }
}

void ifIFNB(string &line, void(*push)(bool)) {
  skipblanks(line); (*push)(!line.empty()); line.clear();
}

void ifIFNID(string &line, void(*push)(bool)) {
  string d=getid(line); (*push)(d.empty()); line.clear();
}

void ifIFNSTR(string &line, void(*push)(bool)) {
  skipblanks(line);
   (*push)(!(line[0]=='"' || line[0]=='\''));
  line.clear();
  }
void ifIFNNUM(string &line, void(*push)(bool)) {
  skipblanks(line);
  (*push)(!(isdigit(line[0]) || (line[0]=='#' && isalnum(line[1])) || (line[0]=='$' && isalnum(line[1])) || line[0]=='%' && isdigit(line[1])));
  line.clear();
}

void ifIFEXISTS(string &line, void(*push)(bool)) {
  string f=getfilename(line);
  f=fileexists(listopt._path,f);
  (*push)(!f.empty());
}

void ifIFNEXISTS(string &line, void(*push)(bool)) {
  string f=getfilename(line);
  f=fileexists(listopt._path,f);
  (*push)(f.empty());
}

void ifIFIN(string &line, void(*push)(bool)) {
  bool c=false;
  string een;
  een=trim(getargument(line));
  while (1) {
    if (!comma(line)) break;
    if (een==trim(getargument(line))) c=true;
  }
  (*push)(c);
}

void ifIFINI(string &line, void(*push)(bool)) {
  bool c=false;
  string een;
  een=tolower(trim(getargument(line)));
  while (1) {
    if (!comma(line)) break;
    if (een==tolower(trim(getargument(line)))) c=true;
  }
  (*push)(c);
}

void preIF(string &line, RawSource *) { ifIF(line,pushif); }
void preIFDEF(string &line, RawSource *) { ifIFDEF(line,pushif); }
void preIFNDEF(string &line, RawSource *) { ifIFNDEF(line,pushif); }
void preIFIDN(string &line, RawSource *) { ifIFIDN(line,pushif); }
void preIFIDNI(string &line, RawSource *) { ifIFIDNI(line,pushif); }
void preIFDIF(string &line, RawSource *) { ifIFDIF(line,pushif); }
void preIFDIFI(string &line, RawSource *) { ifIFDIFI(line,pushif); }
void preIFB(string &line, RawSource *) { ifIFB(line,pushif); }
void preIFID(string &line, RawSource *) { ifIFID(line,pushif); }
void preIFSTR(string &line, RawSource *) { ifIFSTR(line,pushif); }
void preIFNUM(string &line, RawSource *) { ifIFNUM(line,pushif); }
void preIFNB(string &line, RawSource *) { ifIFNB(line,pushif); }
void preIFNID(string &line, RawSource *) { ifIFNID(line,pushif); }
void preIFNSTR(string &line, RawSource *) { ifIFNSTR(line,pushif); }
void preIFNNUM(string &line, RawSource *) { ifIFNNUM(line,pushif); }
void preIFEXISTS(string &line, RawSource *) { ifIFEXISTS(line,pushif); }
void preIFNEXISTS(string &line, RawSource *) { ifIFNEXISTS(line,pushif); }
void preIFIN(string &line, RawSource *) { ifIFIN(line,pushif); }
void preIFINI(string &line, RawSource *) { ifIFINI(line,pushif); }

void preELSEIF(string &line, RawSource *) { ifIF(line,elseif); }
void preELSEIFDEF(string &line, RawSource *) { ifIFDEF(line,elseif); }
void preELSEIFNDEF(string &line, RawSource *) { ifIFNDEF(line,elseif); }
void preELSEIFIDN(string &line, RawSource *) { ifIFIDN(line,elseif); }
void preELSEIFIDNI(string &line, RawSource *) { ifIFIDNI(line,elseif); }
void preELSEIFDIF(string &line, RawSource *) { ifIFDIF(line,elseif); }
void preELSEIFDIFI(string &line, RawSource *) { ifIFDIFI(line,elseif); }
void preELSEIFB(string &line, RawSource *) { ifIFB(line,elseif); }
void preELSEIFID(string &line, RawSource *) { ifIFID(line,elseif); }
void preELSEIFSTR(string &line, RawSource *) { ifIFSTR(line,elseif); }
void preELSEIFNUM(string &line, RawSource *) { ifIFNUM(line,elseif); }
void preELSEIFNB(string &line, RawSource *) { ifIFNB(line,elseif); }
void preELSEIFNID(string &line, RawSource *) { ifIFNID(line,elseif); }
void preELSEIFNSTR(string &line, RawSource *) { ifIFNSTR(line,elseif); }
void preELSEIFNNUM(string &line, RawSource *) { ifIFNNUM(line,elseif); }
void preELSEIFEXISTS(string &line, RawSource *) { ifIFEXISTS(line,elseif); }
void preELSEIFNEXISTS(string &line, RawSource *) { ifIFNEXISTS(line,elseif); }
void preELSEIFIN(string &line, RawSource *) { ifIFIN(line,elseif); }
void preELSEIFINI(string &line, RawSource *) { ifIFINI(line,elseif); }

void preDEFINE(string &line, RawSource *) {
  string n,r;
  StringList a;
  if (pregetdef(line,n,r,a)) deftab.add(n,r,false,a);
}

void preIDEFINE(string &line, RawSource *) {
  string n,r;
  StringList a;
  if (pregetdef(line,n,r,a)) deftab.add(n,r,true,a);
}

void preXDEFINE(string &line, RawSource *) {
  string n,r;
  StringList a;
  if (pregetdef(line,n,r,a)) deftab.add(n,ReplaceDefines(r),false,a);
}

void preXIDEFINE(string &line, RawSource *) {
  string n,r;
  StringList a;
  if (pregetdef(line,n,r,a)) deftab.add(n,ReplaceDefines(r),true,a);
}

void preUNDEF(string &line, RawSource *) {
  string d=getid(line);
  if (!d.empty()) deftab.undef(d);
  else error("Operand expected");
}

void preASSIGN(string &line, RawSource *) {
  string d=getid(line);
  string p=ReplaceDefines(line);
  line.clear();
  int val;
  if (!ParseExpression(p,val)) error1("Expression expected");
  if (labelnotfound) error1("Forward reference");
  checkjunk(p);
  deftab.add(d,tostr(val));
}

void preTOSTR(string &line, RawSource *) {
  string d=getid(line);
  skipblanks(line);
  string l=ReplaceDefines(line);
  makestring(l);
  line.clear();
  deftab.add(d,'"'+l+'"');
}

void preSTRLEN(string &line, RawSource *) {
  string e;
  string d=getid(line);
  skipblanks(line);
  string p=ReplaceDefines(line);
  line.clear();
  getstring(p,e);
  checkjunk(p);
  deftab.add(d,tostr(e.size()));
}

void preSUBSTR(string &line, RawSource *) {
  string e;
  int pos=1,len=BIGVALUE;
  string r,s,d=getid(line);
  skipblanks(line);
  line=ReplaceDefines(line);
  getstring(line,e);
  if(comma(line)) {
    if(!ParseExpression(line,pos)) {
      error("Syntax error"); pos=1;
    }
    if(comma(line)) {
      if(!ParseExpression(line,len)) {
        error("Syntax error"); len=BIGVALUE;
      }
    }
  }
  checkjunk(line);
  if (pos<1 || pos>(int)e.size()) r="";
  else r=e.substr(pos-1,len);
  makestring(r);
  deftab.add(d,'"'+r+'"');
}

void preINCLUDE(string &line, RawSource *rs) {
  string p;
  RawSource i(getfilename(line));
  rs->AddToSource(i.GetSource());
  checkjunk(line);
}

int pregetmacro(string &line, bool icase, RawSource *rs, bool list, bool recursive) {
  Macro mac;
  mac._list=list;
  mac._recursive=recursive;
  string d,name=getid(line);
  if (name.empty()) { error1("Illegal macroname"); return 0; }
  while('>') {
    skipblanks(line);
    if (!line[0]) break;
    if (isalpha(line[0])) {
      d=getid(line);
      mac._namParam.push_back(d);
      if (sbcneed(line,'+')) mac._greedy=true;
      if (sbcneed(line,':')) {
        d=getargument(line,mac._greedy);
      } else d="\01";
      mac._namDefaults.push_back(d);
      if (mac._greedy) break;
      if (comma(line)) continue; break;
    }
    if (isdigit(line[0])) {
      if (!getConstant(line,mac._minnum)) { error1("constant expected"); return 0; }
      if (need(line,"..")) {
        skipblanks(line);
        if (sbcneed(line,'*'))
          mac._maxnum=BIGVALUE;
        else {
          if (!getConstant(line,mac._maxnum)) { error1("constant expected"); return 0; }
        }
      } else mac._maxnum=mac._minnum;
      if (mac._minnum<0 || mac._maxnum<mac._minnum) { error1("Illegal parameter"); return 0; }
      if (sbcneed(line,'+'))
        if (mac._maxnum<BIGVALUE) mac._greedy=true;
        else error1("No greedy stars allowed!");
      break;
    }
    if (sbcneed(line,'*')) {
      mac._minnum=0;
      mac._maxnum=BIGVALUE;
      if (sbcneed(line,'+')) error1("No greedy stars allowed!");
      break;
    }
    error1("Syntax error");
    break;
  }
  if (mac._maxnum>mac._minnum && line[0]) {
    int i=mac._maxnum-mac._minnum;
    bool gr=false;
    while (i--) {
      if (!line[0]) break;
      if (!i) gr=mac._greedy;
      d=getargument(line,gr);
      mac._numDefaults.push_back(d);
    }
  }
  if (mac._minnum)
    mac._minparam=(int)mac._namParam.size()+mac._minnum;
  else {
    int i=(int)mac._namParam.size();
    mac._minparam=0;
    while (i--) if (mac._namDefaults[i]=="\01") { mac._minparam=i+1; break; }
  }
  if (mac._greedy || mac._maxnum==BIGVALUE) mac._maxparam=BIGVALUE; else mac._maxparam=(int)mac._namParam.size()+mac._maxnum;
  if (listopt._macroname.empty()) mac._line=curlin; else mac._line=listopt._macrocurlin;
  mac._filename=listopt._filename;
  mac._body=rs->ReadUntil(macronl,macroel,"macro");
  if (!listopt._macroname.empty()) listopt._macrocurlin+=(int)mac._body.size()+1;
  mactab.add(name,icase,mac);
  return 1;
}

void premacrodot(string &line, RawSource *rs, bool icase) {
  string p;
  bool list=options.listmacros, recursive=false;
  while (!line.empty() && line[0]=='.') {
    line.erase(0,1); p=getinstructionpart(line);
    if (p=="list") list=true;
    else if (p=="nolist") list=false;
    else if (p=="recursive") recursive=true;
    else error("Unknown instruction","macro."+p);
  }
  if(!pregetmacro(line,icase,rs,list,recursive)) error1("Illegal macro definition");
}

void preMACRODOT(string &line, RawSource *rs) {
  premacrodot(line,rs,false);
}

void preIMACRODOT(string &line, RawSource *rs) {
  premacrodot(line,rs,true);
}

void preENDMACRO(string &, RawSource *) {
  error1("ENDMACRO without MACRO");
}

void preROTATE(string &line, RawSource *) {
  string nr;
  int val,rot,max;
  if (!ParseExpression(line,val)) { error1("Expression expected"); return; }
  if (labelnotfound) { error1("Forward reference"); return; }
  nr=macnumtab.getrepl(ROTATE);
  if (nr.empty()) { error1("use of rotate not allowed outside macro"); return; }
  if (!getConstant(nr,rot)) error1("preROTATE",ERRINTERNAL);
  nr=macnumtab.getrepl(0);
  if (nr.empty()) { error1("use of rotate not allowed outside macro"); return; }
  if (!getConstant(nr,max)) error1("preROTATE",ERRINTERNAL);
  rot=rot+val; while (rot<0) rot=rot+max;
  macnumtab.replace(ROTATE,tostr(rot));
}

void prewhile(string &line, RawSource *rs, bool list) {
  string p=line;
  int oerrors=errors;
  int val,i=0,wcurlin=curlin;
  if (!ParseExpression(p,val)) error1("Expression expected",line);
  if (labelnotfound) error1("Forward reference",line);
  checkjunk(p);
  if (!listopt._macroname.empty()) wcurlin=listopt._macrocurlin;
  StringList sl=rs->ReadUntil(whilenl,whileel,"while");
  if (errors<=oerrors) {
    RawSource *repeatbody=new RawSource(sl);
    ListOpt olistopt=listopt;
    listopt._char='>'; listopt._inc=false;
    if (listopt._list) listopt._list=list;
    listopt._macrofilename=listopt._filename;
    listopt._macroname="'while "+trim(line)+"'";
    for (;;) {
      listopt._macrocurlin=wcurlin;
      p=line;
      if (!ParseExpression(p,val)) { error1("Expression expected",line); return; }
      if (labelnotfound) { error1("Forward reference",line); return; }
      if (!val) break;
      int macnumtabpos=macnumtab.getlocation();
      macnumtab.add(ITERATION,tostr(i++));
      startmacrolabel(rs,listopt._macroname,listopt._macrofilename,wcurlin);
      repeatbody->Process();
      rs->AddToSource(repeatbody->GetSource());
      repeatbody->ClearSource();
      endmacrolabel(rs);
      macnumtab.setlocation(macnumtabpos);
      if (errors>oerrors) break;
      if (stop._stop) { 
        if (stop._stop==CONTINUE) { stop._stop=0; continue; }
        if (stop._stop==BREAK) stop._stop=0;
        break;
      }
    }
    listopt=olistopt;
  }
  if (!listopt._macroname.empty()) listopt._macrocurlin+=(int)sl.size()+1;
  line.clear();
}

void preWHILEDOT(string &line, RawSource *rs) {
  string p;
  bool list=options.listloops;
  while (!line.empty() && line[0]=='.') {
    line.erase(0,1); p=getinstructionpart(line);
    if (p=="list") list=true;
    else if (p=="nolist") list=false;
    else error("Unknown instruction","while."+p);
  }
  prewhile(line,rs,list);
}

void preENDWHILE(string &, RawSource *) {
  error1("ENDWHILE without WHILE");
}

void prerepeat(string &line, RawSource *rs, bool list) {
  int oerrors=errors;
  int val,wcurlin=curlin;
  if (!listopt._macroname.empty()) wcurlin=listopt._macrocurlin;
  string rline=line;
  if (!ParseExpression(line,val)) error1("Expression expected");
  if (labelnotfound) error1("Forward reference");
  checkjunk(line);
  StringList sl=rs->ReadUntil(repeatnl,repeatel,"repeat");
  if (val<0) error1("Negative repeat value");
  if (errors<=oerrors && val) {
    RawSource *repeatbody=new RawSource(sl);
    ListOpt olistopt=listopt;
    listopt._char='>'; listopt._inc=false;
    if (listopt._list) listopt._list=list;
    listopt._macrofilename=listopt._filename;
    listopt._macroname="'repeat "+trim(rline)+"'";
    for (int i=0; i!=val; ++i) {
      listopt._macrocurlin=wcurlin;
      int macnumtabpos=macnumtab.getlocation();
      macnumtab.add(ITERATION,tostr(i));
      startmacrolabel(rs,listopt._macroname,listopt._macrofilename,wcurlin);
      repeatbody->Process();
      rs->AddToSource(repeatbody->GetSource());
      repeatbody->ClearSource();
      endmacrolabel(rs);
      macnumtab.setlocation(macnumtabpos);
      if (errors>oerrors) break;
      if (stop._stop) { 
        if (stop._stop==CONTINUE) { stop._stop=0; continue; }
        if (stop._stop==BREAK) stop._stop=0;
        break;
      }
    }
    listopt=olistopt;
  }
  if (!listopt._macroname.empty()) listopt._macrocurlin+=(int)sl.size()+1;
}

void preREPEATDOT(string &line, RawSource *rs) {
  string p;
  bool list=options.listloops;
  while (!line.empty() && line[0]=='.') {
    line.erase(0,1); p=getinstructionpart(line);
    if (p=="list") list=true;
    else if (p=="nolist") list=false;
    else error("Unknown instruction","repeat."+p);
  }
  prerepeat(line,rs,list);
}

void preENDREPEAT(string &, RawSource *) {
  error1("ENDREPEAT without REPEAT");
}

void preBREAK(string &, RawSource *) {
  if (macnumtab.getrepl(ITERATION).empty()) error1("BREAK used outside loop");
  else stop._stop=BREAK;
}

void preCONTINUE(string &, RawSource *) {
  if (macnumtab.getrepl(ITERATION).empty()) error1("CONTINUE used outside loop");
  else stop._stop=CONTINUE;
}

void preEXITMACRO(string &line, RawSource *) {
  if (maclabp.empty()) error1("EXITMACRO used outside macro");
  else stop._stop=EXITMACRO;
  stop._retval=line;
  line.clear();
}

void preXEXITMACRO(string &line, RawSource *) {
  int val;
  if (maclabp.empty()) error1("XEXITMACRO used outside macro");
  else stop._stop=EXITMACRO;
  ParseExpression(line,val);
  if (labelnotfound) error1("Forward reference");
  stop._retval=tostr(val);
}

void preSTRUCT(string &line, RawSource *rs) {
  int offset=0,align=1;
  bool gl=false;
  if (sbcneed(line,'@')) gl=true;
  string n=getid(line); if (n.empty()) error1("Illegal structure name");
  if (comma(line)) {
    switch(comma(line)) {
    case 0:
      if (!ParseExpression(line,offset)) error1("Offset expected");
      if (labelnotfound) error1("Forward reference");
      if (!comma(line)) break;
    default:
      if (!ParseExpression(line,align)) error1("Alignment expected");
      if (labelnotfound) error1("Forward reference");
      if (align<1) { align=1; error1("Negative or zero alignment not allowed"); }
    }
  }
  checkjunk(line);
  int scurlin=curlin, slistcurlin=listcurlin;
  StringList sl=rs->ReadUntil(structurenl,structureel,"structure");
  if (structtab.exists(n)) { error("Structure already exists",n); return; }

  Structure s(n,offset);
  RSStructure rss(offset,align,gl,s,sl);

  int ocurlin=curlin, olistcurlin=listcurlin;
  curlin=scurlin, listcurlin=slistcurlin;
  rss.Process();
  curlin=ocurlin, listcurlin=olistcurlin;

  if (gl || modlabp.empty()) n.clear(); else n=modlabp+'.';
  n+=s.getname();
  int i=labtab.insert(n,false);
  if (i) labtab.setvalue(i,s.getlength(),0);
  else error("Label already exists",n);

  structtab.add(s);
}

void preENDSTRUCT(string &, RawSource *) {
  error1("ENDSTRUCT without STRUCT");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void initpreprocessor() {
  FunctionEntry<pFun> group1if[] = {
    "ifdef",preIFDEF,
    "ifndef",preIFNDEF,
    "else",preELSE,
    "endif",preENDIF,

    "elseifdef",preELSEIFDEF,
    "elseifndef",preELSEIFNDEF,
  };

  FunctionEntry<pFun> group2if[] = {
    "if",preIF,
    "ifidn",preIFIDN,
    "ifidni",preIFIDNI,
    "ifdif",preIFDIF,
    "ifdifi",preIFDIFI,
    "ifb",preIFB,
    "ifid",preIFID,
    "ifstr",preIFSTR,
    "ifnum",preIFNUM,
    "ifnb",preIFNB,
    "ifnid",preIFNID,
    "ifnstr",preIFNSTR,
    "ifnnum",preIFNNUM,
    "ifexists",preIFEXISTS,
    "ifnexists",preIFNEXISTS,
    "ifin",preIFIN,
    "ifini",preIFINI,

    "elseif",preELSEIF,
    "elseifidn",preELSEIFIDN,
    "elseifidni",preELSEIFIDNI,
    "elseifdif",preELSEIFDIF,
    "elseifdifi",preELSEIFDIFI,
    "elseifb",preELSEIFB,
    "elseifid",preELSEIFID,
    "elseifstr",preELSEIFSTR,
    "elseifnum",preELSEIFNUM,
    "elseifnb",preELSEIFNB,
    "elseifnid",preELSEIFNID,
    "elseifnstr",preELSEIFNSTR,
    "elseifnnum",preELSEIFNNUM,
    "elseifexists",preELSEIFEXISTS,
    "elseifnexists",preELSEIFNEXISTS,
    "elseifin",preELSEIFIN,
    "elseifini",preELSEIFINI,
  };

  FunctionEntry<pFun> group1[] = {
    "define",preDEFINE,
    "idefine",preIDEFINE,
    "xdefine",preXDEFINE,
    "xidefine",preXIDEFINE,
    "undef",preUNDEF,
    "assign",preASSIGN,

    "tostr",preTOSTR,
    "strlen",preSTRLEN,
    "substr",preSUBSTR,
  };

  FunctionEntry<pFun> group2[] = {
    "include",preINCLUDE,

    "macro",preMACRODOT,
    "imacro",preIMACRODOT,
    "rotate",preROTATE,
    "while",preWHILEDOT,
    "repeat",preREPEATDOT,
    "break",preBREAK,
    "continue",preCONTINUE,
    "exitmacro",preEXITMACRO,
    "xexitmacro",preXEXITMACRO,
    "struct",preSTRUCT,

    "macro.",preMACRODOT,
    "imacro.",preIMACRODOT,
    "while.",preWHILEDOT,
    "repeat.",preREPEATDOT,

    "endmacro",preENDMACRO,
    "endwhile",preENDWHILE,
    "endrepeat",preENDREPEAT,
    "endstruct",preENDSTRUCT,

    "endm",preENDMACRO,
    "ends",preENDSTRUCT,
  };

  group1iftab.init(group1if, sizeof group1if/sizeof group1if[0]);
  group2iftab.init(group2if, sizeof group2if/sizeof group2if[0]);
  group1tab.init(group1if, sizeof group1if/sizeof group1if[0]);
  group1tab.init(group1, sizeof group1/sizeof group1[0]);
  group2tab.init(group2if, sizeof group2if/sizeof group2if[0]);
  group2tab.init(group2, sizeof group2/sizeof group2[0]);

  macronl.push_back("macro");
  macronl.push_back("imacro");
  macronl.push_back("macro.");
  macronl.push_back("imacro.");
  macroel.push_back("endmacro");
  macroel.push_back("endm");
  whilenl.push_back("while");
  whilenl.push_back("while.");
  whileel.push_back("endwhile");
  repeatnl.push_back("repeat");
  repeatnl.push_back("repeat.");
  repeatel.push_back("endrepeat");
  structureel.push_back("endstruct");
  structureel.push_back("ends");
}

//eof
