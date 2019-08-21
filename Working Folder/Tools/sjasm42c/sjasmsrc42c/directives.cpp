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

// directives.cpp

#include "sjasm.h"

int prevorg=0, prevsize=-1;

void dirZ80(string &line, RawSource *rs) {
  piCPU=pimsx;
  rs->GetSource().push_back(new ListMode(plist=listlistlinez80));
  checkjunk(line);
}

#ifdef METARM
void dirTHUMB(string &line, RawSource *rs) {
  piCPU=pithumb;
  rs->GetSource().push_back(new ListMode(plist=listlistline16));
  checkjunk(line);
}

void dirARM(string &line, RawSource *rs) {
  piCPU=piarm;
  rs->GetSource().push_back(new ListMode(plist=listlistline32));
  checkjunk(line);
}
#endif

void dirEND(string &line, RawSource *) {
  stop._stop=END;
  checkjunk(line);
}

void dirORG(string &line, RawSource *rs) {
  rs->GetSource().push_back(new dOrg(line));
  rs->GetSource().back()->Process();
}

void dirCODE(string &line, RawSource *rs) {
  Rout *r=output[onr]->getnewroutp();
  int rnr=output[onr]->getrout();
  if (!phasestack.empty()) error("CODE not allowed in PHASE block");
  rs->GetSource().push_back(new dPool()); //
  rs->GetSource().push_back(new dRout(rnr,r,line));
  rs->GetSource().back()->Process();
}

void dirMODULE(string &line, RawSource *rs) {
  string n=getid(line); checkjunk(line);
  modulestack.push(modlabp);
  rs->GetSource().push_back(new SetModule(n));
  rs->GetSource().back()->Process();
}

void dirENDMODULE(string &line, RawSource *rs) {
  string n=getid(line); checkjunk(line);
  if (modulestack.empty()) { error("ENDMODULE without MODULE"); return; }
  if (n.empty() || n==modlabp) {
    modlabp=modulestack.top(); modulestack.pop();
  } else {
    do {
      if (modulestack.empty()) { error("MODULE not found"); break; }
      modlabp=modulestack.top(); modulestack.pop();
    } while (n!=modlabp);
    if (modulestack.empty()) modlabp.clear(); else { modlabp=modulestack.top(); modulestack.pop(); }
  }
  rs->GetSource().push_back(new SetModule(modlabp));
  rs->GetSource().back()->Process();
}

void dirPAGE(string &line, RawSource *rs) {
  Rout *r=output[onr]->getnewroutp();
  int rnr=output[onr]->getrout();
  rs->GetSource().push_back(new dPool()); //
  rs->GetSource().push_back(new dPage(rnr,r,line));
  rs->GetSource().back()->Process();
}

void dirDEFPAGE(string &line, RawSource *) {
  int firstpage, lastpage, org, size, val;
  if (!ParseExpression(line,firstpage)) { error("Syntax error",ERRREP); return; }
  if (labelnotfound) error("Forward reference");
  if (firstpage<0 || firstpage>255) { error("Illegal pagenumber"); return; }
  if (need(line,"..")) {
    if (!ParseExpression(line,lastpage)) { error("Syntax error",ERRREP); return; }
    if (labelnotfound) error("Forward reference");
    if (lastpage<0 || firstpage>lastpage) { error("Illegal pagenumber"); return; }
  } else lastpage=firstpage;
  if (comma(line)) {
      if (sbcneed(line,'*')) {
        val=-1;
      } else {
        if (!ParseExpression(line,val)) { error("Syntax error",ERRREP); return; }
        if (labelnotfound) error("Forward reference");
        if (val<0) { val=0; error("Negative origin"); }
      }
    prevorg=val;
    if (comma(line)) {
      if (sbcneed(line,'*')) {
        val=-1;
      } else {
        if (!ParseExpression(line,val)) { error("Syntax error",ERRREP); return; }
        if (labelnotfound) error("Forward reference");
        if (val<0) { val=0; error("Negative size"); }
      }
      prevsize=val;
    }
  }
  org=prevorg; size=prevsize;
  for (val=firstpage;val<=lastpage;++val) output[onr]->defpage(val,org,size);
  checkjunk(line);
}

void dirMAPALIGN(string &line, RawSource *rs) {
  rs->GetSource().push_back(new dMapAlign(line));
  rs->GetSource().back()->Process();
}

void dirMAP(string &line, RawSource *rs) {
  rs->GetSource().push_back(new dMap(line));
  rs->GetSource().back()->Process();
}

void dirENDMAP(string &line, RawSource *rs) {
  rs->GetSource().push_back(new dEndmap());
  rs->GetSource().back()->Process();
  checkjunk(line);
}

void dirALIGN(string &line, RawSource *rs) {
  rs->GetSource().push_back(new dAlign(line));
  rs->GetSource().back()->Process();
}

void dirPHASE(string &line, RawSource *rs) {
  rs->GetSource().push_back(new dPhase(line));
  rs->GetSource().back()->Process();
}

void dirDEPHASE(string &line, RawSource *rs) {
  rs->GetSource().push_back(new dDephase());
  rs->GetSource().back()->Process();
  checkjunk(line);
}
/*
void dirINCBIN(string &line, RawSource *rs) {
  rs->GetSource().push_back(new dIncbin(line,list));
  rs->GetSource().back()->Process();
}
*/
void dirINCBINDOT(string &line, RawSource *rs) {
  string p;
  bool list=options.listbincludes;
  while (!line.empty() && line[0]=='.') {
    line.erase(0,1); p=getinstructionpart(line);
    if (p=="list") list=true;
    else if (p=="nolist") list=false;
    else error("Unknown instruction","incbin."+p);
  }
  rs->GetSource().push_back(new dIncbin(line,list));
  rs->GetSource().back()->Process();
}

void getoutput(string &line, RawSource *rs, FILEMODE m) {
  string n=getfilename(line);
  output.push_back(new Output(n,m));
  onr=(int)output.size()-1;
  rs->GetSource().push_back(new SetOutput(onr));
  rs->GetSource().back()->Process();
  checkjunk(line); line.clear();
  dirCODE(line,rs);
}

void dirOUTPUT(string &line, RawSource *rs) {
  getoutput(line,rs,OVERWRITE);
}

void dirUPDATE(string &line, RawSource *rs) {
  getoutput(line,rs,UPDATE);
}

void dirINCDIR(string &line, RawSource *) {
  skipblanks(line);
  paths.insert(paths.begin(),line);
}

void dirASSERT(string &line, RawSource *rs) {
  rs->GetSource().push_back(new dAssert(line));
}

void dirSIZE(string &line, RawSource *) {
  int ns;
  if (!ParseExpression(line,ns)) { error("Syntax error",ERRREP); return; }
  if (labelnotfound) error("Forward reference");
  if (ns<0) { ns=-1; error("Negative size"); }
  output[onr]->setpagesize(ns);
  checkjunk(line);
}

void dirERROR(string &line, RawSource *) {
  skipblanks(line);
  error(line,ERRPASS1);
  line.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FunctionTable<pFun,RawSource> dirfuntab;

void initDir() {
  FunctionEntry<pFun> funs[] = {
    "end",dirEND,
    "code",dirCODE,
    "module",dirMODULE,
    "endmodule",dirENDMODULE,
    "page",dirPAGE,
    "defpage",dirDEFPAGE,
    "##",dirMAPALIGN,
    "mapalign",dirMAPALIGN,
    "map",dirMAP,
    "endmap",dirENDMAP,
    "align",dirALIGN,
    "phase",dirPHASE,
    "dephase",dirDEPHASE,
    "incbin",/*dirINCBIN,*/ dirINCBINDOT,
    "output",dirOUTPUT,
    "update",dirUPDATE,
    "incdir",dirINCDIR,
    "assert",dirASSERT,
    "org",dirORG,
    "size",dirSIZE,
    "error",dirERROR,

    "z80",dirZ80,
#ifdef METARM
    "thumb",dirTHUMB,
    "arm",dirARM,
#endif

    "incbin.",dirINCBINDOT,
  };
  dirfuntab.init(funs, sizeof funs/sizeof funs[0]);
}

//eof
