/*
   config.h Handles system dependencies.
   Copyright (c) 1996-2008 by Christopher Heng. All rights reserved.
*/

/*
   You need an ANSI C compiler. I assume this everywhere. If you
   have a pre-ANSI C compiler, it's likely that you have to make
   a lot of changes to the sources that you might as well just
   rewrite the program. It *is* afterall a trivial program.

   I have not specifically designed this program so that it is
   portable across systems. The comments below might help if you
   are using anything other than the compilers I used to develop
   the program. Note that the comments and macros in this file
   about system dependencies are not necessarily exhaustive.

   1. These macros are defined for the following systems:
   System         Macros defined
   ------         --------------
   LINUX          LINUX, UNIX
   MSDOS          MSDOS
   WIN32          WIN32

   2. You need a getopt() implementation. It must support the
   usual behaviour of the Unix getopt(), plus the variables
   optind, opterr, and optarg.

   If your system has the header <getopt.h>, define HAVE_GETOPT_H.
   I have defined this for the systems I compile for.

   I have supplied my own version of getopt.c and getopt.h in
   the lib subdirectory since most MSDOS and Win32 compilers do
   not have getopt() in their libraries.

   3. If your system has <unistd.h>, define HAVE_UNISTD_H. This is
   usually relevant only for Unix systems, although the DJGPP GNU C
   compiler has that as well. If you don't have unistd.h, you may
   have to declare some standard Unix functions that are usually
   found there, such as chown(), chmod(), etc.

   4. Note that on MSDOS systems, you will need _splitpath()
   and _makepath(). If you use DJGPP, you probably can get away
   with defining _splitpath() to call fnsplit(), etc (which I have
   done below). Otherwise, you will need to roll your own version.
   I think all the commercial MSDOS C compilers have these functions.

   5. You will also need stricmp() and strnicmp() on MSDOS or
   strcasecmp() or strncasecmp() on Unix. If you have stricmp() and/or
   strnicmp() on a Unix system, define HAVE_STRICMP and/or
   HAVE_STRNICMP respectively. I assume stricmp() for all non-Unix
   systems so if you are neither compiling for Unix or MSDOS, you
   better check out my macros below. If you have a Unix system,
   defining UNIX here will cause the compiler to use strcasecmp()
   and strncasecmp().

   6. You will need a mktemp(). On Unix systems, this is probably
   declared in <unistd.h>.

   Digital Mars declares mktemp() in io.h.

   Borland declares mktemp() in dir.h. If you are writing or using
   a replacement mktemp() you should put the prototype in a
   header called mktemp.h and define HAVE_MKTEMP_H.

   Visual C++ declares mktemp() in io.h.

   I have supplied my own mktemp.c and mktemp.h for use with
   Watcom C/C++ 10.X since these versions of the compiler do not
   have mktemp().

   If your compiler has mktemp() declared somewhere else (other
   than unistd.h on Unix), define MKTEMP_HEADER to be the name
   of the header, eg <whatever.h> (include the angle brackets or
   double quotes), and HAVE_MKTEMP_H to force inclusion of the
   header in the relevant files.

   7. bin2froh.c assumes utime.h exists in <sys/utime.h> for
   Microsoft's compiler and Watcom C/C++ (which tries to emulate
   Microsoft's compiler closely). It assumes that all other compilers
   keep utime.h in the standard include directories which are accessible
   simply by including <utime.h>. I must confess I have not bothered to
   keep this system dependent setting in this file, only noting it here
   for completeness. If you find that you have to tweak this for your
   system, please let me know.
*/

#if !defined(CONFIG_H_INCLUDED)
#define	CONFIG_H_INCLUDED

#if defined(__cplusplus)
extern "C" {
#endif

/* define the systems */
#if defined(__linux__) /* (predefined) */
#if !defined(LINUX)
#define LINUX
#endif
#if !defined(UNIX)
/* make sure this is defined */
#define UNIX
#endif
#endif

#if defined(__FreeBSD__) || defined(__OpenBSD__) /* seems to work like Linux... */
#if !defined(LINUX)
#define LINUX
#endif
#if !defined(UNIX)
/* make sure this is defined */
#define UNIX
#endif
#endif

#if defined(__MSDOS__)
#if !defined(MSDOS)
/* make sure this is defined */
#define MSDOS
#endif
#endif

#if defined(__WIN32__) || defined(__NT__) || defined(_WIN32)
#if !defined(WIN32)
#define WIN32
#endif
#if defined(__BORLANDC__) && defined(MSDOS) /* Borland always defines MSDOS */
#undef MSDOS
#endif
#endif

#if defined(__APPLE__)
#if !defined(UNIX)
#define UNIX
#endif
#define HAVE_UNISTD_H
#define MAXPATHLEN_HEADER <sys/param.h>
#endif

/* define what headers we have (based on the systems) */
#if defined(LINUX)
#define HAVE_GETOPT_H
#define HAVE_UNISTD_H
#endif

#if defined(__WATCOMC__) /* this works on my system only */
#if !defined(HAVE_GETOPT_H)
#define HAVE_GETOPT_H
#endif
#if !defined(HAVE_MKTEMP_H)
#define HAVE_MKTEMP_H
#endif
#if !defined(MKTEMP_HEADER)
#define MKTEMP_HEADER <mktemp.h>
#endif
#endif

#if defined(__BORLANDC__) /* Borland declares mktemp() in dir.h */
#if !defined(HAVE_GETOPT_H)
#define HAVE_GETOPT_H
#endif
#if !defined(HAVE_MKTEMP_H)
#define HAVE_MKTEMP_H
#endif
#if !defined(MKTEMP_HEADER)
#define MKTEMP_HEADER <dir.h>
#endif
#endif

#if defined(__SC__) /* Digital Mars declares mktemp() in <io.h> */
#if !defined(HAVE_GETOPT_H)
#define HAVE_GETOPT_H
#endif
#endif

#if defined(__LCC__) /* LCC-Win32 compiler declares mktemp() in io.h */
#if !defined(HAVE_GETOPT_H)
#define HAVE_GETOPT_H
#endif
#endif


#if defined(_MSC_VER) /* Visual C++ declares mktemp() in <io.h> */
               /* which is already included for other reasons. */
#if !defined(HAVE_GETOPT_H)
#define HAVE_GETOPT_H
#endif
#endif

/* djgpp has unistd.h which also defines getopt() */
#if defined(__DJGPP__) || defined(__GO32__)
#define HAVE_UNISTD_H
#endif

#if defined(__MINGW32__)
#define HAVE_GETOPT_H
#endif

/* if we are in Unix define stricmp to be strcasecmp and strnicmp to */
/* be strncasecmp. I'm not sure if all Unices have these, but Linux */
/* does. */
#if defined(UNIX)
#if !defined(HAVE_STRICMP)
#define stricmp strcasecmp
#endif
#if !defined(HAVE_STRNICMP)
#define strnicmp strncasecmp
#endif
#endif

/* if you are using a Unix-type system that has the function mkstemp() */
/* define HAVE_MKSTEMP here. If not, just leave it undefined; */
/* mktemp() will be used instead. */
#if defined(LINUX)
#define HAVE_MKSTEMP
#endif

/* Borland, Digital Mars, LCC-Win32, Microsoft's compiler have */
/* S_IREAD and S_IWRITE in their sys/stat.h instead of S_IRUSR */
/* and S_IWUSR which are used by the Unix and Watcom compilers. */
/* Borland's 4.52 compiler however defines these in its 32 bit */
/* compiler. Since I can't be sure which versions have already */
/* defined these, I define it for all Borland 32 bit targets. */
#if defined(_MSC_VER) || defined(__SC__) || defined(__LCC__) || (defined(__BORLANDC__) && !defined(__FLAT__))
#define S_IRUSR S_IREAD
#define S_IWUSR S_IWRITE
#endif

/* This block is for Unix-type systems only. It is not relevant to */
/* MSDOS and Windows systems. */
/* Basically we need to have MAXPATHLEN defined somewhere. This is */
/* the maximum size that a pathname can be, and it is used to allocate */
/* storage for a pathname we construct in the program. In BSD systems */
/* and Linux systems, MAXPATHLEN is defined in <sys/param.h>. If your */
/* system has MAXPATHLEN defined somewhere, put the header name in */
/* MAXPATHLEN_HEADER (eg, you can try checking your <limits.h> */
/* and <sys/param.h> to see if MAXPATHLEN is there). If not, you will just */
/* have to create some sort of value for MAXPATHLEN by defining it below. */
/* Note: I realize that there are ways around */
/* this need for a constant value, but I can't be bothered to implement it. */
/* You might also want to try using pathconf() in a test program to find out */
/* if your system returns any sensible value for _PC_PATH_MAX. If all else */
/* fails, you might want to consider the Linux value of 4096 for MAXPATHLEN. */
/* That is, add a line like */
/* #define MAXPATHLEN 4096 */
/* below. */
#if defined(LINUX)
#define MAXPATHLEN_HEADER <sys/param.h>
#endif
/* End of block that is relevant for Unix-type systems only */


/* djgpp does not have _splitpath() and _makepath(), but they do have */
/* functional equivalents which are declared in <dir.h> */
#if defined(__DJGPP__) || defined(__GO32__)
#define _splitpath fnsplit
#define _makepath  fnmerge
#endif

/* Later versions of LCC-Win32 no longer declares utime() */
#if defined(__LCC__)
#define utime(a,b) _utime(a,b)
#endif

#if defined(__WATCOMC__)	/* sighandler() never returns */
#pragma aux SigHandler aborts
#endif

#if defined(__cplusplus)
}
#endif

#endif
