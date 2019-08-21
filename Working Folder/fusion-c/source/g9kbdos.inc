; ___________________________________________________________
;/               __           _                              \
;|              / _|         (_)                             |
;|             | |_ _   _ ___ _  ___  _ __                   |
;|             |  _| | | / __| |/ _ \| '_ \                  |
;|             | | | |_| \__ \ | (_) | | | |                 |
;|             |_|  \__,_|___/_|\___/|_| |_| *               |
;|                                                           |
;|               The MSX C Library for SDCC                  |
;|                   V1.2 - August 2019                      |
;|                                                           |
;|                Eric Boez &  Fernando Garcia               |
;|                                                           |
;|               A S M  S O U R C E   C O D E                |
;|                                                           |
;|               Definitions used by G9KLIB                  |
;|                      By Sylvain G.                        |
;\___________________________________________________________/

;	.ifndef BDOS_INC
;	.define BDOS_INC

; System entry points
BDOS 			.equ	0x0005	;   BDOS entry address
MSX_DOS_SYS_ADR .equ	0x0006  ;   Address of msxdos2.sys

; Bdos functions (DOS2)
_TERM0 		.equ	0x00	;   Program terminate
_CONIN 		.equ	0x01	;   Console input
_CONOUT 	.equ	0x02	;   Console output
_AUXIN 		.equ	0x03	;   Auxiliary input
_AUXOUT 	.equ	0x04	;   Auxiliary output
_LSTOUT 	.equ	0x05	;   Printer output
_DIRIO 		.equ	0x06	;   Direct console I/O
_DIRIN 		.equ	0x07	;   Direct console input
_INNOE 		.equ	0x08	;   Console input without echo
_STROUT 	.equ	0x09	;   String output
_BUFIN 		.equ	0x0A	;   Buffered line input
_CONST 		.equ	0x0B	;   Console status
_CPMVER 	.equ	0x0C	;   Return version number
_DSKRST 	.equ	0x0D	;   Disk reset
_SELDSK 	.equ	0x0E	;   Select disk
_LOGIN      .equ    0x18 	;   Get login vector
_CURDRV 	.equ	0x19	;   Get current drive
_SETDTA		.equ	0x1A	;   Set dsisk transfer adress 
_ALLOC		.equ	0x1B	;   Get alloc information
_GDATE 		.equ	0x2A	;   Get date
_SDATE 		.equ	0x2B	;   Set date
_GTIME 		.equ	0x2C	;   Get time
_STIME 		.equ	0x2D	;   Set time
_VERIFY 	.equ	0x2E	;   Set/reset verify flag
_RDABS 		.equ	0x2F	;   Absolute sector read
_WRABS 		.equ	0x30	;   Absolute sector write
_DPARM 		.equ	0x31	;   Get disk parameters
_FFIRST 	.equ	0x40	;   Find first entry
_FNEXT 		.equ	0x41	;   Find next entry
_FNEW 		.equ	0x42	;   Find equ entry
_OPEN 		.equ	0x43	;   Open file handle
_CREATE 	.equ	0x44	;   Create file handle
_CLOSE 		.equ	0x45	;   Close file handle
_ENSURE 	.equ	0x46	;   Ensure file handle
_DUP 		.equ	0x47	;   Duplicate file handle
_READ 		.equ	0x48	;   Read from file handle
_WRITE 		.equ	0x49	;   Write to file handle
_SEEK 		.equ	0x4A	;   Move file handle pointer
_IOCTL 		.equ	0x4B	;   I/O control for devices
_HTEST 		.equ	0x4C	;   Test file handle
_DELETE 	.equ	0x4D	;   Delete file or subdirectory
_RENAME 	.equ	0x4E	;   Rename file or subdirectory
_MOVE 		.equ	0x4F	;   Move file or subdirectory
_ATTR 		.equ	0x50	;   Get/set file attributes
_FTIME 		.equ	0x51	;   Get/set file date and time
_HDELETE 	.equ	0x52	;   Delete file handle
_HRENAME 	.equ	0x53	;   Rename file handle
_HMOVE 		.equ	0x54	;   Move file handle
_HATTR 		.equ	0x55	;   Get/set file handle attributes
_HFTIME 	.equ	0x56	;   Get/set file handle date and time
_GETDTA 	.equ	0x57	;   Get disk transfer address
_GETVFY 	.equ	0x58	;   Get verify flag setting
_GETCD 		.equ	0x59	;   Get current directory
_CHDIR 		.equ	0x5A	;   Change current directory
_PARSE 		.equ	0x5B	;   Parse pathname
_PFILE 		.equ	0x5C	;   Parse filename
_CHKCHR 	.equ	0x5D	;   Check character
_WPATH 		.equ	0x5E	;   Get whole path string
_FLUSH 		.equ	0x5F	;   Flush disk buffers
_FORK 		.equ	0x60	;   Fork a child process
_JOIN 		.equ	0x61	;   Rejoin parent process
_TERM 		.equ	0x62	;   Terminate with error code
_DEFAB 		.equ	0x63	;   Define abort exit routine
_DEFER 		.equ	0x64	;   Define disk error handler routine
_ERROR 		.equ	0x65	;   Get previous error code
_EXPLAIN 	.equ	0x66	;   Explain error code
_FORMAT 	.equ	0x67	;   Format a disk
_RAMD 		.equ	0x68	;   Create or destroy RAM disk
_BUFFER 	.equ	0x69	;   Allocate sector buffers
_ASSGIN 	.equ	0x6A	;   Logical drive assignment
_GENV 		.equ	0x6B	;   Get environment item
_SENV 		.equ	0x6C	;   Set environment item
_FENV 		.equ	0x6D	;   Find environment item
_DSKCHK 	.equ	0x6E	;   Get/set disk check status
_DOSVER 	.equ	0x6F	;   Get MSX DOS version number
_REDIR 		.equ	0x70	;   Get/set redirection status

; Errors
;ERROR
.NCOMP 		.equ	0x0FF	;   Incompatible disk
.WRERR 		.equ	0x0FE	;   Write error
.DISK 		.equ	0x0FD	;   Disk error
.NRDY 		.equ	0x0FC	;   Not ready
.VERFY 		.equ	0x0FB	;   Verify error
.DATA 		.equ	0x0FA	;   Data error
.RNF 		.equ	0x0F9	;   Sector not found
.WPROT 		.equ	0x0F8	;   Write protected disk
.UFORM 		.equ	0x0F7	;   Unformatted disk
.NDOS 		.equ	0x0F6	;   Not a DOS disk
.WDISK 		.equ	0x0F5	;   Wrong disk
.WFILE 		.equ	0x0F4	;   Wrong disk for file
.SEEK 		.equ	0x0F3	;   Seek error
.IFAT 		.equ	0x0F2	;   Bad file allocation table
.NOUPB 		.equ	0x0F1	;   --
.IFORM 		.equ	0x0F0	;   Cannot format this drive
.INTER 		.equ	0x0DF	;   Internal error
.NORAM 		.equ	0x0DE	;   Not enough memory
.IBDOS 		.equ	0x0DC	;   Invalid MSX-DOS call
.IDRV 		.equ	0x0DB	;   Invalid drive
.IFNM 		.equ	0x0DA	;   Invalid filename
.IPATH      .equ    0x0D9	;   Invalid pathname
.PLONG 		.equ	0x0D8	;   Pathname too long
.NOFIL 		.equ	0x0D7	;   File not found
.NODIR 		.equ	0x0D6	;   Directory not found
.DRFUL 		.equ	0x0D5	;   Root directory full
.DKFUL 		.equ	0x0D4	;   Disk full
.DUPF 		.equ	0x0D3	;   Duplicate filename
.DIRE 		.equ	0x0D2	;   Invalid directory move
.FILRO 		.equ	0x0D1	;   Read only file
.DIRNE 		.equ	0x0D0	;   Directory not empty
.IATTR 		.equ	0x0CF	;   Invalid attributes
.DOT 		.equ	0x0CE	;   Invalid . or .. operation
.SYSX 		.equ	0x0CD	;   System file exists
.DIRX 		.equ	0x0CC	;   Directory exists
.FILEX 		.equ	0x0CB	;   File exists
.FOPEN 		.equ	0x0CA	;   File already in use
.OV64K 		.equ	0x0C9	;   Cannot transfer above 64K
.FILE 		.equ	0x0C8	;   File allocation error
.EOF 		.equ	0x0C7	;   End of file
.ACCV 		.equ	0x0C6	;   File access violation
.IPROC 		.equ	0x0C5	;   Invalid process id
.NHAND 		.equ	0x0C4	;   No spare file handles
.IHAND 		.equ	0x0C3	;   Invalid file handle
.NOPEN 		.equ	0x0C2	;   File handle not open
.IDEV 		.equ	0x0C1	;   Invalid device operation
.IENV 		.equ	0x0C0	;   Invalid environment string
.ELONG 		.equ	0x0BF	;   Environment string too long
.IDATE 		.equ	0x0BE	;   Invalid date
.ITIME 		.equ	0x0BD	;   Invalid time
.RAMDX 		.equ	0x0BC	;   RAM disk (drive H ) already exists
.NRAMD 		.equ	0x0BB	;   RAM disk does not exist
.HDEAD 		.equ	0x0BA	;   File handle has been deleted
.EOL 		.equ	0x0B9	;   Internal error. Should never occur
.ISBFN 		.equ	0x0B8	;   Invalid sub-function number
.STOP 		.equ	0x09F	;   Ctrl-STOP pressed
.CTRLC 		.equ	0x09E	;   Ctrl-C pressed
.ABORT 		.equ	0x09D	;   Disk operation aborted
.OUTERR 	.equ	0x09C	;   Error on standard output
.INERR 		.equ	0x09B	;   Error on standard input
.BADCOM 	.equ	0x08F	;   Wrong version of COMMAND
.BADCM 		.equ	0x08E	;   Unrecognized command
.BUFUL 		.equ	0x08D	;   Command too long
.OKCMD 		.equ	0x08C	;   --
.IPARM 		.equ	0x08B	;   Invalid parameter
.INP 		.equ	0x08A	;   Too many parameters
.NOPAR 		.equ	0x089	;   Missing parameter
.IOPT 		.equ	0x088	;   Invalid option
.BADNO 		.equ	0x087	;   Invalid number
.NOHELP 	.equ	0x086	;   File for HELP not found
.BADVER 	.equ	0x085	;   Wrong version of MSX-DOS
.NOCAT 		.equ	0x084	;   Cannot concatenate destination file
.BADEST 	.equ	0x083	;   Cannot create destination file
.COPY 		.equ	0x082	;   File cannot be copied onto itself
.OVDEST 	.equ	0x081	;   Cannot overwrite previous destination file

; Allocate segment parameters
USER_SEGMENT	   	.equ     0	; Register A
SYSTEM_SEGMENT     	.equ     1  ;   "         "

PRIMARY_MAPPER     	.equ     0  ; Register B 

; Open/create file flags (_OPEN,_CREATE)
OPEN_NO_WRITE	   	.equ     1
OPEN_NO_READ       	.equ     2
OPEN_INHERITABLE   	.equ     4 

; Seek flags  (_SEEK)
SEEK_REL_BEGINING   .equ 	0   ; Relative to the beginning of the file
SEEK_REL_CURRENT	.equ    1   ; Relative to the current position
SEEK_REL_END		.equ	2   ; Relative to the end of the file.


; Parse flags(_PARSE)
;b0 - set if any characters parsed other than drive name
;b1 - set if any directory path specified
;b2 - set if drive name specified
;b3 - set if main filename specified in last item
;b4 - set if filename extension specified in last item
;b5 - set if last item is ambiguous
;b6 - set if last item is "." or ".."
;b7 - set if last item is ".."


; Create flags
CREATE_NEW	   	.equ     128

; File attributes
FILE_READ_ONLY	   	.equ     1   ; 
FILE_HIDDEN	   		.equ     2   ;
FILE_SYSTEM	   		.equ     4   ; 
FILE_VOLUME_NAME   	.equ     8   ; 
FILE_DIRECTORY     	.equ     16  ; 
FILE_ARCHIVE	   	.equ     32  ; 
FILE_RESERVED      	.equ     64  ; 
FILE_DEVICE	   		.equ    128  ;

; Disk error handler routine parameters in register C
ERROR_WRITING	   	.equ	 1  ; Set if writing
ERROR_NO_IGNORE    	.equ     2  ; Set if ignore not recommended
ERROR_AUTO_ABORT   	.equ     4  ; Set if auto-abort suggested
ERROR_VALID_SECTOR 	.equ     8  ; Set if sector number is valid 


; Disk error handler routine return codes
CALL_SYSTEM_ERROR 	.equ     0 ; Call system error routine
ERROR_ABORT       	.equ     1 ; Abort
ERROR_RETRY       	.equ     2 ; Retry
ERROR_IGNORE      	.equ     3 ; Ignore


; Character flags for _CHKCHR
SUPPRESS_UPPER_CASING  	 .equ   0  ; Set to suppress upper casing
FIRST_BYTE_WIDE_CHAR	 .equ   1  ; Set if first byte of 16-bit character
SECOND_BYTE_WIDE_CHAR	 .equ   2  ; Set if first byte of 16-bit character
VOLUME_NAME		 		 .equ   3  ; Set if second byte of 16-bit character
NOT_A_VALID_CHAR	 	 .equ   4  ; Not a valid file/volume name character


;  Params _RAMD
DESTROY_RAMDISK	  	.equ	0
GET_RAMDISK_SIZE  	.equ   0xFF
CREATE_RAMDISK	  	.equ   1	;  CREATE_RAMDISK+n = 1+n

; Misc
CR 			.equ	0x0D	;   Carridge Return
LF 			.equ	0x0A	;   Line Feed
LE 			.equ	0x24	;   Line end
UPPER_CASE_MASK 	.equ     0xDF

CMD_LENGTH		.equ	0x80
CMD_LINE		.equ	0x82

COM_START_ADDR  	.equ    0x0100

BOOT_DRIVE		.equ	0xF2FD

;		STRUCT	FIB
FIB.ff:				.BYTE   0     ; 0 - Always 0FFh		
FIB.fileName:		.BYTE   0,0,0,0,0,0,0,0,0,0,0,0,0  ; Filename as an ASCIIZ string
FIB.fileAttrib:		.BYTE	0     ; File attributes byte
FIB.lastModTime:	.WORD	0      ; Time of last modification
FIB.lastModDate:	.WORD	0      ; Date of last modification
FIB.startCluster:	.WORD	0      ; Start Cluster
FIB.fileSize:		.BYTE	0,0,0,0   ; Files size
FIB.logicalDrive:	.BYTE	0      ; Logical drive
FIB.internalInfo:	.BYTE	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0  ; Internal information, must not be modified
;		ENDS


;		STRUCT DISK_PAR
DISK_PAR.physicalDrive:   	.BYTE	1   ; 0      - Physical drive number (1=A: etc)
DISK_PAR.sectorSize:      	.WORD	0   ; 1,2    - Sector size (always 512 currently)
DISK_PAR.sectorsCluster:  	.BYTE	0   ; 3      - Sectors per cluster (non-zero power of 2)
DISK_PAR.reservedSectors: 	.WORD	0   ; 4,5    - Number of reserved sectors (usually 1)
DISK_PAR.nrOfFats:		 	.BYTE	0   ; 6      - Number of copies of the FAT (usually 2)
DISK_PAR.nrRootEntries:	 	.WORD	0   ; 7,8    - Number of root directory entries
DISK_PAR.logicalSectors:	.WORD	0    ; 9,10   - Total number of logical sectors
DISK_PAR.mediaDescriptor: 	.BYTE	0   ; 11     - Media descriptor byte
DISK_PAR.nrSectorsFat:	 	.BYTE	0   ; 12     - Number of sectors per FAT
DISK_PAR.firstRootSector: 	.WORD	0   ; 13..14 - First root directory sector number
DISK_PAR.firstDataSector: 	.WORD	0   ; 15..16 - First data sector number
DISK_PAR.maxClusterNr:    	.WORD	0   ; 17..18 - Maximum cluster number
DISK_PAR.dirtyDisk:       	.BYTE	0   ; 19     - Dirty disk flag
DISK_PAR.volumeID:		  	.BYTE   0,0,0,0   ; 20..23 - Volume id. (-1 => no volume id.)
DISK_PAR.extLogicalSect:  	.BYTE   0,0,0     ; 24..26 - Extended total number of logical sectors (FAT16)
DISK_PAR.reserved:	 	 	.BYTE	0,0,0,0,0 ; 27..31 - Reserved (currently always zero)
;		ENDS


DPR_PTR_DRIVE_A .equ	0xf355
DPR_PTR_DRIVE_B .equ	0xf357
DPR_PTR_DRIVE_C .equ	0xf359
DPR_PTR_DRIVE_D .equ	0xf35b
DPR_PTR_DRIVE_E .equ	0xf35d
DPR_PTR_DRIVE_F .equ	0xf35f
DPR_PTR_DRIVE_G .equ	0xf361
DPR_PTR_DRIVE_H .equ	0xf363

                
;STRUCT DPR
DPR.driveNumber:		.BYTE	0 ; Drive nummer
DPR.mediaId:     		.BYTE   0 ; Media ID byte
DPR.bytesPerSector: 	.WORD   0 ; number of bytes per sector
DPR.dirMask:			.BYTE   0 ; directory maske
DPR.dirShift:			.BYTE	0 ; directory shift
DPR.clusterMask:		.BYTE 	0 ; cluster mask
DPR.clusterShift:		.BYTE   0 ; cluster shift
DPR.firstFatSector: 	.WORD	0 ; first FAT sector
DPR.nrFats:				.BYTE	0 ; Number of FATs
DPR.nrDirEntries:		.BYTE   0 ; aantal directory entries
DPR.firstDataSector:	.WORD   0 ; eerste data sector
DPR.nrClusters:			.WORD   0 ; Number of clusters + 1
DPR.sectorsFat:			.BYTE   0 ; Number of sectors per FAT
DPR.firstDirSector: 	.WORD   0 ; eerste directory sector
DPR.fatAddress:			.WORD   0 ; FAT-address
;ENDS

DRV_CTRL_1	 		.equ 	0xFB21 ; number of drives connected to the 1st interface
SLOT_DRV_CNTRL_1 	.equ 	0xFB22 ; slotcode of the 1st interface
DRV_CTRL_2	 		.equ 	0xFB23 ; number of drives connected to the 2nd interface
SLOT_DRV_CNTRL_2 	.equ 	0xFB24 ; slotcode of the 2nd interface
DRV_CTRL_3	 		.equ 	0xFB25 ; number of drives connected to the 3rd interface
SLOT_DRV_CNTRL_3 	.equ 	0xFB26 ; slotcode of the 3rd interface
DRV_CTRL_4	 		.equ 	0xFB27 ; number of drives connected to the 4th interface
SLOT_DRV_CNTRL_4 	.equ 	0xFB28 ; slotcode of the 4th interface


SLTWORK_BASE	 .equ    0xFD09

;	STRUCT DISK_WORK_AREA
DISK_WORK_AREA.diskStopCount:	.BYTE 0 ; Diskstop counter
DISK_WORK_AREA.readyFlagA:		.BYTE 0 ; Ready-flag drive A: (=counter after diskstop)   
DISK_WORK_AREA.readyFlagB:      .BYTE 0 ; Ready-flag drive B: (=counter after diskstop)
DISK_WORK_AREA.activePhysDrive: .BYTE 0 ; Active physical drive
DISK_WORK_AREA.lastTrackVirDrA: .BYTE 0 ; Last track position for virtual drive A: 
DISK_WORK_AREA.lastTrackVirDrB: .BYTE 0 ; Last track position for virtual drive B: 
DISK_WORK_AREA.activeLogDrive:  .BYTE 0 ; Active logical drive
DISK_WORK_AREA.nrPhysDrives:    .BYTE 0 ; number of connected physical drives
;	ENDS


;		STRUCT MAPPER_TABLE
MAPPER_TABLE.slotAddress:	.BYTE	0    ; Slot address of the mapper slot.
MAPPER_TABLE.nrSegments:	.BYTE	0    ; Total number of 16k RAM segments. 1...255 (8...255 for the primary)		
MAPPER_TABLE.freeSegments:  .BYTE   0    ; Number of free 16k RAM segments.
MAPPER_TABLE.sysAllocated:  .BYTE   0    ; Number of 16k RAM segments allocated to the system (at least 6 for the primay)
MAPPER_TABLE.userAllocated: .BYTE   0    ; Number of 16k RAM segments allocated to the user
;		ENDS	
		   
; Entries for other mapper slots. If there is none, MAPPER_TABLE+8 will be zero.

;		STRUCT MAPPER_FUNCTIONS
MAPPER_FUNCTIONS.AllocateSeg:	.BYTE	0,0,0	;   Allocate a 16k segment.
MAPPER_FUNCTIONS.FreeSeg:		.BYTE	0,0,0     ;   Free a 16k segment.
MAPPER_FUNCTIONS.ReadSeg:		.BYTE	0,0,0	;   Read byte from address A HL to A.
MAPPER_FUNCTIONS.WriteSeg:	.BYTE	0,0,0	;   Write byte from E to address A HL.
MAPPER_FUNCTIONS.CallSeg:		.BYTE	0,0,0	;   Inter-segment call. Address in IYh IX
MAPPER_FUNCTIONS.CallsSeg:	.BYTE	0,0,0	;   Inter-segment call. Address in line after the call instruction.
MAPPER_FUNCTIONS.PutSeg:		.BYTE	0,0,0	;   Put segment into page (HL).
MAPPER_FUNCTIONS.GetSeg:		.BYTE	0,0,0	;   Get current segment for page (HL)
MAPPER_FUNCTIONS.PutSeg0:		.BYTE	0,0,0	;   Put segment into page 0.
MAPPER_FUNCTIONS.GetSeg0:		.BYTE	0,0,0	;   Get current segment for page 0.
MAPPER_FUNCTIONS.PutSeg1:		.BYTE	0,0,0	;   Put segment into page 1.
MAPPER_FUNCTIONS.GetSeg1:		.BYTE	0,0,0	;   Get current segment for page 1.
MAPPER_FUNCTIONS.PutSeg2:		.BYTE	0,0,0	;   Put segment into page 2.
MAPPER_FUNCTIONS.GetSeg2:		.BYTE	0,0,0	;   Get current segment for page 2.
MAPPER_FUNCTIONS.PutSeg3:		.BYTE	0,0,0	;   Put segment into page 3.
MAPPER_FUNCTIONS.GetSeg3:		.BYTE	0,0,0	;   Get current segment for page 3.
;		ENDS

;		 STRUCT	MAPPER
nrSegments:	 	 	.BYTE	0
priMapperSlot:  	.BYTE	0
freeSegments:	 	.BYTE	0
mapperSupport:    	.WORD	0         ; mapper support routine address
AllocateSegment:  	.BYTE	0,0,0     ;  Allocate a 16k segment.
FreeSegment:		.BYTE	0,0,0     ;  Free a 16k segment.	
ReadSegmentByte:  	.BYTE	0,0,0     ;  Read byte from address A HL to A.
WriteSegmentByte: 	.BYTE	0,0,0     ;  Write byte from E to address A HL.
InterSegCallReg:  	.BYTE	0,0,0     ;  Inter-segment call.     Address in IYh IX 	
InterSegCall:     	.BYTE	0,0,0     ;  Inter-segment call.     Address in line after the call instruction.
PutSegmentHL:     	.BYTE	0,0,0     ;  Put segment into page (HL).
GetSegmentHL:     	.BYTE	0,0,0     ;  Get current segment for page (HL)
PutSegment0:      	.BYTE	0,0,0     ;  Put segment into page 0.
GetSegment0:      	.BYTE	0,0,0     ;  Get current segment for page 0.
PutSegment1:      	.BYTE	0,0,0     ;  Put segment into page 1.
GetSegment1:      	.BYTE	0,0,0     ;  Get current segment for page 1.
PutSegment2:      	.BYTE	0,0,0     ;  Put segment into page 2.
GetSegment2:      	.BYTE	0,0,0     ;  Get current segment for page 2.
PutSegment3:      	.BYTE	0,0,0     ;  Not supported since page-3 must never be changed. Acts like a "NOP" if called.
GetSegment3:      	.BYTE	0,0,0     ;  Get current segment for page 3.
;	 	 ENDS
			   
.MACRO   BdosCall function
    LD      C,function
    CALL    BDOS
.ENDM

.MACRO	CallSegment segment,address	
		LD      IY,(segment-#1)
		LD      IX,address            
		CALL    mapper.InterSegCallReg
.ENDM

;	.endif