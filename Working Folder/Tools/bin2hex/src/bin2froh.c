/*
   Bin2froh.c   Converts binary files to/from C header files.
   Copyright 2012 Ming Chen. All rights reserved.
*/

/* this should always be first */
#include "config.h"

/* standard headers */
#include <signal.h> /* signal() */
#include <stdarg.h> /* va_arg and family */
#include <stdio.h> /* FILE functions */
#include <stdlib.h>
#include <string.h> /* strrchr(), strlen(), strcpy(), strcat() */
#include <sys/stat.h> /* stat() */

#if defined(UNIX)
#if defined(MAXPATHLEN_HEADER)
#include MAXPATHLEN_HEADER
#endif
#endif

#if defined(_MSC_VER) || defined(__WATCOMC__)
/* everybody else keeps this in the include directory */
#include <sys/utime.h>
#else
#if defined(UNIX)
#include <sys/types.h>
#endif
#include <utime.h>
#endif

/* conditionally included headers */
#if defined(MSDOS) || defined(WIN32)
#include <fcntl.h> /* O_BINARY */
#include <io.h> /* chmod(), setmode(), isatty(), mktemp() (VC++) */
#endif

#if defined(HAVE_GETOPT_H)
#include <getopt.h> /* optind, getopt() */
#endif

#if defined(HAVE_MKTEMP_H) && defined(MKTEMP_HEADER)
#include MKTEMP_HEADER
#endif

#if defined(HAVE_UNISTD_H)
#include <unistd.h> /* chmod(), mktemp(), isatty(), chown(), readlink(), */
               /* mkstemp() in some systems, (getopt()) */
#endif

#if defined(__DJGPP__) || defined(__GO32__)
#include <dir.h>
#endif

/* our headers */
#include "version.h"

/* macros */
#define EXEC_NAME                   "bin2froh"

#define HELP_FMT                    "Usage: %s [options] [file...]\n"\
                                    "-a\tAbort processing files on error in any file.\n"\
                                    "-c num\t<num> of columns of the C-style char array.\n"\
                                    "-e name\tExtension <name> of the output file(s).\n"\
                                    "-f\tConvert binary file(s) FROM C header file(s).\n"\
                                    "-h\tDisplay help on usage and quit.\n"\
                                    "-l file\tLog most errors and verbose messages to <file>.\n"\
                                    "-o\tOverwrite output file(s) if already exist (no backup).\n"\
                                    "-s name\tSymbol <name> of the C-style char array.\n"\
                                    "-t\tConvert binary file(s) TO C header file(s).\n"\
                                    "-v\tVerbose.\n"\
                                    "-V\tShow version and quit.\n"

#define OPT_LIST                    "ac:e:fhl:os:tvV"

#define VER_FMT                     "%s Ver %d.%d.%d "\
                                    "Convert binary file(s) to/from C header file(s).\n"\
                                    "Copyright 2012 Ming Chen. All rights reserved.\n"

/*error messages */
#define ERROR_CONVERT               "File read/write error while converting %s.\n"
#define ERROR_NO_FROM_FILE          "Need to specify filename or redirect stdin.\n"
#define ERROR_NO_MEM                "Insufficient memory to run program.\n"
#define ERROR_OPEN_FILE             "Unable to open file %s.\n"
#define ERROR_CREATE_FILE           "Unable to create output file %s for converting %s.\n"
#define ERROR_SIGNAL                "Terminated by user.\n"
#define ERROR_WRONG_DOS_VER         "Requires DOS 3.1 and above.\n"
#define ERROR_ACCESS_FILE           "Unable to access file %s.\n"
#define ERROR_SYMBOLIC_LINK         "Unable to dereference symbolic link %s.\n"
#define ERROR_CREATE_LOG            "Unable to create error log file %s. Defaulting to stderr.\n"
#define ERROR_DIRECTION             "Unknown convert direction: %d.\n"
#define ERROR_UNKNOWN_OPTION        "Terminated by unknown option.\n"

/* warning messages */
#define WARNING_BAD_COL_NUM         "%d is not in the range [4, 32]. Defaulting to %d.\n"

/* verbose messages */
#define VERBOSE_CONVERTING          "Converting %s...\n"
#define VERBOSE_SYMBOLIC_LINK_SRC   "%s resolves to %s.\n"


/* directions of convertion */
/* convert binary file(s) TO C header file(s) */
#define BIN_TO_H                    0
/* convert binary file(s) FROM C header file(s) */
#define BIN_FROM_H                  1

/* name of the executable */
#define BIN_TO_H_NAME               "bintoh"
#define BIN_TO_H_NAME_ALIAS         "bin2h"
#define BIN_FROM_H_NAME             "binfromh"

/* extention of output file */
#define DEFAULT_H_EXTENTION         "h"
#define DEFAULT_BIN_EXTENTION       "bin"

/* symbol name of the C-style array */
#define DEFAULT_SYMBOL_NAME         "Bin_Raw"

/* number of columns of the C-style array */
#define DEFAULT_COL_NUM             16

/* backup file extension */
#define BAK_EXT_STR                 ".bak"
/* works for both MSDOS and Unix */
#define DIR_SLASH                   '/'
#define DIR_SLASH_STR               "/"
/* period for extension */
#define EXT_PERIOD                  '.'
#define EXT_PERIOD_STR              "."

/* buffer size for the files */
#define NEW_BUFSIZ                  16384

/* conditional macros */
#if defined(MSDOS) || defined(WIN32)
#if !defined(_MAX_DIR) || (_MAX_DIR < 260) /* MAXDIRSIZE */
#define MAXDIRSIZE                  260
#else
#define MAXDIRSIZE                  _MAX_DIR
#endif
#if !defined(_MAX_NAME) || (_MAX_NAME < 260) /* MAXFILESIZE */
#define MAXFILESIZE                 260
#else
#define MAXFILESIZE                 MAX_NAME
#endif
#if !defined(_MAX_PATH) || (_MAX_PATH < 260) /* MAXPATHSIZE */
#define MAXPATHSIZE                 260
#else
#define MAXPATHSIZE                 MAX_PATH
#endif
#if !defined(_MAX_DRIVE) /* for the benefit of djgpp */
#define _MAX_DRIVE                  3
#endif
#endif

/* In many environments, such as most UNIX-based systems, it makes no */
/* difference to open a file as a text file or a binary file; Both are */
/* treated exactly the same way, but differentiation is recommended for */
/* a better portability. */
#define READ_AS_TEXT                "r"
#define READ_AS_BINARY              "rb"
#define WRITE_AS_TEXT               "w"
#define WRITE_AS_BINARY             "wb"
#define APPEND_AS_TEXT              "a"
#define APPEND_AS_TEXT_EXPLICIT     "at"
#define APPEND_AS_BINARY            "ab"

#if !defined(MSDOS)
#define CURRENT_DIR                 "./"
#endif

#define BIN_BYTE_NOT_START          0
#define BIN_BYTE_HIGH_FOUR          1
#define BIN_BYTE_LOW_FOUR           2

/* global variables */


/* local variables */
static int  AbortOnErr = 0;
static int  ConvertDirection = BIN_TO_H;
static char *ExtOfToFile = DEFAULT_H_EXTENTION;
static char *LogFileName = NULL;
static int  ColumnNumber = DEFAULT_COL_NUM;
static int  OverWrite = 0;
static char *SymbolName = DEFAULT_SYMBOL_NAME;
static int  Verbose = 0;

static char *FromFileName = "stdin";
static char *ToFileName;
static FILE *ToFileP;
static FILE *LogFileP;

/* local functions */
static int ParseArgs (int argc, char ** argv);
static void WriteLog (char *Format, ...);
static void CloseLogFile(void);
static void SigHandler (int Sig);
static int OpenAndConvert (char *InputFileName);
static int Convert (FILE *FromFP, FILE * ToFP);
static int GetHalfByte (int *Ch);

/*
   main

   bin2froh converts binary files to/from C header files.

   Usage: bin2froh [options] [file...]

   Exit codes: 0 on success, -1 on error. Error messages will be
        displayed on error before returning.
*/
int main ( int argc, char ** argv )
{
   /* Parse the options */
   if (ParseArgs(argc, argv))
   {
      return -1;
   }

   /* check if we are to convert from stdin */
   if (argc == optind)
   {
      if (isatty(fileno(stdin))) 
      {
         /* stdin must be redirected else you should supply a filename. */
         WriteLog(ERROR_NO_FROM_FILE);
         return -1 ;
      }

      /* otherwise stdin has been redirected */
      return OpenAndConvert(NULL);
   }

   /* if we reach here, we have a (list?) of files to convert */
   while (optind < argc)
   {
      if (OpenAndConvert(argv[optind]) != 0 && AbortOnErr)
      {
         return -1;
      }
      optind++;
   }

   return 0;
}

/*
   ParseArgs

   Checks for correct operating system version (DOS only).
   Gets the name of the executable
   Sets the default direction of conversion.
   Sets the signal traps.
   Parses the options.

   Returns 0 on success, -1 on error.
*/
static int ParseArgs (int argc, char ** argv)
{
   int ReturnValue = 0;
   char *FirstArg = argv[0];
   char *ExecName = NULL;

#if defined(MSDOS) || defined(WIN32)
   char FName[MAXFILESIZE];
#else
   char *LastSlash = NULL;
#endif

   int Ch = EOF;
   char *ExtDesignated = NULL;
   int ColumnDesignated = DEFAULT_COL_NUM;
   int ExitNow = 0;

   /* check that we have the minimum version of DOS needed. */
#if defined(MSDOS) && !defined(__DJGPP__) && !defined(__GO32__)
   /* we only run on DOS 3.1 and above. */
   if (_osmajor < 3 || (_osmajor == 3 && _osminor < 10))
   {
      WriteLog(ERROR_WRONG_DOS_VER);
      return -1 ;
   }
#endif

   /* get the name of the executable to determine the default direction */
   /* of convertion */
#if defined(MSDOS) || defined(WIN32)
   /* got to extract the name from the full path and extension */
   _splitpath(FirstArg, NULL, NULL, FName, NULL);
   ExecName = strdup(FName);
   if (ExecName == NULL)
   {
      WriteLog(ERROR_NO_MEM);
      return -1 ;
   }
#else
   /* neither MSDOS nor WIN32 - assume Unix */
   /* got to wipe out the path prefix if any */
   LastSlash = strrchr(FirstArg, DIR_SLASH);
   if (LastSlash != NULL)
   {
      /* we got the last slash - get rid of the path part */
      ExecName = LastSlash + 1;
   }
   else 
   {
      /* current dir, no path part */
      ExecName = FirstArg;
   }
#endif

   /* set the default convert direction: Unless we are explicitly named */
   /* to convert in a particular convert direction, the default convert */
   /* direction depends on the used executable name. */

   /* default to BIN_TO_H */
   ConvertDirection = BIN_TO_H;
   ExtOfToFile = DEFAULT_H_EXTENTION;
   if (!stricmp(ExecName, BIN_FROM_H_NAME))
   {
      ConvertDirection = BIN_FROM_H;
      ExtOfToFile = DEFAULT_BIN_EXTENTION;
   }

   /* set the signal traps - we use the old Unix version 7 signal */
   /* mechanism since that is most portable to DOS. In any case, */
   /* we don't do anything sophisticated when we receive a signal */
   /* except cleaning up and quitting! */
   if (signal(SIGINT, SigHandler) == SIG_IGN)
   {
      signal(SIGINT, SIG_IGN);
   }

   if (signal(SIGTERM, SigHandler) == SIG_IGN)
   {
      signal(SIGTERM, SIG_IGN);
   }

   while ((Ch = getopt(argc, argv, OPT_LIST)) != EOF)
   {
      switch (Ch)
      {
         case 'a':
            /* abort processing list of files if we encounter errors */
            /* in any file in a list of file names */
            AbortOnErr = 1;
            break;
         case 'c':
            /* number of columns of the C-style char array */
            ColumnDesignated = atoi(optarg);
         
            if ((ColumnDesignated >= 4) && (ColumnDesignated <= 32))
            {
               ColumnNumber = ColumnDesignated;
            }
            else
            {
               WriteLog(WARNING_BAD_COL_NUM, ColumnDesignated, DEFAULT_COL_NUM);
            }
            break;
         case 'e':
            /* extension name for the output file(s) */
            ExtDesignated = optarg;
            break;
         case 'f':
            /* convert binary file(s) FROM c header file(s) */
            ConvertDirection = BIN_FROM_H;
            ExtOfToFile = DEFAULT_BIN_EXTENTION;
            break;
         case 'h':
            /* show version and usage, then quit */
            fprintf(stderr, VER_FMT, ExecName, VER_MAJOR, VER_MINOR, VER_PATCH);
            fprintf(stderr, HELP_FMT, ExecName);
            ExitNow = 1;
            goto Func_Exit;
            break;
         case 'l':
            /* log file name for debug messages */
            LogFileName = optarg;
            break;
         case 'o':
            /* overwrite if output file exist (default to 0 : create backup) */
            OverWrite = 1;
            break;
         case 's':
            /* symbol name of the c-style char array */
            SymbolName = optarg;
            break;
         case 't':
            /* convert binary file(s) to C header file(s) */
            ConvertDirection = BIN_TO_H;
            ExtOfToFile = DEFAULT_H_EXTENTION;
            break;
         case 'v':
            /* verbose */
            Verbose = 1;
            break;
         case 'V':
            /* show version and quit */
            fprintf(stderr, VER_FMT, ExecName, VER_MAJOR, VER_MINOR, VER_PATCH);
            ExitNow = 1;
            goto Func_Exit;
            break;
         default:
            /* error, Ch might be modified to 0 by the getopt() function */
            WriteLog(ERROR_UNKNOWN_OPTION);
            ReturnValue = -1;
            goto Func_Exit;
            break;
      }
   }
 
   if (ExtDesignated != NULL)
   {
      ExtOfToFile = ExtDesignated;
   }

Func_Exit:

#if defined(MSDOS) || defined(WIN32)
   /* free the storage allocated from the strdup() call */
   free(ExecName);
#endif

   if (ExitNow)
   {
      exit(ReturnValue);
   }

   return ReturnValue;
}

/*
   WriteLog

   Output debug info to a log file or stderr
*/
static void WriteLog (char *Format, ...)
{
   va_list argp ;

   if (LogFileP == NULL)
   {
      if (LogFileName == NULL)
      {
         LogFileP = stderr;
      }
      else
      {
#if defined(MSDOS) || defined(WIN32)
         /* use explicit text mode "at" for MSDOS and WIN32 */
         LogFileP = fopen(LogFileName, APPEND_AS_TEXT_EXPLICIT);
#else
         LogFileP = fopen(LogFileName, APPEND_AS_TEXT);
#endif
         if (LogFileP == NULL)
         {
            fprintf(stderr, ERROR_CREATE_LOG, LogFileName);
            LogFileP = stderr ;
         }
         else
         {
            fprintf(LogFileP, VER_FMT, EXEC_NAME, VER_MAJOR, VER_MINOR, VER_PATCH);
            /* close error file on exit (not needed, but just being */
            /* pedantically neat) */
            atexit(CloseLogFile); /* ignore errors */
         }
      }
   }

   va_start(argp, Format);
   vfprintf(LogFileP, Format, argp);
   va_end(argp);
   return;
}

/*
   CloseLogFile

   Clost log file when exiting

   Note : there's no need to check for stderr, since we should not have 
             set this function on exit() if only stderr were used.
*/
static void CloseLogFile(void)
{
   if (LogFileP != NULL)
   {
      fclose(LogFileP);
   }
   return;
}

/*
   SigHandler

   Handles SIGINT and SIGTERM. Prints a message, release resources
   and quits with -1.

   It never returns (and Watcom C knows it).
*/
static void SigHandler (int Sig)
{
   /* restore signal handler, in case we have the old unsafe behaviour */
   signal(Sig, SigHandler);

   /* print error message for this only if Verbose */
   if (Verbose)
   {
      WriteLog(ERROR_SIGNAL);
   }

   if (ToFileName != NULL)
   {
      /* close the output file and delete it */
      if (ToFileP != NULL)
      {
         fclose(ToFileP);
         ToFileP = NULL;
      }
      remove(ToFileName);
      free(ToFileName);
      ToFileName = NULL ;
   }

   exit(-1);
}

/*
   OpenAndConvert

   Called to open the files and convert the contents. If you want
   it to convert stdin to stdout, call it with NULL as the filename
   argument; otherwise pass the function the name of the input file.

   Returns: 0 on success, -1 on error. Error messages will be
        displayed on error before returning.
*/
static int OpenAndConvert (char * InputFileName)
{
   int ReturnValue = 0;

#if defined(UNIX)
   struct stat StatBuf;
   char RealFromFileName[MAXPATHLEN + 1] ;
   size_t RealFromFileNameLen = 0;
#endif

   FILE *FromFileP;

#if defined(MSDOS)
   char Drv[_MAX_DRIVE];
   char Dir[MAXDIRSIZE];
   char FName[MAXFILESIZE];
   char ToFilePath[MAXPATHSIZE];
#else
   char *LastSlash = NULL;
   size_t PathOnlyLen = 0;
   char *FileNameOnly = NULL;
   char *LastPeriod = NULL;
   size_t FNameOnlyLen = 0;
#endif

   char *BackFileName = NULL;

#if NEW_BUFSIZ > BUFSIZ
   char *InBufPtr = NULL;
   char *OutBufPtr = NULL;
#endif

   /* stdin is not redirected */
   if (InputFileName != NULL)
   {
      if (Verbose)
      {
         WriteLog(VERBOSE_CONVERTING, InputFileName);
      }

      /* associate FromFileName with infilename for error messages */
      FromFileName = InputFileName;

#if defined(UNIX) 
      /* On a Unix-type system (including Linux and BSD systems), it is */
      /* possible for a given filename to be merely a symbolic link. Below */
      /* code obtains the real filename given the symlink. */

      /* get the input file information */
      if (lstat(InputFileName, &StatBuf))
      {
         /* couldn't stat the file. */
         WriteLog(ERROR_ACCESS_FILE, InputFileName);
         return -1;
      }

      if (S_ISLNK(StatBuf.st_mode))
      {
         /* get the real filename for symbolic links */
         /* Note: the S_ISLNK() macro is supposed to exist in sys/stat.h */
         /* Early Unices may not have this macro. If it does not exist, you */
         /* may have to define it yourself, a la S_ISDIR() and family. */
         /* eg, #define S_ISLNK(x) (((x) & S_IFMT) == S_IFLNK) */
         /* or something like that. */

         RealFromFileNameLen = readlink(InputFileName, 
                                        RealFromFileName, 
                                        (sizeof(RealFromFileName) - 1));
         if (RealFromFileNameLen == -1)
         {
            WriteLog(ERROR_SYMBOLIC_LINK, InputFileName);
            return -1;
         }

         /* got to null terminate the string - there is always space because */
         /* we passed readlink() the size of the buffer less 1. */
         RealFromFileName[RealFromFileNameLen] = '\0' ;
         if (Verbose)
         {
            WriteLog(VERBOSE_SYMBOLIC_LINK_SRC, InputFileName, RealFromFileName);
         }

         /* modify FromFileName to real file name for error messages*/
         FromFileName = RealFromFileName;
      }
#endif

      /* open the fomefilename */
#if defined(MSDOS) || defined(WIN32)
      /* always use "rb" for MSDOS and WIN32 */
      FromFileP = fopen(FromFileName, READ_AS_BINARY);
#else
      if (ConvertDirection == BIN_TO_H)
      {
         FromFileP = fopen(FromFileName, READ_AS_BINARY);
      }
      else
      {
         FromFileP = fopen(FromFileName, READ_AS_TEXT);
      }
#endif
      if (FromFileP == NULL)
      {
         WriteLog(ERROR_OPEN_FILE, FromFileName);
         return -1;
      }

      /* create an output file in the same directory as the FromFileName. */
      /* if that file already exists, rename to backup if OverWrite is 0 */
#if defined(MSDOS)
      /* this is easy to do for DOS since we have _splitpath(), _makepath() */
      /* functions. */
      _splitpath(FromFileName, Drv, Dir, FName, NULL);
      _makepath(ToFilePath, Drv, Dir, FName, ExtOfToFile);
      ToFileName = strdup(ToFilePath);
      if (ToFileName == NULL)
      {
         WriteLog(ERROR_NO_MEM);
         return -1;
      }
#else 
      /* not MSDOS - ie, Linux, Windows, anything else */
      /* check if there is a path prefix */
      LastSlash = strrchr(FromFileName, DIR_SLASH);
      if (LastSlash != NULL)
      {
         PathOnlyLen = LastSlash - FromFileName + 1;
         FileNameOnly = LastSlash + 1;
      }
      else
      {
         /* current dir */
         PathOnlyLen = strlen(CURRENT_DIR);
         FileNameOnly = FromFileName;
      }

      /* last occurrence of '.' in FileNameOnly */
      LastPeriod = strrchr(FileNameOnly, EXT_PERIOD);
      if (LastPeriod != NULL)
      {
         FNameOnlyLen = LastPeriod - FileNameOnly;
      }
      else
      {
         FNameOnlyLen = strlen(FromFileName) - PathOnlyLen;
      }

      /* The first "1" is for the possible period (.) (if applicable) */
      /* The second "1" is for the terminating null character. */
      ToFileName = malloc(PathOnlyLen + FNameOnlyLen + 1 + strlen(ExtOfToFile) + 1);
      if (ToFileName == NULL)
      {
         WriteLog(ERROR_NO_MEM);
         return -1;
      }

      if (LastSlash != NULL)
      {
         /* No null-character is implicitly appended to the end of */
         /* destination, so destination will only be null-terminated if */
         /* the length of the C string in source is less than num. */
         strncpy(ToFileName, FromFileName, PathOnlyLen + FNameOnlyLen);
         ToFileName[PathOnlyLen + FNameOnlyLen] = '\0';
      }
      else
      {
         strncpy(ToFileName, CURRENT_DIR, PathOnlyLen);
         ToFileName[PathOnlyLen] = '\0';
         /* Appends the first num characters of source to destination, plus */
         /* a terminating null-character */
         strncat(ToFileName, FromFileName, FNameOnlyLen);
      }

      if (strchr(ExtOfToFile, EXT_PERIOD) == NULL)
      {
         strcat(ToFileName, EXT_PERIOD_STR);
      }
      strcat(ToFileName, ExtOfToFile);
#endif

      /* create a backup file if ToFileName already exists */
      ToFileP = fopen(ToFileName, READ_AS_BINARY);
      if (ToFileP != NULL)
      {
         fclose(ToFileP);
         /* rename ToFileName to ToFileName.bak  */
         if (OverWrite == 0)
         {
            BackFileName = malloc(strlen(ToFileName) + strlen(BAK_EXT_STR) + 1);
            if (BackFileName == NULL)
            {
               WriteLog(ERROR_NO_MEM);
               ReturnValue = -1;
               goto Func_Exit;
            }
            strcpy(BackFileName, ToFileName);
            strcat(BackFileName, BAK_EXT_STR);

            /* remove existing backup files of same name */
#if defined(MSDOS) || defined(WIN32)
            /* for MSDOS and Windows, we need to make sure the file is */
            /* writeable otherwise we cannot delete! Under Unix, this is */
            /* not necessary, since a file can be deleted if we have write */
            /* permissions for the directory */
            chmod(BackFileName, S_IRUSR|S_IWUSR );
#endif
            remove(BackFileName);
            /* rename the existed output file to the back up name */
            rename(ToFileName, BackFileName);
            free(BackFileName);
         }
      }

#if defined(MSDOS) || defined(WIN32)
      /* always use "wb" for MSDOS and WIN32 */
      ToFileP = fopen(ToFileName, WRITE_AS_BINARY);
#else
      if (ConvertDirection == BIN_TO_H)
      {
         ToFileP = fopen(ToFileName, WRITE_AS_TEXT);
      }
      else
      {
         ToFileP = fopen(ToFileName, WRITE_AS_BINARY);
      }
#endif
      if (ToFileP == NULL)
      {
         WriteLog(ERROR_CREATE_FILE, ToFileName, FromFileName);
         ReturnValue = -1;
         goto Func_Exit;
      }
   }
   else 
   {
      /* InputFileName == NULL, ie stdin is redirected */
#if defined(MSDOS) || defined(WIN32)
      /* need to make sure the input and output files are binary */
      /* on MSDOS and WIN32 */
      setmode(fileno(stdin), O_BINARY);
      setmode(fileno(stdout), O_BINARY);
#endif

      FromFileP = stdin;
      ToFileP = stdout;
   }

#if NEW_BUFSIZ > BUFSIZ
   /* If we can't get what we want, we just don't bother, and go */
   /* ahead with the minimum) */
   if ((InBufPtr = malloc(NEW_BUFSIZ)) != NULL)
   {
      setvbuf(FromFileP, InBufPtr, _IOFBF, NEW_BUFSIZ);
   }

   if ((OutBufPtr = malloc(NEW_BUFSIZ)) != NULL)
   {
      setvbuf(ToFileP, OutBufPtr, _IOFBF, NEW_BUFSIZ);
   }
#endif

   /* do the conversion */
   ReturnValue = Convert(FromFileP, ToFileP);

   /* close the files */
   fclose(FromFileP);
   fclose(ToFileP);

#if NEWBUFSIZ > BUFSIZ
   /* got to free buffers we allocated first */
   if (InBufPtr != NULL)
   {
      free(InBufPtr);
   }

   if (OutBufPtr != NULL)
   {
      free(OutBufPtr);
   }
#endif

Func_Exit:

   if (ToFileName != NULL)
   {
      free(ToFileName);
   }

   return ReturnValue;
}


/*
   Convert

   Does the actual work of converting FromFP to ToFP.

   Returns 0 on success, -1 on error.
*/
static int Convert(FILE *FromFP, FILE *ToFP)
{
   int Ch = EOF;
   int Count = 0;
   int PrevCh = EOF;
   unsigned char BinByte = 0xFF;
   int BinByteState = BIN_BYTE_NOT_START;

   if (ConvertDirection == BIN_TO_H)
   {
      fprintf(ToFP, "unsigned char %s[] = \x0D\x0A{\x0D\x0A   ", SymbolName);

      do
      {
         Ch = getc(FromFP);

         if (PrevCh != EOF)
         {
            if (!feof(FromFP))
            {
               fprintf(ToFP, "0x%.2hX, ", PrevCh);
            }
            else
            {
               fprintf(ToFP, "0x%.2hX", PrevCh);
            }
            Count++;
         }

         PrevCh = Ch;

         if (Count >= ColumnNumber)
         {
            fprintf(ToFP, "\x0D\x0A   ");
            Count = 0;
         }
      } while (!feof(FromFP));

      fprintf(ToFP, "\x0D\x0A};\x0D\x0A");
   }
   else if (ConvertDirection == BIN_FROM_H)
   {
      while ((Ch = getc(FromFP)) != EOF)
      {
         if (Ch == 'x' && PrevCh == '0')
         {
            BinByteState = BIN_BYTE_HIGH_FOUR;
         }
         else if (BinByteState == BIN_BYTE_HIGH_FOUR)
         {
            if (GetHalfByte(&Ch) == -1)
            {
               break;
            }

            BinByte = (unsigned char)Ch << 4;
            BinByteState = BIN_BYTE_LOW_FOUR;
         }
         else if (BinByteState == BIN_BYTE_LOW_FOUR)
         {
            if (GetHalfByte(&Ch) == -1)
            {
               break;
            }

            BinByte |= (0x0F & (unsigned char)Ch);
            if (putc(BinByte, ToFP) == EOF)
            {
               break;
            }

            BinByteState = BIN_BYTE_NOT_START;
         }
         //else
         //{
         //   BinByteState = BIN_BYTE_NOT_START;
         //}

         PrevCh = Ch;
      }
   }
   else
   {
      WriteLog(ERROR_DIRECTION, ConvertDirection);
      return -1;
   }

   /* if we reach here, either we've reached an EOF or an error */
   /* occurred. */
   if (!feof(FromFP))
   {
      WriteLog(ERROR_CONVERT, FromFileName);
      return -1 ;
   }

   return 0 ;
}

/*
   GetHalfByte

   Get half byte from an ascii character.

   Returns 0 on success, -1 on error.
*/
static int GetHalfByte (int *Ch)
{
   if (*Ch >= '0' && *Ch <= '9')
   {
      *Ch -= '0';
   }
   else if (*Ch >= 'A' && *Ch <= 'F')
   {
      *Ch = 0x0A + (*Ch - 'A');
   }
   else if (*Ch>= 'a' && *Ch <= 'f')
   {
      *Ch = 0x0A + (*Ch - 'a');
   }
   else
   {
      // Invalid hex character
      return -1;
   }

   return 0;
}



