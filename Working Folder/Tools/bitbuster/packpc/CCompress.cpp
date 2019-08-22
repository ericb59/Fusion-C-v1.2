
#include <string.h>

#include "CCompress.h"
#include "CEncode.h"
#include "CMatch.h"
#include "CNode.h"


using std::cout;
using std::cerr;
using std::ifstream;
using std::filebuf;
using std::endl;
using std::ios;


//return length of opened file
int CCompress::GetFileLength(ifstream & file)
{
int current_position;
int length;

	//not opened file?
	if (file.fail())
		return -1;

	//get current file position
	current_position = file.tellg();

	//move to end of file
	file.seekg(0, ios::end);

	//get current file position (=length of file)
	length = file.tellg();

	//set file position back to original position
	file.seekg(current_position, ios::beg);

	//return found filelength
	return length;
}



int CCompress::ReadFile(char *filename)
{
ifstream infile;

	//try to open file
	infile.open(filename, ios::in | ios::binary);

	//if file doesn't exist, we can't compress it...
	if (infile.fail())
	{
		cout << "Unable to open file" << endl;

		return -1;
	}

	//get filelength
	length = GetFileLength(infile);

	//allocate space to store file
	data = new unsigned char[length];

	//read file
	infile.read(reinterpret_cast<char*>(data), length);

	//file can be closed
	infile.close();

	return 0;
}




int CCompress::Start(char *sourcefile)
{
CEncode encoder;
CMatch matcher;
CNode *best;
int position;
int total;
char *fname;

	//read file and quit if failed
	if (ReadFile(sourcefile))
		return -1;
	
	//create space for new filename
	fname = new char[strlen(sourcefile)+5];

	//copy old filename
	strcpy(fname, sourcefile);

	//concatenate extension
	strcat(fname, ".pck");

	//initialise encoder
	if (encoder.InitEncode(fname, data, length))
		return -1;

	//start crunching at start of data
	position = 0;

	//minimum size crunched file is 4 bytes + 26 bits
	total = 4 * 8 + 26;

	//initiale matcher
	matcher.Init(data, length);
	
	cout << "Crunching " << sourcefile << "..." << endl;

	//as long there's more data to compress
	while (position < length)
	{
		//find best series of matches from this position
		best = matcher.FindMatches(position);

		//encode the best series of matches
		encoder.Encode(best);

		//increase total cost 
		total += best->GetCost();

		//advance position in data
		position = best->GetPosition();

		//output 8 backspaces
		for (int c = 0; c < 8; c++)
			cout << (char)8;

		//output % done
		cout << (position * 100) / length << "% done";

		//make sure it's printed on screen
		cout.flush();
	}

	//compression summary
	cout << endl << "Original size:" << length << endl;
	cout << "Crunched size:" << (total / 8) + 1 << endl;

	//finish off encoding data
	encoder.EndDecode();

	//destroy data
	delete data;
	delete fname;
		
	return 0;
}
