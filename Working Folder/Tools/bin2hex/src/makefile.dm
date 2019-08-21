# makefile.dm	Makefile for building with Digital Mars.
# Copyright (c) 2002-2005 by Christopher Heng. All rights reserved.

# $Id: makefile.dm,v 1.3 2005/03/14 14:18:40 chris Exp $

# WARNING: This makefile produces a version of BINTOH.EXE and BINFROMH.EXE
# that does *not* handle wildcards on the command line. If you want the
# executable to handle wildcards, used the supplied executables. I haven't
# figured out how to get wildcard preprocessing of command line arguments
# with Digital Mars yet. Is there a obj file somewhere that I can link into
# the application without having to write my own command line globbing routine?

# This makefile assumes the Digital Mars default MAKE.EXE
# It creates a Win32 console application.
# Unlike the other makefiles, this is a barebones makefile -
# I created it mainly to check that the source code compiles
# cleanly with this compiler.

# programs
CC = dmc
CP = copy
LD = dmc
RM = del

# flags
CFLAGS = -o -c -Ilib

# directories
LIBDIR = lib

# filenames
BINTOH = bintoh.exe
BINFROMH = binfromh.exe
OBJS = bin2froh.obj lib\getopt.obj

# implicit rules
.c.obj:
	$(CC) $(CFLAGS) $<


# targets

all: $(BINTOH) $(BINFROMH)

clean:
	$(RM) $(OBJS)

clobber: clean
	$(RM) $(BINTOH) $(BINFROMH)

$(BINTOH): $(OBJS)
	$(LD) -o$(BINTOH) $(OBJS)

$(BINFROMH): $(BINTOH)
	$(CP) $(BINTOH) $(BINFROMH)


# objects

bin2froh.obj: bin2froh.c config.h version.h

lib\getopt.obj: lib\getopt.c lib\getopt.h
	$(CC) $(CFLAGS) -olib\getopt.obj lib\getopt.c
