

#include "CNode.h"


//constructor
CNode::CNode()
{
	//make sure node pointers point to nothing
	literal = NULL;
	match = NULL;
}


//destructor
CNode::~CNode()
{

}


//initialise node
void CNode::Init(CNode *parent, int type, int cost, int position)
{
	//initialise variables in node
	this->parent = parent;
	this->type = type;
	this->cost = cost;
	this->position = position;
}


//set match offset and length
void CNode::InitMatch(int offset, int length)
{
	this->offset = offset;
	this->length = length;
}


//set literal value
void CNode::InitLiteral(unsigned char literal)
{
	lit = literal;
}


