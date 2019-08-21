/*
 ___________________________________________________________
/               __           _                              \
|              / _|         (_)                             |
|             | |_ _   _ ___ _  ___  _ __                   |
|             |  _| | | / __| |/ _ \| '_ \                  |
|             | | | |_| \__ \ | (_) | | | |                 |
|             |_|  \__,_|___/_|\___/|_| |_| *               |
|                                                           |
|               The MSX C Library for SDCC                  |
|                   V1.1 - 09-10-11 2018                    |
|                                                           |
|                Eric Boez &  Fernando Garcia               |
|                                                           |
|   io.h  MSX DOS 1 & 2, File operations functions          |
|                                                           |
|                                                           |
|                                                           |
\___________________________________________________________/
*                         (°□°)
*	Most original code from SOLID MSX C - (c) 1997
*	SDCC port 2015  by ?	
*	added functions by Eric Boez 2019

    MSXDOS1 use FCB for file operations:
	perform the initialisation FCBs(),
	also obtains pointer to the reserved  FCB file list
	...
	FCBlist *FCB = FCBs();
	
	If you are using MSX DOS2, and want to use backward compatibility with MSX DOS 1 FCB
	Set the Os version to 1.  Specific MSXDOS2 Functions like MKDIR will be sisabled

	_os_ver = 1;
	
	int f = open("A.TXT",O_RDWR);
	cputs(FCB->fcb[f].filename);

	
	MSXDOS2 is MSXDOS1 backwards compatible and all files can be processed on MSXturboR
	by using same FCB handles as on MSX1. Anyway MSXturboR can manage MSXDOS2 and this library
	switches to CALL 5 functions above 40h. Of course, manually _os_ver = 1 sets it back to MSX1.
	
	For BlueMSX emulator:
	Use http://www.lexlechz.at/en/software/DiskMgr.html to emulate floppy disk images (.dsk),
	otherwise emulator turns read-only mode for directory. 
	
	*/
#ifndef __IO_FILE_H__
#define __IO_FILE_H__


#define	SEEK_SET	0
#define	SEEK_CUR	1
#define	SEEK_END	2

#define	O_RDONLY	0
#define	O_WRONLY	1
#define	O_RDWR		1
#define	O_CREAT		0x39
#define	O_EXCL		0x04
#define	O_TRUNC		0x31
#define	O_APPEND	0x41
#define	O_TEMP		0x80

/* get OS version	1-> MSXDOS 1.X, 2-> MSXDOS2, 0-not initiated */
extern	int	GetOSVersion( void );
/* available after get_OS_version */
extern	char	_os_ver;	// MSX-DOS kernel version (better set to 1)
extern	char	_mx_ver;	// MSXDOS2.SYS version number (informational)

// Handbook on FCB at
// http://fms.komkon.org/MSX/Handbook/th-3.txt

typedef struct {
	unsigned char drive;     // 0: default drive
	unsigned char filename[11];	// 8+3 for extension, as "MYPROG  PRG"
	unsigned int block;
	unsigned int record_size;
	unsigned long file_size;
	unsigned int date;
	unsigned int time;
	unsigned char device;
	unsigned char dir_location;
	unsigned int top_cluster;
	unsigned int lastacsd_cluster;
	unsigned int clust_from_top;
	unsigned char record;
	unsigned long rand_record;
	unsigned char none;	//+1 byte
} FCBstru;	// 38 bytes

//
typedef struct {
	FCBstru fcb[8];
} FCBlist;

//
// Structure that will receive Get Disk Parameters data (MSX-DOS2 function 0x31)
typedef struct {
	  char DriveN; 						// Physical drive number (1=A: etc)
	  int SectorSize;  					// Sector size (always 512 currently)
	  char SectorPerCluster;  			// Sectors per cluster (non-zero power of 2)
	  int NumberReservedSector;  		// Number of reserved sectors (usually 1)
	  char NumberFatCopy;				// Number of copies of the FAT (usually 2)
	  int NumberRootDirEntries;			// Number of root directory entries
	  int TotalLogicalSectors;			// Total number of logical sectors
	  char MediaDescriptionByte;		// Media descriptor byte
	  char NumberSectorsPerFat;			// Number of sectors per FAT
	  int FirstRootSectorNumber;		// First root directory sector number
	  int FirstDataSectorNumber;		// First data sector number
	  int MaximumCluster;				// Maximum cluster number
	  char DirtyFlag;					// Dirty disk flag
	  char VolumeId[4];					// Volume id. (-1 => no volume id.)
	  char Reserved[8];					// Reserved (currently always zero)
} DSKPARAMS;

///
extern	FCBlist *FCBs( void );

extern	int	_io_errno;	/* to see error code in register A after CALL 5 */

	/* FILE READ,WRITE OPERATIONS */

		
extern	int		Open( char *name, int mode ); 					/* opens file, returns number fH= 3...15 as file handler, or -1 on error */		
extern	int		OpenAttrib( char *name, int mode, int attr ); 	/* opens file providing attributes for MSXDOS2, (attributes = mode, default) */
extern	int		Create(char *name); 							/* creates file, opens, see open */
extern	int		CreateAttrib(char *name, int attr); 			/* creates file providing attributes for MSXDOS2, (attributes = mode, default) */
extern	int		Close( int fH ); 								/* closes file by file handler, returns 0, or -1 on error in _io_error */	
extern	int		Read(int fH, void *buf, unsigned int nbytes); 	/* reads from file to buffer */
extern	int		Write(int fH, void *buf, unsigned int nbytes); 	/* writes buffer to file */
extern	int		GetCWD(char *buf, int bufsize); 				/* gets the A: into buffer, returns 0 on success */
extern	int		GetDisk(void); 									/* gets current drive number */
extern	void 	SetDisk(int diskno); 							/* sets drive number */

extern 	unsigned char	SectorRead(unsigned int SectorStart, unsigned char drive, unsigned char NbSectors); /* read Nbsectors from drive. Start reading at sector */
extern 	unsigned char	SectorWrite(unsigned int SectorStart, unsigned char drive, unsigned char NbSectors); /* Write Nbsectors to drive. Start writing at sector */
extern 	void  			SetDiskTrAddress(unsigned int *address);  /* Set the transfert adress for Read/wrtie Sector */
extern  unsigned int  	GetDiskTrAddress(void); 					/* Get the current disk Transfer adress */
extern	unsigned char 	GetDiskParam(DSKPARAMS *info, unsigned char Drive); /*Get Disk current Parameters . Only for MSX-DOS2 */

//extern	FCBlist _buf8_fcbs;	// internal


		/* read file position, returns 0, or error in _io_error.
		address is like 0xABCD, asm will operate as 4-bytes long value
		*/
extern	int		Ltell(int fH, unsigned long address);

		/* set file position, returns 0, or error in _io_error.
		ot = 0,1,2. On return long value is set to current record */
extern	int		Lseek(int fH, unsigned long address, int ot);	

		
		/* FILE DIRECTORY OPERATIONS */
		/* removes file, returns 0 on success, or error in _io_error */
extern	int		Remove(char *filename);
		/* renames file,folder, returns 0 on success, or error in _io_error */
extern	int		Rename(char *old_name, char *new_name);
		/* finds files or folders by will-card as "*.COM", "????", etc.
		, returns 0 on success, or error in _io_error.
		Provide 0 or attributes for MSXDOS2. */
extern	int		FindFirst(char *willcard, char *result, int attr);
		/* continue search after findfirst */ 
extern	int		FindNext(char *result);


		/* MSXDOS2 only */
		/* sets current path, returns 0, or error in _io_error */
extern	int		ChangeDir(char *path);
		/* creates folder, returns 0, or error in _io_error */
extern	int		MakeDir(char *folderName);
		/* removes folder, returns 0, or error in _io_error */
extern	int		RemoveDir(char *folderName);


/* * MSXDOS1 only file operations c-level, uses FCB file info structure */
	/*
#define	B8dH	_buf8_fcbs.fcb[fH]
	 read file position from FCB 
extern	unsigned long _tell(int fH) { return B8dH.rand_record; }
		// set file position from FCB 
extern	void _seek(int fH, long pos, int ot)
		{
		if(ot==SEEK_CUR) B8dH.rand_record+=pos;
		else B8dH.rand_record = (ot==SEEK_END ? B8dH.file_size+pos : pos );
		}
		// get file size
extern	unsigned long _size(int fH) { return B8dH.file_size; }*/

/*	int diskload( char* filename, unsigned int address, unsigned int runat_address );
*		Load a Binary file at specific memory adress
*		filename - 11 chars DOS1 for FCB, as "MYFILE  DAT"
*		address - where to load the first byte
*		runat_address - if not 0, then where to CALL <runat_address> after loaded
*
*	returns 0 on success
*	
*/

extern	int 	DiskLoad( char* filename, unsigned int address, unsigned int runat );

#endif