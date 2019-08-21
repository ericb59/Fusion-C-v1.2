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

// reader.h

int cmphstr(string&,string);
int getstringlength(string&);
void skipstring(istring&);
string tolower(string);
void skipblanks(string&);
int comma(string&);
int needcomma(string&);
string getinstructionpart(string&);
string getinstruction(string&);
string getid(string&);
void checkjunk(string&);
char nextchar(istring const&, string const&);
int sbcneed(string&,char);
int cneed(string&,char);
int needa(string&,string,int,string="",int=0,string="",int=0);
int need(string&,char*);
int needequ(string&);
int needfield(string&);
int needset(string&);
int islabchar(char);
void checkparen(string&);
int check8(int);
int check5(int);
int check8u(int);
int check3u(int);
int checki(int);
int check16(int);
int check24(int);
void makestring(string&);
void getstring(string&,string&);
void getcharconstchar(string&,int&);
StringList getarguments(string&);
string getargument(string&,bool=false,bool=false);
template <class T> string tostr(T n,int w=0) { ostringstream s; s.width(w); s << n; return s.str(); }
string tohex(int,int=4);
int getinSTRUCTion(string&);
string trim(string);
void getpage(string&,IntList&);

class Find {
public:
  Find(char*s) { for (int i=0;i!=256;++i) _c[i]=false; while (*s) _c[*s++]=true; }
  int find(string&s) { for (int i=0;i!=(int)s.size();++i) if (_c[s[i]]) return i; return (int)string::npos; }
  int findnot(string&s) { for (int i=0;i!=(int)s.size();++i) if (!_c[s[i]]) return i; return (int)string::npos; }
private:
  bool _c[256];
};

//eof
