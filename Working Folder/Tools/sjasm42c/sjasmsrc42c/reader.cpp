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

// reader.cpp

#include "sjasm.h"

int cmphstr(string &p1, string p2) {
  int i=0;
  if (isupper(p1[i])) 
    while (p2[i]) { 
      if (p1[i]!=toupper(p2[i])) return 0; 
      ++i; 
    }
  else 
    while (p2[i]) { 
      if (p1[i]!=p2[i]) return 0; 
      ++i; 
    }
  if (p1[i]>' ') return 0;
  p1=p1.substr(i);
  return 1;
}

int getstringlength(string &s) {
  int p=0;
  if (s[0]=='"') {
    for(;;) {
      ++p; if (!s[p]) break;
      if (s[p]=='\\') {
        if (s[p+1]=='"') { ++p; continue; }
        if (s[p+1]=='\\') { ++p; continue; }
      }
      if (s[p]=='"') { ++p; break; }
    }
  } else if (s[0]=='\'') {
    for(;;) {
      ++p; if (!s[p]) break;
      if (s[p]=='\'') { ++p; break; }
    }
  } else {
    error("getstringlength",ERRINTERNAL);
  }
  return p;
}

string tolower(string b) {
  char *bp,*p;
  const char *op=b.c_str();
  if (b.length()+1>64) bp=p=(char*)malloc(b.length()+1);
  else bp=p=tobuffer;
  while(*p++=(char)tolower(*op)) ++op;
  return bp;
}

inline void skipblanks(string &p) {
  int pos=(int)p.find_first_not_of(' ');
  if (pos==string::npos) p.clear();
  else p=p.substr(pos);
}

int comma(string &p) {
  skipblanks(p);
  if (p[0]!=',') return 0;
  p.erase(0,1); return 1;
}

int needcomma(string &p) {
  if (comma(p)) return 1;
  error("Comma expected");
  return 0;
}

Find fspacedot(" .");

string getinstructionpart(string &p) {
  int pos=fspacedot.find(p),u=0;
  string d=p.substr(0,pos);
  if (pos!=string::npos) p=p.substr(pos); else p.clear();
  for (istring i=d.begin(); i!=d.end(); ++i) if ((*i)==toupper(*i)) ++u;
  if (d.size()==u) return tolower(d);
  return d;
}

string getinstruction(string &p) {
  string d=getinstructionpart(p);
  if (!p.empty()) if (p[0]=='.') d+='.';
  return d;
}

Find fid("qwertyuioplkjhgfdsazxcvbnmQWERTYUIOPLKJHGFDSAZXCVBNM1234567890_");

string getid(string &p) {
  skipblanks(p);
  if (!isalpha(p[0]) && p[0]!='_') return "";
  int pos=fid.findnot(p);
  string d=p.substr(0,pos);
  if (pos!=string::npos) p=p.substr(pos); else p.clear();
  return d;
}

void checkjunk(string &s) {
  skipblanks(s);
  if (s.empty()) return;
#ifdef _DEBUG
  error("Unexpected",s);
#else
  error("Unexpected",s,ERRREP);
#endif
  s.clear();
}

char nextchar(istring const &p, string const &s) {
  if ((p+1)==s.end()) return 0;
  return *(p+1);
}

int sbcneed(string &p, char c) {
  skipblanks(p); if (p[0]!=c) return 0;
  p.erase(0,1); return 1;
}

int cneed(string &p, char c) {
  if (p[0]!=c) return 0;
  p.erase(0,1); return 1;
}

int needa(string &p, string c1, int r1, string c2, int r2, string c3, int r3) {
  if (!isalpha(p[0])) return 0;
  if (cmphstr(p,c1)) return r1;
  if (!c2.empty() && cmphstr(p,c2)) return r2;
  if (!c3.empty() && cmphstr(p,c3)) return r3;
  return 0;
}

int need(string &p, char *c) {
  skipblanks(p);
  while (*c) {
    if (p[0]!=*c) { c+=2; continue; }
    ++c;
    if (*c==' ') { p.erase(0,1); return *(c-1); }
    if (*c=='_' && p[1]!=*(c-1)) { p.erase(0,1); return *(c-1); }
    if (p[1]==*c) { p=p.substr(2); return *(c-1)+*c; }
    ++c;
  }
  return 0;
}

int needequ(string &lp) {
  string olp=lp;
  skipblanks(lp);
  if (lp[0]=='=') { lp.erase(0,1); return 1; }
  if (cmphstr(lp,"equ")) return 1;
  lp=olp;
  return 0;
}

int needfield(string &lp) {
  string olp=lp;
  skipblanks(lp);
  if (lp[0]=='#' && lp[1]!='#') { lp.erase(0,1); return 1; }
  lp=olp;
  return 0;
}

int needset(string &lp) {
  string olp=lp;
  skipblanks(lp);
  if (lp[0]==':' && lp[1]=='=') { lp=lp.substr(2); return 1; }
  lp=olp;
  return 0;
}

int islabchar(char p) {
  if (isalnum(p) || p=='_' || p=='.') return 1;
  return 0;
}

void checkparen(string &s) {
  int t=0,p=0;
  while (1) {
    switch (s[p]) {
    case 0: return;
    case '(': ++t; break;
    case ')': if (t==1) { ++p; s=s.substr(p); skipblanks(s); return; } if (!--t) return; break;
    case '\'': for(;;) { ++p; if (!s[p]) return; if (s[p]=='\'') break; } break;
    case '"': for(;;) { ++p; if (!s[p]) return; if (s[p]=='\\' && s[p+1]=='"') { ++p; continue; } if (s[p]=='"') break; } break;
    default: break;
    }
    ++p;
  }
}

int check8(int val) {
  unsigned int v=val;
  if (v!=(v&255) && ~v>127) errorvalue();
  return val & 255;
}

int check5(int val) {
  unsigned int v=val;
  if (v!=(v&31) && ~v>31) errorvalue();
  return val & 31;
}

int check3u(int val) {
  unsigned int v=val;
  if (v>7) errorvalue();
  return val & 7;
}

int check8u(int val) {
  unsigned int v=val;
  if (v>255) errorvalue();
  return val & 255;
}

int checki(int val) {
  if (val<-128 || val>127) errorvalue("Offset out of range");
  return val & 255;
}

int check16(int val) {
  unsigned int v=val;
  if (v!=(v&65535) && ~v>32767) errorvalue();
  return val & 65535;
}

int check24(int val) {
  unsigned int v=val;
  if (v!=(val&16777215) && ~v>8388607) errorvalue();
  return val & 16777215;
}

void makestring(string &s) {
  string res;
  for(istring i=s.begin();i!=s.end();++i) { if (*i=='"' || *i=='\\') res+='\\'; res+=*i; }
  s=res;
}

void getstring(string &s,string &e) {
  e.clear();
  skipblanks(s);
  if (s.empty()) { error("String expected"); return; }
  if (s[0]=='\'') {
    for(;;) {
      s.erase(0,1);
      if (s.empty()) break;
      if (s[0]=='\'') { s.erase(0,1); break; }
      e.push_back(s[0]);
    }
  } else if (s[0]=='"') {
    s.erase(0,1);
    do {
      int val;
      if (s.empty() || s[0]=='"') { error("Syntax error",ERRREP); break; }
      getcharconstchar(s,val); e.push_back((byte)val);
    } while (s[0]!='"');
    s.erase(0,1);
  } else
    error("String expected");
}

void getcharconstchar(string &op, int &val) {
  val=op[0]; op.erase(0,1);
  if (val!='\\') return;
  val=op[0]; op.erase(0,1);
  switch (val) {
  case '\\': case '\'': case '\"': case '\?':
    return;
  case 'n': case 'N': val=10; return;
  case 't': case 'T': val=9; return;
  case 'v': case 'V': val=11; return;
  case 'b': case 'B': val=8; return;
  case 'r': case 'R': val=13; return;
  case 'f': case 'F': val=12; return;
  case 'a': case 'A': val=7; return;
  case 'e': case 'E': val=27; return;
  case 'd': case 'D': val=127; return;
  default: val='\\'; error("Unknown escape"); return;
  }
}

StringList getarguments(string &p) {
  int pos,haakjes=0,heind,accolade=0;
  StringList a;
  string res;
  if (heind=(p[0]=='(')) p.erase(0,1);
  skipblanks(p);
  for(;;) {
    switch (p[0]) {
    case 0:
      if (heind) error("Closing ) expected");
      if (!res.empty()) a.push_back(res); return a;
    case '(':
      ++haakjes; break;
    case ')':
      if (!haakjes-- && heind) { a.push_back(res); p.erase(0,1); return a; } 
      break;
    case '\'':
    case '"':
      pos=getstringlength(p); res+=p.substr(0,pos); p=p.substr(pos);
      continue;
    case ',':
      if (!accolade) { a.push_back(res); res.clear(); p.erase(0,1); skipblanks(p); continue; }
      break;
    case '{':
      if (!accolade++) { p.erase(0,1); continue; }
      break;
    case '}':
      if (accolade--==1) { p.erase(0,1); continue; }
      if (accolade<0) accolade=0;
      break;
    default:
      break;
    }
    res+=p[0]; p.erase(0,1);
  }
//  return a;
}

string getargument(string &p, bool greedy, bool heind) {
  int pos,accolade=0,haakjes=0;
  string res;
  skipblanks(p);
  for(;;) {
    switch (p[0]) {
    case 0: return res;
    case '\'':
    case '"':
      pos=getstringlength(p); res+=p.substr(0,pos); p=p.substr(pos);
      continue;
    case ',':
      if (!accolade && !greedy) return res;
      break;
    case '{':
      if (!accolade++) { p.erase(0,1); continue; }
      break;
    case '}':
      if (accolade--==1) { p.erase(0,1); continue; }
      if (accolade<0) accolade=0;
      break;
    case '(': 
      ++haakjes;
      break;
    case ')':
      if (!haakjes-- && heind) return res;
      break;
    default:
      break;
    }
    res+=p[0]; p.erase(0,1);
  }
}

string tohex(int n, int w) {
  ostringstream s;
  s.width(w);
  s.fill('0');
  s.flags(ios::hex|ios::uppercase);
  s << n;
  return s.str();
}

int getinSTRUCTion(string &s) {
  string os=s;
  skipblanks(s);
  string n=getinstruction(s);
  if (n.empty()) { s=os; return 0; }
  switch (n.size()) {
  case 1:
    if (n[0]=='#') return 7;
    break;
  case 2:
    if (n[0]=='#' && n[1]=='#') return 6;
    if (n[0]=='d') {
      if (n[1]=='b') return 1;
      if (n[1]=='w') return 2;
      if (n[1]=='t') return 3;
      if (n[1]=='d') return 4;
      if (n[1]=='s') return 5;
    }
    break;
  case 4:
    if (n=="byte") return 1;
    if (n=="word") return 2;
    break;
  case 5:
    if (n=="dword") return 4;
    if (n=="align") return 6;
    break;
  default:
    break;
  }
  error("Syntax error");
  s=os;
  return 0;
}

string trim(string s) {
  if (s[0]==' ') skipblanks(s);
  return s=s.substr(0,s.find_last_not_of(' ')+1);
}

void getpage(string &s, IntList &p) {
  int min,max,i,v;
  p.clear();
  synerr=0;
  while(1) {
    if (ParseExpression(s,v)) min=v; else min=0;
    if ((unsigned)min>255) { min=255; error("Invalid page number"); }
    if (need(s,"..")) {
      if (ParseExpression(s,v)) max=v; else max=255;
      if ((unsigned)max>255) { max=255; error("Invalid pagerange"); }
      if ((unsigned)min>(unsigned)max) { min=max; error("Invalid pagerange"); }
      for (i=min; i!=max+1; ++i) p.push_back(i);
    } else {
      p.push_back(min);
    }
    if (!comma(s)) break;
  }
  synerr=1;
  iIntList ii=p.begin();
  while (ii!=p.end())
    if (!output[onr]->pageok(*ii)) { 
      error("Page does not exist",tostr(*ii)); 
      ii=p.erase(ii);
    }
    else ++ii;
  if (!p.size()) p.push_back(0);
}

//eof
