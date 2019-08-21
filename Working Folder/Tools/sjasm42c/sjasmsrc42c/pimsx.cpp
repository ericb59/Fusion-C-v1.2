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

// pimsx.cpp

#include "sjasm.h"

enum Z80Reg { Z80_B=0,Z80_C,Z80_D,Z80_E,Z80_H,Z80_L,Z80_A=7,
              Z80_I,Z80_R,Z80_F,Z80_IXH,Z80_IXL,Z80_IYH,Z80_IYL,
              Z80_BC=0x10,
              Z80_DE=0x20,
              Z80_HL=0x30,
              Z80_IX=0xdd,
              Z80_IY=0xfd,
              Z80_SP=0x40,Z80_AF=0x50,
              Z80_M,Z80_NC,Z80_NZ,Z80_P,Z80_PE,Z80_PO,Z80_Z,
              Z80_nn,Z80_UNK=-1,
              Z80mBC=Z80_BC+0x100,Z80miBC,Z80mdBC,Z80mBCi,Z80mBCd,
              Z80mDE=Z80_DE+0x100,Z80miDE,Z80mdDE,Z80mDEi,Z80mDEd,
              Z80mHL=Z80_HL+0x100,Z80miHL,Z80mdHL,Z80mHLi,Z80mHLd,
              Z80mIX=Z80_IX+0x100,Z80miIX,Z80mdIX,Z80mIXi,Z80mIXd,
              Z80mIY=Z80_IY+0x100,Z80miIY,Z80mdIY,Z80mIYi,Z80mIYd,
              Z80mSP=Z80_SP+0x100,
              Z80mnn=Z80_nn+0x100,
              Z80mC=Z80_C+0x100,
              Z80_DUMMY
            };

Z80Reg getz80reg(string &ns) {
  Z80Reg res=Z80_UNK;
  string s=ns;
  int p=0;
  skipblanks(s);
  switch (s[p++]) {
  case '+':
    if (s[p++]!='+') break;
    if (!islabchar(s[p]) || !islabchar(s[p+1])) break;
    switch (s[p++]) {
    case 'b': if (s[p]=='c') { ++p; res=Z80miBC; } break;
    case 'B': if (s[p]=='C') { ++p; res=Z80miBC; } break;
    case 'd': if (s[p]=='e') { ++p; res=Z80miDE; } break;
    case 'D': if (s[p]=='E') { ++p; res=Z80miDE; } break;
    case 'h': if (s[p]=='l') { ++p; res=Z80miHL; } break;
    case 'H': if (s[p]=='L') { ++p; res=Z80miHL; } break;
    case 'i': 
      if (s[p]=='x') { ++p; res=Z80miIX; break; }
      if (s[p]=='y') { ++p; res=Z80miIY; break; }
      break;
    case 'I': 
      if (s[p]=='X') { ++p; res=Z80miIX; break; }
      if (s[p]=='Y') { ++p; res=Z80miIY; break; }
      break;
    default: ;
    }
    break;
  case '-':
    if (s[p++]!='-') break;
    if (!islabchar(s[p]) || !islabchar(s[p+1])) break;
    switch (s[p++]) {
    case 'b': if (s[p]=='c') { ++p; res=Z80mdBC; } break;
    case 'B': if (s[p]=='C') { ++p; res=Z80mdBC; } break;
    case 'd': if (s[p]=='e') { ++p; res=Z80mdDE; } break;
    case 'D': if (s[p]=='E') { ++p; res=Z80mdDE; } break;
    case 'h': if (s[p]=='l') { ++p; res=Z80mdHL; } break;
    case 'H': if (s[p]=='L') { ++p; res=Z80mdHL; } break;
    case 'i': 
      if (s[p]=='x') { ++p; res=Z80mdIX; break; }
      if (s[p]=='y') { ++p; res=Z80mdIY; break; }
      break;
    case 'I': 
      if (s[p]=='X') { ++p; res=Z80mdIX; break; }
      if (s[p]=='Y') { ++p; res=Z80mdIY; break; }
      break;
    default: ;
    }
    break;
  case 'a':
    if (!islabchar(s[p])) { res=Z80_A; break; }
    if (s[p]=='f' && !islabchar(s[p+1])) { ++p; res=Z80_AF; }
    break;
  case 'b':
    if (!islabchar(s[p])) { res=Z80_B; break; }
    if (s[p]!='c') break; ++p;
    if (s[p]=='+' && s[p+1]=='+') { p+=2; res=Z80mBCi; break; }
    if (s[p]=='-' && s[p+1]=='-') { p+=2; res=Z80mBCd; break; }
    if (!islabchar(s[p])) res=Z80_BC;
    break;
  case 'c': case 'C':
    if (!islabchar(s[p])) res=Z80_C;
    break;
  case 'd':
    if (!islabchar(s[p])) { res=Z80_D; break; }
    if (s[p]!='e') break; ++p;
    if (s[p]=='+' && s[p+1]=='+') { p+=2; res=Z80mDEi; break; }
    if (s[p]=='-' && s[p+1]=='-') { p+=2; res=Z80mDEd; break; }
    if (!islabchar(s[p])) res=Z80_DE;
    break;
  case 'e': case 'E':
    if (!islabchar(s[p])) res=Z80_E;
    break;
  case 'f': case 'F':
    if (!islabchar(s[p])) res=Z80_F;
    break;
  case 'h':
    if (!islabchar(s[p])) { res=Z80_H; break; }
    if (s[p]!='l') break; ++p;
    if (s[p]=='+' && s[p+1]=='+') { p+=2; res=Z80mHLi; break; }
    if (s[p]=='-' && s[p+1]=='-') { p+=2; res=Z80mHLd; break; }
    if (!islabchar(s[p])) res=Z80_HL;
    break;
  case 'i':
    if (s[p]=='x') {
      if (s[p+1]=='+' && s[p+2]=='+') { p+=3; res=Z80mIXi; break; }
      if (s[p+1]=='-' && s[p+2]=='-') { p+=3; res=Z80mIXd; break; }
      if (!islabchar(s[p+1])) { ++p; res=Z80_IX; break; }
      if (s[p+1]=='h' && !islabchar(s[p+2])) { p+=2; res=Z80_IXH; break; }
      if (s[p+1]=='l' && !islabchar(s[p+2])) { p+=2; res=Z80_IXL; break; }
    }
    if (s[p]=='y') {
      if (s[p+1]=='+' && s[p+2]=='+') { p+=3; res=Z80mIYi; break; }
      if (s[p+1]=='-' && s[p+2]=='-') { p+=3; res=Z80mIYd; break; }
      if (!islabchar(s[p+1])) { ++p; res=Z80_IY; break; }
      if (s[p+1]=='h' && !islabchar(s[p+2])) { p+=2; res=Z80_IYH; break; }
      if (s[p+1]=='l' && !islabchar(s[p+2])) { p+=2; res=Z80_IYL; break; }
    }
    if (!islabchar(s[p])) res=Z80_I;
    break;
  case 'l': case 'L':
    if (!islabchar(s[p])) res=Z80_L;
    break;
  case 'm': case 'M':
    if (!islabchar(s[p])) res=Z80_M;
    break;
  case 'n':
    if (islabchar(s[p+1])) break;
    if (s[p]=='z') { ++p; res=Z80_NZ; break; }
    if (s[p]=='c') { ++p; res=Z80_NC; break; }
    if (s[p]=='s') { ++p; res=Z80_P; break; }
    break;
  case 'p':
    if (!islabchar(s[p])) { res=Z80_P; break; }
    if (islabchar(s[p+1])) break;
    if (s[p]=='e') { ++p; res=Z80_PE; break; }
    if (s[p]=='o') { ++p; res=Z80_PO; break; }
    break;
  case 'r': case 'R':
    if (!islabchar(s[p])) res=Z80_R;
    break;
  case 's':
    if (!islabchar(s[p])) { res=Z80_M; break; }
    if (s[p]=='p' && !islabchar(s[p+1])) { ++p; res=Z80_SP; }
    break;
  case 'z': case 'Z': 
    if (!islabchar(s[p])) res=Z80_Z;
    break;

  case 'A':
    if (!islabchar(s[p])) { res=Z80_A; break; }
    if (s[p]=='F' && !islabchar(s[p+1])) { ++p; res=Z80_AF; }
    break;
  case 'B':
    if (!islabchar(s[p])) { res=Z80_B; break; }
    if (s[p]!='C') break; ++p;
    if (s[p]=='+' && s[p+1]=='+') { p+=2; res=Z80mBCi; break; }
    if (s[p]=='-' && s[p+1]=='-') { p+=2; res=Z80mBCd; break; }
    if (!islabchar(s[p])) res=Z80_BC;
    break;
  case 'D':
    if (!islabchar(s[p])) { res=Z80_D; break; }
    if (s[p]!='E') break; ++p;
    if (s[p]=='+' && s[p+1]=='+') { p+=2; res=Z80mDEi; break; }
    if (s[p]=='-' && s[p+1]=='-') { p+=2; res=Z80mDEd; break; }
    if (!islabchar(s[p])) res=Z80_DE;
    break;
  case 'H':
    if (!islabchar(s[p])) { res=Z80_H; break; }
    if (s[p]!='L') break; ++p;
    if (s[p]=='+' && s[p+1]=='+') { p+=2; res=Z80mHLi; break; }
    if (s[p]=='-' && s[p+1]=='-') { p+=2; res=Z80mHLd; break; }
    if (!islabchar(s[p])) res=Z80_HL;
    break;
  case 'I':
    if (s[p]=='X') {
      if (s[p+1]=='+' && s[p+2]=='+') { p+=3; res=Z80mIXi; break; }
      if (s[p+1]=='-' && s[p+2]=='-') { p+=3; res=Z80mIXd; break; }
      if (!islabchar(s[p+1])) { ++p; res=Z80_IX; break; }
      if (s[p+1]=='H' && !islabchar(s[p+2])) { p+=2; res=Z80_IXH; break; }
      if (s[p+1]=='L' && !islabchar(s[p+2])) { p+=2; res=Z80_IXL; break; }
    }
    if (s[p]=='Y') {
      if (s[p+1]=='+' && s[p+2]=='+') { p+=3; res=Z80mIYi; break; }
      if (s[p+1]=='-' && s[p+2]=='-') { p+=3; res=Z80mIYd; break; }
      if (!islabchar(s[p+1])) { ++p; res=Z80_IY; break; }
      if (s[p+1]=='H' && !islabchar(s[p+2])) { p+=2; res=Z80_IYH; break; }
      if (s[p+1]=='L' && !islabchar(s[p+2])) { p+=2; res=Z80_IYL; break; }
    }
    if (!islabchar(s[p])) res=Z80_I;
    break;
  case 'N':
    if (islabchar(s[p+1])) break;
    if (s[p]=='Z') { ++p; res=Z80_NZ; break; }
    if (s[p]=='C') { ++p; res=Z80_NC; break; }
    if (s[p]=='S') { ++p; res=Z80_P; break; }
    break;
  case 'P':
    if (!islabchar(s[p])) { res=Z80_P; break; }
    if (islabchar(s[p+1])) break;
    if (s[p]=='E') { ++p; res=Z80_PE; break; }
    if (s[p]=='O') { ++p; res=Z80_PO; break; }
    break;
  case 'S':
    if (!islabchar(s[p])) { res=Z80_M; break; }
    if (s[p]=='P' && !islabchar(s[p+1])) { ++p; res=Z80_SP; }
    break;
  default: break;
  }
  if (res!=Z80_UNK) ns=s.substr(p);
  return res;
}

struct Op {
  Z80Reg reg;
  int val,ind;
  string str;
};

void getOperand(string &ns, Op &op) {
  string s=ns,hs;
  int blokhaakje=0;
  skipblanks(s); op.reg=Z80_UNK; op.val=0; op.ind=0; op.str=s;
  if (s[0]=='[') { op.ind=2; blokhaakje=1; s.erase(0,1); }
  else if (!options.onlybp && s[0]=='(') { op.ind=1; blokhaakje=0; hs=s; s.erase(0,1); }
  op.reg=getz80reg(s);
  if (op.ind && 
     (op.reg==Z80_IX  || op.reg==Z80_IY ||
      op.reg==Z80miIX || op.reg==Z80miIY || 
      op.reg==Z80mdIX || op.reg==Z80mdIY || 
      op.reg==Z80mIXi || op.reg==Z80mIYi || 
      op.reg==Z80mIXd || op.reg==Z80mIYd )) { 
    synerr=0; if (ParseExpression(s,op.val)) op.ind=3; synerr=1; 
  } else if (op.reg==Z80_UNK) {
    if (op.ind && !blokhaakje) s='('+s;
    if (ParseExpression(s,op.val)) op.reg=Z80_nn;
  }
  skipblanks(s);
  if (op.ind)
    if (blokhaakje) {
      if (!sbcneed(s,']')) error("Closing ] expected");
    } else {
      if (op.reg!=Z80_nn && !sbcneed(s,')')) error("Closing ) expected");
      checkparen(hs); if (hs!=s) op.ind=0;
    }
  switch (op.reg) {
  case Z80_C: case Z80_BC: case Z80_DE: case Z80_HL: case Z80_IX: case Z80_IY: case Z80_SP: case Z80_nn:
    if (op.ind) op.reg=(enum Z80Reg)(op.reg+0x100);
    break;
  case Z80miBC: case Z80mdBC: case Z80mBCi: case Z80mBCd:
  case Z80miDE: case Z80mdDE: case Z80mDEi: case Z80mDEd:
  case Z80miHL: case Z80mdHL: case Z80mHLi: case Z80mHLd:
  case Z80miIX: case Z80mdIX: case Z80mIXi: case Z80mIXd:
  case Z80miIY: case Z80mdIY: case Z80mIYi: case Z80mIYd:
    if (!op.ind) op.reg=Z80_UNK;
    break;
  default:
    if (op.ind) op.reg=Z80_UNK;
    break;
  }
  ns=s;
}

int getOperands3(string &s, Op &x, Op &y, Op &z) {
  getOperand(s,x);
  if (!comma(s)) return 1;
  getOperand(s,y);
  if (!comma(s)) return 2;
  getOperand(s,z);
  return 3;
}

int getOperands2(string &s, Op &x, Op &y) {
  getOperand(s,x);
  if (!comma(s)) return 1;
  getOperand(s,y);
  return 2;
}

int getjumpoperand(string &ns, Op &op) {
  int res,v,n;
  string s=ns;
  skipblanks(s); op.reg=Z80_UNK; op.val=0; op.ind=0; op.str=s;
  if (!isdigit(s[0])) return 0;
  again=1;
  v=(int)s.find_first_not_of("0123456789");
  getval(s,n,10,v);
  if (s.empty()) return 0;
  switch(s[0]) {
  case 'b': case 'B': res=numlabtab.getvalueb(n,v); break;
  case 'f': case 'F': res=numlabtab.getvaluef(n,v); break;
  default: return 0;
  }
  s.erase(0,1);
  if (isalnum(s[0])) return 0;
  op.reg=Z80_nn; op.val=v;
  ns=s;
  if (!res) if (pass==1) labsnok=1; else error("Label not found",ns);
  return 1;
}

void getJumpOperand(string &s, Op &x) {
  if (!getjumpoperand(s,x)) getOperand(s,x);
}

int getJumpOperands2(string &s, Op &x, Op &y) {
  if (!getjumpoperand(s,x)) getOperand(s,x);
  if (!comma(s)) return 1;
  if (!getjumpoperand(s,y)) getOperand(s,y);
  return 2;
}
void NegateCondition(Z80Reg &x) {
  switch (x) {
    case Z80_C: x=Z80_NC; break;
    case Z80_M: x=Z80_P; break;
    case Z80_NC: x=Z80_C; break;
    case Z80_NZ: x=Z80_Z; break;
    case Z80_P: x=Z80_M; break;
    case Z80_PE: x=Z80_PO; break;
    case Z80_PO: x=Z80_PE; break;
    case Z80_Z: x=Z80_NZ; break;
    default: error("Cannot negate condition");
  }
}

void preinc(Data &e, Op x) {
  switch (x.reg) {
  case Z80miBC: case Z80miDE: case Z80miHL: e.push(x.reg-14); break;
  case Z80miIX: case Z80miIY: e.push(x.reg-1); e.push(0x23); break;
  case Z80mdBC: case Z80mdDE: case Z80mdHL: e.push(x.reg-7); break;
  case Z80mdIX: case Z80mdIY: e.push(x.reg-2); e.push(0x2b); break;
  }
}

void postinc(Data &e, Op x) {
  switch (x.reg) {
  case Z80mBCi: case Z80mDEi: case Z80mHLi: e.push(x.reg-16); break;
  case Z80mIXi: case Z80mIYi: e.push(x.reg-3); e.push(0x23); break;
  case Z80mBCd: case Z80mDEd: case Z80mHLd: e.push(x.reg-9); break;
  case Z80mIXd: case Z80mIYd: e.push(x.reg-4); e.push(0x2b); break;
  }
}

int undoc(string &p, int r) {
  if (!comma(p)) return r+6;
  switch(int reg=getz80reg(p)) {
  case Z80_B: case Z80_C: case Z80_D: case Z80_E:
  case Z80_H: case Z80_L: case Z80_A: return r+reg;
  default: error("Illegal operand"); break;
  }
  return r+6;
}

void pmADC(string line,Data &e) {
  Op x,y;
  if (getOperands2(line,x,y)==1) { y=x; x.reg=Z80_A; }
  switch (x.reg) {
  case Z80_A:
    switch (y.reg) {
    case Z80_IXH: e[0]=0xdd; e[1]=0x8c; break;
    case Z80_IXL: e[0]=0xdd; e[1]=0x8d; break;
    case Z80_IYH: e[0]=0xfd; e[1]=0x8c; break;
    case Z80_IYL: e[0]=0xfd; e[1]=0x8d; break;
    case Z80_B: case Z80_C: case Z80_D: case Z80_E:
    case Z80_H: case Z80_L: case Z80_A: e[0]=0x88+y.reg; break;
    case Z80mnn: if (y.ind>1) { error("Indirection not allowed"); break; }
    case Z80_nn: e[0]=0xce; e[1]=check8(y.val); break;
    case Z80mHL: case Z80miHL: case Z80mdHL: case Z80mHLi: case Z80mHLd: 
      preinc(e,y); e.push(0x8e); postinc(e,y); break;
    case Z80mIX: case Z80miIX: case Z80mdIX: case Z80mIXi: case Z80mIXd:
      preinc(e,y); e.push(0xdd); e.push(0x8e); e.push(checki(y.val)); postinc(e,y); break;
    case Z80mIY: case Z80miIY: case Z80mdIY: case Z80mIYi: case Z80mIYd:
      preinc(e,y); e.push(0xfd); e.push(0x8e); e.push(checki(y.val)); postinc(e,y); break;
    default: error("Illegal operand",y.str,ERRREP); break;
    }
    break;
  case Z80_HL:
    switch (y.reg) {
    case Z80_BC: case Z80_DE: case Z80_HL: case Z80_SP: e[0]=0xed; e[1]=y.reg+0x3a; break;
    default: error("Illegal operand",y.str,ERRREP); break;
    }
    break;
  default:
    error("Illegal operand",x.str,ERRREP); break;
  }
  checkjunk(line);
}

void pmADD(string line,Data &e) {
  Op x,y;
  if (getOperands2(line,x,y)==1) { y=x; x.reg=Z80_A; }
  switch (x.reg) {
  case Z80_A:
    switch (y.reg) {
    case Z80_IXH: e[0]=0xdd; e[1]=0x84; break;
    case Z80_IXL: e[0]=0xdd; e[1]=0x85; break;
    case Z80_IYH: e[0]=0xfd; e[1]=0x84; break;
    case Z80_IYL: e[0]=0xfd; e[1]=0x85; break;
    case Z80_B: case Z80_C: case Z80_D: case Z80_E:
    case Z80_H: case Z80_L: case Z80_A: e[0]=0x80+y.reg; break;
    case Z80mnn: if (y.ind>1) { error("Indirection not allowed"); break; }
    case Z80_nn: e[0]=0xc6; e[1]=check8(y.val); break;
    case Z80mHL: case Z80miHL: case Z80mdHL: case Z80mHLi: case Z80mHLd: 
      preinc(e,y); e.push(0x86); postinc(e,y); break;
    case Z80mIX: case Z80miIX: case Z80mdIX: case Z80mIXi: case Z80mIXd:
      preinc(e,y); e.push(0xdd); e.push(0x86); e.push(checki(y.val)); postinc(e,y); break;
    case Z80mIY: case Z80miIY: case Z80mdIY: case Z80mIYi: case Z80mIYd:
      preinc(e,y); e.push(0xfd); e.push(0x86); e.push(checki(y.val)); postinc(e,y); break;
    default: error("Illegal operand",ERRREP); break;
    }
    break;
  case Z80_HL:
    switch (y.reg) {
    case Z80_BC: case Z80_DE: case Z80_HL: case Z80_SP: e[0]=y.reg-7; break;
    default: error("Illegal operand",ERRREP); break;
    }
    break;
  case Z80_IX:
    switch (y.reg) {
    case Z80_IX: y.reg=Z80_HL;
    case Z80_BC: case Z80_DE: case Z80_SP: e[0]=0xdd; e[1]=y.reg-7; break;
    default: error("Illegal operand",ERRREP); break;
    }
    break;
  case Z80_IY:
    switch (y.reg) {
    case Z80_IY: y.reg=Z80_HL;
    case Z80_BC: case Z80_DE: case Z80_SP: e[0]=0xfd; e[1]=y.reg-7; break;
    default: error("Illegal operand",ERRREP); break;
    }
    break;
  default:
    error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmAND(string line,Data &e) {
  Op x,y;
  if (getOperands2(line,x,y)==1) { y=x; x.reg=Z80_A; }
  switch (x.reg) {
  case Z80_A:
    switch (y.reg) {
    case Z80_IXH: e[0]=0xdd; e[1]=0xa4; break;
    case Z80_IXL: e[0]=0xdd; e[1]=0xa5; break;
    case Z80_IYH: e[0]=0xfd; e[1]=0xa4; break;
    case Z80_IYL: e[0]=0xfd; e[1]=0xa5; break;
    case Z80_B: case Z80_C: case Z80_D: case Z80_E:
    case Z80_H: case Z80_L: case Z80_A: e[0]=0xa0+y.reg; break;
    case Z80mnn: if (y.ind>1) { error("Indirection not allowed"); break; }
    case Z80_nn: e[0]=0xe6; e[1]=check8(y.val); break;
    case Z80mHL: case Z80miHL: case Z80mdHL: case Z80mHLi: case Z80mHLd: 
      preinc(e,y); e.push(0xa6); postinc(e,y); break;
    case Z80mIX: case Z80miIX: case Z80mdIX: case Z80mIXi: case Z80mIXd:
      preinc(e,y); e.push(0xdd); e.push(0xa6); e.push(checki(y.val)); postinc(e,y); break;
    case Z80mIY: case Z80miIY: case Z80mdIY: case Z80mIYi: case Z80mIYd:
      preinc(e,y); e.push(0xfd); e.push(0xa6); e.push(checki(y.val)); postinc(e,y); break;
    default: error("Illegal operand",ERRREP); break;
    }
    break;
  default:
    error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmBIT(string line,Data &e) {
  int bit;
  Op x,y;
  getOperands2(line,x,y);
  switch (x.reg) {
  case Z80mnn: if (x.ind>1) { error("Indirection not allowed"); break; }
  case Z80_nn:
    bit=x.val;
    if (bit<0 || bit>7) { error("Illegal operand",ERRREP); bit&=7; }
    switch (y.reg) {
    case Z80_B: case Z80_C: case Z80_D: case Z80_E:
    case Z80_H: case Z80_L: case Z80_A:
      e[0]=0xcb; e[1]=8*bit+0x40+y.reg; break;
    case Z80mHL: case Z80miHL: case Z80mdHL: case Z80mHLi: case Z80mHLd:
      preinc(e,y); e.push(0xcb); e.push(8*bit+0x46); postinc(e,y); break;
    case Z80mIX: case Z80miIX: case Z80mdIX: case Z80mIXi: case Z80mIXd:
      preinc(e,y); e.push(0xdd); e.push(0xcb); e.push(checki(y.val)); e.push(8*bit+0x46); postinc(e,y); break;
    case Z80mIY: case Z80miIY: case Z80mdIY: case Z80mIYi: case Z80mIYd:
      preinc(e,y); e.push(0xfd); e.push(0xcb); e.push(checki(y.val)); e.push(8*bit+0x46); postinc(e,y); break;
    default: error("Illegal operand",ERRREP); break;
    }
    break;
  default:
    error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmCALL(string line,Data &e) {
  int c=0;
  Op x,y;
  bool negcond=false;
  if (sbcneed(line,'!')) negcond=true;
  if (getJumpOperands2(line,x,y)==1) { y=x; x.reg=Z80_DUMMY; }
  switch (y.reg) {
  case Z80mnn: if (y.ind>1) { error("Indirection not allowed"); break; }
  case Z80_nn:
    if (negcond) NegateCondition(x.reg);
    switch (x.reg) {
    case Z80_C: c=0xdc; break;
    case Z80_M: c=0xfc; break;
    case Z80_NC: c=0xd4; break;
    case Z80_NZ: c=0xc4; break;
    case Z80_P: c=0xf4; break;
    case Z80_PE: c=0xec; break;
    case Z80_PO: c=0xe4; break;
    case Z80_Z: c=0xcc; break;
    case Z80_DUMMY: c=0xcd; break;
    default: error("Illegal operand",ERRREP); break;
    }
    break;
  default:
    error("Illegal operand",ERRREP); break;
  }
  if (c) { e[0]=c; check16(y.val); e[1]=y.val&255; e[2]=y.val>>8; }
  checkjunk(line);
}

void pmCCF(string line,Data &e) {
  e[0]=0x3f;
  checkjunk(line);
}

void pmCP(string line,Data &e) {
  Op x,y;
  if (getOperands2(line,x,y)==1) { y=x; x.reg=Z80_A; }
  switch (x.reg) {
  case Z80_A:
    switch (y.reg) {
    case Z80_IXH: e[0]=0xdd; e[1]=0xbc; break;
    case Z80_IXL: e[0]=0xdd; e[1]=0xbd; break;
    case Z80_IYH: e[0]=0xfd; e[1]=0xbc; break;
    case Z80_IYL: e[0]=0xfd; e[1]=0xbd; break;
    case Z80_B: case Z80_C: case Z80_D: case Z80_E:
    case Z80_H: case Z80_L: case Z80_A: e[0]=0xb8+y.reg; break;
    case Z80mnn: if (y.ind>1) { error("Indirection not allowed"); break; }
    case Z80_nn: e[0]=0xfe; e[1]=check8(y.val); break;
    case Z80mHL: case Z80miHL: case Z80mdHL: case Z80mHLi: case Z80mHLd: 
      preinc(e,y); e.push(0xbe); postinc(e,y); break;
    case Z80mIX: case Z80miIX: case Z80mdIX: case Z80mIXi: case Z80mIXd:
      preinc(e,y); e.push(0xdd); e.push(0xbe); e.push(checki(y.val)); postinc(e,y); break;
    case Z80mIY: case Z80miIY: case Z80mdIY: case Z80mIYi: case Z80mIYd:
      preinc(e,y); e.push(0xfd); e.push(0xbe); e.push(checki(y.val)); postinc(e,y); break;
    default: error("Illegal operand",ERRREP); break;
    }
    break;
  default: error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmCPD(string line,Data &e) {
  e[0]=0xed; e[1]=0xa9;
  checkjunk(line);
}

void pmCPDR(string line,Data &e) {
  e[0]=0xed; e[1]=0xb9;
  checkjunk(line);
}

void pmCPI(string line,Data &e) {
  e[0]=0xed; e[1]=0xa1;
  checkjunk(line);
}

void pmCPIR(string line,Data &e) {
  e[0]=0xed; e[1]=0xb1;
  checkjunk(line);
}

void pmCPL(string line,Data &e) {
  e[0]=0x2f;
  checkjunk(line);
}

void pmDAA(string line,Data &e) {
  e[0]=0x27;
  checkjunk(line);
}

void pmDEC(string line,Data &e) {
  Op x;
  getOperand(line,x);
  switch (x.reg) {
  case Z80mHL: case Z80miHL: case Z80mdHL: case Z80mHLi: case Z80mHLd: 
    preinc(e,x); e.push(0x35); postinc(e,x); break;
  case Z80mIX: case Z80miIX: case Z80mdIX: case Z80mIXi: case Z80mIXd:
    preinc(e,x); e.push(0xdd); e.push(0x35); e.push(checki(x.val)); postinc(e,x); break;
  case Z80mIY: case Z80miIY: case Z80mdIY: case Z80mIYi: case Z80mIYd:
    preinc(e,x); e.push(0xfd); e.push(0x35); e.push(checki(x.val)); postinc(e,x); break;
  case Z80_A: e[0]=0x3d; break;
  case Z80_B: e[0]=0x05; break;
  case Z80_BC: e[0]=0x0b; break;
  case Z80_C: e[0]=0x0d; break;
  case Z80_D: e[0]=0x15; break;
  case Z80_DE: e[0]=0x1b; break;
  case Z80_E: e[0]=0x1d; break;
  case Z80_H: e[0]=0x25; break;
  case Z80_HL: e[0]=0x2b; break;
  case Z80_IX: e[0]=0xdd; e[1]=0x2b; break;
  case Z80_IY: e[0]=0xfd; e[1]=0x2b; break;
  case Z80_L: e[0]=0x2d; break;
  case Z80_SP: e[0]=0x3b; break;
  case Z80_IXH: e[0]=0xdd; e[1]=0x25; break;
  case Z80_IXL: e[0]=0xdd; e[1]=0x2d; break;
  case Z80_IYH: e[0]=0xfd; e[1]=0x25; break;
  case Z80_IYL: e[0]=0xfd; e[1]=0x2d; break;
  default: error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmDI(string line,Data &e) {
  e[0]=0xf3;
  checkjunk(line);
}

void pmDJNZ(string line,Data &e) {
  int jmp;
  Op x;
  string p;
  bool opt=options.optimizejumps;
  while (!line.empty() && line[0]=='.') {
    line.erase(0,1); p=getinstructionpart(line);
    if (p=="") opt=true;
    else if (p=="o") opt=true;
    else if (p=="no") opt=false;
    else error("Unknown instruction","djnz."+p);
  }
  getJumpOperand(line,x);
  switch (x.reg) {
  case Z80mnn: if (x.ind>1) { error("Indirection not allowed"); break; }
  case Z80_nn:
    jmp=x.val-adres-2;
    if (jmp<-128 || jmp>127) {
      if (opt) {
        e[0]=0x05; e[1]=0xc2; check16(x.val); e[2]=x.val&255; e[3]=x.val>>8;
      } else {
      errortarget(jmp); jmp=0;
      }
    } else {
      e[0]=0x10; e[1]=jmp;
    }
    break;
  default: error("Illegal operand",ERRREP); break;
  }
  checkjunk(line); again=1;
}

void pmEI(string line,Data &e) {
  e[0]=0xfb;
  checkjunk(line);
}

void pmEX(string line,Data &e) {
  Op x,y;
  int no=getOperands2(line,x,y);
  switch (x.reg) {
  case Z80mSP:
    switch (y.reg) {
    case Z80_IX: case Z80_IY: e[0]=y.reg;
    case Z80_HL: e.push(0xe3); break;
    default: error("Illegal operand",ERRREP); break;
    }
    break;
  case Z80_AF:
    if (no==2 && y.reg!=Z80_AF) { error("Illegal operand",ERRREP); break; }
    e[0]=0x08;
    break;
  case Z80_DE:
    if (y.reg!=Z80_HL) { error("Illegal operand",ERRREP); break; }
    e[0]=0xeb;
    break;
  default: error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmEXX(string line,Data &e) {
  e[0]=0xd9;
  checkjunk(line);
}

void pmHALT(string line,Data &e) {
  e[0]=0x76;
  checkjunk(line);
}

void pmIM(string line,Data &e) {
  Op x;
  getOperand(line,x);
  switch (x.reg) {
  case Z80mnn: if (x.ind>1) { error("Indirection not allowed"); break; }
  case Z80_nn:
    switch (x.val) {
    case 0: e[0]=0xed; e[1]=0x46; break;
    case 1: e[0]=0xed; e[1]=0x56; break;
    case 2: e[0]=0xed; e[1]=0x5e; break;
    default: error("Illegal operand",ERRREP); break;
    }
    break;
  default: error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmIN(string line,Data &e) {
  Op x,y;
  if (getOperands2(line,x,y)==1) y.reg=Z80_DUMMY;
  switch (y.reg) {
  case Z80mC:
    switch (x.reg) {
    case Z80_B: e[0]=0xed; e[1]=0x40; break;
    case Z80_C: e[0]=0xed; e[1]=0x48; break;
    case Z80_D: e[0]=0xed; e[1]=0x50; break;
    case Z80_E: e[0]=0xed; e[1]=0x58; break;
    case Z80_H: e[0]=0xed; e[1]=0x60; break;
    case Z80_L: e[0]=0xed; e[1]=0x68; break;
    case Z80_F: e[0]=0xed; e[1]=0x70; break;
    case Z80_A: e[0]=0xed; e[1]=0x78; break;
    default: error("Illegal operand",ERRREP); break;
    }
    break;
  case Z80mnn:
    if (x.reg!=Z80_A) { error("Illegal operand",ERRREP); break; }
    e[0]=0xdb; check8(y.val); e[1]=y.val;
    break;
  case Z80_DUMMY:
    if (x.reg==Z80mC) { e[0]=0xed; e[1]=0x70; break; }
  default: error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmINC(string line,Data &e) {
  Op x;
  getOperand(line,x);
  switch (x.reg) {
  case Z80mHL: case Z80miHL: case Z80mdHL: case Z80mHLi: case Z80mHLd: 
    preinc(e,x); e.push(0x34); postinc(e,x); break;
  case Z80mIX: case Z80miIX: case Z80mdIX: case Z80mIXi: case Z80mIXd:
    preinc(e,x); e.push(0xdd); e.push(0x34); e.push(checki(x.val)); postinc(e,x); break;
  case Z80mIY: case Z80miIY: case Z80mdIY: case Z80mIYi: case Z80mIYd:
    preinc(e,x); e.push(0xfd); e.push(0x34); e.push(checki(x.val)); postinc(e,x); break;
  case Z80_A: e[0]=0x3c; break;
  case Z80_B: e[0]=0x04; break;
  case Z80_BC: e[0]=0x03; break;
  case Z80_C: e[0]=0x0c; break;
  case Z80_D: e[0]=0x14; break;
  case Z80_DE: e[0]=0x13; break;
  case Z80_E: e[0]=0x1c; break;
  case Z80_H: e[0]=0x24; break;
  case Z80_HL: e[0]=0x23; break;
  case Z80_IX: e[0]=0xdd; e[1]=0x23; break;
  case Z80_IY: e[0]=0xfd; e[1]=0x23; break;
  case Z80_L: e[0]=0x2c; break;
  case Z80_SP: e[0]=0x33; break;
  case Z80_IXH: e[0]=0xdd; e[1]=0x24; break;
  case Z80_IXL: e[0]=0xdd; e[1]=0x2c; break;
  case Z80_IYH: e[0]=0xfd; e[1]=0x24; break;
  case Z80_IYL: e[0]=0xfd; e[1]=0x2c; break;
  default: error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmIND(string line,Data &e) {
  e[0]=0xed; e[1]=0xaa;
  checkjunk(line);
}

void pmINDR(string line,Data &e) {
  e[0]=0xed; e[1]=0xba;
  checkjunk(line);
}

void pmINI(string line,Data &e) {
  e[0]=0xed; e[1]=0xa2;
  checkjunk(line);
}

void pmINIR(string line,Data &e) {
  e[0]=0xed; e[1]=0xb2;
  checkjunk(line);
}

void pmJP(string line,Data &e) {
  int val,c=0,cr=0;
  Op x,y;
  string p;
  bool opt=options.optimizejumps,negcond=false;
  while (!line.empty() && line[0]=='.') {
    line.erase(0,1); p=getinstructionpart(line);
    if (p=="") opt=true;
    else if (p=="o") opt=true;
    else if (p=="no") opt=false;
    else error("Unknown instruction","jp."+p);
  }
  if (sbcneed(line,'!')) negcond=true;
  if (getJumpOperands2(line,x,y)==1) { y=x; x.reg=Z80_DUMMY; }
  switch (y.reg) {
  case Z80_HL: case Z80mHL: 
    if (x.reg!=Z80_DUMMY) { error ("Illegal operand"); break; }
    e[0]=0xe9; 
    break;
  case Z80_IX: case Z80mIX: case Z80_IY: case Z80mIY: 
    if (x.reg!=Z80_DUMMY) { error ("Illegal operand"); break; }
    if (y.ind==3) { error("Offsets not allowed with jumps"); break; }
    e[0]=y.reg&255; e[1]=0xe9;
    break;
  case Z80mnn: if (y.ind>1) { error("Indirection not allowed"); break; }
  case Z80_nn:
    if (negcond) NegateCondition(x.reg);
    switch (x.reg) {
    case Z80_C: c=0xda; cr=0x38; break;
    case Z80_M: c=0xfa; break;
    case Z80_NC: c=0xd2; cr=0x30; break;
    case Z80_NZ: c=0xc2; cr=0x20; break;
    case Z80_P: c=0xf2; break;
    case Z80_PE: c=0xea; break;
    case Z80_PO: c=0xe2; break;
    case Z80_Z: c=0xca; cr=0x28; break;
    case Z80_DUMMY: c=0xc3; cr=0x18; break;
    default: error("Illegal operand",ERRREP); break;
    }
    break;
  default:
    error("Illegal operand",ERRREP); break;
  }
  if (c) {
    switch (opt) {
    case true:
      if (cr) {
        again=1;
        val=y.val-adres-2;
        if (val>-129 && val<128) { e[0]=cr; e[1]=val; break; }
      }
    case false:
      e[0]=c; check16(y.val); e[1]=y.val&255; e[2]=y.val>>8;
    }
  }
  checkjunk(line);
}

void pmJR(string line,Data &e) {
  int val,c=0,ca=0;
  Op x,y;
  string p;
  bool opt=options.optimizejumps,negcond=false;
  while (!line.empty() && line[0]=='.') {
    line.erase(0,1); p=getinstructionpart(line);
    if (p=="") opt=true;
    else if (p=="o") opt=true;
    else if (p=="no") opt=false;
    else error("Unknown instruction","jr."+p);
  }
  if (sbcneed(line,'!')) negcond=true;
  if (getJumpOperands2(line,x,y)==1) { y=x; x.reg=Z80_DUMMY; }
  switch (y.reg) {
  case Z80mnn: if (y.ind>1) { error("Indirection not allowed"); break; }
  case Z80_nn:
    if (negcond) NegateCondition(x.reg);
    switch (x.reg) {
    case Z80_C: c=0x38; ca=0xda; break;
    case Z80_NC: c=0x30; ca=0xd2; break;
    case Z80_NZ: c=0x20; ca=0xc2; break;
    case Z80_Z: c=0x28; ca=0xca; break;
    case Z80_DUMMY: c=0x18; ca=0xc3; break;
    default: error("Illegal operand",ERRREP); break;
    }
    break;
  default:
    error("Illegal operand",ERRREP); break;
  }

  if (c) {
    val=y.val-adres-2;
    if (val<-128 || val>127) {
      if (opt) {
        e[0]=ca; check16(y.val); e[1]=y.val&255; e[2]=y.val>>8;
      } else {
        errortarget(val); val=0;
        e[0]=c; e[1]=0;
      }
    } else {
      e[0]=c; e[1]=val;
    }
  }
  checkjunk(line); again=1;
}

void pmLD(string line,Data &e) {
  Op x,y;
  if (getOperands2(line,x,y)!=2) { error("Illegal operand",ERRREP); return; }
  switch (x.reg) {
  case Z80mBC: case Z80miBC: case Z80mdBC: case Z80mBCi: case Z80mBCd: 
    if (y.reg!=Z80_A) break;
    preinc(e,x); e.push(0x02); postinc(e,x); break;
  case Z80mDE: case Z80miDE: case Z80mdDE: case Z80mDEi: case Z80mDEd: 
    if (y.reg!=Z80_A) break;
    preinc(e,x); e.push(0x12); postinc(e,x); break;
  case Z80mHL: case Z80miHL: case Z80mdHL: case Z80mHLi: case Z80mHLd:
    switch (y.reg) {
    case Z80_A: case Z80_B: case Z80_C: case Z80_D: case Z80_E: case Z80_H: case Z80_L:
      preinc(e,x); e.push(0x70+y.reg); postinc(e,x); break;
    case Z80mnn: if (y.ind==2) { error("indirection not allowed"); break; }
    case Z80_nn: preinc(e,x); e.push(0x36); e.push(check8(y.val)); postinc(e,x); break;
    }
    break;
  case Z80mIX: case Z80miIX: case Z80mdIX: case Z80mIXi: case Z80mIXd:
    switch (y.reg) {
    case Z80_A: case Z80_B: case Z80_C: case Z80_D: case Z80_E: case Z80_H: case Z80_L:
      preinc(e,x); e.push(0xdd); e.push(0x70+y.reg); e.push(checki(x.val)); postinc(e,x); break;
    case Z80mnn: if (y.ind==2) { error("indirection not allowed"); break; }
    case Z80_nn: preinc(e,x); e.push(0xdd); e.push(0x36); e.push(checki(x.val)); e.push(check8(y.val)); postinc(e,x); break;
    }
    break;
  case Z80mIY: case Z80miIY: case Z80mdIY: case Z80mIYi: case Z80mIYd:
    switch (y.reg) {
    case Z80_A: case Z80_B: case Z80_C: case Z80_D: case Z80_E: case Z80_H: case Z80_L:
      preinc(e,x); e.push(0xfd); e.push(0x70+y.reg); e.push(checki(x.val)); postinc(e,x); break;
    case Z80mnn: if (y.ind==2) { error("indirection not allowed"); break; }
    case Z80_nn: preinc(e,x); e.push(0xfd); e.push(0x36); e.push(checki(x.val)); e.push(check8(y.val)); postinc(e,x); break;
    }
    break;
  case Z80mnn:
    switch (y.reg) {
    case Z80_A: e.push(0x32); break;
    case Z80_HL: e.push(0x22); break;
    case Z80_IX: case Z80_IY: e.push(y.reg); e.push(0x22); break;
    case Z80_BC: case Z80_DE: case Z80_SP: e.push(0xed); e.push(0x33+y.reg); break;
    }
    if (!e.size()) break;
    check16(x.val); e.push((byte)x.val); e.push((byte)(x.val>>8));
    break;
  case Z80_A:
    switch (y.reg) {
    case Z80mBC: case Z80miBC: case Z80mdBC: case Z80mBCi: case Z80mBCd: 
      preinc(e,y); e.push(0x0a); postinc(e,y); break;
    case Z80mDE: case Z80miDE: case Z80mdDE: case Z80mDEi: case Z80mDEd: 
      preinc(e,y); e.push(0x1a); postinc(e,y); break;
    case Z80mnn: e.push(0x3a); check16(y.val); e.push((byte)y.val); e.push((byte)(y.val>>8)); break;
    case Z80_I: e[0]=0xed; e[1]=0x57; break;
    case Z80_R: e[0]=0xed; e[1]=0x5f; break;
    }
    if (e.size()) break;
  case Z80_B: case Z80_C: case Z80_D: case Z80_E: case Z80_H: case Z80_L:
    switch (y.reg) {
    case Z80mHL: case Z80miHL: case Z80mdHL: case Z80mHLi: case Z80mHLd:
      preinc(e,y); e.push(0x46+x.reg*8); postinc(e,y); break;
    case Z80mIX: case Z80miIX: case Z80mdIX: case Z80mIXi: case Z80mIXd:
      preinc(e,y); e.push(0xdd); e.push(0x46+x.reg*8); e.push(checki(y.val)); postinc(e,y); break;
    case Z80mIY: case Z80miIY: case Z80mdIY: case Z80mIYi: case Z80mIYd:
      preinc(e,y); e.push(0xfd); e.push(0x46+x.reg*8); e.push(checki(y.val)); postinc(e,y); break;
    case Z80_A: case Z80_B: case Z80_C: case Z80_D: case Z80_E: case Z80_H: case Z80_L:
      e.push(0x40+x.reg*8+y.reg); break;
    case Z80mnn: if (y.ind==2) { error("indirection not allowed"); break; }
    case Z80_nn: e.push(0x06+x.reg*8); e.push(check8(y.val)); break;
    case Z80_IXH: if (x.reg==Z80_H || x.reg==Z80_L) break; e.push(0xdd); e.push(0x44+x.reg*8); break;
    case Z80_IXL: if (x.reg==Z80_H || x.reg==Z80_L) break; e.push(0xdd); e.push(0x45+x.reg*8); break;
    case Z80_IYH: if (x.reg==Z80_H || x.reg==Z80_L) break; e.push(0xfd); e.push(0x44+x.reg*8); break;
    case Z80_IYL: if (x.reg==Z80_H || x.reg==Z80_L) break; e.push(0xfd); e.push(0x45+x.reg*8); break;
    }
    break;
  case Z80_SP:
    switch (y.reg) {
    case Z80_HL: e.push(0xf9); break;
    case Z80_IX: case Z80_IY: e.push(y.reg); e.push(0xf9); break;
    }
    if (e.size()) break;
  case Z80_BC: case Z80_DE: case Z80_HL:
    switch (y.reg) {
    case Z80mnn: if (x.reg==Z80_HL) { e.push(0x2a); break; } e.push(0xed); e.push(0x3b+x.reg); break;
    case Z80_nn: e.push(x.reg-15); break;
    }
    if (!e.size()) break;
    check16(y.val); e.push((byte)y.val); e.push((byte)(y.val>>8));
    break;
  case Z80_I:
    if (y.reg!=Z80_A) break;
    e[0]=0xed; e[1]=0x47;
    break;
  case Z80_IX: case Z80_IY:
    switch (y.reg) {
    case Z80mnn: e.push(x.reg); e.push(0x2a); break;
    case Z80_nn: e.push(x.reg); e.push(0x21); break;
    }
    if (!e.size()) break;
    check16(y.val); e.push((byte)y.val); e.push((byte)(y.val>>8));
    break;
  case Z80_IXH: case Z80_IXL: case Z80_IYH: case Z80_IYL:
    switch (y.reg) {
    case Z80mnn: if (y.ind==2) { error("indirection not allowed"); break; }
    case Z80_nn:
      switch (x.reg) {
      case Z80_IXH: e.push(0xdd); e.push(0x26); e.push(check8(y.val)); break;
      case Z80_IXL: e.push(0xdd); e.push(0x2e); e.push(check8(y.val)); break;
      case Z80_IYH: e.push(0xfd); e.push(0x26); e.push(check8(y.val)); break;
      case Z80_IYL: e.push(0xfd); e.push(0x2e); e.push(check8(y.val)); break;
      }
      break;
    case Z80_IXH: if (y.reg==Z80_IXH) if (x.reg==Z80_IYH || x.reg==Z80_IYL) break; else y.reg=Z80_H;
    case Z80_IXL: if (y.reg==Z80_IXL) if (x.reg==Z80_IYH || x.reg==Z80_IYL) break; else y.reg=Z80_L;
    case Z80_IYH: if (y.reg==Z80_IYH) if (x.reg==Z80_IXH || x.reg==Z80_IXL) break; else y.reg=Z80_H;
    case Z80_IYL: if (y.reg==Z80_IYL) if (x.reg==Z80_IXH || x.reg==Z80_IXL) break; else y.reg=Z80_L;
    case Z80_A: case Z80_B: case Z80_C: case Z80_D: case Z80_E:
      switch (x.reg) {
      case Z80_IXH: e.push(0xdd); e.push(0x60+y.reg); break;
      case Z80_IXL: e.push(0xdd); e.push(0x68+y.reg); break;
      case Z80_IYH: e.push(0xfd); e.push(0x60+y.reg); break;
      case Z80_IYL: e.push(0xfd); e.push(0x68+y.reg); break;
      }
    }
    break;
  case Z80_R:
    if (y.reg!=Z80_A) break;
    e[0]=0xed; e[1]=0x4f;
    break;
  }
  if (!e.size()) error("Illegal operand",ERRREP);
  checkjunk(line);
}

void pmLDD(string line,Data &e) {
  e[0]=0xed; e[1]=0xa8;
  checkjunk(line);
}

void pmLDDR(string line,Data &e) {
  e[0]=0xed; e[1]=0xb8;
  checkjunk(line);
}

void pmLDI(string line,Data &e) {
  e[0]=0xed; e[1]=0xa0;
  checkjunk(line);
}

void pmLDIR(string line,Data &e) {
  e[0]=0xed; e[1]=0xb0;
  checkjunk(line);
}

void pmMULUB(string line,Data &e) {
  Op x,y;
  if (getOperands2(line,x,y)==1) { y=x; x.reg=Z80_A; }
  switch (x.reg) {
  case Z80_A:
    switch (y.reg) {
    case Z80_B: case Z80_C: case Z80_D: case Z80_E:
    case Z80_H: case Z80_L: case Z80_A: e[0]=0xed; e[1]=0xc1+y.reg*8; break;
    default: error("Illegal operand",ERRREP); break;
    }
    break;
  default:
    error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmMULUW(string line,Data &e) {
  Op x,y;
  if (getOperands2(line,x,y)==1) { y=x; x.reg=Z80_HL; }
  switch (x.reg) {
  case Z80_HL:
    switch (y.reg) {
    case Z80_BC: case Z80_SP: e[0]=0xed; e[1]=0xb3+y.reg; break;
    default: error("Illegal operand",ERRREP); break;
    }
    break;
  default:
    error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmNEG(string line,Data &e) {
  e[0]=0xed; e[1]=0x44;
  checkjunk(line);
}

void pmNOP(string line,Data &e) {
  e[0]=0x00;
  checkjunk(line);
}

void pmOR(string line,Data &e) {
  Op x,y;
  if (getOperands2(line,x,y)==1) { y=x; x.reg=Z80_A; }
  switch (x.reg) {
  case Z80_A:
    switch (y.reg) {
    case Z80_IXH: e[0]=0xdd; e[1]=0xb4; break;
    case Z80_IXL: e[0]=0xdd; e[1]=0xb5; break;
    case Z80_IYH: e[0]=0xfd; e[1]=0xb4; break;
    case Z80_IYL: e[0]=0xfd; e[1]=0xb5; break;
    case Z80_B: case Z80_C: case Z80_D: case Z80_E:
    case Z80_H: case Z80_L: case Z80_A: e[0]=0xb0+y.reg; break;
    case Z80mnn: if (y.ind>1) { error("Indirection not allowed"); break; }
    case Z80_nn: e[0]=0xf6; e[1]=check8(y.val); break;
    case Z80mHL: case Z80miHL: case Z80mdHL: case Z80mHLi: case Z80mHLd: 
      preinc(e,y); e.push(0xb6); postinc(e,y); break;
    case Z80mIX: case Z80miIX: case Z80mdIX: case Z80mIXi: case Z80mIXd:
      preinc(e,y); e.push(0xdd); e.push(0xb6); e.push(checki(y.val)); postinc(e,y); break;
    case Z80mIY: case Z80miIY: case Z80mdIY: case Z80mIYi: case Z80mIYd:
      preinc(e,y); e.push(0xfd); e.push(0xb6); e.push(checki(y.val)); postinc(e,y); break;
    default: error("Illegal operand",ERRREP); break;
    }
    break;
  default:
    error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmOTDR(string line, Data &e) {
  e[0]=0xed; e[1]=0xbb;
  checkjunk(line);
}

void pmOTIR(string line, Data &e) {
  e[0]=0xed; e[1]=0xb3;
  checkjunk(line);
}

void pmOUT(string line,Data &e) {
  Op x,y;
  getOperands2(line,x,y);
  switch (x.reg) {
  case Z80mC:
    switch (y.reg) {
    case Z80_A: e[0]=0xed; e[1]=0x79; break;
    case Z80_B: e[0]=0xed; e[1]=0x41; break;
    case Z80_C: e[0]=0xed; e[1]=0x49; break;
    case Z80_D: e[0]=0xed; e[1]=0x51; break;
    case Z80_E: e[0]=0xed; e[1]=0x59; break;
    case Z80_H: e[0]=0xed; e[1]=0x61; break;
    case Z80_L: e[0]=0xed; e[1]=0x69; break;
    case Z80mnn: if (y.ind>1) { error("Indirection not allowed"); break; }
    case Z80_nn: if (y.val==0) { e[0]=0xed; e[1]=0x71; break; }
    default: error("Illegal operand",ERRREP); break;
    }
    break;
  case Z80mnn:
    if (y.reg!=Z80_A) { error("Illegal operand",ERRREP); break; }
    e[0]=0xd3; check8(x.val); e[1]=x.val;
    break;
  default: error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmOUTD(string line, Data &e) {
  e[0]=0xed; e[1]=0xab;
  checkjunk(line);
}

void pmOUTI(string line, Data &e) {
  e[0]=0xed; e[1]=0xa3;
  checkjunk(line);
}

void pmPOP(string line, Data &e) {
  Op x;
  getOperand(line,x);
  switch (x.reg) {
  case Z80_AF: e[0]=0xf1; break;
  case Z80_BC: e[0]=0xc1; break;
  case Z80_DE: e[0]=0xd1; break;
  case Z80_HL: e[0]=0xe1; break;
  case Z80_IX: e[0]=0xdd; e[1]=0xe1; break;
  case Z80_IY: e[0]=0xfd; e[1]=0xe1; break;
  default: error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmPUSH(string line, Data &e) {
  Op x;
  getOperand(line,x);
  switch (x.reg) {
  case Z80_AF: e[0]=0xf5; break;
  case Z80_BC: e[0]=0xc5; break;
  case Z80_DE: e[0]=0xd5; break;
  case Z80_HL: e[0]=0xe5; break;
  case Z80_IX: e[0]=0xdd; e[1]=0xe5; break;
  case Z80_IY: e[0]=0xfd; e[1]=0xe5; break;
  default: error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmRES(string line, Data &e) {
  int bit;
  Op x,y;
  getOperands2(line,x,y);
  switch (x.reg) {
  case Z80mnn: if (x.ind>1) { error("Indirection not allowed"); break; }
  case Z80_nn:
    bit=x.val;
    if (bit<0 || bit>7) { error("Illegal operand",ERRREP); bit&=7; }
    switch (y.reg) {
    case Z80_B: case Z80_C: case Z80_D: case Z80_E:
    case Z80_H: case Z80_L: case Z80_A:
      e[0]=0xcb; e[1]=8*bit+0x80+y.reg; break;
    case Z80mHL: case Z80miHL: case Z80mdHL: case Z80mHLi: case Z80mHLd:
      preinc(e,y); e.push(0xcb); e.push(8*bit+0x86); postinc(e,y); break;
    case Z80mIX: case Z80miIX: case Z80mdIX: case Z80mIXi: case Z80mIXd:
      preinc(e,y); e.push(0xdd); e.push(0xcb); e.push(checki(y.val)); e.push(undoc(line,8*bit+0x80)); postinc(e,y); break;
    case Z80mIY: case Z80miIY: case Z80mdIY: case Z80mIYi: case Z80mIYd:
      preinc(e,y); e.push(0xfd); e.push(0xcb); e.push(checki(y.val)); e.push(undoc(line,8*bit+0x80)); postinc(e,y); break;
    default: error("Illegal operand",ERRREP); break;
    }
    break;
  default:
    error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmRET(string line, Data &e) {
  Op x;
  getOperand(line,x);
  switch (x.reg) {
  case Z80_C: e[0]=0xd8; break;
  case Z80_M: e[0]=0xf8; break;
  case Z80_NC: e[0]=0xd0; break;
  case Z80_NZ: e[0]=0xc0; break;
  case Z80_P: e[0]=0xf0; break;
  case Z80_PE: e[0]=0xe8; break;
  case Z80_PO: e[0]=0xe0; break;
  case Z80_Z: e[0]=0xc8; break;
  case Z80_UNK: e[0]=0xc9; break;
  default: error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmRETI(string line, Data &e) {
  e[0]=0xed; e[1]=0x4d;
  checkjunk(line);
}

void pmRETN(string line, Data &e) {
  e[0]=0xed; e[1]=0x45;
  checkjunk(line);
}

void pmRL(string line, Data &e) {
  Op x;
  getOperand(line,x);
  switch (x.reg) {
  case Z80_B: case Z80_C: case Z80_D: case Z80_E:
  case Z80_H: case Z80_L: case Z80_A:
    e[0]=0xcb; e[1]=0x10+x.reg; break;
  case Z80mHL: case Z80miHL: case Z80mdHL: case Z80mHLi: case Z80mHLd:
    preinc(e,x); e.push(0xcb); e.push(0x16); postinc(e,x); break;
  case Z80mIX: case Z80miIX: case Z80mdIX: case Z80mIXi: case Z80mIXd:
    preinc(e,x); e.push(0xdd); e.push(0xcb); e.push(checki(x.val)); e.push(undoc(line,0x10)); postinc(e,x); break;
  case Z80mIY: case Z80miIY: case Z80mdIY: case Z80mIYi: case Z80mIYd:
    preinc(e,x); e.push(0xfd); e.push(0xcb); e.push(checki(x.val)); e.push(undoc(line,0x10)); postinc(e,x); break;
  default: error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmRLA(string line, Data &e) {
  e[0]=0x17;
  checkjunk(line);
}

void pmRLC(string line, Data &e) {
  Op x;
  getOperand(line,x);
  switch (x.reg) {
  case Z80_B: case Z80_C: case Z80_D: case Z80_E:
  case Z80_H: case Z80_L: case Z80_A:
    e[0]=0xcb; e[1]=0x00+x.reg; break;
  case Z80mHL: case Z80miHL: case Z80mdHL: case Z80mHLi: case Z80mHLd:
    preinc(e,x); e.push(0xcb); e.push(0x06); postinc(e,x); break;
  case Z80mIX: case Z80miIX: case Z80mdIX: case Z80mIXi: case Z80mIXd:
    preinc(e,x); e.push(0xdd); e.push(0xcb); e.push(checki(x.val)); e.push(undoc(line,0x00)); postinc(e,x); break;
  case Z80mIY: case Z80miIY: case Z80mdIY: case Z80mIYi: case Z80mIYd:
    preinc(e,x); e.push(0xfd); e.push(0xcb); e.push(checki(x.val)); e.push(undoc(line,0x00)); postinc(e,x); break;
  default: error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmRLCA(string line, Data &e) {
  e[0]=0x07;
  checkjunk(line);
}

void pmRLD(string line, Data &e) {
  e[0]=0xed; e[1]=0x6f;
  checkjunk(line);
}

void pmRR(string line, Data &e) {
  Op x;
  getOperand(line,x);
  switch (x.reg) {
  case Z80_B: case Z80_C: case Z80_D: case Z80_E:
  case Z80_H: case Z80_L: case Z80_A:
    e[0]=0xcb; e[1]=0x18+x.reg; break;
  case Z80mHL: case Z80miHL: case Z80mdHL: case Z80mHLi: case Z80mHLd:
    preinc(e,x); e.push(0xcb); e.push(0x1e); postinc(e,x); break;
  case Z80mIX: case Z80miIX: case Z80mdIX: case Z80mIXi: case Z80mIXd:
    preinc(e,x); e.push(0xdd); e.push(0xcb); e.push(checki(x.val)); e.push(undoc(line,0x18)); postinc(e,x); break;
  case Z80mIY: case Z80miIY: case Z80mdIY: case Z80mIYi: case Z80mIYd:
    preinc(e,x); e.push(0xfd); e.push(0xcb); e.push(checki(x.val)); e.push(undoc(line,0x18)); postinc(e,x); break;
  default: error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmRRA(string line, Data &e) {
  e[0]=0x1f;
  checkjunk(line);
}

void pmRRC(string line, Data &e) {
  Op x;
  getOperand(line,x);
  switch (x.reg) {
  case Z80_B: case Z80_C: case Z80_D: case Z80_E:
  case Z80_H: case Z80_L: case Z80_A:
    e[0]=0xcb; e[1]=0x08+x.reg; break;
  case Z80mHL: case Z80miHL: case Z80mdHL: case Z80mHLi: case Z80mHLd:
    preinc(e,x); e.push(0xcb); e.push(0x0e); postinc(e,x); break;
  case Z80mIX: case Z80miIX: case Z80mdIX: case Z80mIXi: case Z80mIXd:
    preinc(e,x); e.push(0xdd); e.push(0xcb); e.push(checki(x.val)); e.push(undoc(line,0x08)); postinc(e,x); break;
  case Z80mIY: case Z80miIY: case Z80mdIY: case Z80mIYi: case Z80mIYd:
    preinc(e,x); e.push(0xfd); e.push(0xcb); e.push(checki(x.val)); e.push(undoc(line,0x08)); postinc(e,x); break;
  default: error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmRRCA(string line, Data &e) {
  e[0]=0x0f;
  checkjunk(line);
}

void pmRRD(string line, Data &e) {
  e[0]=0xed; e[1]=0x67;
  checkjunk(line);
}

void pmRST(string line, Data &e) {
  Op x;
  getOperand(line,x);
  if (x.reg==Z80mnn) if (x.ind>1) { error("Indirection not allowed"); return; } else x.reg=Z80_nn;
  if (x.reg!=Z80_nn) { error ("Illegal operand"); return; }
  switch (x.val) {
  case 0x00: case 0x08: case 0x10: case 0x18:
  case 0x20: case 0x28: case 0x30: case 0x38:
    e[0]=0xc7+x.val; break;
  default: error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmSBC(string line,Data &e) {
  Op x,y;
  if (getOperands2(line,x,y)==1) { y=x; x.reg=Z80_A; }
  switch (x.reg) {
  case Z80_A:
    switch (y.reg) {
    case Z80_IXH: e[0]=0xdd; e[1]=0x9c; break;
    case Z80_IXL: e[0]=0xdd; e[1]=0x9d; break;
    case Z80_IYH: e[0]=0xfd; e[1]=0x9c; break;
    case Z80_IYL: e[0]=0xfd; e[1]=0x9d; break;
    case Z80_B: case Z80_C: case Z80_D: case Z80_E:
    case Z80_H: case Z80_L: case Z80_A: e[0]=0x98+y.reg; break;
    case Z80mnn: if (y.ind>1) { error("Indirection not allowed"); break; }
    case Z80_nn: e[0]=0xde; e[1]=check8(y.val); break;
    case Z80mHL: case Z80miHL: case Z80mdHL: case Z80mHLi: case Z80mHLd: 
      preinc(e,y); e.push(0x9e); postinc(e,y); break;
    case Z80mIX: case Z80miIX: case Z80mdIX: case Z80mIXi: case Z80mIXd:
      preinc(e,y); e.push(0xdd); e.push(0x9e); e.push(checki(y.val)); postinc(e,y); break;
    case Z80mIY: case Z80miIY: case Z80mdIY: case Z80mIYi: case Z80mIYd:
      preinc(e,y); e.push(0xfd); e.push(0x9e); e.push(checki(y.val)); postinc(e,y); break;
    default: error("Illegal operand",ERRREP); break;
    }
    break;
  case Z80_HL:
    switch (y.reg) {
    case Z80_BC: case Z80_DE: case Z80_HL: case Z80_SP: e[0]=0xed; e[1]=y.reg+0x32; break;
    default: error("Illegal operand",ERRREP); break;
    }
    break;
  default:
    error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmSCF(string line, Data &e) {
  e[0]=0x37;
  checkjunk(line);
}

void pmSET(string line, Data &e) {
  int bit;
  Op x,y;
  getOperands2(line,x,y);
  switch (x.reg) {
  case Z80mnn: if (x.ind>1) { error("Indirection not allowed"); break; }
  case Z80_nn:
    bit=x.val;
    if (bit<0 || bit>7) { error("Illegal operand",ERRREP); bit&=7; }
    switch (y.reg) {
    case Z80_B: case Z80_C: case Z80_D: case Z80_E:
    case Z80_H: case Z80_L: case Z80_A:
      e[0]=0xcb; e[1]=8*bit+0xc0+y.reg; break;
    case Z80mHL: case Z80miHL: case Z80mdHL: case Z80mHLi: case Z80mHLd:
      preinc(e,y); e.push(0xcb); e.push(8*bit+0xc6); postinc(e,y); break;
    case Z80mIX: case Z80miIX: case Z80mdIX: case Z80mIXi: case Z80mIXd:
      preinc(e,y); e.push(0xdd); e.push(0xcb); e.push(checki(y.val)); e.push(undoc(line,8*bit+0xc0)); postinc(e,y); break;
    case Z80mIY: case Z80miIY: case Z80mdIY: case Z80mIYi: case Z80mIYd:
      preinc(e,y); e.push(0xfd); e.push(0xcb); e.push(checki(y.val)); e.push(undoc(line,8*bit+0xc0)); postinc(e,y); break;
    default: error("Illegal operand",ERRREP); break;
    }
    break;
  default:
    error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmSLA(string line, Data &e) {
  Op x;
  getOperand(line,x);
  switch (x.reg) {
  case Z80_B: case Z80_C: case Z80_D: case Z80_E:
  case Z80_H: case Z80_L: case Z80_A:
    e[0]=0xcb; e[1]=0x20+x.reg; break;
  case Z80mHL: case Z80miHL: case Z80mdHL: case Z80mHLi: case Z80mHLd:
    preinc(e,x); e.push(0xcb); e.push(0x26); postinc(e,x); break;
  case Z80mIX: case Z80miIX: case Z80mdIX: case Z80mIXi: case Z80mIXd:
    preinc(e,x); e.push(0xdd); e.push(0xcb); e.push(checki(x.val)); e.push(undoc(line,0x20)); postinc(e,x); break;
  case Z80mIY: case Z80miIY: case Z80mdIY: case Z80mIYi: case Z80mIYd:
    preinc(e,x); e.push(0xfd); e.push(0xcb); e.push(checki(x.val)); e.push(undoc(line,0x20)); postinc(e,x); break;
  default: error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmSLL(string line, Data &e) {
  Op x;
  getOperand(line,x);
  switch (x.reg) {
  case Z80_B: case Z80_C: case Z80_D: case Z80_E:
  case Z80_H: case Z80_L: case Z80_A:
    e[0]=0xcb; e[1]=0x30+x.reg; break;
  case Z80mHL: case Z80miHL: case Z80mdHL: case Z80mHLi: case Z80mHLd:
    preinc(e,x); e.push(0xcb); e.push(0x36); postinc(e,x); break;
  case Z80mIX: case Z80miIX: case Z80mdIX: case Z80mIXi: case Z80mIXd:
    preinc(e,x); e.push(0xdd); e.push(0xcb); e.push(checki(x.val)); e.push(undoc(line,0x30)); postinc(e,x); break;
  case Z80mIY: case Z80miIY: case Z80mdIY: case Z80mIYi: case Z80mIYd:
    preinc(e,x); e.push(0xfd); e.push(0xcb); e.push(checki(x.val)); e.push(undoc(line,0x30)); postinc(e,x); break;
  default: error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmSRA(string line, Data &e) {
  Op x;
  getOperand(line,x);
  switch (x.reg) {
  case Z80_B: case Z80_C: case Z80_D: case Z80_E:
  case Z80_H: case Z80_L: case Z80_A:
    e[0]=0xcb; e[1]=0x28+x.reg; break;
  case Z80mHL: case Z80miHL: case Z80mdHL: case Z80mHLi: case Z80mHLd:
    preinc(e,x); e.push(0xcb); e.push(0x2e); postinc(e,x); break;
  case Z80mIX: case Z80miIX: case Z80mdIX: case Z80mIXi: case Z80mIXd:
    preinc(e,x); e.push(0xdd); e.push(0xcb); e.push(checki(x.val)); e.push(undoc(line,0x28)); postinc(e,x); break;
  case Z80mIY: case Z80miIY: case Z80mdIY: case Z80mIYi: case Z80mIYd:
    preinc(e,x); e.push(0xfd); e.push(0xcb); e.push(checki(x.val)); e.push(undoc(line,0x28)); postinc(e,x); break;
  default: error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmSRL(string line, Data &e) {
  Op x;
  getOperand(line,x);
  switch (x.reg) {
  case Z80_B: case Z80_C: case Z80_D: case Z80_E:
  case Z80_H: case Z80_L: case Z80_A:
    e[0]=0xcb; e[1]=0x38+x.reg; break;
  case Z80mHL: case Z80miHL: case Z80mdHL: case Z80mHLi: case Z80mHLd:
    preinc(e,x); e.push(0xcb); e.push(0x3e); postinc(e,x); break;
  case Z80mIX: case Z80miIX: case Z80mdIX: case Z80mIXi: case Z80mIXd:
    preinc(e,x); e.push(0xdd); e.push(0xcb); e.push(checki(x.val)); e.push(undoc(line,0x38)); postinc(e,x); break;
  case Z80mIY: case Z80miIY: case Z80mdIY: case Z80mIYi: case Z80mIYd:
    preinc(e,x); e.push(0xfd); e.push(0xcb); e.push(checki(x.val)); e.push(undoc(line,0x38)); postinc(e,x); break;
  default: error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmSUB(string line, Data &e) {
  Op x,y;
  if (getOperands2(line,x,y)==1) { y=x; x.reg=Z80_A; }
  switch (x.reg) {
  case Z80_A:
    switch (y.reg) {
    case Z80_IXH: e[0]=0xdd; e[1]=0x94; break;
    case Z80_IXL: e[0]=0xdd; e[1]=0x95; break;
    case Z80_IYH: e[0]=0xfd; e[1]=0x94; break;
    case Z80_IYL: e[0]=0xfd; e[1]=0x95; break;
    case Z80_B: case Z80_C: case Z80_D: case Z80_E:
    case Z80_H: case Z80_L: case Z80_A: e[0]=0x90+y.reg; break;
    case Z80mnn: if (y.ind>1) { error("Indirection not allowed"); break; }
    case Z80_nn: e[0]=0xd6; e[1]=check8(y.val); break;
    case Z80mHL: case Z80miHL: case Z80mdHL: case Z80mHLi: case Z80mHLd: 
      preinc(e,y); e.push(0x96); postinc(e,y); break;
    case Z80mIX: case Z80miIX: case Z80mdIX: case Z80mIXi: case Z80mIXd:
      preinc(e,y); e.push(0xdd); e.push(0x96); e.push(checki(y.val)); postinc(e,y); break;
    case Z80mIY: case Z80miIY: case Z80mdIY: case Z80mIYi: case Z80mIYd:
      preinc(e,y); e.push(0xfd); e.push(0x96); e.push(checki(y.val)); postinc(e,y); break;
    default: error("Illegal operand",ERRREP); break;
    }
    break;
  default:
    error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

void pmXOR(string line,Data &e) {
  Op x,y;
  if (getOperands2(line,x,y)==1) { y=x; x.reg=Z80_A; }
  switch (x.reg) {
  case Z80_A:
    switch (y.reg) {
    case Z80_IXH: e[0]=0xdd; e[1]=0xac; break;
    case Z80_IXL: e[0]=0xdd; e[1]=0xad; break;
    case Z80_IYH: e[0]=0xfd; e[1]=0xac; break;
    case Z80_IYL: e[0]=0xfd; e[1]=0xad; break;
    case Z80_B: case Z80_C: case Z80_D: case Z80_E:
    case Z80_H: case Z80_L: case Z80_A: e[0]=0xa8+y.reg; break;
    case Z80mnn: if (y.ind>1) { error("Indirection not allowed"); break; }
    case Z80_nn: e[0]=0xee; e[1]=check8(y.val); break;
    case Z80mHL: case Z80miHL: case Z80mdHL: case Z80mHLi: case Z80mHLd: 
      preinc(e,y); e.push(0xae); postinc(e,y); break;
    case Z80mIX: case Z80miIX: case Z80mdIX: case Z80mIXi: case Z80mIXd:
      preinc(e,y); e.push(0xdd); e.push(0xae); e.push(checki(y.val)); postinc(e,y); break;
    case Z80mIY: case Z80miIY: case Z80mdIY: case Z80mIYi: case Z80mIYd:
      preinc(e,y); e.push(0xfd); e.push(0xae); e.push(checki(y.val)); postinc(e,y); break;
    default: error("Illegal operand",ERRREP); break;
    }
    break;
  default: error("Illegal operand",ERRREP); break;
  }
  checkjunk(line);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FunctionTable<mFun,void> msxfuntab;

void initPiMSX() {
  FunctionEntry<mFun> msxfuns[] = {
    "adc",pmADC,
    "add",pmADD,
    "and",pmAND,
    "bit",pmBIT,
    "call",pmCALL,
    "ccf",pmCCF,
    "cp",pmCP,
    "cpd",pmCPD,
    "cpdr",pmCPDR,
    "cpi",pmCPI,
    "cpir",pmCPIR,
    "cpl",pmCPL,
    "daa",pmDAA,
    "dec",pmDEC,
    "di",pmDI,
    "djnz",pmDJNZ,
    "ei",pmEI,
    "ex",pmEX,
    "exx",pmEXX,
    "halt",pmHALT,
    "im",pmIM,
    "in",pmIN,
    "inc",pmINC,
    "ind",pmIND,
    "indr",pmINDR,
    "ini",pmINI,
    "inir",pmINIR,
    "jp",pmJP,
    "jr",pmJR,
    "ld",pmLD,
    "ldd",pmLDD,
    "lddr",pmLDDR,
    "ldi",pmLDI,
    "ldir",pmLDIR,
    "mulub",pmMULUB,
    "muluw",pmMULUW,
    "neg",pmNEG,
    "nop",pmNOP,
    "or",pmOR,
    "otdr",pmOTDR,
    "otir",pmOTIR,
    "out",pmOUT,
    "outd",pmOUTD,
    "outi",pmOUTI,
    "pop",pmPOP,
    "push",pmPUSH,
    "res",pmRES,
    "ret",pmRET,
    "reti",pmRETI,
    "retn",pmRETN,
    "rl",pmRL,
    "rla",pmRLA,
    "rlc",pmRLC,
    "rlca",pmRLCA,
    "rld",pmRLD,
    "rr",pmRR,
    "rra",pmRRA,
    "rrc",pmRRC,
    "rrca",pmRRCA,
    "rrd",pmRRD,
    "rst",pmRST,
    "sbc",pmSBC,
    "scf",pmSCF,
    "set",pmSET,
    "sla",pmSLA,
    "sll",pmSLL,
    "sra",pmSRA,
    "srl",pmSRL,
    "sub",pmSUB,
    "xor",pmXOR,

    "djnz.",pmDJNZ,
    "jp.",pmJP,
    "jr.",pmJR
  };
  msxfuntab.init(msxfuns, sizeof msxfuns/sizeof msxfuns[0]);
}

void pimsx(string &line,SourceList *source) {
  if (line.empty()) return;
  string d=getinstruction(line);
  if (d.empty()) return;
  mFun funp=msxfuntab.getfunp(d);
  if (!funp) { error1("Unrecognized instruction",d); return; }
  source->push_back(new Mnemonic(line,funp));
  source->back()->Process();
}

//eof
