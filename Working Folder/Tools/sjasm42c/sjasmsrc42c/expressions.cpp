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

// expressions.cpp

#include "sjasm.h"

Find findlabelchar("qazxswedcvfrtgbnhyujmkiolpQWERTYUIOPLKJHGFDSAZXCVBNM0123456789_.");
Find findconstantchar("_0123456789abcdefhoqABCDEFHOQ");

int getLabelValue(string &ns, int &nval, int &npag) {
  string mlp=maclabp;
  int pos,glb=0,loc=0;
  string s=ns,n,nn;
  again=1;
  if (!mlp.empty()) {
    if (cneed(s,'@')) mlp.clear();
    else if (need(s,".@")) {
      do {
        loc=(int)mlp.find_last_of('.');
        if (loc!=string::npos) mlp=mlp.substr(0,loc);
        else { mlp.clear(); /*error("No nested macro");*/ break; } // bug! .@.label wordt .label (moet ..label zijn)
      } while (cneed(s,'@'));
      loc=1;
    }
  }
  if (cneed(s,'.')) loc=1;
  else if (cneed(s,'@')) glb=1;
  pos=findlabelchar.findnot(s);
  n=s.substr(0,pos);
  if (!isalpha(n[0]) && n[0]!='.' && n[0]!='_') { error("Illegal label name",s); return 0; }
  if (pos!=string::npos) s=s.substr(pos); else s.clear();
  ns=s;

  if (!mlp.empty() && loc) { 
    while(1) {
      if (labtab.getvalue(mlp+'>'+n,nval,npag)) return 1;
      loc=(int)mlp.find_last_of('.');
        if (loc!=string::npos) mlp=mlp.substr(0,loc); else break;
    }
    loc=1;
  }

  if (loc) nn=vorlabp+'.'+n; else nn=n;
  if (!glb && !modlabp.empty()) nn=modlabp+'.'+nn;
  
  if (labtab.getvalue(nn,nval,npag)) return 1;
  if (!loc && !glb && labtab.getvalue(n,nval,npag)) return 1;
  nval=0; labelnotfound=1; 
  if (pass==1) labsnok=1; else error("Label not found",nn);
  return 1;
}

int getval(string &p, int &nval, unsigned int base, int len) {
  unsigned int nv,ov,v;
  int pos=0;
  if (len==-1) len=(int)p.length();
  nv=0;
  while (len--) {
    switch (p[pos]) {
    case '_':
      v=(unsigned)-1; break;
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      v=p[pos]-'0'; break;
    default:
      if (isupper(p[pos])) v=p[pos]-'A'+10;
      else if (islower(p[pos])) v=p[pos]-'a'+10;
      else { len=0; v=(unsigned)-1; --pos; }
      break;
    }
    if (v!=-1) {
      if (v>base) { error("Digit not in base",p); return 0; }
      ov=nv; nv=nv*base+v;
      if (ov>nv) error("Overflow");
    }
    ++pos;
  }
  nval=nv;
  p=p.substr(pos);
  return 1;
}

int getConstant(string &p, int &nval) {
  int c;
  int pos,base=10;
  switch (p[0]) {
  case '#':
  case '$':
    p.erase(0,1);
    return getval(p,nval,16);
  case '%':
    p.erase(0,1);
    return getval(p,nval,2);
  case '0':
    if (p[1]=='x' || p[1]=='X') {
      p.erase(0,2);
      return getval(p,nval,16);
    }
  default:
    pos=findconstantchar.findnot(p);
    if (pos==string::npos) pos=(int)p.length();
    c=p[pos-1];
    if (c=='b' || c=='B') { base=2; --pos; }
    if (c=='h' || c=='H') { base=16; --pos; }
    if (c=='o' || c=='O') { base=8; --pos; }
    if (c=='q' || c=='Q') { base=8; --pos; }
    c=getval(p,nval,base,pos);
    if (base!=10) p.erase(0,1);
    return c;
  }
//  return 0;
}

int getCharConst(string &p, int &nval) {
  if (p[0]=='\'') {
    p.erase(0,1);
    if (p[0]=='\'') return 0;
    nval=p[0]; p.erase(0,1);
    if (p[0]!='\'') return 0;
    p.erase(0,1); return 1;
  } else if (p[0]=='"') {
    p.erase(0,1);
    getcharconstchar(p,nval);
    if (p[0]!='"') return 0;
    p.erase(0,1); return 1;
  }
  return 0;
}

int getRepeatCount(string &p, int &v) {
  int s=0;
  v=0; again=1;
  while (cneed(p,'%')) ++s;
  int i=(int)repeatstack.size()-s;
  if (i<1) { error("% without repeatblock"); return 0; }
  v=repeatstack[i-1];
  return 1;
}

int ParseExp(string&,int&);

int ParseExpPrim(string &p, int &nval) {
  int dummy;
  int res=0;
  skipblanks(p);
  if (!p[0]) { return 0; }
  if (cneed(p,'(')) { res=ParseExp(p,nval); if (!sbcneed(p,')')) { error("')' Expected"); return 0; } }
  else if (isdigit(p[0]) || (p[0]=='#' && isalnum(p[1])) || (p[0]=='$' && isalnum(p[1])) || p[0]=='%' && isdigit(p[1])) { return getConstant(p,nval); }
  else if (isalpha(p[0]) || p[0]=='_' || p[0]=='.' || p[0]=='@') { return getLabelValue(p,nval,dummy); }
  else if (cneed(p,'$')) { nval=adres; again=1; return 1; }
  else if (cneed(p,'#')) { nval=page; again=1; return 1; }
  else if (cneed(p,'%')) return getRepeatCount(p,nval);
  else if (cneed(p,':')) {
    if (cneed(p,':')) { res=ParseExp(p,nval); nval=output[onr]->getpagesize(nval); again=1; return 1; }
    else return getLabelValue(p,dummy,nval);
  }
  else if (!(res=getCharConst(p,nval))) { if (synerr) error("Syntax error",ERRREP); return 0; }
  return res;
}

int ParseExpUnair(string &p, int &nval) {
  int right;
  int oper;
  if ((oper=need(p,"! ~ + - ")) || (oper=needa(p,"not",'!',"low",'l',"high",'h'))) {
    switch (oper) {
    case '!': if(!ParseExpUnair(p,right)) return 0; nval=-!right; break;
    case '~': if(!ParseExpUnair(p,right)) return 0; nval=~right; break;
    case '+': if(!ParseExpUnair(p,right)) return 0; nval=right; break;
    case '-': if(!ParseExpUnair(p,right)) return 0; nval=~right+1; break;
    case 'l': if(!ParseExpUnair(p,right)) return 0; nval=right&255; break;
    case 'h': if(!ParseExpUnair(p,right)) return 0; nval=(right>>8)&255; break;
    default: error("Hoppa? Noppa!",ERRINTERNAL);
    }
    return 1;
  } else return ParseExpPrim(p,nval);
}

int ParseExpPow(string &p, int &nval) {
  int left,right;
  if (!ParseExpUnair(p,left)) return 0;
  while (need(p,"**")) {
    if (!ParseExpUnair(p,right)) return 0;
    if (right<0) nval=0;
    else for (nval=right&1?left:1,right>>=1;right;right>>=1) { left*=left; if (right&1) nval*=left; }
    left=nval;
  }
  nval=left; return 1;
}

int ParseExpMul(string &p, int &nval) {
  int left,right;
  int oper;
  if (!ParseExpPow(p,left)) return 0;
  while ((oper=need(p,"* / % ")) || (oper=needa(p,"mod",'%'))) {
    if (!ParseExpPow(p,right)) return 0;
    switch (oper) {
    case '*': left*=right; break;
    case '/': if (right) left/=right; else { cout << "Division by zero\n"; left=0; } break;
    case '%': if (right) left%=right; else { cout << "Division by zero\n"; left=0; } break;
    default: error("Hoppa? Noppa!",ERRINTERNAL);
    }
  }
  nval=left; return 1;
}

int ParseExpAdd(string &p, int &nval) {
  int left,right;
  int oper;
  if (!ParseExpMul(p,left)) return 0;
  while (oper=need(p,"+ - ")) {
    if (!ParseExpMul(p,right)) return 0;
    switch (oper) {
    case '+': left+=right; break;
    case '-': left-=right; break;
    default: error("Hoppa? Noppa!",ERRINTERNAL);
    }
  }
  nval=left; return 1;
}

int ParseExpShift(string &p, int &nval) {
  int left,right;
  int oper;
  if (!ParseExpAdd(p,left)) return 0;
  while ((oper=need(p,"<<>>")) || (oper=needa(p,"shl",'<'+'<',"shr",'>'))) {
    if (oper=='>'+'>' && p[0]=='>') { p.erase(0,1); oper='>'+'@'; }
    if (!ParseExpAdd(p,right)) return 0;
    switch (oper) {
    case '<'+'<': if (right>=32) left=0; else left<<=right; break;
    case '>':
    case '>'+'>': if (right>=32) right=31; left>>=right; break;
    case '>'+'@': if (right>=32) left=0; else { unsigned long l=left; l>>=right; left=l; } break;
    default: error("Hoppa? Noppa!",ERRINTERNAL);
    }
  }
  nval=left; return 1;
}

int ParseExpMinMax(string &p, int &nval) {
  int left,right;
  int oper;
  if (!ParseExpShift(p,left)) return 0;
  while (oper=need(p,"<?>?")) {
    if (!ParseExpShift(p,right)) return 0;
    switch (oper) {
    case '<'+'?': left=left<right?left:right; break;
    case '>'+'?': left=left>right?left:right; break;
    default: error("Hoppa? Noppa!",ERRINTERNAL);
    }
  }
  nval=left; return 1;
}

int ParseExpCmp(string &p, int &nval) {
  int left,right;
  int oper;
  if (!ParseExpMinMax(p,left)) return 0;
  while (oper=need(p,"<=>=< > ")) {
    if (!ParseExpMinMax(p,right)) return 0;
    switch (oper) {
    case '<': left=-(left<right); break;
    case '>': left=-(left>right); break;
    case '<'+'=': left=-(left<=right); break;
    case '>'+'=': left=-(left>=right); break;
    default: error("Hoppa? Noppa!",ERRINTERNAL);
    }
  }
  nval=left; return 1;
}

int ParseExpEqu(string &p, int &nval) {
  int left,right;
  int oper;
  if (!ParseExpCmp(p,left)) return 0;
  while (oper=need(p,"=_==!=")) {
    if (!ParseExpCmp(p,right)) return 0;
    switch (oper) {
    case '=':
    case '='+'=': left=-(left==right); break;
    case '!'+'=': left=-(left!=right); break;
    default: error("Hoppa? Noppa!",ERRINTERNAL);
    }
  }
  nval=left; return 1;
}

int ParseExpBitAnd(string &p, int &nval) {
  int left,right;
  if (!ParseExpEqu(p,left)) return 0;
  while (need(p,"&_") || needa(p,"and",'&')) {
    if (!ParseExpEqu(p,right)) return 0;
    left&=right;
  }
  nval=left; return 1;
}

int ParseExpBitXor(string &p, int &nval) {
  int left,right;
  if (!ParseExpBitAnd(p,left)) return 0;
  while (need(p,"^ ") || needa(p,"xor",'^')) {
    if (!ParseExpBitAnd(p,right)) return 0;
    left^=right;
  }
  nval=left; return 1;
}

int ParseExpBitOr(string &p, int &nval) {
  int left,right;
  if (!ParseExpBitXor(p,left)) return 0;
  while (need(p,"|_") || needa(p,"or",'|')) {
    if (!ParseExpBitXor(p,right)) return 0;
    left|=right;
  }
  nval=left; return 1;
}

int ParseExpDot(string &p, int &nval) {
  int left,right;
  if (!ParseExpBitOr(p,left)) return 0;
  while (need(p,": ")) {
    if (!ParseExpBitOr(p,right)) return 0;
    left=left*256+check8(right);
  }
  nval=left; return 1;
}

int ParseExp(string &p, int &nval) {
  if (ParseExpDot(p,nval)) return 1;
  nval=0; 
  return 0;
}

int ParseExpression(string &p, int &nval) {
  labelnotfound=0;
  if (ParseExp(p,nval)) return 1;
  nval=0; 
  return 0;
}

//eof
