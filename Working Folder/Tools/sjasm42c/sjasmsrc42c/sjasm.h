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

// sjasm.h

#define VERSION "004230"
//#define METARM

#ifdef WIN32
#define SLASH '\\'
#define BADSLASH '/'
#define NEWLINE "\r\n"

//#include <windows.h>

//#pragma warning(disable: 4267)
//#pragma warning(disable: 4018)
//#pragma warning(disable: 4800)

//#pragma warning(disable: 4311)
//#pragma warning(disable: 4244)
//#pragma warning(disable: 4786)

#pragma warning(disable: 4996) // this function or variable may be unsafe
#pragma warning(disable: 4706) // assignment within conditional expression
#pragma warning(disable: 4127) // conditional expression is constant
#pragma warning(disable: 4389) // signed/unsigned mismatch
//#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4510) // default constructor could not be generated
#pragma warning(disable: 4610) // struct can never be instantiated - user defined constructor
#pragma warning(disable: 4512) // assignment operator could not be generated
#pragma warning(disable: 4244) // conversion from 'bla' to 'blabla', possible loss of data

#else
#define SLASH '/'
#define BADSLASH '\\'
#define NEWLINE "\n"
#endif

#include <time.h>
#include <list>
#include <stack>
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;

const int BIGVALUE=2147483647; // :p

struct Options {
  Options() : listloops(false), listmacros(true), onlybp(false), useldradr(true), usemovlsl(true), allpages(false),
    optimizejumps(false) {}
  bool listloops;
  bool listmacros;
  bool listbincludes;
  bool onlybp;             // only [] for indirections, no ()               MSX
  bool useldradr;          // use ldr when add/sub does not work with adr   ARM
  bool usemovlsl;          // use mov\lsl if possible with ldr              ARM
  bool allpages;           // must all pages exist?
  bool optimizejumps;      // convert jumps (jp/jr/djnz)                    MSX
};

class Rout;

#include "datastructures.h"

extern Options options,defaultoptions;

extern string starttime,startdate,sourcefilename,destfilename,listfilename,expfilename,symfilename;
extern int listcurlin,adres,page,pass,labsnok,mapadr,synerr,again,labelnotfound,macronummer,unieknummer,curlin;
extern int lpass,errors,adrerrors,symfile,dolistfile,labellisting,partlisting,lablin;
extern string maclabp,vorlabp,modlabp,version;
extern char tobuffer[64];

extern DefineTable deftab;
extern DefineArgTable defargtab;
extern MacNumTable macnumtab;
extern LabelTable labtab;
extern MacroTable mactab;
extern StructTable structtab;
extern NumLabelTable numlabtab;

#include "fileio.h"
#include "reader.h"
#include "datadir.h"
#include "preprocessor.h"
#include "source.h"
#include "output.h"
#include "rawsource.h"
#include "expressions.h"
#include "directives.h"
#include "errors.h"
#include "pimsx.h"
#ifdef METARM
#include "pithumb.h"
#include "piarm.h"
#endif

extern ErrorTable errtab;
extern stack<int> mapadrstack;
extern stack<string> modulestack;
extern IntList pages;
extern vector<string> cmdparameter;

extern vector<Output*> output;
extern int onr;

extern void (*piCPU)(string&,SourceList*);

//eof
