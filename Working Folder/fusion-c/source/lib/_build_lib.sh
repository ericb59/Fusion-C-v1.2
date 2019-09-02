#!/bin/sh
clear
echo "    "
echo "  F U S I O N - C   V 1 . 2"
echo "  T h e   U l t i m a t e   SDCC   L i b r a r y   f o r   M S X"
echo "  Eric Boez & Fernando Garcia 2018-2019 : Made for coders !"
echo "  (Library builder script)"
echo "_________________________________________________________________"
echo "    "
echo "    "
date 
echo "    "
echo "------------------------------------ FUSION-C" >> log.txt
date >> log.txt
echo "------------------------------------" >> log.txt
echo "Now Building FUSION-C Library..."

rm fusion.lib
#-----------------------------------------------Building ASM .s functions
echo "... Compiling ASM functions"

for file in *.s ; do
    echo $file;tput cuu1;tput el;
     
    sdasz80 -o $file >> log.txt
    err=$?
    if [ $err -gt 0 ];then
        echo "Error when compiling $file"
        exit 1
    fi
done

#----------------------------------------------- Building C .c functions
echo "... Compiling C functions"

for file in *.c ; do
    echo $file;tput cuu1;tput el;
    
    sdcc --use-stdout -mz80 -c $file >> log.txt
    err=$?
    if [ $err -gt 0 ];then
        echo "Error when compiling $file"
        exit 1
    fi
done

#----------------------------------------------- Building the Library by fusioning all functions
echo "... including functions to the library"

for file in *.rel ; do
    echo $file;tput cuu1;tput el;
    
    sdar -rc fusion.lib $file
    err=$?
    if [ $err -gt 0 ];then
        echo "Error $err when including $file file"
        exit 1
    fi
done

#----------------------------------------------- Cleaning folder, y removing temp files
echo "... cleaning folder"
rm *.rel
rm *.asm
rm *.lst
rm *.sym

#----------------------------------------------- Copying the fusion.lib file to another place
cp fusion.lib ../../lib/

echo " Done... Have a good day sir"

exit 0