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
|                   V1.2 - August -2019                     |
|                                                           |
|                Eric Boez &  Fernando Garcia               |
|                                                           |
|                       msx_fusion.h                        |
|                                                           |
|                                                           |
|                                                           |
\___________________________________________________________/
*                         (°□°)
* 
*/

#ifndef	NULL
#define	NULL	(void *)0
#endif

//Logical operators
//
// |    or
// &    and
// ^    xor
// >>   Right Shifting
// << Left shifting
//
// Logical operators not implemented in C but must be programmed
// nor = ~(a|b);
// nand = ~(a&b);


#ifndef __MSX_FUSION__
#define __MSX_FUSION__

// Alias for deleted or renamed functions from V1.0 to 1.2, Trying to keep compatibility between versions 1.0 and 1.1 's source codes
#define WaitForKey WaitKey
#define KeyboardHit Inkey
#define Getcon Getche
#define EnableInterupt EnableInterrupt
#define DisableInterupt DisableInterrupt
#define HMCM_SC5 HMCM
#define Rect BoxLine

/// Fusion C ...
#define FCB_SUCCESS	0x00				/* return code */

/* FCB_DIR::attribute */
#define	FCB_ATTR_READONLY	0x01		/* MSX-DOS2 */
#define FCB_ATTR_HIDDEN		0x02
#define	FCB_ATTR_SYSTEM		0x04		/* MSX-DOS2 */
#define FCB_ATTR_VOLUME		0x08		/* MSX-DOS2 */
#define	FCB_ATTR_DIR		0x10		/* MSX-DOS2 */
#define	FCB_ATTR_ARCHIVE	0x20		/* MSX-DOS2 */

#define SCRMOD  			0xFCAF      /* current screen mode */
#define ACPAGE  			0xFAF6      /* active page number */
#define DPPAGE  			0xFAF5      /* display page number */
#define ATRBAS  			0xF928      /* sprite attribute table base address */
#define PATBAS 				0xF926      /* sprite pattern generator table base address */

#define  KB_RIGHT  			0x80
#define  KB_DOWN   			0x40
#define  KB_UP     			0x20
#define  KB_LEFT   			0x10
#define  KB_DEL    			0x08
#define  KB_INS    			0x04
#define  KB_HOME   			0x02
#define  KB_SPACE  			0x01 

#define CURSORKEYS     		0
#define JOYSTICK1      		1
#define JOYSTICK2B     		2

#define SPACEBAR       		0
#define JOY1_BUTTONA   		1
#define JOY2_BUTTONA   		2
#define JOY1_BUTTONB   		3
#define JOY2_BUTTONB   		4

#define INACTIVE    		0
#define UP          		1
#define UP_RIGHT    		2
#define RIGHT       		3
#define DOWN_RIGHT  		4
#define DOWN        		5
#define DOWN_LEFT   		6
#define LEFT        		7
#define UP_LEFT     		8
#define UNPRESSED   		0
#define PRESSED    			255

#define MousePort1 			0x1310
#define MousePort2 			0x6C20


// Structure for ReadMouseTo output data
typedef struct {
    signed char dx;
    signed char dy;
    unsigned char lbutton;
    unsigned char rbutton;
} MOUSE_DATA;

//Logical operations (OP) for Graphic Function


#define	LOGICAL_IMP		0	// DC := SC	assign, set new colour
#define	LOGICAL_AND		1	// DC &= SC	assign with AND
#define	LOGICAL_OR		2	// DC |= SC	assign with OR
#define	LOGICAL_XOR		3	// DC ^= SC	assign with XOR
#define	LOGICAL_NOT		4	// DC = !SC	assign with NOT
// if transparent SC==0, 
#define	LOGICAL_TIMP	8	// if SC>0 then IMP
#define	LOGICAL_TAND	9	// if SC>0 then AND
#define	LOGICAL_TOR		10	// if SC>0 then OR
#define	LOGICAL_TXOR	11	// if SC>0 then XOR
#define	LOGICAL_TNOT	12	// if SC>0 then NOT


// fill logical for rectangle and circle
#define	FILL_ALL	0xFF
#define	NO_FILL		0x00



/* --------------------------------------------------------- */
/* console	functions										 */
void 				bchput(char value);
void 				num2Dec16(unsigned int aNumber, char *address, char emptyChar); 
void 				Print(char *text);												// Print a string to Text screen mode
void 				PrintFNumber(unsigned int value, char emptyChar, char length);  // Print a  Number on Screen with formating parameters to Text screen mode
void 				PrintNumber(unsigned int num);								// Print a number on Screen to Text screen mode
char				InputChar (void);												// read char from console 
int	 				InputString (char *dest, int len); 								// String input from console. dest - pointer of buffer where to store entered string. len - [0..255] is length of buffer dest, user can enter (len-2) chars, max.length=253. Returns length of string 
char				Getche (void);													// read and display char from console 
void				PrintChar (char c);												// display char 
void				Locate (char x, char y); 										// set cursor to x,y 
void				PrintDec (int num);												// displays signed integer value  -32768 to 32767  (larges code) 
void 				PrintHex (unsigned int num);									// Print Hexadecimal representation of the Int variable passed
void				PrintString(char* str);											// Same as Print
void 				PutCharHex(char c);												// Print Hexadecimal representation of the char variable passed								
int					CheckBreak(void);												// Check CTRL-STOP. returns 0 if CTRL-STOP was not pressed, -1 otherwise


/* --------------------------------------------------------- */
/* miscellaneous functions									 */
void 				KeySound(char n);											// Disable/Enable KeySound (0|1)
void 				FunctionKeys (char n);										// Hide or Show Function Keys (0|1)
void 				Beep(void);													// Emit a Beep
char 				ReadMSXtype(void);											// Read and return the MSX type (0,1,2,3)
void 				ChangeCap(char n);											// Change Cap led State (0|1)
void 				Cls(void);													// Clear Screen
void 				Screen( unsigned char mode );								// Define activ screen mode
void 				SetRealTimer(unsigned int value);							// Set The Real Timer to a specific value
unsigned int 		RealTimer(void);											// Return the Timer value (Timer is incemented 50 or 60 times / Seconds)
void 				CovoxPlay(char page, unsigned int start_adress, unsigned int length, char speed); // Play Wav Sample stored in Vram, to Covox Module
void 				RleWBToVram(unsigned int *RamAddress, unsigned int VramAddress); // RLE Decompression data to VRAM
void 				RleWBToRam (unsigned int *RamSource, unsigned int *RamDest); 	// RLE Decompression data to RAM

/* --------------------------------------------------------- */
/* Joystick functions										 */
unsigned char 		JoystickRead(char joyNumber);								// Read Joystick Port (joynumber)
unsigned char 		TriggerRead(char TriggerNumber);							// Read Button state (Joynnmber)
unsigned int 		MouseRead(int  MousePort);									// Read Mouse Offset x and y
void		 		MouseReadTo(unsigned char MousePort, MOUSE_DATA *md); 		// Read Mouse Offset x and y, mouse button and return to the MOUSE_DATA Structure


/* --------------------------------------------------------- */
/* I/O port functions										 */
void 				OutPort( unsigned char port, unsigned char data);
unsigned char 		InPort( unsigned char port );
void 				OutPorts( unsigned char port, unsigned char *p_data, unsigned char count );


/* --------------------------------------------------------- */
/*	VDP functions	
*/
#define 			IsVsync()	((VDPstatusNi( 0 ) & 0x80) != 0)
#define 			IsHsync()	((VDPstatusNi( 1 ) & 0x01) != 0)
#define 			VpokeNext( data ) OutPort( 0x98, (data) )
#define 			VpeekNext() InPort( 0x98 )
void 				SetColors (unsigned char foreground, unsigned char background, unsigned char border); // Set Colors
void 				SetBorderColor(unsigned char BorderCol);					// Set Screen 's border color
void 				VDPwrite( unsigned char vdpreg, unsigned char data );		// write to VDP Register
unsigned char 		VDPstatus( unsigned char vdpreg );							// Read VDP Status Register
void 				VDPwriteNi( unsigned char vdpreg, unsigned char data );		// write to VDP Register 
unsigned char 		VDPstatusNi( unsigned char vdpreg );						// Read VDP Status Register
void 				VpokeFirst( unsigned int address );							// First adress for a vpoke. Use VpokeNext after
void 				Vpoke( unsigned int address, unsigned char data );
void 				VpeekFirst( unsigned int address );							// First adress for a vpeek. Use VpeekNext after
unsigned char 		Vpeek( unsigned int address );
void 				SetDisplayPage( unsigned char page );						// Set the Display Page
void 				SetActivePage( unsigned char page );						// Set the active Page
void 				SetScrollH(int n);											// Horizontal hardware scroll MSX2 	 MSX2+ and TR
void 				SetScrollV(int n);											// Vertical hardware scroll  MSX2+ and TR
void 				Width(char n);												// Screen Width in Text mode
void 				HideDisplay(void);											// Disable Video Display (Black Screen)
void 				ShowDisplay(void);											// Enable Video Display
void 				FillVram(int Startadress, char value, int length);			// Fill the Vram from startadress, with Value, for Length bytes
void 				PutText( int X, int Y,  char *str, char LogOp );			// Print Text string on Graphic screen
void				VDP50Hz(void);												// Switch MSX2 VDP to 50hz (PAL) Mode
void				VDP60Hz(void);												// Switch MSX2 VDP to 60Hz NTSC Mode
void 				VDPLinesSwitch(void);										// Switch MSX2 VDP to 192 / 212 vertical lines
void 				CopyRamToVram(void *SrcRamAddress, unsigned int DestVramAddress, unsigned int Length);
void 				CopyVramToRam(unsigned int SrcVramAddress, void *DestRamAddress, unsigned int Length);
unsigned char 		GetVramSize(void);


/* --------------------------------------------------------- */
/*	Keyboard Control  functions								 */
char         		GetKeyMatrix(char line);  									// Get a Key from Matrix keyboard (See at the end of file for desciption
unsigned char	    WaitKey( void );											// Wait for key-press, returns key code 
unsigned char		Inkey( void );												// Checks key-press, returns key code, otherwise 0 
void         		KillKeyBuffer(void);                   						// Erase key buffer
unsigned char       KeyboardRead( void );       								// verifies MSX keyboard status by ports 0xAA,0xA9 


/* --------------------------------------------------------- */
/*	String functions										 */
char				CharToLower	( char c );										// convert to lowercase 
char				CharToUpper	( char c );										// convert to uppercase 
void				StrCopy( char *dst, char *src ); 							// copy string from src to dst
void				NStrCopy( char *dst, char *src, int n ); 					// copy string from src to dst with no more than n characters 
void				StrConcat( char *dst, char *src ); 							// concatenate string from src to dst 
void				NStrConcat( char *dst, char *src, int n ); 					// concatenate string from src to dst with no more than n characters 
int					StrLen( char *adr ); 										//returns length of string
int					StrCompare( char *s1, char *s2 ); 							// compares two strings s1 and s2,  returns -1 (s1<s2), 0 (s1=s2), 1 (s1>s2)  	
int					NStrCompare( char *s1, char *s2, int n ); 					// compares two strings s1 and s2, no more than n characters,  returns -1 (s1<s2), 0 (s1=s2), 1 (s1>s2)
int					StrChr( char *adr, char c );  								// returns i, for which adr[i] = c, or -1 if not found
int					StrPosStr( char *s1, char *s2 ); 							// finds substring s2 in string s1 and returns position s1[i], returns -1 if not found 
int					StrSearch( char *s1, char *s2 ); 							// returns the the first occurrence in the string s1 of any character from the string s2, or -1 if not found 
int					StrPosChr( char *adr, char c ); 							// returns the last i, for which adr[i] = c, or -1 if not found
void				StrLeftTrim( char *adr ); 									// removes left spaces
void				StrRightTrim( char *adr ); 									// removes right spaces
void				StrReplaceChar( char *adr, char c, char nc );				// replaces all chars c to nc in string adr
char*				StrReverse(char *str);
char* 				Itoa(int num, char* str, int base);


/* --------------------------------------------------------- */
/*	Type  functions   										 */

int					IsAlphaNum	( char c );										// A-Za-z0-9 
int					IsAlpha	( char c );											// A-Za-z 
int					IsAscii	( char c );											// !..~ 
int					IsCntrl	( char c );											// unprintable control symbol 
int					IsDigit	( char c );											// 0..9 
int					IsGraph	( char c );											// has graphic representation 
int					IsLower	( char c );											// lowercase test 
int					IsPrintable	( char c );										// printable test 
int					IsPunctuation ( char c );									// punctuation sign test 
int					IsSpace	( char c );											// space test 
int					IsUpper	( char c );											// uppercase test 
int					IsHexDigit( char c );										// hex digit test 
char 				IsPositive(int c);											// Check if positive (return 1) or Negative (return -1)
float 				IntToFloat(int c);											// Return Float version of the int
void 				IntSwap(int *a, int *b);									// Swap two Int Variables


/* --------------------------------------------------------- */
/*	Memory  functions										 */
#define 			Poke( address, data )	( *( (volatile unsigned char*)(address) ) = ( (unsigned char)(data) ) )
#define 			Pokew( address, data )	( *( (volatile unsigned int*)(address) ) = ( (unsigned int)(data) ) )
#define 			Peek( address )			( *( (volatile unsigned char*)(address) ) )
#define 			Peekw( address )		( *( (volatile unsigned int*)(address) ) )
char				*MemChr( char *adr, char c, int n ); 						// returns pointer to char in n bytes of adr, or NULL if not found
void				MemFill( char *adr, char c, int n ); 						// FIll mem at *adr with n bytes of c 
void				MemCopy( char *dst, char *src, int n ); 					// copy n bytes from src to dst 
void				MemCopyReverse( char *dst, char *src, int n ); 				// copy n bytes from src to dst From end to the begining
int 				MemCompare( char *s1, char *s2, int n ); 					// compares n bytes of s1 and s2, returns -1 (s1<s2), 0 (s1=s2), 1 (s1>s2)
void   				*MMalloc(unsigned int size);								// Memory Allocation system Adapted for MSX
unsigned int 		ReadTPA(void);												// Read and return High TPA Adress
unsigned int 		ReadSP(void); 												// Read SP register. Return the adress of the stack


/* --------------------------------------------------------- */
/* Interrupt	functions										 */
#define				EnableInterrupt()	{ __asm ei;   __endasm; }				// Enable and Disable Interupt 
#define				DisableInterrupt()	{ __asm di;   __endasm; }
#define 			Halt()				{ __asm halt; __endasm; }
void 				Suspend( void);
void 				InitInterruptHandler(void);
void 				EndInterruptHandler(void);
void 				SetInterruptHandler(char (*p_handler)( void ));

/* --------------------------------------------------------- */
/*	PSG functions											 */
unsigned char 		PSGRead( unsigned char psgreg );							// Read a PSG Register
void 				PSGwrite( unsigned char psgreg, unsigned char data);		// Write to a PSG Register
void 				InitPSG(void);												// init the PSG to all 0


/* --------------------------------------------------------- */
/*	File functions											 */
typedef struct {
	unsigned char	drive_no;
	unsigned char	name[8];
	unsigned char	ext[3];
	unsigned int	current_block;
	unsigned int	record_size;
	unsigned long	file_size;
	unsigned int	date;
	unsigned int	time;
	unsigned char	device_id;
	unsigned char	directory_location;
	unsigned int	start_cluster_no;
	unsigned int	last_access_cluster_no;
	unsigned int	cluster_offset;
	unsigned char	current_record;
	unsigned long	random_record;
} FCB;

typedef struct {
	unsigned char	name[8];
	unsigned char	ext[3];
	unsigned char	attribute;
	unsigned char	undel_char;			/* MSX-DOS2 */
	unsigned char	reserve[9];
	unsigned int	time;
	unsigned int	date;
	unsigned int	start_cluster_no;
	unsigned long	file_size;
} FCB_DIR;

typedef struct {
	unsigned char	drive_no;
	FCB_DIR			dirinfo;
} FCB_FIND;

unsigned char 		fcb_open( FCB *p_fcb );
unsigned char 		fcb_create( FCB *p_fcb );
unsigned char 		fcb_close( FCB *p_fcb );
unsigned int  		fcb_read( FCB *p_fcb, void *p_buffer, int size );
unsigned char 		fcb_write( FCB *p_fcb, const void *p_buffer, int size );
unsigned char 		fcb_find_first( FCB *p_fcb, FCB_FIND *p_result );
unsigned char 		fcb_find_next( FCB_FIND *p_result );


/* --------------------------------------------------------- */
/*	MSX DOS functions										 */

typedef struct {
	unsigned int af, bc, de, hl, ix, iy, Cf, Zf;	/* 2 registers 0..65535 */
} REGS;
/* Internal, get pointer by _REGs() function.
Set before calling intdos().
Check data after intdos() call, except ix,iy that are the same.
Cf=1 if carry flag set, Zf=1 if zero flag set */

typedef struct {
	int hour;	/* Hours 0..23 */
	int min;	/* Minutes 0..59 */
	int sec;	/* Seconds 0..59 */
} TIME;

typedef struct {
	int year;	/* Year 1980...2079 */
	int month;	/* Month 1=Jan..12=Dec */
	int day;	/* Day of the month 1...31 */
	int dow;	/* On getdate() gets Day of week 0=Sun...6=Sat */
} DATE;

void				GetDate (DATE *date);  										// get date 
void				GetTime (TIME *time);										// get time 
int					SetDate (int Year, char month, char day);  										// set date, returns 0 if valid 
int					SetTime (TIME *time);										// set time, returns 0 if valid 

REGS				*_REGs( void );												// this returns address to internal word registers 
void				IntDos(void);												// CALL 5. Call a DOS Function
void				IntBios(void);												// CALL 1Ch (RST 30h), set IX,IY. Call a Bios Function from MSXDOS
void 				DosCLS (void);  											// CLS for Dos
void 				Exit(char n);												// Proper Exit program, and come back to MSX DOS, sending  n as error code


/* --------------------------------------------------------- */
/* Turbo-R functions										 */
unsigned char 		GetCPU (void);												// Returns the CPU mode of the MSX TURBO-R   :  0=Z80 mode    1=R800 Rom Mode    2=R800 DRAM Mode
void 				ChangeCPU (char n);											// Set the CPU mode of the MSX TURBO-R
void 				PCMPlay(int start, int lenght); 							// Play a PCM sound stored in the VRAM of the MSX TURBO-R  start VRAM Adress of the begining of the PCM sound , lenght  of the PCM sample


/* =============================================================================
   GetKeyMatrix
   Version: 1.0
   Date: 2 March 2016
   Author: mvac7/303bcn

   Function : Returns the value of the specified line from the keyboard matrix.
              Each line provides the status of 8 keys.
              To know which keys correspond, you will need documentation that 
              includes a keyboard table.
   Input    : [char] line 
   Output   : [char] state of the keys. 1 = not pressed; 0 = pressed

   International
  
bit  7     6     5     4     3     2     1     0
----------------------------------------------------
  0  7 &   6 ^   5 %   4 $   3 #   2 @   1 !   0 )
  1  ; :   ] }   [ {   \ |   = +   - _   9 (   8 *
  2  B     A     acent / ?   . >   , <   `     ' "
  3  J     I     H     G     F     E     D     C
  4  R     Q     P     O     N     M     L     K
  5  Z     Y     X     W     V     U     T     S
  6  F3    F2    F1    CODE  CAPS  GRAPH CTRL  SHIFT
  7  RET   SEL   BS    STOP  TAB   ESC   F5    F4
  8  Right Down  Up    Left  DEL   INS   HOME  SPACE
  9  NUM4  NUM3  NUM2  NUM1  NUM0  NUM/  NUM+  NUM*
 10  NUM.  NUM,  NUM-  NUM9  NUM8  NUM7  NUM6  NUM5
============================================================================= */

/* =============================================================================
JoystickRead
 Returns the joystick status
 
 Input    : [char] joy port: 
                  0=cursor keys 
                  1=joystick port 1 
                  2=joystick port 2
 Return   : [char]
                  0=inactive
                  1=up
                  2=up & right
                  3=right
                  4=down & right
                  5=down
                  6=down & left
                  7=left
                  8=up & left 
============================================================================= */

/* =============================================================================
 triggerRead
 Returns current trigger status
            
 Input    : [char] 
                    0 - space key
                    1 - button 1 joystick port A
                    2 - button 1 joystick port B
                    3 - button 2 joystick port A
                    4 - button 2 joystick port B
                    
 return   : [char] status:
                    0 = inactive
                    #FF = when the speified trigger is pressed down 
============================================================================= */

/* --------------------------------------------------------- */
/* FUSION-C Version tag									 	 */
static const unsigned char Done_Version[]="Made with FUSION-C 1.2 (ebsoft)";
#endif
