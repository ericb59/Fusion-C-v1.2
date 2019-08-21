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

// source.h

class SourceNode;

enum { NOLIST, LISTLINE, LISTDATA, LISTINFO, LISTPOOL };

typedef list<SourceNode*> SourceList;
typedef SourceList::iterator iSourceList;

struct phaseinfo {
  int _phase;
  int _curlin,_listcurlin;
  string _filename;
};

extern stack<phaseinfo> phasestack;
extern vector<int> repeatstack;

void process(SourceList sl);
void listlistlinez80(int curlin, int page, int adr, string &line, Data &e, string &i);
void listlistline8(int curlin, int page, int adr, string &line, Data &e, string &i);
void listlistline16(int curlin, int page, int adr, string &line, Data &e, string &i);
void listlistline32(int curlin, int page, int adr, string &line, Data &e, string &i);
void listlist(SourceList &sl, StringList &dl);
void outputsymbols();
//void initsource(SourceList &sl);

typedef void (*pListMode)(int,int,int,string&,Data&,string&);
extern pListMode plist;

struct ListOpt {
  void init() { _list=true; _char=' '; _include=-1; _inc=true; _filename.clear(); _path.clear(); _macrocurlin=-1; }
  void addlist(SourceList&,string);
  bool _list;
  int _include;
  int _curlin;
  char _char;
  bool _inc;
  string _filename;
  string _path;
//  bool _forget;    // uhm, I forgot what this should do ;)
  string _macroname;
  string _macrofilename;
  int _macrocurlin;
};

extern ListOpt listopt;

class SourceNode {
public:
  SourceNode() : _listtype(NOLIST) {}
  virtual int getlisttype() { return _listtype; }
  virtual string listline() { return ""; }
  virtual Data &listdata() { static Data e; return e; }
  virtual void Process()=0;
protected:
  int _listtype;
};

class ListMode : public SourceNode {
public:
  ListMode(pListMode n_plist) : _plist(n_plist) {}
  void Process() { plist=_plist; }
private:
  pListMode _plist;
};

class SourceListLine : public SourceNode {
public:
  SourceListLine(string n_line) : _line(n_line) { ++listcurlin; _listtype=LISTLINE; }
  void Process() { ++listcurlin; }
  string listline() { return _line; }
private:
  string _line;
};

class SourceListLineInc : public SourceNode {
public:
  SourceListLineInc(string n_line) : _line(n_line) { ++listcurlin; ++curlin; _listtype=LISTLINE; }
  void Process() { ++listcurlin; ++curlin; }
  string listline() { return _line; }
private:
  string _line;
};

class SourceListLineMacroInc : public SourceNode {
public:
  SourceListLineMacroInc(string n_line) : _line(n_line) { ++listcurlin; ++listopt._macrocurlin; _listtype=LISTLINE; }
  void Process() { ++listcurlin; ++listopt._macrocurlin; }
  string listline() { return _line; }
private:
  string _line;
};

class SourceMacroInc : public SourceNode {
public:
  SourceMacroInc() { ++listopt._macrocurlin; }
  void Process() { ++listopt._macrocurlin; }
private:
};

class AdresLabel : public SourceNode {
public:
  AdresLabel(int n_location) : _location(n_location) { _listtype=LISTINFO; }
  void Process();
private:
  int _location;
  int _p,_a;
};

class NumberLabel : public SourceNode {
public:
  NumberLabel(int index) : _index(index) {}
  void Process();
  int _index;
};

class FieldLabel : public SourceNode {
public:
  FieldLabel(int n_location, string n_expr) : _location(n_location), _expr(n_expr) { _listtype=LISTINFO; }
  void Process();
  string listline() { return '('+tohex(_p,2)+':'+tohex(_a)+')'; }
private:
  string _expr;
  int _location;
  int _p,_a;
};

class EquLabel : public SourceNode {
public:
  EquLabel(int n_location, string n_expr) : _location(n_location), _expr(n_expr) { _listtype=LISTINFO; }
  void Process();
  string listline() { return '('+tohex(_p,2)+':'+tohex(_a)+')'; }
private:
  string _expr;
  int _location;
  int _p,_a;
};

class SetLabel : public SourceNode {
public:
  SetLabel(int n_location, string n_expr) : _location(n_location), _expr(n_expr) { _listtype=LISTINFO; }
  void Process();
  string listline() { return '('+tohex(_p,2)+':'+tohex(_a)+')'; }
private:
  string _expr;
  int _location;
  int _p,_a;
};

class Mnemonic : public SourceNode {
public:
  Mnemonic() {}
  Mnemonic(string n_line, mFun n_funp) : _line(n_line), _funp(n_funp) { _listtype=LISTDATA; }
  Data &listdata() { return _e; }
  void Process();
protected:
  string _line;
  Data _e;
  mFun _funp;
};

class dOrg : public SourceNode {
public:
  dOrg (string n_line) : _line(n_line) { _listtype=LISTINFO; }
  void Process();
private:
  string _line;
  int _listinfo;
};

class dRout : public SourceNode {
public:
  dRout(int n_rnr, Rout *n_rout, string n_line) : _rnr(n_rnr), _page(page), _rout(n_rout), _line(n_line) { _listtype=LISTINFO; }
  void Process();
private:
  int _page, _rnr;
  Rout *_rout;
  int _p,_a;
  string _line;
};

class dPool : public SourceNode {
public:
  dPool() { _listtype=LISTPOOL; }
  Data &listdata() { return _e; }
  void Process();
protected:
  Data _e;
};

class dPage : public SourceNode {
public:
  dPage(int n_rnr, Rout *n_r, string n_line) : _line(n_line), _drout(n_rnr, n_r, "") { _listtype=LISTINFO; }
  void Process();
  string listline() { return '('+tohex(_listinfo,2)+')'; }
private:
  string _line;
  bool _infook;
  int _listinfo;
  dRout _drout;
};

class dMapAlign : public SourceNode {
public:
  dMapAlign(string n_line) : _line(n_line) { _listtype=LISTINFO; }
  void Process();
  string listline() { return '('+tohex(_listinfo)+')'; }
private:
  string _line;
  int _listinfo;
};

class dMap : public SourceNode {
public:
  dMap(string n_line) : _line(n_line) { _listtype=LISTINFO; }
  void Process();
  string listline() { return '('+tohex(_listinfo)+')'; }
private:
  string _line;
  int _listinfo;
};

class dEndmap : public SourceNode {
public:
  dEndmap() { _listtype=LISTINFO; }
  void Process();
  string listline() { return '('+tohex(_listinfo)+')'; }
private:
  int _listinfo;
};

class dAlign : public SourceNode {
public:
  dAlign(string n_line) : _line(n_line) { _listtype=LISTINFO; }
  void Process();
  string listline() { return '('+tohex(_listinfo)+')'; }
private:
  string _line;
  int _listinfo;
};

class dPhase : public SourceNode {
public:
  dPhase(string n_line) : _line(n_line) { _listtype=LISTINFO; }
  void Process();
  string listline() { return '('+tohex(_listinfo)+')'; }
private:
  string _line;
  int _listinfo;
};

class dDephase : public SourceNode {
public:
  dDephase() { _listtype=LISTINFO; }
  void Process();
  string listline() { return '('+tohex(_listinfo)+')'; }
private:
  int _listinfo;
};

class StartMacro : public SourceNode {
public:
  StartMacro(string n_lab, string n_name, string n_file, int n_line) : 
      _lab(n_lab), _name(n_name), _file(n_file), _line(n_line) {}
  void Process();
private:
  string _lab,_name,_file;
  int _line;
};

class EndMacro : public SourceNode {
public:
  EndMacro() {}
  void Process();
private:
};

class SetModule : public SourceNode {
public:
  SetModule(string n_lab) : _lab(n_lab) {}
  void Process();
private:
  string _lab;
};

class VorNamLabel : public SourceNode {
public:
  VorNamLabel(string n_lab) : _lab(n_lab) {}
  void Process() { vorlabp=_lab; }
private:
  string _lab;
};

class SetListOpt : public SourceNode {
public:
  SetListOpt(ListOpt n_listopt) : _listopt(n_listopt) {}
  void Process();
private:
  ListOpt _listopt;
};

class piDS : public SourceNode {
public:
  piDS(string n_line) : _line(n_line) { _listtype=LISTINFO; }
  void Process();
  string listline() { return tohex(_b,2)+" ("+tostr(_n)+')'; }
private:
  string _line;
  int _b,_n;
};

class dIncbin : public SourceNode {
public:
  dIncbin(string n_line, bool list);
  void Process();
  string listline() { return '('+tohex(_listinfo)+')'; }
  Data &listdata();
private:
  string _line;
  Data _data,_e;
  int _listinfo;
  int _listdata;
};

class SetOutput : public SourceNode {
public:
  SetOutput(int n_onr) : _onr(n_onr) {}
  void Process();
private:
  int _onr;
};
/*
class SetList : public SourceNode {
public:
  SetList(bool n_list) : _list(n_list) {}
  void Process() { listopt._list=_list; }
private:
  bool _list;
};
*/
class StructExp : public SourceNode {
public:
  StructExp(string n_line, int n_structure, string n_label)
    : _line(n_line), _structure(n_structure), _label(n_label) { _listtype=LISTDATA; }
  Data &listdata() { return _e; }
  void Process();
private:
  Data _e;
  string _line,_label;
  int _structure;
};

class Repeat : public SourceNode {
public:
  Repeat(string count, SourceNode *sn) : _count(count), _sn(sn) {}
  int getlisttype() { return _sn->getlisttype(); }
  string listline() { return _sn->listline(); }
  Data &listdata() { return _e; }
  void Process();
protected:
  string _count;
  SourceNode *_sn;
  Data _e;
};

class dAssert : public SourceNode {
public:
  dAssert(string n_line) : _line(n_line) {}
  void Process();
private:
  string _line;
};

//eof
