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

// errors.h

enum { ERRNORMAL,ERRFATAL,ERRINTERNAL,ERRLPASS,ERRREP,ERRPASS1 };

void error(int curlin, int listcurlin, string error, string args,int=0);
void error(string,string,int=0);
void error(string,int=0);
void error1(string,string,int=ERRPASS1);
void error1(string,int=ERRPASS1);
void errorvalue();
void errorvalue(string);
void errortarget(string);
void errortarget(int);

class ErrorTable {
public:
  ErrorTable() : _nextlocation(1), _size(23) {
    _hashtable.resize(_size,0);
    _errtab.resize(_size);
  }
  int add(int,string,int);
  void geterrors(int,StringList&);
private:
  class _ErrorEntry {
  public:
    int _line;
    string _msg;
    int _type;
  };
  vector<int> _hashtable;
  int _nextlocation,_size;
  vector<_ErrorEntry> _errtab;
  int _hash(int s) { unsigned h=s*7; return h%_size; }
  void _grow();
};

extern ErrorTable errtab;

//eof
