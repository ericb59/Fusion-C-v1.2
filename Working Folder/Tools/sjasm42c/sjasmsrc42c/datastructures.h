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

// datastructures.h

const int FUNTABSIZE=255;

typedef vector<int> IntList;
typedef IntList::iterator iIntList;
typedef vector<string> StringList;
typedef StringList::iterator iStringList;
typedef string::iterator istring;
typedef unsigned char byte;

class RawSource;
typedef void(*pFun)(string&,RawSource*);
class Data;
typedef void(*mFun)(string,Data&);

void startmacrolabel(RawSource *rs, string name, string file, int line);
void endmacrolabel(RawSource *rs);

class Data {
public:
  Data() : _data(0), _size(0), _maxsize(4)  { _resize(_maxsize); }
  Data(int n_maxsize) : _data(0), _size(0), _maxsize(n_maxsize) { _resize(_maxsize); }
  Data(byte *n_data,int n_size) : _data(0), _size(n_size), _maxsize(n_size) { _resize(n_size); memcpy(_data,n_data,n_size); }
  ~Data() { delete [] _data; }
  void setdata(byte *buffer, int size) { _data=new byte[size]; memcpy(_data,buffer,size); _size=_maxsize=size; }
  void push(byte n_byte) { if (_size>=_maxsize) _grow(_size); _data[_size]=n_byte; ++_size; }
  void push(const Data &n_data) { 
    if (_size+n_data._size>=_maxsize) _grow(_size+n_data._size);
    memcpy(_data+_size,n_data._data,n_data._size);
    _size+=n_data._size;
  }
  byte operator[](int index) const { return _data[index]; }
  byte &operator[](int index) { if (index>=_maxsize) _grow(index); if (_size<=index) _size=index+1; return _data[index]; }
  int size() { return _size; }
  void clear() { _size=0; }
  byte *getdatap(int offset=0) { return _data+offset; }
  bool empty() { return _size==0; }
private:
  byte *_data;
  int _size,_maxsize;
  void _grow(int);
  void _resize(int);
};

class DefineArgTable {
public:
  DefineArgTable() : _nextlocation(1), _size(10) {
    _deftab.resize(_size);
    for (int i=0; i!=64; _used[i++]=false);
  }
  void add(string,string);
  string getrepl(string,int=0);
  int getlocation();
  void setlocation(int);
  void replace(string,string);
private:
  class _DefineArgEntry {
  public:
    _DefineArgEntry() : _name("") {}
    string _name, _repl;
  };
  bool _used[64];
  int _nextlocation,_size;
  vector<_DefineArgEntry> _deftab;
};

class MacNumTable {
public:
  MacNumTable() : _nextlocation(1), _size(20) {
    _numtab.resize(_size);
  }
  void add(int,string);
  string getrepl(int,int=0);
  int getlocation();
  void setlocation(int);
  void replace(int,string);
private:
  class _Entry {
  public:
    _Entry() : _num(0) {}
    int _num;
    string _repl;
  };
  int _nextlocation,_size;
  vector<_Entry> _numtab;
};

class DefineTable {
public:
  DefineTable() : _nextlocation(1), _size(71) {
    _hashtable.resize(_size,0);
    _deftab.resize(_size);
    for (int i=0; i!=64; _used[i++]=false);
  }
  void add(string,string);
  void add(string,string,bool,StringList);
  string getrepl(string);
  string getrepl(string,string&);
  int exists(string,int);
  void undef(string);
private:
  class _DefineEntry {
  public:
    _DefineEntry() : _name(""), _used(false), _args(0)  {}
    string _name, _repl;
    bool _used,_icase;
    StringList _args;
  };
  bool _used[64];
  vector<int> _hashtable;
  int _nextlocation,_size;
  vector<_DefineEntry> _deftab;
  int _hash(string);
  void _grow();
};

template <class T>
class FunctionEntry {
public:
  char *_name;
  T _funp;
};

template <class T, class TT>
class FunctionTable {
public:
  FunctionTable() : _nextlocation(1) {}
  void init(FunctionEntry<T> fune[],int size) {
    int i;
    for (i=0;i!=size;++i) _insert(fune[i]._name,fune[i]._funp);
  }
  int exec(string nname, string &line, TT *container) {
    int tr,htr,otr;
    otr=tr=_hash(nname);
    while(htr=_hashtable[tr]) {
      if (_funtab[htr]._name==nname) { (*_funtab[htr]._funp)(line,container); return 1; }
      if (++tr>=FUNTABSIZE) tr=0;
      if (tr==otr) break;
    }
    return 0;
  }
  T getfunp(string nname) {
    int tr,htr,otr;
    otr=tr=_hash(nname);
    while((htr=_hashtable[tr])) {
      if (_funtab[htr]._name==nname) return _funtab[htr]._funp;
      if (++tr>=FUNTABSIZE) tr=0;
      if (tr==otr) break;
    }
    return 0;
  }
private:
  class _functionEntry {
  public:
    T _funp;
    string _name;
  };
  int _hashtable[FUNTABSIZE],_nextlocation;
  _functionEntry _funtab[FUNTABSIZE];

  int _insert(string nname, T nfunp) {
#ifdef _DEBUG
    if (_nextlocation>=FUNTABSIZE) error("funtab full");
#endif
    int tr,htr;
    tr=_hash(nname);
    while(htr=_hashtable[tr]) {
      if (_funtab[htr]._name==nname) return 0;
      else { if (++tr>=FUNTABSIZE) tr=0; }
    }
    _hashtable[tr]=_nextlocation;
    _funtab[_nextlocation]._name=nname;
    _funtab[_nextlocation]._funp=nfunp;
    ++_nextlocation;
    return 1;
  }
  int _hash(string s) {
    const char *ss=s.c_str();
    unsigned h=0;
    while (*ss) h=(h<<1)+*ss++;
    return h % FUNTABSIZE;
  }
};

class LabelTable {
public:
  LabelTable() : _nextlocation(1), _size(3701) {
    _hashtable.resize(_size,0);
    _labtab.resize(_size);
  }
  int insert(string,bool=false);
  void setvalue(int,int,int,bool=false);
  void setforward(int,bool);
  void getvalue(int,int&,int&);
  int getvalue(string,int&,int&);
  int getindex(string);
  string getname(int);
  void listdump(StringList&);
  void symdump(StringList&);
private:
  class _labelEntry {
  public:
    _labelEntry() : _val(0), _page(0), _set(false),_forward(false),_lastpass(0),_rout(0) { _name.clear(); }
    string _name;
    int _val,_page;
    bool _set,_forward;
    Rout *_rout;
    int _lastpass;
  };
  vector<int> _hashtable;
  int _nextlocation,_size;
  vector<_labelEntry> _labtab;
  int _hash(string);
  void _grow();
};

class NumLabelTable {
public:
  NumLabelTable() {}
  int insert(int,int);
  void setvalue(int,int);
  void getvalue(int,int&);
  int getvalueb(int,int&);
  int getvaluef(int,int&);
  void resetindex() { _index=0; }
private:
  class _numLabelEntry {
  public:
    _numLabelEntry(int,int);
    int _num,_val,_lablin;
  };
  vector<_numLabelEntry> _labtab;
  int _index;
};

class Macro {
public:
  Macro() : _minparam(0),_maxparam(0),_minnum(0),_maxnum(0),_used(false),_recursive(false),_greedy(false),_list(true) {}
  int _minparam,_maxparam;
  int _minnum,_maxnum;
  bool _used,_recursive;
  bool _greedy,_list;
  StringList _body,_namParam,_numDefaults,_namDefaults;
  string _filename;
  int _line;
};

class MacroTable {
public:
  MacroTable() : _nextlocation(1), _size(17) {
    _hashtable.resize(_size,0);
    _mactab.resize(_size);
  }
  void add(string,bool,Macro);
  int emit(string&,RawSource*,bool=false);
  int exists(string);
private:
  class _macroEntry {
  public:
    void add(Macro);
    string _name;
    bool _icase;
    list<Macro> _macro;
  };
  vector<int> _hashtable;
  int _nextlocation,_size;
  vector<_macroEntry> _mactab;
  int _hash(string);
  void _grow();
};

class Structure {
  friend class StructTable;
public:
  Structure() {}
  Structure(string n_name, int n_offset) : _name(n_name), _offset(n_offset) { _memtab.clear(); }
  void addmember(string&,int,int,int,string&);
  void emitlabels(const string&,RawSource*);
  void emitdata(string,Data&,string);
  string getname() { return _name; }
  int getlength() { return _length; }
private:
  struct _Member {
    _Member(string &n_name,int n_type,int n_length,int n_offset,string &n_default)
      :_name(n_name), _type(n_type),_length(n_length), _offset(n_offset), _default(n_default) {}
    string _name;
    int _type, _length, _offset;
    string _default;
  };
  string _name;
  int _length, _offset;
  vector<_Member*> _memtab;
};

class StructTable {
public:
  StructTable() : _nextlocation(1), _size(17) {
    _hashtable.resize(_size,0);
    _structtab.resize(_size);
  }
  int add(Structure);
  int emit(string&,RawSource*,const string& ="");
  int exists(string);
  Structure *get(int n) { return &_structtab[n]; }
private:
  vector<int> _hashtable;
  int _nextlocation,_size;
  vector<Structure> _structtab;
  int _hash(string);
  void _grow();
};

//eof
