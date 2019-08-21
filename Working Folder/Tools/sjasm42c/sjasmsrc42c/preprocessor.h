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

// preprocessor.h

struct ifinfo {
  bool _active;
  bool _beenactive;
  bool _else;
  int _level;
  int _nest;
  int _curlin,_listcurlin;
  string _filename;
};

struct Stop {
  int _stop;
  string _retval;
};

extern stack<ifinfo> ifstack;
extern Stop stop;

enum { ROTATE=-1, ITERATION=-2, BREAK=1, CONTINUE, EXITMACRO, END };

void initpreprocessor();
string ReplaceDefines(string);

//eof
