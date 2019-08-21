# makefile.gcc	Makefile for building with the GNU C compiler on Linux.
# Copyright (c) 1996-2005 by Christopher Heng. All rights reserved.

# $Id: makefile.gcc,v 1.2 2005/03/06 05:40:49 chris Exp $ /*

# This makefile requires the GNU make to work (we use the GNU extensions).

# Define the following if you need
#
#	DEBUG	to add debugging information, eg "make DEBUG=1 all"


ifdef DEBUG
CDEBUG = -g
LDEBUG = -g
else
CDEBUG = -O2
LDEBUG = -s
endif

ifndef VERSTR
VERSTR = 0.0.0
endif

# programs
CC = gcc
CD = cd
CP = cp
GZIP = gzip
INSTALL = install
LN = ln
LD = gcc
MAKE = make
MKDIR = mkdir
MV = mv
RM = rm -f
TAR = tar
ZIP = zip

# flags
CFLAGS = $(DEFINES) $(TFLAG) $(CDEBUG) -c -Wall
GZIPFLAGS = -9
INSTALLBINFLAGS = -m 755
INSTALLDATAFLAGS = -m 644
LDFLAGS = $(LDEBUG)
LNFLAGS = -sf
RMRECURFLAGS = -rf
TARFLAGS = cvf
ZIPSRCFLAGS = -r -9

# directories
BINDIR = /usr/bin
DISTDIR = bin2froh-$(VERSTR)
LIBDIR = lib
LIBDISTDIR = $(DISTDIR)/lib
MANDIR = /usr/man/man1

# filenames
BINTOH = bintoh
BINTOHMAN = bintoh.1                  
BINTOHTXT = bintoh.txt
BINFROMH = binfromh
BINFROMHMAN = binfromh.1                  
BINFROMHTXT = binfromh.txt

TARNAME = bin2froh-$(VERSTR).tar
TARGZNAME = $(TARNAME).gz
ZIPSRCNAME = bin2froh.zip

# lists of filenames
DISTFILES = config.h \
	../COPYING \
	bintoh \
	makefile.bc \
	makefile.dj2 \
	makefile.dm \
	makefile.gcc \
	makefile.lcc \
	makefile.min \
	makefile.vs8 \
	makefile.wcc \
	../readme.txt \
	bin2froh.c \
	../bin2froh.lsm \
	version.h \
	$(BINTOHMAN)
LIBDISTFILES = $(LIBDIR)/getopt.c \
	$(LIBDIR)/getopt.h

# Linux, BSD, and other Unix-type systems already have a fully-functioning 
# and thoroughly-debugged version of getopt()
OBJS = bin2froh.o

# implicit rules
.c.o:
	$(CC) $(CFLAGS) $<

# user visible rules
all: $(BINTOH) $(BINFROMH)

clean:
	$(RM) $(OBJS)

clobber: clean
	$(RM) $(BINTOH) $(BINFROMH)

dist: $(DISTFILES) $(DISTDIR) $(LIBDISTDIR)
	$(CP) $(DISTFILES) $(DISTDIR)
	$(CP) $(LIBDISTFILES) $(LIBDISTDIR)
	$(MV) $(DISTDIR)/makefile.gcc $(DISTDIR)/Makefile
	$(MV) $(DISTDIR)/readme.txt $(DISTDIR)/README.TXT
	($(CD) $(DISTDIR) ; $(LN) $(LNFLAGS) $(BINTOH) $(BINFROMH))

distclean: clobber
	$(RM) $(RMRECURFLAGS) $(DISTDIR)

install: installman
	$(INSTALL) $(INSTALLBINFLAGS) $(BINTOH) $(BINDIR)
	($(CD) $(BINDIR) ; $(LN) $(LNFLAGS) $(BINTOH) $(BINFROMH))

installman: $(BINTOHMAN)
	$(INSTALL) $(INSTALLDATAFLAGS) $(BINTOHMAN) $(MANDIR)
	($(CD) $(MANDIR) ; $(LN) $(LNFLAGS) $(BINTOHMAN)  $(BINFROMHMAN))

save:
	$(ZIP) $(ZIPSRCFLAGS) $(ZIPSRCNAME) *

tardist: $(TARGZNAME)


# real rules
$(DISTDIR):
	$(MKDIR) $(DISTDIR)

$(BINTOH): $(OBJS)
	$(CC) $(LDFLAGS) -o $(BINTOH) $(OBJS)

$(BINTOHTXT): $(BINTOHMAN)
	gtbl $(BINTOHMAN) | groff -Tascii -mandoc | \
		troff -Tascii -mandoc | grotty > $@

$(BINFROMHTXT): $(BINFROMHMAN)
	gtbl $(BINFROMHMAN) | groff -Tascii -mandoc | \
		troff -Tascii -mandoc | grotty > $@

$(LIBDISTDIR):
	$(MKDIR) $(LIBDISTDIR)

$(TARGZNAME): dist
	$(TAR) $(TARFLAGS) $(TARNAME) $(DISTDIR)
	$(GZIP) $(GZIPFLAGS) $(TARNAME)

$(BINFROMH): $(BINTOH)
	$(LN) $(LNFLAGS) $(BINTOH) $(BINFROMH)

# objects
bin2froh.o: bin2froh.c config.h version.h


# The following information is for the information of the maintainer of
# the package only.

# To make the distribution tar file, you need to define VERSTR to the
# version number. Eg, for a version 1.0.0 release, define VERSTR=1.0.0
# Hence a make command line would read
#	make VERSTR=1.0.0 tardist
# If VERSTR is not defined at any time, it defaults to 0.0.0 (to remind
# you that you forgot something!).
#                                                         
# Before making tardist, you need to compile first.

