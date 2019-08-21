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
|              Eric Boez &  Fernando Garcia                 |
|                                                           |
| rammapper.h : Use MSXDOS2 Memory mapper  functions        |
|                                                           |
|                                                           |
|                                                           |
\___________________________________________________________/
*                         (°□°)
*  Original code from :Fernando Garcia - Bit Vision 2018
* 
*/


/*
info at
http://map.grauw.nl/resources/dos2_environment.php#c5

address			function
+0		Slot address of the mapper slot.
+1		Total number of 16k RAM segments. 1...255 (8...255 for the primary)
+2		Number of free 16k RAM segments.
+3		Number of 16k RAM segments allocated to the system (at least 6 for the primay)
+4		Number of 16k RAM segments allocated to the user.
+5...+7		Unused. Always zero.
+8...		Entries for other mapper slots. If there is none, +8 will be zero.

*/


#ifndef  __MEMORY_MAPPER_H__
#define  __MEMORY_MAPPER_H__

typedef struct {
	unsigned char slot;
	unsigned char number16KBSegments;
	unsigned char numberFree16KBSegments;
	unsigned char numberAllocatedSystem16KBSegments;
	unsigned char numberUser16KBSegments;
	unsigned char notInUse0;
	unsigned char notInUse1;	
	unsigned char notInUse2;	
} MAPPERINFOBLOCK;


typedef struct {
	unsigned char allocatedSegmentNumber;
	unsigned char slotAddressOfMapper;
	unsigned char carryFlag;			//0->off 1->on
} SEGMENTSTATUS;


/* Devices Ids at https://www.msx.org/forum/msx-talk/software-and-gaming/extbio-device-identifiers */
/*;parm 
;	unsigned char deviceId
;								;devices list 
								;
								;00h  reserved for broadcasts
								;04h  DOS2 memory routines
								;08h  RS232
								;0Ah  MSX-AUDIO
								;11h  Kanji
								;22h  UNAPI (konamiman)
								;4Dh  Memman
								;FFh  System exclusive
								;		
;usually 04h
*/
extern void					InitRamMapperInfo( unsigned char deviceId );

extern MAPPERINFOBLOCK		*_GetRamMapperBaseTable( void );

/*
 * segmentType = A
 * slotAddress = B
;		   Parameters:   A=0 => allocate user segment
;                        A=1 => allocate system segment
;                        B=0 => allocate primary mapper
;                        B!=0 => allocate 
;                        FxxxSSPP slot address (primary mapper, if 0)
;                        xxx=000 allocate specified slot only
;                        xxx=001 allocate other slots than specified
;                        xxx=010 try to allocate specified slot and, if it failed, try another slot (if any)
;                        xxx=011 try to allocate other slots than specified and, if it failed, try specified slot
;		   Results:      Carry set => no free segments
;                        Carry clear => segment allocated
;                                       A=new segment number
;                                       B=slot address of mapper slot (0 if called as B=0)
*/
extern SEGMENTSTATUS		*AllocateSegment( unsigned char segmentType, unsigned char slotAddress );


/*
 * segmentType = A
 * slotAddress = B
;		   Parameters:   A=segment number to free
;                        B=0 primary mapper
;                        B!=0 mapper other than primary
;          Returns:      Carry set => error
;                        Carry clear => segment freed OK 
*/
extern SEGMENTSTATUS		*FreeSegment( unsigned char segmentType, unsigned char slotAddress );

extern unsigned char		Get_PN ( unsigned char page );

extern void					Put_PN ( unsigned char page, unsigned char segment);

#endif
