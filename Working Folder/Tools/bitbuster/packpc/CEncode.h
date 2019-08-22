


#ifndef CENCODE_H
#define CENCODE_H


#include <iostream>
#include <fstream>


#include "CNode.h"




class CEncode
{
private:
	std::ofstream outfile;
	int bitstream_position;
	unsigned char bitdata;
	int bitcount;
	unsigned char *data;

public:
	int InitEncode(char *filename, unsigned char *data, int length);
	void Encode(CNode *best);
	void EndDecode();
	void WriteByte(unsigned char value);
	void WriteBit(int value);
	void WriteBits(int value, int bits);
	int GetGammaSize(int value);
	void WriteEliasGamma(int value);
};





#endif
