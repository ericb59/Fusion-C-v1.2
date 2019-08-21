Bin2froh Ver 1.0.0
Copyright 2012 Ming Chen. All rights reserved.
----------------------------------------------------------


Contents
--------

1. What Is bin2froh?
2. What's In Version 1.0.0
3. How To Install bin2froh?
4. Compiling bin2froh In Linux and Unix-type Systems
5. Compiling bin2froh In Msdos And Win32
6. Porting To Other Systems
7. History Of Changes
8. Copyright And Contacting The Author


1. What Is Bin2froh?
--------------------

Binary files, like Bitmap, have binary format ,while C compiler requires
that they are converted to char arrays in a C header file.

Bin2froh comprises two programs, "bintoh" and "binfromh", which convert
binary files to and from C-style char arrays. Use "bintoh" to convert 
binary files to C-style char arrays, and "binfromh" to revert the works 
done by "bintoh".

It looks like a strange name, but I prefer this because of its close 
relationship with the various bin2h tools and the Tofrodos tool. To be plain 
with you, I used Tofrodos as the template. -:)


2. What's In Version 1.0.0
------------------------------

- Only compiled the source code with MS 2005 on a Windows 7 system and 
GCC on a Ubuntu 11.10 system. All other makefiles have not been tested.
If you find you need to make any changes to get them to work, please
let me know.


3. How To Install Bin2froh?
---------------------------

a. Windows 9x/ME/NT/2k/XP/Vista/7

The Bin2froh package comes with sources as well as precompiled
executables (binaries) for the Win32 console mode. If you
want to install everything, just unpack the archive into
a directory of your choice.

Alternatively, if you only want to use the executables and
don't want to bother with the sources, just do the following
from a Win32 console (command prompt):

	copy bintoh.exe {destination directory}\bintoh.exe
	copy binfromh.exe {destination directory}\binfromh.exe 
	copy bin2froh.html {destination directory}\bin2froh.html

To read the manual, open the bin2froh.html file with a web browser.

If you work on the command line often, you might want to
consider putting the directory where you placed the bin2froh
executables in your PATH.

Bin2froh automatically detects the name under which it is run, and 
will change its behaviour accordingly. (That is, "bintoh" will convert 
input files to C header files, and "binfromh" will convert 
input files to binary files.)

b. MSDOS/FreeDOS and Clones

You will need to compile the sources. See the section
"Compiling Bin2froh In MSDOS and Win32", particularly
the section on using OpenWatcom C/C++.

c. Linux

See the section "Compiling Bin2froh In Linux and Unix-type Systems"

d. Other systems

See the section "Porting to Other Systems".


4. Compiling Bin2froh In Linux and Unix-type Systems
----------------------------------------------------

I've not distributed precompiled versions of Bin2froh for
Linux for two main reasons. There are just too many distributions
to compile for and it's a simple matter to do it yourself. The
process is painless and fast, since the source code compiles
out-of-the-box.

To compile everything, simply do the following:

	cd src
	make all

You can then install by typing

	make install

To install them somewhere other than /usr/bin, redefine
BINDIR and MANDIR for the binary and manual page destinations
respectively. For example, to install the binaries in /usr/local/bin
and the manual page in /usr/local/man/man1, do

	make BINDIR=/usr/local/bin MANDIR=/usr/local/man/man1 install

To read the manual page after installation, simply type

	man bintoh

There should be no errors or warnings in the compilation.


5. Compiling Bin2froh In MSDOS And Win32
------------------------------------------

Bin2froh comes with binaries (executables) for the Win32 console
mode. However, if you prefer to recompile Bin2froh yourself,
there are a few makefiles you may use, depending on the compiler
you have on your system.

To compile Bin2froh under MSDOS or Windows 9x/XP/Vista/7, simply use
the appropriate makefile for your compiler or roll your own.

Makefiles for use with the following compilers and their MAKE
utilities are as follows:

	makefile.vs8	Visual Studio 2005 and NMAKE.EXE (Win32 target)
	makefile.wcc	OpenWatcom C/C++ 1.9 and WMAKE.EXE (DOS 16/32 bit
			and Win32 targets)
	makefile.bc	Borland C/C++ 5.5 and MAKE.EXE (Win32 targets)
	makefile.dj2	DJGPP 2.0x and GNU make.exe (DOS 32 bit target)
	makefile.dm	Digital Mars C/C++ 8.X+ and MAKE.EXE
			(Win32 target). See notes below for limitations.
	makefile.lcc	LCC-Win32 3.X+ (Win32 target). See notes below
			for limitations.
	makefile.min	MinGW 3.1.0 and mingw32-make.exe (Win32 target)

Note that the default Makefile is for use with the GNU C compiler and
GNU make on a Linux system. Do not use it under Windows or MSDOS.

You can probably adapt the makefiles for other compilers or other
versions of the above compilers.

Note that you should be able to get free versions of most (if not
all) of the supported compilers from
https://sourceforge.net/projects/bin2froh

i. Recompiling with Visual C++

Visual C++ (from Visual Studio) can only create Win32 console
executables. Use the following procedure to create bintoh.exe
and binfromh.exe.

	cd src
	nmake -f makefile.vs8 all

There should be no warnings or errors.

ii. Recompiling with OpenWatcom C/C++

To recompile with OpenWatcom C/C++, copy the files
	src\startup\wildargv.c
	src\startup\initarg.h
from your WATCOM directory to the src\lib subdirectory. For example, if
you installed Watcom to c:\watcom, and you expanded the bin2froh
package into d:\bin2froh, do the following:
	copy c:\watcom\src\startup\wildargv.c d:\bin2froh\src\lib
	copy c:\watcom\src\startup\initarg.h d:\bin2froh\src\lib

If you cannot find wildargv.c on your hard disk, you probably did not
install the startup source code or have somehow deleted it. Just reinstall
OpenWatcom again, this time with the necessary options.

To create the MSDOS executables, do the following:
	cd src
	wmake -f makefile.wcc TARGET=16bit all

To create a Win32 console executable, the following command line can be
used:
	cd src
	wmake -f makefile.wcc TARGET=Win32 all

Make sure you do a
	wmake -f makefile.wcc clean
before compiling again for a different target or the response files
and object files will be wrong for the new target.

Note that the included precompiled Windows executable was compiled
with OpenWatcom 1.9.

Note that this version of bin2froh was not tested with OpenWatcom 
C/C++.

iii. Recompiling with Borland C/C++

To compile with the free command line version of Borland C++ 5.5,
installed with the directory structure used by default by the
Borland installer (ie into C:\Borland\BCC55), change into the
Bin2froh source directory, ie,

	cd src

Then type in the following command line. The command line is to be
typed in as one line even though it appears as two lines below (the
command is too long to fit into one line for the purpose of this
Readme file).

make -f makefile.bc all

This will make the Win32 console version of bintoh and binfromh.

Note that this version of bin2froh was not tested with Borland
C/C++.

iv. Recompiling with DJGPP 2.0x

To compile with DJGPP 2.0x, use makefile.dj2 provided in the package
and run it as follows:

	cd src
	make -f makefile.dj2 all

Note that this version of bin2froh was not tested with DJGPP 
2.0x

v. Recompiling with Digital Mars C/C++

To compile with the Digital Mars compiler, use the makefile.dm
provided in the package and run it with the Digital Mars MAKE.EXE
utility as follows:

	cd src
	make -fmakefile.dm all

There should be no warnings or errors. Note that if you compile
with this compiler, the resulting executable will not be able
to process wildcard arguments. If you want bin2froh to handle
arguments like *.bmp or *.h, use the precompiled executable that
came with the package.

Note that this version of bin2froh was not tested with Digital Mars
C/C++.

vi. Recompiling with LCC-Win32

To compile with LCC-Win32, use the makefile.lcc provided in the
package and run it with the LCC-Win32 MAKE.EXE utility as follows:

	cd src
	make -f makefile.lcc all

Note that if you compile with this compiler, the resulting executable 
will not be able to process wildcard arguments. If you want bin2froh 
to handle arguments like *.bmp or *.h, use the precompiled executable 
that came with the package.

Note that this version of bin2froh was not tested with LCC-Win32.

vii. Recompiling with MinGW

To compile with MinGW, use the GNU Make utility (mingw32-make.exe)
with makefile.min as follows:

	cd src
	mingw32-make -f makefile.min all

Note that this version of bin2froh was not tested with MinGW.
However, since MinGW uses GCC, and I test all versions of bin2froh 
using GCC on Linux, it should theoretically work fine.


6. Porting To Other Systems
---------------------------

If you want to compile Bin2froh for a system other than Linux, MSDOS
or Win32 you may or may not have some work in store for you. The
program is actually VERY trivial, so the work you need to do is
probably (hopefully) minimal.

The first place to look into is probably the config.h file, where I
tried to place as many system and compiler macros as I could bother.


7. History Of Changes
---------------------

Dates given are the dates where the code base was finalised and do not
necessarily refer to the date of public release.

Version 1.0.0	21 June 2012		Initial version.


8. Copyright And Contacting The Author
--------------------------------------

The program and its accompanying files and documentation are
Copyright 2012 Ming Chen. All rights reserved.
They are distributed under the terms of the GNU General Public License
Version 2, a copy of which is enclosed in this package in the file "copying".

You can retrieve the latest version of bin2froh from the following
website:
	https://sourceforge.net/projects/bin2froh

If you need to contact me, you can use the feedback form at the
following URL:
	https://sourceforge.net/projects/bin2froh

While I generally like to hear from you if you are using this program,
especially if you find any bugs in it, I make no promises about fixing
anything or even replying. If you do contact me, please also let me have
the following information:

	1. The version of bin2froh that you are using;
	2. The operating system (and its version) on which you are
	running the program (eg, Debian Linux [Woody], or MSDOS 6.22,
	Windows 95/98/ME/NT/2k/XP/Vista/7, Mac OS/X with version number,
	or all of the above).
	If the bug only surfaces in one of the operating systems and not
	the other, please also note this.
	3. Any other information which you think might be relevant.

This will help me give you a more relevant reply (and also to trace
whatever bugs that may be present).
