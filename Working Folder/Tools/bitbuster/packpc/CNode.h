

#ifndef CNODE_H
#define CNODE_H


#define	NODE_TYPE_TOP		1
#define	NODE_TYPE_LITERAL	2
#define NODE_TYPE_MATCH		3
#define	NODE_TYPE_REPEAT	4
#define NODE_TYPE_OPTIMAL	5


#ifndef NULL
#define NULL 0
#endif


class CNode;


class CNode
{
private:
	int type;		//node type
	int cost;		//cost in number of bits to get here
	int position;	//number of bytes encoded

	unsigned char lit;	//literal value
	int offset;
	int length;

	CNode *parent;	//pointer to parent node
	CNode *literal;	//pointer to next literal node
	CNode *match;	//pointer to next match node
public:
	CNode();
	~CNode();

	void Init(CNode *parent, int type, int cost, int position);	//initialise node

	void InitLiteral(unsigned char literal);
	void InitMatch(int offset, int length);

	unsigned char GetLiteralValue() {return this->lit;}
	int GetMatchOffset() {return this->offset;}
	int GetMatchLength() {return this->length;}

	void SetLiteral(CNode *literal) {this->literal=literal;}
	void SetMatch(CNode *match) {this->match=match;}

	int GetType() { return this->type;}
	int GetCost() { return this->cost;}
	int GetPosition() { return this->position;}
	CNode* GetParent() { return this->parent;}
	CNode* GetLiteral() { return this->literal;}
	CNode* GetMatch() { return this->match;}
};








#endif
