#ifndef _SCLC_LEXER_
#define _SCLC_LEXER_

#include <bits/stdc++.h>
using namespace std;

//Token
#define _ENUM_TOKEN_
#include "SCLCEnum.h"
#undef _ENUM_TOKEN_

class DFANode{
	public:
		int nodeId;
		DFAToken status;
		bool isEndPtr;
		DFANode(){
			nodeId=0;
		}
		DFANode(int id,DFAToken tokenname,bool isEnd=false){
			nodeId=id;
			status=tokenname;
			isEndPtr=isEnd;
		}
};

class DFAJmpTable{
	public:
	    DFANode* data;
	    //int* x;//IFNOTUSE?
	    DFAJmpTable(){
			data=NULL;
		}
		void SetData(DFANode* node){
			data=node;
		}
};

class WordExpression{
	public:
		DFAToken status;
		char* value;
		int lineNo=-1;
		int startColNo=-1;
		WordExpression(){
		}
		WordExpression(DFAToken TokenName,char* theValue){
			status=TokenName;
			value=theValue;
		}
		WordExpression(DFAToken TokenName,char* theValue,int line,int column){
			status=TokenName;
			value=theValue;
			lineNo=line;
			startColNo=column;
		}
};

//Setup
void ExamineDFANode();
void ConstructDFANode();
bool SetJmpTable(int ObjNodeId,int TargNodeId,vector<unsigned char> TransCond);
void ConstructDFAJmpTable();

//Lex
int getNextStateInGraph(int state,unsigned char word);
bool getTypeInGraph(int state);
DFAToken getCategoryInGraph(int state);
string TKWord(DFAToken Token);
bool Lex(char* input);
void SkipUselessToken();

//extern
extern int DFAJmpTableLength;//missing Initialization
extern int DFAWordMaxLength;
extern char*** Errorjt;//when there is no available jmp but at NOT-FINAL node, try this
//modify comment:char*** is not good enough,try func[][]
//but if there is no error at dfa status need func?
//or just raise_error at Lex?
extern string DFAWord;
extern DFAJmpTable** DFAjt;
extern vector<WordExpression*> LexWordExp;
extern map<int,DFANode*> DFAMap;
#endif