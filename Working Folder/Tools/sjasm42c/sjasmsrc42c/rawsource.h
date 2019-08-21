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

// rawsource.h

class RawSource {
public:
  RawSource(string);
  RawSource(StringList);
  StringList ReadUntil(StringList,StringList,string);
  void Process();
  SourceList &GetSource() { return _source; }
  void ClearSource() { _source.clear(); }
  void AddToSource(SourceList n) { _source.splice(_source.end(),n); }
protected:
  RawSource() {}
  void _Preprocess();
  string _KillComments();
  void _Group1(string&);
  void _ReplaceDefs(string &);
  void _Group2(string&);
  void _CutLines(string&);
  void virtual _Parse(string&);
  int _ParseLabel(string&);
  void _ParseRepeat(string&);
  void _ExpandMacros(string&);
  int _AssemblerDirectives(string&);
  void _Mnemonics(string&);

  int _nextline();
  StringList _sl;
  iStringList _it;
  SourceList _source;
  string _unexpectedin;
  int _unexcurlin,_unexlistcurlin;
};

class RSRepeat : public RawSource {
public:
  RSRepeat() {}
  void parserepeat(string &s) { _ParseRepeat(s); }
};

class RSStructure : public RawSource {
public:
  RSStructure(int n_offset, int n_align, bool n_global, Structure &n_s, StringList &sl) : 
      _offset(n_offset), _defaultalign(n_align), _global(n_global), _s(n_s), _mustalign(false), RawSource(sl) {}
private:
  void _Parse(string&);
  int _offset,_newalign,_defaultalign;
  bool _global, _mustalign;
  Structure &_s;
};

//eof
