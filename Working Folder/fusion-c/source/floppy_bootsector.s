;---------------------------------------------
	;Dynamic MSXDOS 1 BootSector From Fast DiskROM
	;Version 0.2
	;ChangeLog:
	; - Added Disable Screen at start
	;---------------------------------------------
	
	output bootsector.bin




start:
				ret nc
				ld     (diskrom_addr),de			;DE = F368 C9 ROMUSE Switch DISKROM to page 1 (DOS only)
				ld     (temp),a					;Backup A Register
				
				ld     de,err_handler
				ld     (hl),e
				inc    hl
				ld     (hl),d


				; Disable Screen
				
                		ld ix,#0041					;Disable Screen BIOS Call
				ld iy,(#fcc0)					;Load BIOS location
				call #1c					;Inter Slot Call
                
boot:
				ld     sp,#f51f
				ld     de,fcb_msxdosys				;FCB Pointer to MSXDOS.SYS
				ld     c,#0f					;Open File
				call   #f37d
				inc    a					;A=00 Read OK | A=FF Read Error
				jp     z,boot_error				;Boot Error ?
				ld     de,#0100					;Set Destination for load
				ld     c,#1a					;Set DMA
				call   #f37d
				ld     hl,#0001
				ld     (temp_space),hl				;Write 0100 to Tempspace ?
				ld     hl,#3f00					;Number of records to load
				ld     de,fcb_msxdosys				;FCB Pointer to MSXDOS.SYS
				ld     c,#27					;Random Read
				call   #f37d
				jp     #0100					;Start MSXDOS
				
				
				; Error handling
err_handler:
				ld     e,b
				ret    nz
				db     #cd					;call   #0000	;Call Error Handler
diskrom_addr:
				db     00,00
				
				ld     a,c
				and    #fe
				cp     #02
				jp     nz,print_err_msg
boot_error:
				ld     a,(temp)					;Restore A Register
				and    a
				jp     z,#4022					;Jump to Basic
print_err_msg:
				ld     de,error_msg
				ld     c,#09					;Print string to screen
				call   #f37d
				ld     c,#07					;Get one character from console (Press any Key for retry)
				call   #f37d
				jr     boot					;Try booting again
fcb_msxdosys:				
				db     00,"MSXDOS  SYS"
				db     00,00
temp_space:
				db     00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00

error_msg:
				db     "Boot error",#0d,#0a
				db     "Press any key for retry",#0d,#0a,"$"
temp:
				db     00,00