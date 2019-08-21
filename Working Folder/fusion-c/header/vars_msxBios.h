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
|                   V1.0 - 09-10-11 2018                    |
|                                                           |
|                Eric Boez &  Fernando Garcia               |
|                                                           |
|   vars_msxBios.h : List of MSX Bios routines ans call     |
|    For information or for use in your own programs        |
|                                                           |
|                                                           |
\___________________________________________________________/
*                         (°□°)
*	
* MSX BIOS v1.2
*  definitions & descriptions by MSX Assembly Page
*  For details see the WEB document at:  http://map.grauw.nl/resources/msxbios.php
*/


#ifndef  __MSXBIOS12_H__
#define  __MSXBIOS12_H__



// use RST assembler mnemonic to call
#define CHKRAM  0x00 // RST 0x00 > Check RAM and sets slot for command area.
#define SYNCHR  0x08 // RST	0x08 > Checks if then current character pointed by HL is one desired.
#define CHRGTR  0x10 // RST	0x10 > Gets the next character (or token) of the Basic-text
#define OUTDO   0x18 // RST	0x18 > Output to current outputchannel (printer, diskfile, etc.)
#define DCOMPR  0x20 // RST	0x20 > Compares HL with DE
#define GETYPR  0x28 // RST	0x28 > Returns Type of DAC
#define CALLF   0x30 // RST	0x30 > Executes an interslot call
#define KEYINT  0x38 // RST	0x38 > Executes the timer interrupt process routine

// use CALL assembler mnemonic
#define RDSLT   0x000C // Reads the value of an address in another slot
#define WRSLT   0x0014 // Writes a value to an address in another slot
#define CALSLT  0x001C // Executes inter-slot call
#define ENASLT  0x0024 // Switches indicated slot at indicated page on perpetual

// Initialization-routines
#define INITIO  0x003B // Initialises the device
#define INIFNK  0x003E // Initialises the contents of the function keys

// VDP routines
#define DISSCR  0x0041 // inhibits the screen display
#define ENASCR  0x0044 // displays the screen
#define WRTVDP  0x0047 // write data in the VDP-register
#define RDVRM   0x004A // Reads the content of VRAM
#define WRTVRM  0x004D // Writes data in VRAM
#define SETRD   0x0050 // Enable VDP to read
#define SETWRT  0x0053 // Enable VDP to write
#define FILVRM  0x0056 // fill VRAM with value
#define LDIRMV  0x0059 // Block transfer to memory from VRAM
#define LDIRVM  0x005C // Block transfer to VRAM from memory
#define CHGMOD  0x005F // Switches to given screenmode
#define CHGCLR  0x0062 // Changes the screencolors
#define CLRSPR  0x0069 // Initialises all sprites
#define INITXT  0x006C // Switches to SCREEN 0 (text screen with 40 * 24 characters)
#define INIT32  0x006F // Switches to SCREEN 1 (text screen with 32*24 characters)
#define INIGRP  0x0072 // Switches to SCREEN 2 (high resolution screen with 256*192 pixels)
#define INIMLT  0x0075 // Switches to SCREEN 3 (multi-color screen 64*48 pixels)
#define SETTXT  0x0078 // Switches to VDP in SCREEN 0 mode
#define SETT32  0x007B // Switches VDP in SCREEN mode 1
#define SETGRP  0x007E // Switches VDP to SCREEN 2 mode
#define SETMLT  0x0081 // Switches VDP to SCREEN 3 mode
#define CALPAT  0x0084 // Returns the address of the sprite pattern table
#define CALATR  0x0087 // Returns the address of the sprite attribute table
#define GSPSIZ  0x008A // Returns current sprite size
#define GRPPRT  0x008D // Displays a character on the graphic screen
        
// PSG routines
#define GICINI  0x0090 // Initialises PSG and sets initial value for the PLAY statement
#define WRTPSG  0x0093 // Writes data to PSG-register
#define RDPSG   0x0096 // Reads value from PSG-register
#define STRTMS  0x0099 /* Tests whether the PLAY statement is being executed as 
             a background task. If not, begins to execute the PLAY statement  */

// Console routines
#define CHSNS   0x009C // Tests the status of the keyboard buffer
#define CHGET   0x009F // One character input (waiting)
#define CHPUT   0x00A2 // Displays one character
#define LPTOUT  0x00A5 // Sends one character to printer
#define LPTSTT  0x00A8 // Tests printer status
#define CNVCHR  0x00AB // tests for the graphic header and transforms the code
#define PINLIN  0x00AE /* Stores in the specified buffer the character codes  
                        input until the return key or STOP key is pressed     */
#define INLIN   0x00B1 // Same as PINLIN except that AUGFLG (#F6AA) is set
#define QINLIN  0x00B4 // Prints a questionmark andone space
#define BREAKX  0x00B7 // Tests status of CTRL-STOP 
#define ISCNTC  0x00BA // Tests status of SHIFT-STOP 
#define CKCNTC  0x00BD // Same as ISCNTC. used in Basic
#define BEEP    0x00C0 // generates beep
#define BCLS    0x00C3 // Clears the screen
#define POSIT   0x00C6 // Places the cursor at the specified location
#define FNKSB   0x00C9 /* Tests whether the function key display is active (FNKFLG)
                          If so, displays them, otherwise erase them */
#define ERAFNK  0x00CC // Erase functionkey display
#define DSPFNK  0x00CF // Displays the function keys
#define TOTEXT  0x00D2 // Forces the screen to be in the text mode

// Controller routines                      
#define GTSTCK  0x00D5 // Returns the joystick status
#define GTTRIG  0x00D8 // Returns current trigger status
#define GTPAD   0x00DB // Returns current touch pad status
#define GTPDL   0x00DE // Returns currenct value of paddle

// Tape device routines                      
#define TAPION  0x00E1 // Reads the header block after turning the cassette motor on
#define TAPIN   0x00E4 // Read data from the tape
#define TAPIOF  0x00E7 // Stops reading from the tape
#define TAPOON  0x00EA // Turns on the cassette motor and writes the header
#define TAPOUT  0x00ED // Writes data on the tape
#define TAPOOF  0x00F0 // Stops writing on the tape
#define STMOTR  0x00F3 // Sets the cassette motor action

// Queue routines                      
#define LFTQ    0x00F6 // Gives number of bytes in queue
#define PUTQ    0x00F9 // Put byte in queue

// Graphic routines
//More info (MSX Assembly Page): http://map.grauw.nl/resources/msxbios.php                      
#define RIGHTC  0x00FC // Shifts screenpixel to the right
#define LEFTC   0x00FF // Shifts screenpixel to the left
#define UPC     0x0102 // Shifts screenpixel up
#define TUPC    0x0105 // Tests whether UPC is possible, if possible, execute UPC
#define DOWNC   0x0108 // Shifts screenpixel down
#define TDOWNC  0x010B // Tests whether DOWNC is possible, if possible, execute DOWNC
#define SCALXY  0x010E // Scales X and Y coordinates
#define MAPXY   0x0111 // Places cursor at current cursor address
#define FETCHC  0x0114 // Gets current cursor addresses mask pattern
#define STOREC  0x0117 // Record current cursor addresses mask pattern
#define SETATR  0x011A // Set attribute byte
#define READC   0x011E // Reads attribute byte of current screenpixel
#define SETC    0x0120 // Returns currenct screenpixel of specificed attribute byte
#define NSETCX  0x0123 // Set horizontal screenpixels
#define GTASPC  0x0126 // Gets screen relations
#define PNTINI  0x0129 // Initalises the PAINT instruction
#define SCANR   0x012C // Scans screenpixels to the right
#define SCANL   0x012F // Scans screenpixels to the left

// Graphic routines MSX2 
//More info (MSX Assembly Page): http://map.grauw.nl/resources/msxbios.php#msx2bios
#define CHKNEW  0x0165 //Tests screen mode > C-flag set if screenmode = 5, 6, 7 or 8
#define BIGFIL  0x016B //fill VRAM with value (total VRAM can be reached) HL address, BC length, A data
#define NSETRD  0x016E //Enable VDP to read.(with full 16 bits VRAM-address)
#define NSTWRT  0x0171 //Enable VDP to write.(with full 16 bits VRAM-address) 
#define NRDVRM  0x0174 //Reads VRAM like in RDVRM.(with full 16 bits VRAM-address)
#define NWRVRM  0x0177 //Writes to VRAM like in WRTVRM.(with full 16 bits VRAM-address)

                     
// Misc routines
#define CHGCAP  0x0132 // Alternates the CAP lamp status
#define CHGSND  0x0135 // Alternates the 1-bit sound port status
#define RSLREG  0x0138 // Reads the primary slot register
#define WSLREG  0x013B // Writes value to the primary slot register
#define RDVDP   0x013E // Reads VDP status register
#define SNSMAT  0x0141 // Returns the value of the specified line from the keyboard matrix
#define PHYDIO  0x0144 // Executes I/O for mass-storage media like diskettes
#define FORMAT  0x0147 // Initialises mass-storage media like formatting of diskettes
#define ISFLIO  0x014A // Tests if I/O to device is taking place
#define OUTDLP  0x014E // Printer output
#define GETVCP  0x0150 // Returns pointer to play queue
#define GETVC2  0x0153 // Returns pointer to variable in queue number VOICEN (byte op #FB38)
#define KILBUF  0x0156 // Clear keyboard buffer
#define CALBAS  0x0159 // Executes inter-slot call to the routine in BASIC interpreter
        
#endif