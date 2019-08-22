
//Bitbuster 1.2
//Compressor

#include "CCompress.h"

#if defined(WIN32) && !defined(MINGW)
	#define USEWINFIND
#endif

#ifdef USEWINFIND
#include <string>
#include <stdio.h>
#include <io.h>
#endif
#include <time.h>

using std::cout;
using std::endl;

int main(int argc, char *argv[])
{
CCompress compressor;

#ifdef USEWINFIND
_finddata_t search_result;
long file_handle;
#endif
clock_t start,finish;

	//give an error message if no arguments supplied
	if (argc == 1)
	{
		cout << "Nothing to compress!!";

		return 0;
	}

	start = clock();



	for ( int argi = 1 ; argi < argc; ++argi)
	{
#ifdef USEWINFIND

		//search for first file matching argument
		file_handle = _findfirst(argv[argi], &search_result);
		
		//if no results
		if ( file_handle == -1L)
		{	//display message
			cout << "File " << argv[argi] << " not found!" << endl;
		}
		else
		{	
			do {
				//compress file if it's not a subdirectory
    			if ( !(search_result.attrib & _A_SUBDIR))
				{	//and file is not compressed already
					if (strstr(search_result.name, ".pck") == NULL)
					{
						if ( compressor.Start(search_result.name))
							return 1;
					}
				}
    			//search more files
			} while (_findnext(file_handle, &search_result) == 0);		
		}

		//release resources used by search
		_findclose(file_handle);

#else
		if ( compressor.Start(argv[argi]))
			return 1;
#endif
	}

	finish = clock();

	cout << "Time elapsed:" << (double)(finish - start) / CLOCKS_PER_SEC << endl;

	return 0;
}
