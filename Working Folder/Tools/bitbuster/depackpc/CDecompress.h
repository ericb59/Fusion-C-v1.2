


#ifndef CDECOMPRESS_H
#define CDECOMPRESS_H


#include <iostream>
#include <fstream>


class CDecompress
{
private:
	std::ofstream outfile;

	unsigned char *data;
	int length;

	unsigned char *output_data;
	int original_length;

	int position;
	unsigned char bit_data;
	int bit_count;

public:
	int	GetFileLength(std::ifstream & file);
	int ReadFile(char *filename);

	int ReadBit();
	unsigned char ReadByte();
	int ReadGammaValue();

	void Start(char *filename, char *outputname);

};


#endif
