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

// fileio.h

enum FILEMODE { OVERWRITE, UPDATE };
extern StringList paths;

string fileexists(string,string);
string getpath(string&);
string getfilename(string&);

class ReadFile {
public:
  ReadFile(string);
  ~ReadFile();
  void readtostringlist(StringList&);
  void getbuffer(byte *&buffer,int &size) { buffer=_buffer; size=_size; }
  bool ok() { return _buffer!=0; }
private:
  int _size;
  byte *_buffer;
};

class WriteFile {
public:
  WriteFile(string name, FILEMODE mode=OVERWRITE);
  ~WriteFile();
  void write(Data&);
  void write(StringList&);
  void skip(int);
  bool ok() { return _file!=0; }
private:
  void _writebuf();
  FILE *_file;
  FILEMODE _mode;
  Data _buffer;
};

//eof
