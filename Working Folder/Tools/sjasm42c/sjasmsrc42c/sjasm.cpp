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

#include "sjasm.h"

Options options;

string version="0.42c";

string starttime,startdate,sourcefilename,destfilename,listfilename,expfilename,symfilename;
int listcurlin,adres,page,pass,labsnok,mapadr,synerr,again,labelnotfound,macronummer=0,unieknummer=0,curlin,lablin;
int lpass=0,errors=0,adrerrors=0,symfile=0,dolistfile=1,labellisting=1,partlisting=1;
char tobuffer[64];

IntList pages;
LabelTable labtab;
NumLabelTable numlabtab;
vector<string> cmdparameter;

void (*piCPU)(string&,SourceList*);

string maclabp,vorlabp,modlabp;

void initpass() {
  listopt.init();
  ++pass;
  onr=0;
  listcurlin=0;
  curlin=0;
  adres=0;
  page=0;
  pages.clear();
  pages.push_back(page);
  mapadr=0;
  labsnok=0;
  synerr=1;
  lablin=0;
  maclabp="";
  vorlabp="_";
  modlabp="";
  piCPU=pimsx;
//  plist=listlistlinez80;
  while (!modulestack.empty()) modulestack.pop();
  while (!mapadrstack.empty()) mapadrstack.pop();
  resetoutput();
  numlabtab.resetindex();
  routlabel=0;
}

void endpass() {
  ListOpt olistopt=listopt;
  while (!phasestack.empty()) {
    listopt._filename=phasestack.top()._filename;
    error(phasestack.top()._curlin,phasestack.top()._listcurlin,"PHASE without DEPHASE","");
    phasestack.pop();
  }
  while (!ifstack.empty()) {
    listopt._filename=ifstack.top()._filename;
    error(ifstack.top()._curlin,ifstack.top()._listcurlin,"IF without ENDIF","");
    ifstack.pop();
  }
  listopt=olistopt;
}

void getoptions(char **&argv,int &i) {
  char *p,c;
  while (argv[i] && *argv[i]=='-') {
    p=argv[i++]+1; 
    do {
      c=*p++;
      switch (tolower(c)) {
//      case 'q': dolistfile=0; break;
      case 's': symfile=1; break;
//      case 'l': labellisting=1; partlisting=1; break;
      case 'i': paths.push_back(p); p=""; break;
      case 'j': options.optimizejumps=true; break;
      default:
        cout << "Unrecognised option: " << c << endl;
        break;
      }
    } while (*p);
  }
}

string makefilename(string &fn, string ext) {
  int p=(int)fn.find_last_of('.');
  if (p==string::npos) return fn+ext;
  else return fn.substr(0,p)+ext;
}

#ifdef WIN32
int __cdecl main(int argc, char *argv[]) {
#else
int main(int argc, char *argv[]) {
#endif
#ifdef _DEBUG
  cout << "Sjasm v" << version << " [" << __DATE__ " " << __TIME__ << "]\n";
#else
#ifdef _METARM
  cout << "Sjasm v" << version << " - www.xl2s.tk\n";
#else
  cout << "Sjasm Z80 Assembler v" << version << " - www.xl2s.tk\n";
#endif
#endif

  if (argc==1) {
    cout << "Copyright 2011 Sjoerd Mastijn\n";
    cout << "\nUsage:\nsjasm [-options] sourcefile [targetfile [parameters]]\n";
    cout << "\nOption flags as follows:\n";
//    cout << "  -l        Label table in listing\n";
    cout << "  -s        Generate .SYM symbol file\n";
//    cout << "  -q        No listing\n";
    cout << "  -i<path>  Includepath\n";
    cout << "  -j        Optimize jumps\n";
    exit(1);
  }

  int i=1,p;
  getoptions(argv,i);
  if (argv[i]) sourcefilename=argv[i++];
  if (argv[i]) destfilename=argv[i++];
  p=10; while (p--) cmdparameter.push_back(" ");
  p=1; while (argv[i] && p<10) cmdparameter[p++]=argv[i++];
  if (argv[i]) error("Too many command line parameters");

  if (!sourcefilename[0]) { cout << "No inputfile" << endl; exit(1); }
  if (!destfilename[0]) destfilename=makefilename(sourcefilename,".out");
  listfilename=makefilename(destfilename,".lst");
  expfilename=makefilename(destfilename,".exp");
  symfilename=makefilename(destfilename,".sym");

  initpreprocessor();
  initPidata();
  initDir();
  initPiMSX();
#ifdef METARM
  initPiThumb();
  initPiArm();
#endif

  time_t st=time(0);
  strftime(tobuffer,64,"%H:%M:%S",localtime(&st)); starttime=tobuffer;
  strftime(tobuffer,64,"%Y.%m.%d",localtime(&st)); startdate=tobuffer;

  pass=0;
  initpass();
  output.push_back(new Output(destfilename));
  RawSource *rawsource=new RawSource(sourcefilename);
  rawsource->GetSource().push_back(new dPool()); //
  endpass();
  sortoutput();

  do {
    initpass();
    process(rawsource->GetSource());
    endpass();
    sortoutput();
  } while (pass<3 || labsnok && !errors);

  StringList dump;
  if (partlisting) { dumpoutput(dump); }

  saveoutput();

  initpass();
  lpass=1;
  listlist(rawsource->GetSource(),dump);

  if (symfile) outputsymbols();

#ifdef _DEBUG
  cout << "\nPRESS ENTER";
  cin.get();
#endif

  return (errors+adrerrors)!=0;
}

//eof
