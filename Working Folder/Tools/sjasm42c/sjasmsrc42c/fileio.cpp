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

// fileio.cpp

#include "sjasm.h"

StringList paths;

string fileexists(string npath, string filename) {
  string np;
  if (filename[0]==SLASH) np=filename;
  else { np=npath; if (!np.empty() && np[np.size()-1]!=SLASH) np+=SLASH; np+=filename; }
  FILE *f=fopen(np.c_str(),"r"); if (f) fclose(f);
  if (f) return np; else return "";
}

string getpath(string &filename) {
  string np;
  np=fileexists(listopt._path,filename);
  if (np.empty()) {
    sbcneed(filename,'<');
    for (iStringList i=paths.begin(); i!=paths.end(); ++i) { np=fileexists(*i,filename); if (!np.empty()) break; }
  }
  if (np.empty()) np=fileexists(listopt._path,filename);
  if (np.empty()) { np=listopt._path; if (!np.empty() && np[np.size()-1]!=SLASH) np+=SLASH; np+=filename; }
  int pos=(int)np.find_last_of(SLASH);
  if (pos==string::npos) { listopt._path.clear(); return filename; }
  listopt._path=np.substr(0,pos);
  return np;
}

string getfilename(string &p) {
  int pos=0;
  string res;
  if (sbcneed(p,'"')) {
    res=p.substr(0,pos=(int)p.find_first_of('"'));
    if (pos==string::npos) error1("No closing '\"'");
    else ++pos;
  } else if (p[0]=='<') {
    res=p.substr(0,pos=(int)p.find_first_of('>'));
    if (pos==string::npos) error1("No closing '>'");
    else ++pos;
  } else {
    res=p.substr(0,pos=(int)p.find_first_of(','));
  }
  if (pos==string::npos) p.clear(); else p=p.substr(pos);
  for (istring i=res.begin(); i!=res.end(); ++i) if (*i==BADSLASH) *i=SLASH;
  return res;
}

ReadFile::ReadFile(string n_name) : _buffer(0) {
    FILE *file;
    if ((file=fopen(n_name.c_str(),"rb"))==NULL) { error1("Error opening file",n_name); return; }
    fseek(file,0,SEEK_END); _size=ftell(file); fseek(file,0,SEEK_SET);
    _buffer=new unsigned char[_size+1];
    if (fread(_buffer,_size,1,file)) _buffer[_size]=0;
    else { _buffer=0; error1("Error opening file",n_name); }
    fclose(file);
}

ReadFile::~ReadFile() {
  delete [] _buffer;
}

void ReadFile::readtostringlist(StringList &sl) {
  char *ls,*le;
  ls=le=(char *)_buffer;
  while (*le)
    switch (*le) {
    case 10: *le++=0; if (*le==13) ++le; sl.push_back(ls); ls=le; break;
    case 13: *le++=0; if (*le==10) ++le; sl.push_back(ls); ls=le; break;
    default: ++le; break;
    }
  if (ls!=le) sl.push_back(ls);
}

WriteFile::WriteFile(string name, FILEMODE n_mode) {
  _mode=n_mode;
  if (_mode==UPDATE && fileexists("",name).empty()) _mode=OVERWRITE;
  _file=fopen(name.c_str(),_mode==OVERWRITE?"wb":"rb+");
  fseek(_file,0,SEEK_SET);
  if (!_file) { error("Error opening file",name); return; }
}

WriteFile::~WriteFile() {
  _writebuf();
  fclose(_file);
}

void WriteFile::write(Data &e){
  _buffer.push(e);
}

void WriteFile::write(StringList &sl) {
  for (iStringList isl=sl.begin(); isl!=sl.end(); ++isl) {
    fputs(isl->c_str(),_file);
    fputs(NEWLINE,_file);
  }
}

void WriteFile::skip(int len){
  if (_mode==OVERWRITE) {
    while (len--) _buffer.push(0);
  } else {
    _writebuf();
    fseek(_file,len,SEEK_CUR);
  }
}

void WriteFile::_writebuf() {
  if (_buffer.size()) if((int)fwrite(_buffer.getdatap(),1,_buffer.size(),_file)<_buffer.size()) error("Write error",ERRFATAL);
  _buffer.clear();
}

//eof
