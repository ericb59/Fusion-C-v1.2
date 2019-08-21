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

// rawsource.cpp

#include "sjasm.h"

extern FunctionTable<pFun,RawSource> group1iftab,group2iftab,group1tab,group2tab;

unsigned illchar;

RawSource::RawSource(string filename) {
  _sl.clear();
  _source.clear();
  ListOpt olistopt=listopt;
  unsigned ocurlin=curlin; 
  illchar=0;

  ReadFile file(getpath(filename));
  if (file.ok()) {
    file.readtostringlist(_sl);
    ++listopt._include;
    if (!listopt._include) { string d=""; dirCODE(d,this); }
    listopt._filename=filename;
    listopt._curlin=0;
    _source.push_back(new SetListOpt(listopt));
    _source.back()->Process();
    _Preprocess();
  }
  listopt=olistopt;
  listopt._curlin=ocurlin;
  _source.push_back(new SetListOpt(listopt));
  _source.back()->Process();
}

RawSource::RawSource(StringList nsl) {
  _source.clear();
  _sl.assign(nsl.begin(),nsl.end());
}

int RawSource::_nextline() {
  ++_it;
  if (_it==_sl.end()) {
    stop._stop=END;
    if (_unexpectedin.empty())
      error1("Unexpected end of file");
    else
      error(_unexcurlin,_unexlistcurlin,"Unexpected end of file in",_unexpectedin);
    return 1;
  }
  listopt.addlist(_source,*_it);
  return 0;  
}

void RawSource::Process() {
  _it=_sl.begin();
  if (_it!=_sl.end()) do {
    listopt.addlist(_source,*_it);
    string line=*_it;
    _Group1(line);
    if (stop._stop) return;
    ++_it;
  } while (_it!=_sl.end());
}

void RawSource::_Preprocess() {
  _it=_sl.begin();
  if (_it!=_sl.end()) do {
    listopt.addlist(_source,*_it);
    string line=_KillComments();
    _Group1(line);
    if (stop._stop) return;
    ++_it;
  } while (_it!=_sl.end());
}

StringList RawSource::ReadUntil(StringList lnest, StringList lend, string unex) {
  unsigned nest=1;
  string line,p,d;
  StringList s;
  _unexpectedin=unex;
  _unexcurlin=curlin;
  _unexlistcurlin=listcurlin;
  ListOpt olistopt=listopt;
  listopt._char='<';
  for (;;) {
    if (_nextline()) { listopt=olistopt; return s; }
    p=line=_KillComments();
    skipblanks(p);
      d=ReplaceDefines(getinstruction(p));                        // is dit wel zo slim?
      for (iStringList i=lnest.begin(); i!=lnest.end(); ++i)
        if (d==*i) { ++nest; break; }
      for (iStringList i=lend.begin(); i!=lend.end(); ++i)
        if (d==*i) { if (!--nest) { listopt=olistopt; return s; } break; }
    s.push_back(line);
  }
}

string RawSource::_KillComments() {
  char ochar=listopt._char;
  unsigned pos,af=0;
  string res="";
  istring p,op;

  for(;;) {
    op=p=_it->begin();
    for(;;) {
      if (p==_it->end()) break;
      if (*p==';') break;
      if (*p=='/') {
        if (nextchar(p,*_it)=='/') break;
        else if (nextchar(p,*_it)=='*') {
          int nest=1; p+=2; res+=' ';
          listopt._char='~';
          for (;;) {
            if (p==_it->end()) {
              if (_nextline()) return res;
              else { op=p=_it->begin(); continue; }
            }
            if (*p=='/' && nextchar(p,*_it)=='*') { ++nest; p+=2; } 
            else if (*p=='*' && nextchar(p,*_it)=='/') { --nest; p+=2; if (!nest) break; }
            else ++p;
          }
          listopt._char=ochar;
        }
      } 
      if (p==_it->end()) break;
      if (!*p) break;

      if (*p=='"') {
        string sp(p,_it->end());
        int len=getstringlength(sp);
        res+=sp.substr(0,len);
        p+=len;
        continue;
      }

      if (*p=='\'') {
        if (af==2) {
          ++p; af=0;
        } else {
          string sp(p,_it->end());
          int len=getstringlength(sp);
          res+=sp.substr(0,len);
          p+=len;
        }
        continue;
      }

      if (!af && *p=='a' || *p=='A') {
        af=1; res+=*p; ++p; continue; 
      }
      if (af==1 && *p=='f' || *p=='F') { 
        af=2; res+=*p; ++p; continue; 
      }
      af=0;

      if (*p==9) res+=' ';
      else if (*p<32) { res+=' '; if (!illchar) { error1("Illegal character(s)"); ++illchar; } }
      else res+=*p;
      ++p;
    }

    pos=(int)res.find_last_not_of(' '); if (pos!=string::npos) res.erase(pos+1);
    pos=(int)res.length();

    if (pos && res[pos-1]=='\\') {
      res.erase(pos-1);
      if (_nextline()) return res;
    } else break;
  }
  return res;
}

void RawSource::_Group1(string &line) {
  string d,p=line;
  if (p.empty()) return;
  skipblanks(p);
  d=getinstruction(p);
  if (!d.empty()) {
    int r=0;
    if (ifstack.empty() || ifstack.top()._active) r=group1tab.exec(d,p,this);
    else if (!(r=group1iftab.exec(d,p,this))) r=group2iftab.exec(d,p,this);
    if (r) { checkjunk(p); return; }
  }
  if (ifstack.empty() || ifstack.top()._active) _ReplaceDefs(line);
}

void RawSource::_ReplaceDefs(string &line) {
  string s=ReplaceDefines(line);
  line=s;
  _Group2(line);
}

void RawSource::_Group2(string &line) {
  string d,p=line;
  if (p.empty()) return;
  skipblanks(p);
  d=getinstruction(p);
  if (!d.empty() && group2tab.exec(d,p,this)) {
    checkjunk(p);
    return;
  }
  _CutLines(line);
}

void RawSource::_CutLines(string &line) {
  string res="";
  unsigned pos;
  while ('o') {
    pos=(int)line.find_first_of("\\\"'");
    if (pos) res+=line.substr(0,pos);
    if (pos==string::npos) break;
    line=line.substr(pos);
    if (line[0]=='\\') {
      _Parse(res);
      res.clear(); line.erase(0,1);
    } else {
      pos=getstringlength(line);
      res+=line.substr(0,pos); line=line.substr(pos);
    }
  }
  if (!res.empty()) _Parse(res);
}

void RawSource::_Parse(string &line) {
  if (_ParseLabel(line)) _ParseRepeat(line);
}

int RawSource::_ParseLabel(string &line) {
  string n,mlp=maclabp;
  if (line.empty() || line[0]==' ' || line[0]=='[') return 1;
  if (isdigit(line[0])) {
    int num;
    if (!getval(line,num,10)) { error("Illegal label name"); return 0; }
    if (line[0]==':' && line[1]!='=') line.erase(0,1);
    _source.push_back(new NumberLabel(numlabtab.insert(num,adres)));
    if (needset(line) || needfield(line) || needequ(line)) {
      error("Numberlabels only allowed as adresslabels");
      return 0;
    }
    return 1;
  }
  int pos,glb=0,nl=0;
  if (!mlp.empty()) {
    if (cneed(line,'@')) mlp.clear();
    else if (need(line,".@")) {
      do {
        int loc=(int)mlp.find_last_of('.');
        if (loc!=string::npos) mlp=mlp.substr(0,loc);
        else { mlp.clear(); /*error("No nested macro");*/ break; }
      } while (cneed(line,'@'));
      line='.'+line;
    }
  } else {
    if (cneed(line,'!')) nl=1;
    if (cneed(line,'@')) glb=1;
  }
  if (!isalpha(line[0]) && line[0]!='.' && line[0]!='_') { error("Illegal label name"); return 0; }
  pos=findlabelchar.findnot(line);
  n=line.substr(0,pos); if (pos!=string::npos) line=line.substr(pos); else line.clear();
  if (line[0]==':' && line[1]!='=') line.erase(0,1);
  if (!mlp.empty() && n[0]=='.') n=mlp+'>'+n.substr(1);
  else {
    if (n[0]=='.' && !glb) n=vorlabp+n;
    else if (!nl) {
      _source.push_back(new VorNamLabel(n));
      _source.back()->Process();
    }
    if (!glb && !modlabp.empty()) n=modlabp+'.'+n;
  }
  if (needfield(line)) {
    int lnr=labtab.insert(n);
    if (!lnr) { error("Duplicate labelname",n); return 0; }
    string s=line; int val;
    skipblanks(s);
    if (s.empty()) { val=0; line="0"; } 
    else if (!ParseExpression(s,val)) { error("Syntax error"); return 0; }
    labtab.setforward(lnr,labelnotfound!=0);
    checkjunk(s); labtab.setvalue(lnr,mapadr,page); mapadr+=val;
    _source.push_back(new FieldLabel(lnr,line));
    return 0;
  } else if (needset(line)) {
    int lnr=labtab.getindex(n);
    if (!lnr) lnr=labtab.insert(n,true);
    if (!lnr) { error("dirSET",ERRINTERNAL); return 0; }
    string s=line; int val;
    skipblanks(s); if (s.empty()) { error("Expression expected"); return 0; }
    if (!ParseExpression(s,val)) { error("Syntax error"); return 0; }
    labtab.setforward(lnr,labelnotfound!=0);
    checkjunk(s); labtab.setvalue(lnr,val,page,true);
    _source.push_back(new SetLabel(lnr,line));
    return 0;
  } else if (needequ(line)) {
    int lnr=labtab.insert(n);
    if (!lnr) { error("Duplicate labelname",n); return 0; }
    string s=line; int val;
    skipblanks(s); if (s.empty()) { error("Expression expected"); return 0; }
    if (!ParseExpression(s,val)) { error("Syntax error"); return 0; }
    labtab.setforward(lnr,labelnotfound!=0);
    checkjunk(s); labtab.setvalue(lnr,val,page);
    _source.push_back(new EquLabel(lnr,line));
    return 0;
  } else {
    int lnr=labtab.insert(n);
    if (!lnr) { error("Duplicate labelname",n); return 1; }
    if (routlabel) { routlabel->setlabelindex(lnr); routlabel=0; }
    labtab.setforward(lnr,true);
    labtab.setvalue(lnr,adres,page);
    _source.push_back(new AdresLabel(lnr));
    if (structtab.emit(line,this,n)) return 0;
  }
  return 1;
}

void RawSource::_ParseRepeat(string &line) {
  if (sbcneed(line,'[')) {
    int p=(int)line.find_first_of(']');
    if (p==string::npos) { error("] expected"); return; }
    RSRepeat *rsr=new RSRepeat;
    string count=line.substr(0,p);
    line.erase(0,p+1);
    repeatstack.push_back(1);
    int olablin=lablin; // :(
    rsr->parserepeat(line);
    lablin=olablin; //
    repeatstack.pop_back();
    if (rsr->GetSource().empty()) error("Nothing to repeat");
    else _source.push_back(new Repeat(count,*rsr->GetSource().begin()));
    _source.back()->Process();
  } else
    _ExpandMacros(line);
}

void RawSource::_ExpandMacros(string &line) {
  if (!sbcneed(line,'@')) {
    if (mactab.emit(line,this)) return;
  }
  if (structtab.emit(line,this)) return;
  if (_AssemblerDirectives(line)) return;
  _Mnemonics(line);
}

int RawSource::_AssemblerDirectives(string &line) {
  string d,p=line;
  if (p.empty()) return 1;
  d=getinstruction(p);
  if (!d.empty()) {
    if (dirfuntab.exec(d,p,this)) return 1;
    if (datafuntab.exec(d,p,this)) return 1;
  }
  return 0;
}

void RawSource::_Mnemonics(string &line) {
  (*piCPU)(line,&_source);
}

void RSStructure::_Parse(string &line) {
  if (line.empty()) return;
  int i,len=0,al,lnr=0;
  string n,def;
  if (_mustalign) { _mustalign=false; al=_newalign; } else al=_defaultalign;
  if (line[0]!=' ') { 
    if (line[0]=='.') line.erase(0,1);
    n=getid(line);
    if (n.empty()) error1("Illegal label name");
    else {
      if (_global || modlabp.empty()) def.clear(); else def=modlabp+'.';
      def+=_s.getname()+'.'+n;
      lnr=labtab.insert(def,false);
      if (lnr) labtab.setvalue(lnr,0,0);
      else error("Label already exists",def);
    }
  }

  i=getinSTRUCTion(line);
  skipblanks(line); len=i; def.clear();
  if (i) {
    switch(i) {
    case 1: case 2: case 3: case 4:
      if (!line.empty()) {
        int d;
        string arg;
        if (!ParseExpression(arg=getargument(line),d)) error1("Default value expected");
        if (labelnotfound) error1("Forward reference");
        switch(i) {
        case 1: d=check8(d); break;
        case 2: d=check16(d); break;
        case 3: d=check24(d); break;
        case 4: break;
        }
        def=tostr(d);
      }
      break;
    case 5: 
      if (!ParseExpression(line,len)) error1("Size expected");
      if (labelnotfound) error1("Forward reference");
      if (comma(line)) {
        def=getargument(line,true);
        string d(def); Data e;
        getbytes(d,e);
        if (labelnotfound) error1("Forward reference");
      }
      break;
    case 6: 
      i=0; _mustalign=true;
      if (!ParseExpression(line,al)) { 
        al=1; error1("Alignment value expected");
      } else {
        if (labelnotfound) error1("Forward reference");
        if (al<1) { al=1; error1("Alignment should be positive"); }
      }
      _newalign=al;
      break;
    case 7:
      if (!ParseExpression(line,len)) error1("Size expected");
      if (labelnotfound) error1("Forward reference");
      break;
    default:
      break;
    }
  }
  checkjunk(line);
  if (i || lnr) {
    if (_offset%al) _offset+=al-abs(_offset%al);
    if (lnr) labtab.setvalue(lnr,_offset,0);
    _s.addmember(n,i,len,_offset,def);
    _offset+=len;
  }
}

//eof
