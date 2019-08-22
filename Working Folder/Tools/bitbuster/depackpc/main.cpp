
//Bitbuster
//decompressor


#include "CDecompress.h"

using std::cout;
using std::endl;


int main(int argc, char *argv[])
{

CDecompress decompressor;

	//supply two filename to be able to compress something
	if ( argc < 3)
	{
		cout << "Nothing to compress!!" << endl;
		cout << "Usage:" << endl;
		cout << "depack inputfile outputfile"  << endl;

		return 0;
	}

	decompressor.Start(argv[1], argv[2]);

	return 0;
}

