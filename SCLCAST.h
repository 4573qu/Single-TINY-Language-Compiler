#ifndef _SCLC_AST_
#define _SCLC_AST_

#include <bits/stdc++.h>
using namespace std;

#define _ENUM_OPERATOR_
#include "SCLCEnum.h"
#undef _ENUM_OPERATOR_

class ASTNode{
	public:
		OPType OPstatus;
		WordExpression* LexWord;
		vector<ASTNode*> subtrees;
		ASTNode(){
			subtrees.clear();
		}
		ASTNode(WordExpression* theWord){
			LexWord=theWord;
			subtrees.clear();
		}
};

#endif