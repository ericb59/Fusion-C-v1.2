

#ifndef CCOMPRESS_H
#define CCOMPRESS_H


#include <iostream>
#include <fstream>


class CCompress
{
private:
	unsigned char *data;
	int length;

public:
	int	GetFileLength(std::ifstream & file);
	int ReadFile(char *filename);
	int Start(char *sourcefile);
};





#endif

