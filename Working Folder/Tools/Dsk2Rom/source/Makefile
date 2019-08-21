.PHONY: all clean dsk2rom dsk2rom.h

ifneq ($(WINDIR),)
dsk2rom_e = dsk2rom.exe
bin2h_e   = bin2h.exe
else
dsk2rom_e = dsk2rom
bin2h_e   = bin2h
endif

CC = g++
CFLAGS = -O2 -Wall -fomit-frame-pointer

all: bin2h dsk2rom.h dsk2rom

bin2h: bin2h.c
	$(CC) $(CFLAGS) $^ -o $(bin2h_e)

pletter/pletter.o: pletter/pletter.cpp pletter/pletter.h

dsk2rom.h: bin2h
	$(shell ./${bin2h_e} dsk2rom dsk2rom.rom dsk2rom.h)

dsk2rom: dsk2rom.c dsk2rom.h pletter/pletter.o
	$(CC) $(CFLAGS) $^ -o $(dsk2rom_e)

clean:
	rm -f $(dsk2rom_e)
	rm -f $(bin2h_e)
	rm -f dsk2rom.h
	rm -f */*.o
