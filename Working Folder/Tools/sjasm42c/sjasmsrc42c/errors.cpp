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

// errors.cpp

#include "sjasm.h"

ErrorTable errtab;
int lasterror=-1;

void error(int line, int listline, string e, string a, int i) { 
  if (i==ERRREP && lasterror==listline) return;

  again=0;
  lasterror=listline;
  string msg;

  //if (!listopt._macroname.empty()) {
  //  msg="from "+listopt._macroname;
  //  if (listopt._macrofilename!=listopt._filename) msg+=" in "+listopt._macrofilename;
  //  if (listopt._macrocurlin) msg+=" line "+tostr(listopt._macrocurlin);
  //  errtab.add(listline,msg,i);
  //}

  //if (line) msg=listopt._filename+" line "+tostr(line)+": "+e; else msg=e;
  //if (!a.empty()) msg+=": "+a;
  //if (errtab.add(listline,msg,i)) ++errors;

  if (line) msg=listopt._filename+"("+tostr(line)+") : "+e; else msg=e;
  if (!a.empty()) msg+=": "+a;
  if (errtab.add(listline,msg,i)) ++errors;

  if (!listopt._macroname.empty()) {
    msg="\t"+listopt._macrofilename+"("+tostr(listopt._macrocurlin)+") : see "+listopt._macroname;
    errtab.add(listline,msg,i);
  }

  if (i==ERRFATAL || i==ERRINTERNAL) {
//#ifdef _DEBUG
    cout << msg << endl;
    cout << "FATAL ERROR - PRESS ENTER" << endl;
    cin.get();
//#endif
    exit(1);
  }
}

void error(string e, string a, int i) { error(curlin,listcurlin,e,a,i); }
void error(string e,int i) { error(curlin,listcurlin,e,"",i); }
void error1(string e, string a, int i) { error(curlin,listcurlin,e,a,i); }
void error1(string e,int i) { error(curlin,listcurlin,e,"",i); }

void errorvalue() { errorvalue("Value out of range"); }

void errorvalue(string n_msg) {
  if (again && !lpass) return;
  error(n_msg,ERRNORMAL);
}

void errortarget(string n_msg) {
  if (!lpass || errors) { again=1; return; }
  int oerrors=errors;
  error(n_msg,ERRNORMAL);
  if (oerrors!=errors) { --errors; ++adrerrors; }
}

void errortarget(int n_offset) {
  errortarget("Target out of range ("+tostr(n_offset)+")");
}

int ErrorTable::add(int n_line,string n_msg,int n_type) {
  if (_nextlocation>=_size*2/3) _grow();
  int tr,htr;
  tr=_hash(n_line);
  while(htr=_hashtable[tr]) {
    if (_errtab[htr]._line==n_line && pass>2 || _errtab[htr]._msg==n_msg) return 0;
    if (++tr>=_size) tr=0;
  }
  _hashtable[tr]=_nextlocation;
  _errtab[_nextlocation]._line=n_line;
  _errtab[_nextlocation]._msg=n_msg;
  _errtab[_nextlocation]._type=n_type;
  ++_nextlocation;
  return 1;
}

void ErrorTable::geterrors(int linenr, StringList &list) {
  int e,p=_hash(linenr);
  while (e=_hashtable[p]) {
    if (_errtab[e]._line==linenr) {
      cout << _errtab[e]._msg << endl;
      list.push_back(_errtab[e]._msg);
    }
    if (++p>=_size) p=0;
  }
}

void ErrorTable::_grow() {
  int tr;
  _size=_size*2+1;
  _hashtable.clear();
  _hashtable.resize(_size);
  _errtab.resize(_size);
  for (int i=1;i!=_nextlocation;++i) {
    tr=_hash(_errtab[i]._line);
    while(_hashtable[tr]) if (++tr>=_size) tr=0;
    _hashtable[tr]=i;
  }
}

//eof
