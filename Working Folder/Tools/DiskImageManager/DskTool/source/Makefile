CC=gcc
#CC=i686-w64-mingw32-g++
#STATIC=-static
CCFLAGS=$(STATIC) -Wall -O3 
OUT=dsktool

default: dsktool

all: clean default

dsktool.o: dsktool.c msxboot.h
	$(CC) -c dsktool.c

dsktool: dsktool.o
	$(CC) dsktool.o -o $(OUT) $(CCFLAGS)
	strip $(OUT)

clean:
	rm -f *.o dsktool
