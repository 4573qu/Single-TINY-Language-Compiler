#ifndef _SCLC_PARSER_
#define _SCLC_PARSER_

#include "SCLCAST.h"
#include "SCLCGenerator.h"
#include <bits/stdc++.h>
using namespace std;

#define _ENUM_LRTYPE_
#include "SCLCEnum.h"
#undef _ENUM_LRTYPE_


class Sentence{
	public:
		OPType LeftOp;
		vector<OPType> RightOp;
		vector<OPType> tempFirstSet;
		ASTNode* (*ParseFunction)(vector<ASTNode*>);
		vector<IRCode*> (*IRFuntion)(vector<vector<IRCode*>>);
		Sentence(){
			RightOp.clear();
			tempFirstSet.clear();
		}
};

class LRNode{
	public:
		int id;
		vector<Sentence*> SentenceList;
		map<OPType,LRNode*> NextNodeEdge;
		LRNode(){
			id=-1;
			SentenceList.clear();
			NextNodeEdge.clear();
		}		
		LRNode(int i){
			id=i;
			SentenceList.clear();
			NextNodeEdge.clear();
		}
};

class LRSheetNode{
	public:
		LRSheetType type=LR_error;
		union nodeId{
			LRNode* LR;
			Sentence* Sentence;
		}index;
		
};

bool operator == (const Sentence& s1,const Sentence& s2);
bool operator != (const Sentence& s1,const Sentence& s2);
bool operator < (const Sentence& s1,const Sentence& s2);
bool operator > (const Sentence& s1,const Sentence& s2);
bool operator == (const LRNode& s1,const LRNode& s2);
bool operator != (const LRNode& s1,const LRNode& s2);

extern int SentenceAmount;
extern int LRNodeAmount;
extern Sentence* StartSentence;
extern vector<Sentence*> SentenceList;
extern vector<LRNode*> LRNodeList;
extern map<OPType,int> PtrMap;//int=0 EndPtr int=1 NotEndPtr
extern map<OPType,vector<OPType>> FirstSet;
extern map<OPType,vector<OPType>> FollowSet;
extern map<pair<OPType,int>,LRSheetNode> LRSheet;
#endif