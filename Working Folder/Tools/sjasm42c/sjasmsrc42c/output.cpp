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

// output.cpp

#include "sjasm.h"

Rout *routlabel;
vector<Output*> output;
int onr;

int Rout::pool(int d) {
  for (int i=0;i!=_pool.size();++i) if (_pool[i]==d) return _pooladres+4*i;
  _pool.push_back(d);
  return _pooladres+4*(int)_pool.size()-4;
}

void Part::dump(int &adres, StringList &sl) {
  if (adres!=_adres) {
    string s="   "+tohex(adres,8)+"  "+tostr(_adres-adres,6)+"       <empty>";
    sl.push_back(s);
  }
  string s="   "+tohex(_adres,8)+"  "+tostr(_len,6)+"  ";
  switch(_alignmode) {
  case NORMAL: if (_align==1) s+="    "; else s+=tostr(_align,4); break;
  case BYTEADR: s+="   #"; break;
  case FIXEDADR: s+="   @"; break;
  case RANGEADR: s+="  .."; break;
  default: error("Unknown alignmode",ERRFATAL);
  }
  switch(_multipage) {
  case 0: s+="   "; break;
  case 1: s+=" < "; break;
  case 2: s+=" > "; break;
  default: error("Unknown multipagemode",ERRFATAL);
  }
  sl.push_back(s+labtab.getname(_rout->getlabelindex()));
  adres=_adres+_len;
}

void Part::dumpo(StringList &sl) {
  string s="   "+tohex(_adres,8)+"  "+tostr(_len,6)+"  ";
  switch(_alignmode) {
  case FIXEDADR: s+="   @"; break;
  default: error("Unknown alignmode",ERRFATAL);
  }
  sl.push_back(s+"  "+labtab.getname(_rout->getlabelindex()));
}

void Rout::set(string line) {
  int val=0;
  _overlay=false;
  _adrmode=ANYADRES; _minadres=0; _maxadres=BIGVALUE;
  _align=1;
  _byteadr=false;
  _pages.assign(pages.begin(),pages.end());
  _multipage=0;

  switch (line.empty()) {
  case 0:
    if (sbcneed(line,'!')) {
      _overlay=true;
      if (!ParseExpression(line,val)) error("Overlay code part needs a address");
      _minadres=_maxadres=val;
      _adrmode=ATADRES;
      break;
    }
    if (sbcneed(line,'?')) _forgetmode=DOFORGET; else _forgetmode=NEVERFORGET;
    comma(line);
    if (sbcneed(line,'@')) {
      if (sbcneed(line,'*')) {
        _adrmode=ANYADRES; _minadres=0; _maxadres=BIGVALUE;
      } else {
        synerr=0;
        if (ParseExpression(line,val)) _minadres=val; else _minadres=0;
        if (need(line,"..")) {
          if (ParseExpression(line,val)) _maxadres=val; else _maxadres=BIGVALUE;
          _adrmode=ARANGE;
        } else {
          _adrmode=ATADRES; _maxadres=_minadres;
        }
        synerr=1;
      }
      if (!comma(line)) break;
    }
    if (sbcneed(line,'#')) {
      if (cneed(line,'#')) _byteadr=true;
      else if (!ParseExpression(line,_align) || _align<1) { error("Illegal alignment"); _align=1; }
      if (!comma(line)) break;
    }
    skipblanks(line);
    if (cmphstr(line,"page")) {
      getpage(line,_pages);
      break;
    }
    if (cmphstr(line,"pages")) {
      getpage(line,_pages);
      _multipage=1;
      break;
    }
    break;
  default:
    break;
  }

  if ((unsigned)_minadres>(unsigned)_maxadres) { _minadres=_maxadres; error("Invalid addressrange"); }

  if (pass>1) {
    adres=_adres; page=_page;
    if (_adres==-1) adres=output[onr]->getpageorg(_page);
  }  else {
    _page=_pages.front();
    switch(_adrmode) {
    case ANYADRES:
      adres=_adres=output[onr]->getpageorg(_page);
      break;
    case ARANGE:
    case ATADRES:
      adres=_adres=_minadres;
      break;
    default:
      error("illegal addressmode :)",ERRINTERNAL);
    }
  }

  checkjunk(line);
}

int Rout::makepart(bool &overlay, int &cpagm, int &cadrm,int &npage, int &nadres,int &nlen, int &nalign,
                   ALIGNMODE &nalignmode, int &nmultipage) {
  overlay=_overlay;
  cadrm=(int)_adrmode;
//  _forget=false;
  if (!_data.size() || _forget) return 0;
  if (_forgetmode==DOFORGET && pass>2 && _lastused<pass-1) { _forget=true; labsnok=1; return 0; }
  npage=(int)_pages.size();
  if (npage>1) cpagm=1; else cpagm=0;
  nalignmode=NORMAL;
  if (_byteadr) nalignmode=BYTEADR;
  switch (_adrmode) {
  case ATADRES: nadres=_minadres; nalignmode=FIXEDADR; break;
  case ARANGE: nadres=_maxadres-_minadres; if (_align==1) nalignmode=RANGEADR; break;
  case ANYADRES: nadres=BIGVALUE; if (_multipage) cadrm=(int)ANYADRESMULTI; break;
  default: break;
  }
  nmultipage=_multipage;
  if (!_pool.empty()) {
    int a=_adres+_data.size();
    while (a&3) { _data.push(0); ++a; }
    if (_pooladres!=a) { labsnok=1; _pooladres=a; } //
    for (iIntList i=_pool.begin();i!=_pool.end();++i) {
      _data.push(*i); _data.push(*i>>8); _data.push(*i>>16); _data.push(*i>>24);
    }
  }
  nlen=_data.size(); nalign=_align;
  return 1;
}

void Rout::getadres(int &min, int &max) {
  switch (_adrmode) {
  case ATADRES: min=max=_minadres; break;
  case ARANGE: max=_maxadres; min=_minadres; break;
  case ANYADRES: min=0; max=BIGVALUE; break;
  default: break;
  }
}

void Rout::setadres(int n_adres, int n_page) {
  if (n_adres!=_adres) { labsnok=1; _adres=n_adres; }
  if (n_page!=_page) { labsnok=1; _page=n_page; }
}


int align(int adres, int align, int len, ALIGNMODE alignmode) {
  if (alignmode==BYTEADR) {
    if ((adres+len)/256!=adres/256) return (adres+len)&-256;
    return adres;
  }
  if (align==1) return adres;
  if (adres%align) return adres+(align-adres%align); else return adres;
}

void Page::setsize(int n_size) {
  if (_size!=-1) error("Size already set");
  _size=n_size;
}

void Page::setorg(int org) {
  if (_relorg) { if (_org!=org) labsnok=1; _org=org; }
}

int Page::addpart(Part &n_part) {
  int minadres, maxadres,ap;
  list<Part> *lp;
  if (n_part._rout->overlay()) lp=&_opart; else lp=&_part;

  n_part._rout->getadres(minadres,maxadres);
  if (_org>minadres) minadres=_org;
  if (_org>maxadres) return 0;
  if (_size>0 && (_org+_size<minadres+n_part._len)) return 0;
  if (lp->empty()) {
    ap=align(minadres,n_part._align,n_part._len,n_part._alignmode);
    if (ap>maxadres) return 0;
    n_part._page=_pagenr;
    n_part._adres=ap;
    lp->push_back(n_part);
    ap+=n_part._len; if (ap>_highestad) _highestad=ap;
    return 1;
  }
  ap=_org;
  if (ap<minadres) ap=minadres;
  list<Part>::iterator ip=lp->begin();
  while (ip!=lp->end()) {
    if (align(ap,n_part._align,n_part._len,n_part._alignmode)+n_part._len<=ip->_adres) break;
    if (ip->_adres+ip->_len>ap) ap=ip->_adres+ip->_len;
    ++ip;
  }
  ap=align(ap,n_part._align,n_part._len,n_part._alignmode);
  if (_size>0 && (ap+n_part._len>_org+_size)) return 0;
  if (ap>maxadres) return 0;
  n_part._page=_pagenr;
  n_part._adres=ap;
  lp->insert(ip,n_part);
  ap+=n_part._len; if (ap>_highestad) _highestad=ap;
  return 1;
}

bool Page::addfirstpart(Part &n_part,int offset,int size) {
  _part.push_front(Part(n_part._rnr,_pagenr,_org,size,n_part._align,n_part._alignmode,n_part._rout,2));
  _part.begin()->_offset=offset;
  if (_org+size>_highestad) _highestad=_org+size;
  return 1;
}
bool Page::addlastpart(Part &n_part,int adres,int size) {
  n_part._page=_pagenr;
  n_part._adres=adres;
//  n_part._multipage=1;
  n_part._len=size;
  _part.push_back(n_part);
  adres+=n_part._len; if (adres>_highestad) _highestad=adres;
  return 1;
}

void Page::checkparts() {
  for (list<Part>::iterator ip=_part.begin(); ip!=_part.end(); ++ip) {
    if (ip->_multipage!=2) ip->_rout->setadres(ip->_adres,ip->_page);
    list<Part>::iterator ipn=ip; ++ipn;
    if (ipn!=_part.end())
      if (ip->_adres+ip->_len>ipn->_adres) error("Overlapping routines",ERRINTERNAL);
  }
  for (list<Part>::iterator ip=_opart.begin(); ip!=_opart.end(); ++ip) {
    ip->_rout->setadres(ip->_adres,ip->_page);
    list<Part>::iterator ipn=ip; ++ipn;
    if (ipn!=_opart.end())
      if (ip->_adres+ip->_len>ipn->_adres) error("Overlapping routines",ERRINTERNAL);
  }
  _usedsize=_highestad-_org;
  if (_usedsize<0) _usedsize=0;
}

void Page::dump(StringList &sl) {
  int adres=_org;
  string s="  Org: "+tohex(_org,8)+"  Size: ";
  if (_size==-1) s+="*"; else s+=tohex(_size,8);
  s+="  Used: "+tohex(_usedsize,8);
  sl.push_back(s);
  sl.push_back("");
  if (_part.size()) {
    sl.push_back("   Address   Length Align   Label");
    for(list<Part>::iterator ip=_part.begin(); ip!=_part.end(); ++ip) ip->dump(adres,sl);
    if (_size!=-1 && adres!=_org+_size) {
      string s="   "+tohex(adres,8)+"  "+tostr(_org+_size-adres,6)+"       <empty>";
      sl.push_back(s);
    }
  } else {
    if (_size==-1) sl.push_back("    No output");
    else sl.push_back("   "+tohex(_org,8)+"  "+tostr(_size,6)+"       <empty>");
  }
  if (_opart.size()) {
    sl.push_back("");
    sl.push_back("  Overlay parts:");
    sl.push_back("   Address   Length Align  Label");
    for(list<Part>::iterator ip=_opart.begin(); ip!=_opart.end(); ++ip) ip->dumpo(sl);
  }
}

void Page::save(WriteFile &file) {
  int adres=_org;
  list<Part>::iterator ip=_part.begin();
  while (ip!=_part.end()) {
    if (ip->_adres>adres) file.skip(ip->_adres-adres);
    switch(ip->_multipage) {
    case 0: file.write(ip->_rout->getdata()); break;
    case 1: file.write(ip->_rout->getdata(0,ip->_len)); break;
    case 2: file.write(ip->_rout->getdata(ip->_offset,ip->_len)); break;
      break;
    default: error("Oops!",ERRFATAL);
    }
    adres=ip->_adres+ip->_len;
    ++ip;
  }
  if (_size>0 && adres-_org<_size) file.skip(_size-(adres-_org));
}

void Page::saveoverlay(WriteFile &file) {
  int adres=_org;
  list<Part>::iterator ip=_opart.begin();
  while (ip!=_opart.end()) {
    if (ip->_adres>adres) file.skip(ip->_adres-adres);
    file.write(ip->_rout->getdata());
    adres=ip->_adres+ip->_len;
    ++ip;
  }
  if (_size>0 && adres-_org<_size) file.skip(_size-(adres-_org));
}

void Output::defpage(int n_page, int n_org, int n_size) {
  if (n_page+1>=(int)_page.size()) _page.resize(n_page+1);
  if (_page[n_page]) error("Page already exists");
  _page[n_page]=new Page(n_page,n_org, n_size);
}

void Output::setpagesize(int n_size) {
  if (_page.empty()) defpage(0,output[onr]->getorg(),n_size); else _page[page]->setsize(n_size);
}

int Output::getpagesize(int page) {
  if ((unsigned)page>=_page.size()) { error("Page does not exist",tostr(page)); return 0; }
  return _page[page]->getusedsize();
}

void Output::dump(StringList &sl) {
  sl.push_back("");
  sl.push_back(" Output: "+_filename);
  sl.push_back("-------------------------------------------------");
  for(int i=0; i!=_page.size(); ++i) 
    if (_page[i]) { 
      sl.push_back(""); sl.push_back(" Page: "+tohex(i,2));
      _page[i]->dump(sl);
    }
}

bool sortlen(Part x, Part y) {
  return x._len > y._len;
}

bool sortalign(Part x, Part y) {
//  int xa=x._align, ya=y._align;
//  if (!x._byteadr && xa==1) xa=0;
//  if (!y._byteadr && ya==1) ya=0;

  int xa=x._align, ya=y._align;
  if (x._alignmode==BYTEADR) xa=x._len;
  if (y._alignmode==BYTEADR) ya=y._len;

  return xa > ya;
}

bool sortadres(Part x, Part y) {
  return x._adres < y._adres;
}

bool sortpage(Part x, Part y) {
  return x._page < y._page;
}

void Output::sort() {
  bool overlay;
  ALIGNMODE nalignmode;
  int pm,am;
  int i,npage,nadres,nlen,nalign,nmultipart;
  list<Part> oparts;
  list<Part> parts[2][4];
  list<Part> p;

  if (_page.empty()) { defpage(0,output[onr]->getorg(),-1); }  // size???

  for (i=1; i!=_page.size(); ++i) {
    int j=i-1;
    while (j && !_page[j]) --j;
    if (_page[i] && _page[j]) _page[i]->setorg(_page[j]->getorg()+_page[j]->getsize());
  }

  for (i=0; i!=_rout.size(); ++i)
    if (_rout[i] && _rout[i]->makepart(overlay,pm,am,npage,nadres,nlen,nalign,nalignmode,nmultipart))
      if (overlay) oparts.push_back(Part(i,npage,nadres,nlen,nalign,nalignmode,_rout[i],0));
      else parts[pm][am].push_back(Part(i,npage,nadres,nlen,nalign,nalignmode,_rout[i],nmultipart));
  for (pm=0; pm!=2; ++pm) {
    parts[pm][ATADRES].sort(sortadres);
    parts[pm][ARANGE].sort(sortlen);
    parts[pm][ARANGE].sort(sortadres);
    parts[pm][ARANGE].sort(sortalign);
    parts[pm][ANYADRES].sort(sortlen);
    parts[pm][ANYADRES].sort(sortalign);
    parts[pm][ANYADRESMULTI].sort(sortlen);
    parts[pm][ANYADRESMULTI].sort(sortalign);
  }
  for (am=0; am!=4; ++am) {
    parts[0][am].sort(sortpage);
    parts[1][am].sort(sortpage);
  }

  for (am=0; am!=4; ++am)
    for (pm=0; pm!=2; ++pm) 
      p.splice(p.end(),parts[pm][am]);

  for (i=0; i!=_page.size(); ++i) if (_page[i]) _page[i]->reset();

  list<Part>::iterator ip=p.begin();
  while (ip!=p.end()) {
    IntList page;
    if (ip->_alignmode==BYTEADR && ip->_len>256 && pass>2) {
      ip->_rout->geterrlin();
      error("Maximum part size (256) exceeded",labtab.getname(ip->_rout->getlabelindex())); labsnok=1;
    }
    ip->_rout->getpages(page);
    bool ok=false;
    size_t n;
    for (n=0; n!=page.size(); ++n) {
      i=page[n];
      if (i>=(int)_page.size()) break;
      if (!_page[i]) continue;
      if (_page[i]->addpart(*ip)) { ok=true; break; }
      if (ip->_multipage) {
        if (i+1>=(int)_page.size()) break;
        if (!_page[i+1]) continue;
        int ad=align(_page[i]->gethighestad(),ip->_align,ip->_len,ip->_alignmode);
        int psize=_page[i]->getsize()+_page[i]->getorg()-ad;
        if (!psize) continue;
        int size=ip->_len-psize;
        if (size<=_page[i+1]->getfreestartspace()) {
          _page[i]->addlastpart(*ip,ad,psize);
          _page[i+1]->addfirstpart(*ip,psize,size);
          ok=true;
          break;
        }
      }
    }
    if (!ok && pass>2) { 
      ip->_rout->geterrlin();
      listopt._filename=ip->_rout->getsourcefile();
      string es="Part",name=labtab.getname(ip->_rout->getlabelindex());
      if (!name.empty()) es+=" '"+name+"'";
      es+=" does not fit in page";
      if (page.size()>1) {
        es+="s ";
        for (size_t n=0; n!=page.size(); ++n) {
          if (n) es+=", ";
          es+=tostr(page[n]);
        }
      } else {
        es+=" "+tostr(page[0]);
      }
      error(es);
//      error("Part does not fit",labtab.getname(ip->_rout->getlabelindex()));
      labsnok=1;
      listopt._filename.clear();
    }
    ++ip;
  }

  oparts.sort(sortadres);
  ip=oparts.begin();
  while (ip!=oparts.end()) {
    IntList p;
    ip->_rout->getpages(p);
    i=p[0];
    if (!_page[i] || i>(int)_page.size() || !_page[i]->addpart(*ip))
      if (pass>2) { 
        ip->_rout->geterrlin();
        error("Part does not fit",labtab.getname(ip->_rout->getlabelindex())); labsnok=1;
      }
    ++ip;
  }

  for(int i=0; i!=_page.size(); ++i) if (_page[i]) _page[i]->checkparts();
}

void Output::save() {
  if (_rout.size()<2) {
    if (!_rout.size()) {
#ifdef _DEBUG
      error("output leeg?");
#endif
      return;
    }
    if (_rout[0]->getdata().size()==0) return;
  }

  {
    WriteFile file(_filename,_mode);
    if (file.ok()) {
      for(int i=0; i!=_page.size(); ++i) 
        if (_page[i]) _page[i]->save(file); 
        else if (options.allpages) error("Page does not exist",tostr(i));
    }
  }

  {
    WriteFile file(_filename,UPDATE);
    if (file.ok()) for(int i=0; i!=_page.size(); ++i) if (_page[i]) _page[i]->saveoverlay(file);
  }
}

//void Page::addpool(int n_adres, int n_val) {
//  _Pool p(n_val,n_adres);
//  _pool.push_back(p);
//}

//void Output::emitpool(int _val) {
//  if (!_outp) getroutp();
//  _page[page]->addpool(_outp->emitpool(_val),_val);
//}

void Output::reset() {
  //cout << "!!!" << pass << endl;
  //unsigned int pl=0,g=0;
  //if (pass>4) {
  //  for (vector<Rout*>::iterator ip=_rout.begin(); ip!=_rout.end(); ++ip) {
  //    if (pl<_routlen.size() && _routlen[pl]!=(*ip)->getsize()) {
  //      cout << labtab.getname((*ip)->getlabelindex()) << "    " << _routlen[pl] << "    " << (*ip)->getsize() << endl;
  //      ++g;
  //    }
  //    ++pl;
  //  }
  //  if (g) cin >> g;
  //}
  //_routlen.clear();
  //for (vector<Rout*>::iterator ip=_rout.begin(); ip!=_rout.end(); ++ip) _routlen.push_back((*ip)->getsize());


  for(int i=0; i!=_rout.size(); ++i) if (_rout[i]) _rout[i]->reset();
  for(int i=0; i!=_page.size(); ++i) if (_page[i]) _page[i]->reset();
  _rn=0; _outp=0;
}

void resetoutput() {
  for(int i=0; i!=output.size(); ++i) if (output[i]) output[i]->reset();
}

void dumpoutput(StringList &sl) {
  sl.push_back("");
  for(int i=0; i!=output.size(); ++i) if (output[i]) output[i]->dump(sl);
}

void sortoutput() {
  for(int i=0; i!=output.size(); ++i) if (output[i]) output[i]->sort();
}

void saveoutput() {
  for(int i=0; i!=output.size(); ++i) if (output[i]) output[i]->save();
}

void setoutput(int nonr) {
  onr=nonr;
}

//eof
