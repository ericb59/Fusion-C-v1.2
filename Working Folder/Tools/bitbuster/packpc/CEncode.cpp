

#include <string>
#include "CEncode.h"


using std::cout;
using std::cerr;
using std::ios;
using std::ofstream;


//write byte to file
void CEncode::WriteByte(unsigned char value)
{
	outfile.write(reinterpret_cast<const char*>(&value),1);
}


void CEncode::WriteBit(int value)
{
	//if 8 bits has been added
	if (bitcount == 8)
	{	//get current file position
		int file_position = outfile.tellp();

		//back to position where bitdata should be inserted
		outfile.seekp(bitstream_position, ios::beg);

		//write bitdata
		outfile.write(reinterpret_cast<const char*>(&bitdata), 1);

		//back to end of file
		outfile.seekp(file_position, ios::beg);

		//remember this as next bitdata position
		bitstream_position = file_position;

		//write bogus data
		outfile.write(reinterpret_cast<const char*>(&bitdata), 1);

		//clear number of bits added
		bitcount = 0;
	}

	//shift value
	bitdata <<= 1;		

	//set bit if value is non-zero
	if (value)
		bitdata ++;		

	//increase number of bits added to value
	bitcount++;			
}


void CEncode::WriteBits(int value, int bits)
{
	//loop through all bits
	for (int c = 0; c < bits; c++)
	{
		//write one bit
		WriteBit(value & 1);
	}
}


int CEncode::GetGammaSize(int value)
{
int gamma_size = 1;

	//increase size if there's still bits left after shifting one bit out
	while (value)
	{
		value--;

		gamma_size+=2;	//each time 2 extra bits are needed

		value>>=1;
	}

	//return calculated gamma size
	return gamma_size;
}


//0		: 0
//1/2	: 10x
//3-6	: 110xx  (00=3, 01=4, 10=5, 11=6)
//7-14	: 1110xxx (000=7,001=8,010=9,011=10,100=11,101=12,110=13,111=14)
void CEncode::WriteEliasGamma(int value)
{
int bit_size = GetGammaSize(value);
int bit_mask = 1;

	//calculate number of bits needed to encode 
	bit_size--;
	bit_size/=2;

	//write bits to determine bitlength
	for (int c = 0; c < bit_size; c++)
	{
		WriteBit(1);

		bit_mask <<= 1;
	}

	//output bitlength terminator
	WriteBit(0);

	value++;

	bit_mask>>=1;

	//write bits to encode value
	for (int b = 0; b < bit_size; b++)
	{
		WriteBit(value & bit_mask);

		bit_mask>>=1;
	}
}


//init the encoder
int CEncode::InitEncode(char *filename, unsigned char *data, int length)
{
	//copy data pointer
	this->data = data;

	//create new file
	outfile.open(filename, ios::out|ios::binary);

	if (outfile.fail())
		return 1;

	//write length of original data to output file
	outfile.write((char*)&length, 4);

	//get current position of file
	bitstream_position = outfile.tellp();

	//write bogus value which will be overwritten by valid data later on
	outfile.write((char*)&length, 1);

	//reset bitstream data
	bitdata = 0;

	//reset number of bits added to bitstream
	bitcount = 0;

	return 0;
}


//encode more data from received node
void CEncode::Encode(CNode *best)
{
CNode* nodes[16];
int node_count;
int bit_mask;

	//no nodes counted get
	node_count = 0;

	//get parent node of best node (=last node to output)
	best = best->GetParent();

	//keep adding nodes to the list until the top has been reached
	while( best->GetType() != NODE_TYPE_TOP )
	{
		//add node to the list
		nodes[node_count++] = best;

		//get parent of this node
		best = best->GetParent();
	}

	//keep looping until out of nodes
	while (node_count--)
	{
		//literal node?
		if (nodes[node_count]->GetType() == NODE_TYPE_LITERAL)
		{
			//write encoding type
			WriteBit(0);

			//write literal byte
			WriteByte(nodes[node_count]->GetLiteralValue());
		}

		//match node?
		if (nodes[node_count]->GetType() == NODE_TYPE_MATCH)
		{
			//get offset of match
			int offset = nodes[node_count]->GetMatchOffset() - 1;

			//get length of match
			int length = nodes[node_count]->GetMatchLength() - 2;

			//write encoding type
			WriteBit(1);

			//offset > 127 uses 12 bits for offset
			if (offset > 127)
			{
				//write 7 bits of offset + extend mark
				WriteByte(offset | 128);

				//get rid of lowest 7 bits
				offset >>= 7;

				//4 bits left 
				bit_mask = 8;

				//output 4 bits
				for (int c = 0; c < 4; c++)
				{
					//write remaining bits
					WriteBit(offset & bit_mask);

					//shift bitmask to output correct bit
					bit_mask>>=1;
				}
			}
			else
				WriteByte(offset);	//write 7 bits for offset plus no extending mark

			//write length as elias gamma coode
			WriteEliasGamma(length);
		}
	}
}


void CEncode::EndDecode()
{
	//lz77/rle mark
	WriteBit(1);		

	//offset is zero -> end of file mark will be checked by RLE decompressing
	WriteByte(0);
	
	//set 16 bits
	WriteBits(1,16);
	
	//gamma bit length terminator
	WriteBit(0);		

	//if there are bits left in the bitstream
	if (bitcount)		
	{	
		//get file position
		int file_position = outfile.tellp();	

		//go to bitstream data position
		outfile.seekp(bitstream_position, ios::beg);

		//set bits correctly
		bitdata <<= (8 - bitcount);

		//write byte
		outfile.write(reinterpret_cast<const char*>(&bitdata), 1);

		//filepointer back to original position
		outfile.seekp(file_position, ios::beg);
	}

	//close decompressed file
	outfile.close();
}

