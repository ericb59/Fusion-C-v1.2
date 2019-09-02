
@echo off
echo -------- Compilation of : 
echo %1
echo .
SET HEX2BINDIR=
SET HEX2BIN=hex2bin.exe 
SET ASM=sdasz80 
SET CC=sdcc 
SET DEST=dsk\

SET INCLUDEDIR=fusion-c\include\
SET LIBDIR=fusion-c\lib\

SET proga=%1


SET INC1=%INCLUDEDIR%crt0_msxdos.rel
REM SET INC2=%INCLUDEDIR
REM SET INC3=%INCLUDEDIR
REM SET INC4=%INCLUDEDIR%
REM SET INC5=%INCLUDEDIR%
REM SET INC6=%INCLUDEDIR%
REM SET INC7=%INCLUDEDIR%
REM SET INC8=%INCLUDEDIR%
REM SET INC9=%INCLUDEDIR%
REM SET INCA=%INCLUDEDIR%
REM SET INCB=%INCLUDEDIR%
REM SET INCC=%INCLUDEDIR%
REM SET INCD=%INCLUDEDIR%
REM SET INCE=%INCLUDEDIR%
REM SET INCF=%INCLUDEDIR%

SET ADDR_CODE=0x106
SET ADDR_DATA=0x0



SDCC --code-loc %ADDR_CODE% --data-loc %ADDR_DATA% --disable-warning 196 -mz80 --no-std-crt0 --opt-code-size fusion.lib -L %LIBDIR% %INC1% %INC2% %INC3% %INC4% %INC5% %INC6% %INC7% %INC8% %INC9% %INCA% %INCB% %INCC% %INCD% %INCE% %INCF% %proga%.c



SET cpath=%~dp0


IF NOT EXIST %proga%.ihx GOTO _end_
echo ... Compilation OK
@echo on

hex2bin -e com %proga%.ihx

@echo off

copy %proga%.com DSK\%proga%.com /y

del %proga%.com
del %proga%.asm
del %proga%.ihx
del %proga%.lk
del %proga%.lst
del %proga%.map
del %proga%.noi
del %proga%.sym
del %proga%.rel

echo Done.

:Emulator
Set MyProcess=openmsx.exe
tasklist | find /i "%MyProcess%">nul  && (echo %MyProcess% Already running) || start openMSX\openmsx.exe -script openMSX\emul_start_config.txt

:_end_