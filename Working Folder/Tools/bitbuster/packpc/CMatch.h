


#ifndef CMATCH_H
#define CMATCH_H


#include "CNode.h"



class CMatch
{
private:
	int depth;
	int length;
	unsigned char* data;
	CNode *top;
	CNode *best;

	int current_node_index;
public:
	CMatch();
	~CMatch();

	int GetMatchLength(int current, int past);
	int GetGammaSize(int value);
	void GetNextMatch(CNode *node);
	void Init(unsigned char *data, int length);
	CNode *FindMatches(int position);
};








#endif
