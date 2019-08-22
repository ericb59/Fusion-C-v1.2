



#include "CDecompress.h"

#include <string>

using std::ifstream;
using std::ios;
using std::cout;
using std::endl;




//return length of opened file
int CDecompress::GetFileLength(ifstream & file)
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


//read the complete compressed data
int CDecompress::ReadFile(char *filename)
{
ifstream infile;

	//try to open file
	infile.open(filename, ios::in | ios::binary);

	//if file doesn't exist, we can't compress it...
	if (infile.fail())
	{
		cout << "Unable to open file";

		return -1;
	}

	//get filelength
	length = GetFileLength(infile) - 4;

	//allocate space to store file (some extra room for end of file mark)
	data = new unsigned char[length + 4];

	//read original length
	infile.read(reinterpret_cast<char*>(data), 4);

	//get original length of file
	original_length = *(int*)data;

	//print length original data
	cout << "Length original data: " << original_length << endl;

	//read compressed data
	infile.read(reinterpret_cast<char*>(data), length);

	//file can be closed
	infile.close();

	return 0;
}


//read a bit from the bitstream
int CDecompress::ReadBit()
{
int value;

	//if 8 bit reads, time to get 8 new bits
	if (bit_count == 8)
	{
		//get 8 new bits
		bit_data = data[position++];

		//reset number of bits read
		bit_count = 0;
	}

	//increase number of bits read
	bit_count++;

	//get bit 7
	value = (bit_data & 128) >> 7;

	//shift bits
	bit_data <<= 1;

	return value;
}


//read byte from compressed data
unsigned char CDecompress::ReadByte()
{
	//read data and increase position in data
	return data[position++];
}


//get a gamma encoded value from the bitstream
int CDecompress::ReadGammaValue()
{
int value = 1;
int bit_count = 0;

	//determine number of bits used to encode gamma value
	while (ReadBit())
		bit_count++;

	//get more bits of gamma value while needed
	while (bit_count--)
	{
		//shift bits
		value <<= 1;

		//get new bit
		if (ReadBit())
			value++;
	}

	//correct gamma value
	value--;

	return value;
}


//decompress file
void CDecompress::Start(char *filename, char *outputname)
{
int source_position;
int output_position;
int offset;
int match_length;
int repeat_length;

	//try to read compressed data and quit if failed
	if (ReadFile(filename))
		return;

	//create enough room for original data
	output_data = new unsigned char[original_length];

	//initialise bitstream, next call to ReadBit will read 8 new bits from bitstream
	bit_count = 8;
	bit_data = 0;

	//set output position in decompressed data
	output_position = 0;

	//set position in compressed data
	position = 0;

	//decompresses while length decrompressed data is smaller than length original data
	while (output_position < original_length)
	{
		//get data type
		if (ReadBit())
		{	//1 = packed data
			offset = ReadByte();

			//rle type?
			if (offset == 0)
			{	//rle

				//get repeat length
				repeat_length = ReadGammaValue() + 2;

				memset(output_data + output_position, output_data[output_position-1], repeat_length); 

				output_position += repeat_length;
			}
			else
			{	//if offset extension mark set
				if (offset & 128)
				{	//offset uses 11 bits
					
					//clear extension mark
					offset &= 127;	

					//get fourm more bits
					offset += ReadBit() << 10;
					offset += ReadBit() << 9;
					offset += ReadBit() << 8;
					offset += ReadBit() << 7;
				}

				//calculate source position
				source_position = output_position - offset - 1;

				//get match length
				match_length = ReadGammaValue() + 2;

				//copy data 
				while(match_length--)
					output_data[output_position++] = output_data[source_position++];
			}
		}
		else
		{	//0 = literal byte
			output_data[output_position++] = ReadByte();
		}
	}

	//create new file
	outfile.open(outputname,ios::out | ios::binary);

	//output decompressed data to file
	outfile.write(reinterpret_cast<char*>(output_data), original_length);
	
	//close file
	outfile.close();

	//delete decompressed data
	delete output_data;

	//delete compressed data
	delete data;
}
