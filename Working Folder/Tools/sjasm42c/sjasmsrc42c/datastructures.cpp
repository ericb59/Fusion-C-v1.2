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

// datastructures.cpp

#include "sjasm.h"

void startmacrolabel(RawSource *rs, string name, string file, int line) {
  string mlab;
  if (!maclabp.empty()) mlab=maclabp+".";
  mlab+=tostr(macronummer++);
  rs->GetSource().push_back(new StartMacro(mlab,name,file,line));
  rs->GetSource().back()->Process();
}

void endmacrolabel(RawSource *rs) {
  rs->GetSource().push_back(new EndMacro());
  rs->GetSource().back()->Process();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Data::_grow(int nsize) {
  _maxsize*=2;
  if (nsize>=_maxsize) _maxsize=nsize+2;
  _resize(_maxsize);
}

void Data::_resize(int nsize) {
  byte *odata=_data;
  _data=new byte[nsize];
  memset(_data+_size,0,nsize-_size);
  if (odata) {
    memcpy(_data,odata,_size);
    delete [] odata;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DefineArgTable::add(string naam, string repl) {
  if (_nextlocation>=_size) {
    _size=_size*2+1;
    _deftab.resize(_size);
  }
  _deftab[_nextlocation]._name=naam;
  _deftab[_nextlocation]._repl=repl;
  ++_nextlocation;
  _used[naam[0]&63]=true;
}

int DefineArgTable::getlocation() {
  return _nextlocation;
}

void DefineArgTable::setlocation(int nnl) {
  _nextlocation=nnl;
  if (nnl==1) for (int i=0; i!=64; _used[i++]=false);
}

string DefineArgTable::getrepl(string naam, int skip) {
  if (!_used[naam[0]&63]) return "";
  int p=_nextlocation;
  while (--p) if (naam==_deftab[p]._name && !skip--) return _deftab[p]._repl;
  return "";
}

void DefineArgTable::replace(string naam, string repl) {
  int p=_nextlocation;
  while (naam!=_deftab[--p]._name);
  _deftab[p]._repl=repl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MacNumTable::add(int num, string repl) {
  if (_nextlocation>=_size) {
    _size=_size*2+1;
    _numtab.resize(_size);
  }
  _numtab[_nextlocation]._num=num;
  _numtab[_nextlocation]._repl=repl;
  ++_nextlocation;
}

int MacNumTable::getlocation() {
  return _nextlocation;
}

void MacNumTable::setlocation(int nnl) {
  _nextlocation=nnl;
}

string MacNumTable::getrepl(int num, int skip) {
  int p=_nextlocation;
  while (--p) if (num==_numtab[p]._num && !skip--) return _numtab[p]._repl;
  return "";
}

void MacNumTable::replace(int num, string repl) {
  int p=_nextlocation;
  while (num!=_numtab[--p]._num);
  _numtab[p]._repl=repl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DefineTable::add(string n_name,string n_repl) {
  int p=exists(n_name,0);
  if (p==-1) { error("Overloading not allowed"); return; }
  if (p) {
    _deftab[p]._repl=n_repl;
  } else {
    int tr,htr;
    tr=_hash(n_name);
    while(htr=_hashtable[tr]) {
      if (_deftab[htr]._name==n_name && _deftab[htr]._args.size()==0) error("DefineTable::add",ERRINTERNAL);
      else if (++tr>=_size) tr=0;
    }
    _hashtable[tr]=_nextlocation;
    _deftab[_nextlocation]._name=n_name;
    _deftab[_nextlocation]._repl=n_repl;
    _deftab[_nextlocation]._used=0;
    _deftab[_nextlocation]._icase=false;
    StringList n_args;
    _deftab[_nextlocation]._args=n_args;
    ++_nextlocation;
    _used[n_name[0]&63]=true;
  }
}

void DefineTable::add(string n_name,string n_repl,bool n_icase,StringList n_args) {
  if (_nextlocation>=_size*2/3) _grow();
  int p=exists(n_name,(int)n_args.size());
  if (p==-1) { error("Overloading not allowed"); return; }
  if (p) {
    if (n_args.size()) { error("Redefinition of multiparameter define not allowed"); return; }
    _deftab[p]._repl=n_repl;
  } else {
    if (n_icase) n_name=tolower(n_name);
    int tr,htr;
    tr=_hash(n_name);
    while(htr=_hashtable[tr]) {
      if (_deftab[htr]._name==n_name && _deftab[htr]._args.size()==n_args.size()) error("DefineTable::add",ERRINTERNAL);
      else if (++tr>=_size) tr=0;
    }
    _hashtable[tr]=_nextlocation;
    _deftab[_nextlocation]._name=n_name;
    _deftab[_nextlocation]._repl=n_repl;
    _deftab[_nextlocation]._used=0;
    _deftab[_nextlocation]._icase=n_icase;
    _deftab[_nextlocation]._args=n_args;
    ++_nextlocation;
    _used[n_name[0]&63]=true;
    if (n_icase) _used[(n_name[0]-32)&63]=true;
  }
}

string DefineTable::getrepl(string n) {
  int p=exists(n,0);
  if (p<1) return "";
  return _deftab[p]._repl;
}

string DefineTable::getrepl(string n,string &op) {
  StringList a;
  string res,p=op;
  int aa=0,pos,location;
  if (p[0]=='(') {
    a=getarguments(p);
    aa=(int)a.size();
  }
  pos=exists(n,aa);
  if (pos<1) return "";
  _deftab[pos]._used=1;
  location=defargtab.getlocation();
  if (aa) {
    iStringList ni=_deftab[pos]._args.begin(), ri=a.begin();
    while (aa--) {
      defargtab.add(*ni,*ri);
      ++ni; ++ri;
    }
  }
  res=ReplaceDefines(_deftab[pos]._repl);
  defargtab.setlocation(location);
  _deftab[pos]._used=0;
  op=p;
  return res;
}

int DefineTable::exists(string name,int narg) {
  int tr,htr,otr;
  if (!_used[name[0]&63]) return 0;
  otr=tr=_hash(name);
  while(htr=_hashtable[tr]) {
    if (_deftab[htr]._name==name && !_deftab[htr]._used) {
      if (narg) {
       if (!_deftab[htr]._args.size()) return -1;
       if (narg==_deftab[htr]._args.size()) return htr;
      } else {
        if (_deftab[htr]._args.size()) return -1;
        return htr;
      }
    }
    if (++tr>=_size) tr=0;
    if (tr==otr) break;
  }

  name=tolower(name);
  otr=tr=_hash(name);
  while(htr=_hashtable[tr]) {
    if (_deftab[htr]._name==name && _deftab[htr]._icase && !_deftab[htr]._used) {
      if (narg) {
       if (!_deftab[htr]._args.size()) return -1;
       if (narg==_deftab[htr]._args.size()) return htr;
      } else {
        if (_deftab[htr]._args.size()) return -1;
        return htr;
      }
    }
    if (++tr>=_size) tr=0;
    if (tr==otr) break;
  }
  return 0;
}

void DefineTable::undef(string s) {
  int p=exists(s,0);
  if (!p) return;
  if (p>0) _deftab[p]._name.clear();
  else error("Can't undef a define with arguments",s);
}

int DefineTable::_hash(string s) {
  const char *ss=s.c_str();
  unsigned h=0;
  while (*ss) h=(h<<3)+*ss++;
  return h % _size;
}

void DefineTable::_grow() {
  int tr;
  _size=_size*2+1;
  _hashtable.clear();
  _hashtable.resize(_size);
  _deftab.resize(_size);
  for (int i=1;i!=_nextlocation;++i) {
    tr=_hash(_deftab[i]._name);
    while(_hashtable[tr]) if (++tr>=_size) tr=0;
    _hashtable[tr]=i;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int LabelTable::insert(string n_name,bool n_set) {
  if (_nextlocation>=_size*2/3) _grow();
  int tr,htr;
  tr=_hash(n_name);
  while(htr=_hashtable[tr]) {
    if (_labtab[htr]._name==n_name) return 0;
    else if (++tr>=_size) tr=0;
  }
  _hashtable[tr]=_nextlocation;
  _labtab[_nextlocation]._rout=output[onr]->getroutp();
  _labtab[_nextlocation]._name=n_name;
  _labtab[_nextlocation]._set=n_set;
  _labtab[_nextlocation]._forward=false;
  _labtab[_nextlocation]._lastpass=0;
  return _nextlocation++;
}

void LabelTable::setvalue(int loc,int nval,int npage,bool nset) {
  if (nset!=_labtab[loc]._set) error1("Mixing of set and normal labels not allowed");
  _labtab[loc]._val=nval;
  _labtab[loc]._page=npage;
}

void LabelTable::setforward(int loc, bool n_forward) {
  _labtab[loc]._forward=n_forward;
}

void LabelTable::getvalue(int loc,int &val,int &page) {
  val=_labtab[loc]._val;
  page=_labtab[loc]._page;
}

int LabelTable::getvalue(string name,int &val,int &page) {
  int htr;
  if (htr=getindex(name)) {
    val=_labtab[htr]._val;
    page=_labtab[htr]._page;
    if (pass==1 && _labtab[htr]._forward) labelnotfound=1;
    Rout* thisrout=output[onr]->getroutp();
    if (thisrout!=_labtab[htr]._rout && !thisrout->forget()) _labtab[htr]._rout->used();
    _labtab[htr]._lastpass=pass;
    return 1;
  }
  return 0;
}

int LabelTable::getindex(string name) {
  int tr,htr,otr;
  otr=tr=_hash(name);
  while(htr=_hashtable[tr]) {
    if (_labtab[htr]._name==name) return htr;
    if (++tr>=_size) tr=0;
    if (tr==otr) break;
  }
  return 0;
}

string LabelTable::getname(int index) {
  return _labtab[index]._name;
}

void LabelTable::listdump(StringList &sl) {
  string s;
  sl.push_back("");
  sl.push_back("    LABELS");
  sl.push_back("-------------------------------------------------");
  for (int i=1; i<_nextlocation; ++i)
    if (!isdigit(_labtab[i]._name[0])) {
      s.clear();
      s=tohex(_labtab[i]._page,2)+':'+tohex(_labtab[i]._val,8)+"   "+_labtab[i]._name;
      if (_labtab[i]._rout->forget()) s[12]='-';
      else if (_labtab[i]._lastpass!=pass) s[12]='X';
      else if (_labtab[i]._set) s[12]='S';
      sl.push_back(s);
    }
}

void LabelTable::symdump(StringList &sl) {
  for (int i=1; i<_nextlocation; ++i)
    if (!isdigit(_labtab[i]._name[0]))
      if (!_labtab[i]._rout->forget())
        sl.push_back(tostr(_labtab[i]._name+": equ "+tohex(_labtab[i]._val,8))+"h");
}

int LabelTable::_hash(string s) {
  const char *ss=s.c_str();
  unsigned h=0;
  while (*ss) h=(h<<1)+*ss++;
  return h % _size;
}

void LabelTable::_grow() {
  int tr;
  _size=_size*2+1;
  _hashtable.clear();
  _hashtable.resize(_size);
  _labtab.resize(_size);
  for (int i=1;i!=_nextlocation;++i) {
    tr=_hash(_labtab[i]._name);
    while(_hashtable[tr]) if (++tr>=_size) tr=0;
    _hashtable[tr]=i;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

NumLabelTable::_numLabelEntry::_numLabelEntry(int num,int val) : _num(num),_val(val),_lablin(lablin) {
}

int NumLabelTable::insert(int num,int val) {
  _labtab.push_back(_numLabelEntry(num,val));
  return (int)_labtab.size()-1;
}

void NumLabelTable::setvalue(int index,int val) {
  _labtab[index]._val=val;
  _index=index;
}

void NumLabelTable::getvalue(int index,int &val) {
  val=_labtab[index]._val;
}

int NumLabelTable::getvalueb(int num,int &val) {
  int index=_index;
  if (!_labtab.size()) return 0;
  while(index!=-1) {
    if (_labtab[index]._num==num && _labtab[index]._lablin<lablin) {
      val=_labtab[index]._val;
      return 1;
    }
    --index;
  }
  return 0;
}

int NumLabelTable::getvaluef(int num,int &val) {
  int maxindex=(int)_labtab.size(),index=_index;
  if (!maxindex) return 0;
  while(index!=maxindex) {
    if (_labtab[index]._num==num && _labtab[index]._lablin>=lablin) {
      val=_labtab[index]._val;
      return 1;
    }
    ++index;
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MacroTable::add(string n_name, bool n_icase, Macro n_mac) {
  if (_nextlocation>=_size*2/3) _grow();
  int p=exists(n_name);
  if (p) {
    if (_mactab[p]._icase!=n_icase) { error("Macro already exists"); return; }
  } else {
    if (n_icase) n_name=tolower(n_name);
    int tr,htr;
    tr=_hash(n_name);
    while(htr=_hashtable[tr]) {
      if (_mactab[htr]._name==n_name) error("MacroTable::add",ERRINTERNAL);
      else if (++tr>=_size) tr=0;
    }
    _hashtable[tr]=_nextlocation;
    _mactab[_nextlocation]._name=n_name;
    _mactab[_nextlocation]._icase=n_icase;
    p=_nextlocation++;
  }
  _mactab[p].add(n_mac);
}

int MacroTable::emit(string &line, RawSource *rs, bool fun) {
  string p=line;
  string name=getid(p);
  int m=exists(name);
  if (!m) return 0;
  if (fun && !cneed(p,'(')) return 0;
  list<Macro>::iterator im=_mactab[m]._macro.begin();
  StringList args;
  int i,narg=0,max=im->_maxnum==BIGVALUE?BIGVALUE:(int)im->_namParam.size()+im->_maxnum;
  for(;;) {
    skipblanks(p);
    if (fun && p[0]==')') break;
    if (!p[0]) break;
    ++narg;
    if (narg>max) {
      ++im;
      if (im==_mactab[m]._macro.end()) { --im; break; }
      max=max=im->_maxnum==BIGVALUE?BIGVALUE:(int)im->_namParam.size()+im->_maxnum;
    }
    if (narg==max) args.push_back(getargument(p,im->_greedy,fun)); else args.push_back(getargument(p,false,fun));
    if (!comma(p)) break;
  }
  if (narg<im->_minparam) return 0;
  if (im->_used && !im->_recursive && !fun) return 0;
  if (fun && !sbcneed(p,')')) error("Closing ) expected");
  iStringList ia=args.begin();
  int defargtabpos=defargtab.getlocation();
  int macnumtabpos=macnumtab.getlocation();
  for (i=0; i!=im->_namParam.size(); ++i) {
    if (ia==args.end()) {
      string a=im->_namDefaults[i];
      if (a=="\01") { a=" "; error("Parameter missing"); }
      defargtab.add(im->_namParam[i],a);
    } else {
      if (*ia=="") {
        *ia=im->_namDefaults[i];
        if (*ia=="\01") { *ia=" "; error("Parameter missing"); }
      }
      defargtab.add(im->_namParam[i],*ia);
      ++ia;
    }
  }
  int anum=0;
  for (i=1; i<=im->_minnum; ++i) {
    if (ia==args.end()) { error("Parameter missing"); break; }
    macnumtab.add(++anum,*ia);
    ++ia;
  }
  for (i=0; i!=im->_numDefaults.size(); ++i) {
    if (ia==args.end()) {
      macnumtab.add(++anum,im->_numDefaults[i]);
    } else {
      macnumtab.add(++anum,*ia);
      ++ia;
    }
  }
  while (ia!=args.end()) {
    macnumtab.add(++anum,*ia);
    ++ia;
  }
  macnumtab.add(0,tostr(anum));
  macnumtab.add(ROTATE,"0");

  im->_used=true;
  ListOpt olistopt=listopt;
  listopt._char='>'; listopt._inc=false;
  if (listopt._list) listopt._list=im->_list;
  RawSource *body=new RawSource(im->_body);
  listopt._macrofilename=im->_filename;
  listopt._macroname="macro '"+name+"'";
  listopt._macrocurlin=im->_line;

  if (fun) {
    startmacrolabel(body,"macro "+name,im->_filename,im->_line);
    body->Process();
    endmacrolabel(body);
    if (stop._stop!=EXITMACRO) stop._retval.clear();
    // check code output => error
  } else {
    startmacrolabel(rs,"macro "+name,im->_filename,im->_line);
    body->Process();
    rs->AddToSource(body->GetSource());
    endmacrolabel(rs);
    // ignore retval silently?
  }

  im->_used=false;
  listopt=olistopt;
  if (stop._stop==EXITMACRO) stop._stop=0;
  macnumtab.setlocation(macnumtabpos);
  defargtab.setlocation(defargtabpos);
  if (fun) line=p;
  else checkjunk(p);
  return 1;
}

int MacroTable::exists(string name) {
  int tr,htr,otr;
  otr=tr=_hash(name);
  while(htr=_hashtable[tr]) {
    if (_mactab[htr]._name==name) return htr;
    if (++tr>=_size) tr=0;
    if (tr==otr) break;
  }
  name=tolower(name);
  otr=tr=_hash(name);
  while(htr=_hashtable[tr]) {
    if (_mactab[htr]._name==name && _mactab[htr]._icase) return htr;
    if (++tr>=_size) tr=0;
    if (tr==otr) break;
  }
  return 0;
}

int MacroTable::_hash(string s) {
  const char *ss=s.c_str();
  unsigned h=0;
  while (*ss) h=(h<<4)+*ss++;
  return h % _size;
}

void MacroTable::_grow() {
  int tr;
  _size=_size*2+1;
  _hashtable.clear();
  _hashtable.resize(_size);
  _mactab.resize(_size);
  for (int i=1;i!=_nextlocation;++i) {
    tr=_hash(_mactab[i]._name);
    while(_hashtable[tr]) if (++tr>=_size) tr=0;
    _hashtable[tr]=i;
  }
}

bool sortminparam(Macro x, Macro y) { 
  return x._minparam < y._minparam;
}

void MacroTable::_macroEntry::add(Macro n_macro) {
  if (_macro.size())
    for (list<Macro>::iterator im=_macro.begin(); im!=_macro.end(); ++im) 
      if (n_macro._maxparam>=im->_minparam && n_macro._maxparam<=im->_maxparam ||
          n_macro._minparam>=im->_minparam && n_macro._minparam<=im->_maxparam) {
        error("Macro already exists"); return;
      }
  _macro.push_back(n_macro);
  _macro.sort(sortminparam);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Structure::addmember(string &n_name,int n_type,int n_length,int n_offset,string &n_default) {
  _memtab.push_back(new _Member(n_name,n_type,n_length,n_offset,n_default));
  _length=n_offset+n_length-_offset;
}

void Structure::emitlabels(const string &label, RawSource *) {
  for (vector<_Member*>::iterator im=_memtab.begin(); im!=_memtab.end(); ++im) {
    if (!(*im)->_name.empty()) {
      string n=label+'.'+(*im)->_name;
      int p=labtab.insert(n,false);
      if (p) labtab.setvalue(p,adres+(*im)->_offset,page);
      else error("Label already exists",n);
    }
  }
}

void Structure::emitdata(string line, Data &e, string label) {
  Data f;
  string s,d;
  int val,offset=_offset;
  for (vector<_Member*>::iterator im=_memtab.begin(); im!=_memtab.end(); ++im) {
    while(offset<(*im)->_offset) { ++offset; e.push(0); }
    if (!label.empty() && !(*im)->_name.empty()) {
      string n;
      if ((*im)->_name[0]=='.') n=label+(*im)->_name; else n=label+'.'+(*im)->_name;
      int p=labtab.getindex(n);
      if (p) labtab.setvalue(p,adres+(*im)->_offset,page);
      else error("Label defined by structure not found?",ERRINTERNAL);
    }

    d.clear();
    switch ((*im)->_type) {
    case 0: break;
    case 1: case 2: case 3: case 4:
    case 5: d=getargument(line); comma(line); break;
    case 7: break;
    default: error("error"); break;
    }
    if (d.empty()) d=(*im)->_default;
    if (d.empty()) {
      switch((*im)->_type) {
      case 0: break;
      case 1: case 2: case 3: case 4:
      case 5: case 7:
        for (int i=0;i!=(*im)->_length;++i) e.push(0);
        break;
      default:
        error("Structure::emitdata",ERRINTERNAL);
        break;
      }
    } else {
      switch((*im)->_type) {
      case 0: break;
      case 1:
        if (!ParseExpression(d,val)) error("Syntax error"); checkjunk(d);
        val=check8(val); e.push((byte)val);
        break;
      case 2:
        if (!ParseExpression(d,val)) error("Syntax error"); checkjunk(d);
        val=check16(val); e.push((byte)val); e.push((byte)(val>>8));
        break;
      case 3:
        if (!ParseExpression(d,val)) error("Syntax error"); checkjunk(d);
        val=check24(val); for (int i=0;i!=3;++i) { e.push((byte)val); val>>=8; }
        break;
      case 4:
        if (!ParseExpression(d,val)) error("Syntax error"); checkjunk(d);
        /*val=check32(val);*/ for (int i=0;i!=4;++i) { e.push((byte)val); val>>=8; }
        break;
      case 5:
        f.clear();
        getbytes(d,f);
        while (f.size()<(*im)->_length) f.push(0);
        if (f.size()>(*im)->_length) error("Too much data");
        e.push(f);
        break;
      default:
        error("No default allowed!");
        break;
      }
    }
    offset+=(*im)->_length;
  }
  checkjunk(line);
}

int StructTable::add(Structure ns) {
  if (_nextlocation>=_size*2/3) _grow();
  if(exists(ns._name)) { error("Structure already exists",ns._name); return 0; }
  int tr,htr;
  tr=_hash(ns._name);
  while(htr=_hashtable[tr]) {
    if (_structtab[htr]._name==ns._name) error("StructTable::add",ERRINTERNAL);
    else if (++tr>=_size) tr=0;
  }
  _hashtable[tr]=_nextlocation;
  _structtab[_nextlocation]=ns;
  ++_nextlocation;
  return 1;
}

int StructTable::emit(string &line, RawSource *rs, const string &label) {
  string p=line;
  int m=exists(getid(p));
  if (!m) return 0;
  if (!label.empty()) _structtab[m].emitlabels(label,rs);
  rs->GetSource().push_back(new StructExp(p,m,label));
  rs->GetSource().back()->Process();
  line.clear();
  return 1;
}

int StructTable::exists(string name) {
  int tr,htr,otr;
  otr=tr=_hash(name);
  while(htr=_hashtable[tr]) {
    if (_structtab[htr]._name==name) return htr;
    if (++tr>=_size) tr=0;
    if (tr==otr) break;
  }
  return 0;
}

int StructTable::_hash(string s) {
  const char *ss=s.c_str();
  unsigned h=0;
  while (*ss) h=(h<<3)+*ss++;
  return h % _size;
}

void StructTable::_grow() {
  int tr;
  _size=_size*2+1;
  _hashtable.clear();
  _hashtable.resize(_size);
  _structtab.resize(_size);
  for (int i=1;i!=_nextlocation;++i) {
    tr=_hash(_structtab[i]._name);
    while(_hashtable[tr]) if (++tr>=_size) tr=0;
    _hashtable[tr]=i;
  }
}

//eof
