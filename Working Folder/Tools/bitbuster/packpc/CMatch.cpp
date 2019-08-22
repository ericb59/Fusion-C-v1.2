

#include <iostream>
using std::cout;
using std::cerr;

#include <string.h>

#include "CMatch.h"
#include "CNode.h"


const int max_match_depth = 8;

CNode node_pool[512];


//constructor
CMatch::CMatch()
{
	//get new top node
	top = &node_pool[0];

	//get new best node
	best = &node_pool[1];
}


//destructor
CMatch::~CMatch()
{

}


//return match-length of current data in past data
int CMatch::GetMatchLength(int current, int past)
{
int match_length;	
int current_start = current;	//remember current position to prevent string overlaps

	//no matches possible before start of data...
	if (past < 0)
		return 0;

	//start with no match
	match_length = 0;

	//keep comparing strings while (note that strings may be overlapping for extra compression!)
	while( (current < length) &&				//string doesn't get out of bounds
		   (data[current++] == data[past++]) )	//data is the same
	{
		match_length++;	//increase match length 
	}

	//return found match length
	return match_length;
}


//return number of bits needed to encode value in Elias Gamma Code
//0		: 0
//1/2	: 10x
//3-6	: 110xx
//7-14	: 1110xxx
int CMatch::GetGammaSize(int value)
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



void CMatch::GetNextMatch(CNode *parent)
{
CNode *node; 
int cost = parent->GetCost();
int position = parent->GetPosition();
int match_length, match_size;
int best_match_offset = 0;
int best_match_length = 0;
int best_match_size = 1024;

	//next level
	depth++;

	//stop compressing if depth is too high or end of file has been reached
	if ( (depth > max_match_depth) || (position == length) )
	{	//if no best node found yet
		if (best->GetType() == NODE_TYPE_TOP)
		{
			best->Init(parent, NODE_TYPE_OPTIMAL, parent->GetCost(), parent->GetPosition());
		}
		else
		{	//best node has already been defined

			//check if new is actually better than current best node
			if (parent->GetPosition() > best->GetPosition() )
			{	//if so, change best node
				best->Init(parent, NODE_TYPE_OPTIMAL, parent->GetCost(), parent->GetPosition());
			}
			else
			{	//maybe this node costs less than best node?
				if ( (parent->GetPosition() == best->GetPosition()) &&
					 (parent->GetCost() < best->GetCost()) )
				{	//change best node
					best->Init(parent, NODE_TYPE_OPTIMAL, parent->GetCost(), parent->GetPosition());
				}
			}
		}

		//previous level
		depth--;

		return;
	}

	//create new literal node
	node = &node_pool[current_node_index++];

	//init literal, costs 9 bits, advances 1 byte in data
	node->Init(parent, NODE_TYPE_LITERAL, cost + 9, position + 1);

	node->InitLiteral(data[position]);

	//add literal node to parent
	parent->SetLiteral(node);

	//start compression from new literal node
	GetNextMatch(node);

	//find best string match
	for (int match_offset = 1; match_offset < 2047; match_offset++)
	{
		//get match length from this position
		match_length = GetMatchLength(parent->GetPosition(), parent->GetPosition() - match_offset);

		//compression possible of length > 2
		if (match_length > 1)
		{
			//limit maximum length
			if (match_length > 65535)
				match_length = 65535;

			//new best match if current match is better than best match
			if ( match_length > best_match_length )
			{
				//use 8 or 12 bits for encoding offset
				if (match_offset < 128 + 1)
					match_size = GetGammaSize(match_length - 2) + 1 + 8;
				else
					match_size = GetGammaSize(match_length - 2) + 1 + 12;

				//change best match found
				best_match_offset = match_offset;
				best_match_length = match_length;
				best_match_size = match_size;
			}
		}
	}

	//found a match if length is at least 2
	if (best_match_length > 1)
	{
		//create new match node
		node = &node_pool[current_node_index++];	//new CNode();
		
		//initialise new match node
		node->Init(parent, NODE_TYPE_MATCH, cost + best_match_size, position + best_match_length);

		node->InitMatch(best_match_offset, best_match_length);

		//add match node to parent
		parent->SetMatch(node);

		//continue compressing from new point
		GetNextMatch(node);
	}

	//previous level
	depth--;
}


//init data
void CMatch::Init(unsigned char *data, int length)
{
	this->data = data;
	this->length = length;
}


//find best match from starting point
CNode* CMatch::FindMatches(int position)
{
	current_node_index = 2;

	//initialise top node
	top->Init(NULL, NODE_TYPE_TOP, 0, position);

	//initialise best node
	best->Init(NULL, NODE_TYPE_OPTIMAL, 0, 0);

	//current recursion depth is 0
	depth = 0;

	//find next match
	GetNextMatch(top);

	//return best match found
	return best;
}
