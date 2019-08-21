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

// output.h

extern Rout *routlabel;

enum ADRMODE { ATADRES, ARANGE, ANYADRES, ANYADRESMULTI };
enum FORGETMODE { NEVERFORGET, DOFORGET };
enum ALIGNMODE { NORMAL, FIXEDADR, BYTEADR, RANGEADR };

class Rout {
public:
  Rout() : _adres(adres), _pooladres(adres),_page(page), _minadres(adres), _align(1), _overlay(false),
    _lastused(0), _adrmode(ANYADRES), _forgetmode(NEVERFORGET), _forget(false), _labelindex(0), _byteadr(false),
    _curlin(curlin),_listcurlin(listcurlin),_sourcefile(listopt._filename), _maxadres(0), _multipage(false) {
      routlabel=this; _pages.clear(); _pages.push_back(page); _data.clear(); _pool.clear();
  }
  void emit(const Data &e) { _data.push(e); }
  int pool(int d);
  void used() { _lastused=pass; }
  void reset() { _data.clear(); _pool.clear(); }
  int makepart(bool &overlay, int &cpagm, int &cadrm, int &npage, int &nadres, int &nlen, int &nalign, ALIGNMODE &nalignmode, int &nmultipart);
  void getadres(int &min, int &max);
  void getpages(vector<int> &p) { p=_pages; }
  bool overlay() { return _overlay; }
  void set(string line);
  void setadres(int n_adres, int n_page);
  Data &getdata() { return _data; }
  Data &getdata(int offset, int length) { Data *datap=new Data(_data.getdatap(offset),length); return *datap; }
  bool forget() { return _forget; }
  int getlabelindex() { return _labelindex; }
  void setlabelindex(int index) { _labelindex=index; }
  void geterrlin() { curlin=_curlin; listcurlin=_listcurlin; } // :(
  string &getsourcefile() { return _sourcefile; }
  IntList &getpool() { return _pool; }

//  unsigned int getsize() { return _data.size(); } //##
private:
  int _adres, _page, _lastused,_pooladres,_multipage;
  bool _forget,_overlay, _byteadr;
  int _minadres, _maxadres, _align;
  IntList _pages;
  ADRMODE _adrmode;
  FORGETMODE _forgetmode;
  Data _data;
  IntList _pool;
  int _labelindex,_curlin,_listcurlin;
  string _sourcefile;
};

class Part {
public:
  Part(int n_rnr, int n_page, int n_adres, int n_len, int n_align, ALIGNMODE n_alignmode, Rout *n_rout, 
    int n_multipage)
    : _rnr(n_rnr), _page(n_page), _adres(n_adres), _len(n_len), _align(n_align),
      _alignmode(n_alignmode), _rout(n_rout), _multipage(n_multipage) {}
  void dump(int&,StringList&);
  void dumpo(StringList&);
  ALIGNMODE _alignmode;
  int _page,_adres,_len,_align,_rnr;
  int _multipage,_offset;
  Rout *_rout;
};

class Page {
public:
  Page(int n_pagenr, int n_org, int n_size) :
      _pagenr(n_pagenr), _org(n_org), _size(n_size), _usedsize(0), _highestad(0) {
        _part.clear(); _opart.clear(); /*_pool.clear();*/
        _relorg=(_org==-1);
      }
  void setsize(int);
  int getusedsize() { return _usedsize; }
  int gethighestad() { return _highestad; }
  int getsize() { if (_size==-1) return _usedsize; return _size; }
  int getfreestartspace() {
    if (_part.empty()) if (_size==-1) return BIGVALUE; else return _size;
    return _part.begin()->_adres-_org;
  }
  int getorg() { return _org; }
  void setorg(int org);
  void reset() { _part.clear(); _opart.clear(); _highestad=0; }
  int addpart(Part &n_part);
  bool addfirstpart(Part &n_part,int offset,int size);
  bool addlastpart(Part &n_part,int adres,int size);
//  void addpool(int,int); //
  void checkparts();
  void dump(StringList&);
  void save(WriteFile &file);
  void saveoverlay(WriteFile &file);
private:
//  struct _Pool { 
//    _Pool (int n_data,int n_adres) : _data(n_data), _adres(n_adres) {}
//    int _data, _adres;
//  };
  int _pagenr, _org, _size, _highestad, _usedsize;
  bool _relorg;
  list<Part> _part;
  list<Part> _opart;
//  list<_Pool> _pool;
};

class Output {
public:
  Output(string n_filename,FILEMODE n_mode=OVERWRITE) :
      _filename(n_filename), _mode(n_mode), _rn(0), _outp(0), _org(-1) { _page.clear(); _rout.clear(); }
  Rout *getroutp() { if (_rout.empty()) { _rout.push_back(new Rout()); _rn=0; } return _outp=_rout[_rn]; }
  Rout *getnewroutp() { _rout.push_back(new Rout()); _rn=(int)_rout.size()-1; return _outp=_rout[_rn]; }
  int getrout() { return _rn; }
  void setrout(int n_rn) { _outp=_rout[_rn=n_rn]; }
  void defpage(int,int,int);
  void setpagesize(int);
  int getpagesize(int);
  void setorg(int n_org) { if (_org==-1) _org=n_org; }
  int getorg() { if (_org==-1) return 0; return _org; }
  int getpageorg(int p) { if (_page.empty() || (unsigned)p>_page.size()) return 0; return _page[p]->getorg(); }
  int pageok(int np) { if (_page.empty() || (unsigned)np>=_page.size()) return 0; return _page[np]!=0; }
  void emit(Data &e) { if (!e.size()) return; if (!_outp) getroutp(); _outp->emit(e); adres+=e.size(); }
  int pool(int d) { if (!_outp) getroutp(); return _outp->pool(d); }
  void emitblock(int d, int len) { Data e; while (len--) e.push((byte)d); emit(e); }
  void reset();
  void dump(StringList&);
  void sort();
  void save();
private:
  string _filename;
  FILEMODE _mode;
  vector<Page*> _page;
  vector<Rout*> _rout;
  int _rn, _org;
  Rout *_outp;

//  vector<unsigned int> _routlen;  //##
};

void resetoutput();
void sortoutput();
void dumpoutput(StringList&);
void saveoutput();
void setoutput(int);

//eof
