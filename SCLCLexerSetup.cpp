#include "SingleCLanCompiler.h"
using namespace std;

int DFAJmpTableLength=1024;
int DFAWordMaxLength;
char*** Errorjt;
string DFAWord;
DFAJmpTable** DFAjt;
vector<WordExpression*> LexWordExp;
map<int,DFANode*> DFAMap;

void ExamineDFANode(){
	cout<<"ExamineDFANodeBegin!\n";
	int errsum=0;
	if(DFAMap.empty()){
		char* error_text=new char[100];
		sprintf(error_text,"DFA not Constructed!");
		raise_dev_error(error_text);
		return;
	}
	for(auto i:DFAMap){
		if(i.first!=i.second->nodeId){
			int tempInt=i.first;
			string mess="DFANode:"+to_string(tempInt)+" is not match nodeId as "+to_string(i.second->nodeId);
			raise_dev_error(StrToChar(mess));
			errsum++;
		}
	}
	if(!errsum) cout<<"ExamineDFANodeComplete!\n";
	else cout<<"ExamineDFANodeFailed!\n";//if quit?bool->main?
}

void ConstructDFANode(){
	DFAMap.clear();
	DFAMap[0]=new DFANode(0,TK_Start,false);
	DFAMap[1]=new DFANode(1,TK_NumberLiteral,true);
	DFAMap[2]=new DFANode(2,TK_UselessToken,true);
	DFAMap[3]=new DFANode(3,TK_incompleteId,false);
	DFAMap[4]=new DFANode(4,TK_Comment,true);
	DFAMap[5]=new DFANode(5,TK_mul,true);
	DFAMap[6]=new DFANode(6,TK_add,true);
	DFAMap[7]=new DFANode(7,TK_sub,true);
	DFAMap[8]=new DFANode(8,TK_div,true);
	DFAMap[9]=new DFANode(9,TK_incompleteId,false);
	DFAMap[10]=new DFANode(10,TK_semicolon,true);
	DFAMap[11]=new DFANode(11,TK_less,true);
	DFAMap[12]=new DFANode(12,TK_equal,true);
	DFAMap[13]=new DFANode(13,TK_id,true);
	DFAMap[14]=new DFANode(14,TK_id,true);
	DFAMap[15]=new DFANode(15,TK_id,true);
	DFAMap[16]=new DFANode(16,TK_id,true);
	DFAMap[17]=new DFANode(17,TK_id,true);
	DFAMap[18]=new DFANode(18,TK_id,true);
	DFAMap[19]=new DFANode(19,TK_assign,true);
	DFAMap[20]=new DFANode(20,TK_id,true);
	DFAMap[21]=new DFANode(21,TK_id,true);
	DFAMap[22]=new DFANode(22,TK_if,true);
	DFAMap[23]=new DFANode(23,TK_id,true);
	DFAMap[24]=new DFANode(24,TK_id,true);
	DFAMap[25]=new DFANode(25,TK_id,true);
	DFAMap[26]=new DFANode(26,TK_id,true);
	DFAMap[27]=new DFANode(27,TK_id,true);
	DFAMap[28]=new DFANode(28,TK_end,true);
	DFAMap[29]=new DFANode(29,TK_id,true);
	DFAMap[30]=new DFANode(30,TK_id,true);
	DFAMap[31]=new DFANode(31,TK_id,true);
	DFAMap[32]=new DFANode(32,TK_id,true);
	DFAMap[33]=new DFANode(33,TK_id,true);
	DFAMap[34]=new DFANode(34,TK_else,true);
	DFAMap[35]=new DFANode(35,TK_read,true);
	DFAMap[36]=new DFANode(36,TK_id,true);
	DFAMap[37]=new DFANode(37,TK_then,true);
	DFAMap[38]=new DFANode(38,TK_id,true);
	DFAMap[39]=new DFANode(39,TK_id,true);
	DFAMap[40]=new DFANode(40,TK_id,true);
	DFAMap[41]=new DFANode(41,TK_until,true);
	DFAMap[42]=new DFANode(42,TK_write,true);
	DFAMap[43]=new DFANode(43,TK_repeat,true);
	DFAMap[44]=new DFANode(44,TK_id,true);
	DFAMap[45]=new DFANode(45,TK_id,true);
}

bool SetJmpTable(int ObjNodeId,int TargNodeId,vector<unsigned char> TransCond){
	if(DFAMap.find(ObjNodeId)==DFAMap.end() || DFAMap.find(TargNodeId)==DFAMap.end()){
		raise_error(1/*pos*/,(char*)"Error at constructing jmptable with empty DFANode in DFAMap");
		return false;
	}else for(auto i:TransCond) DFAjt[ObjNodeId][(int)i].SetData(DFAMap[TargNodeId]);		
	return true;
}

void ConstructDFAJmpTable(){
	DFAjt=new DFAJmpTable*[DFAJmpTableLength];
	for(int i=0;i<DFAJmpTableLength;i++) DFAjt[i]=new DFAJmpTable[256];
	vector<unsigned char> TransCond;
	/*Construct Number JT*/
	TransCond.clear();
	for(char i='0';i<='9';i++) TransCond.push_back(i);
	SetJmpTable(00, 1,TransCond);
	
	/*Construct \n JT*/
	TransCond.clear();
	TransCond.push_back('\n');
	TransCond.push_back(' ');
	TransCond.push_back('\t');
	SetJmpTable(00, 2,TransCond);
	
	/*Construct Comment JT*/
	TransCond.clear();
	TransCond.push_back('{');
	SetJmpTable(00, 3,TransCond);
	TransCond.clear();
	for(char i=' ';i<='~';i++) TransCond.push_back(i);
	TransCond.push_back('\n');
	TransCond.push_back(' ');
	TransCond.push_back('\t');
	SetJmpTable( 3, 3,TransCond);
	TransCond.clear();
	TransCond.push_back('}');
	SetJmpTable( 3, 4,TransCond);
	
	/*Construct ID JT*/
	TransCond.clear();	
	for(char i='a';i<='z';i++) TransCond.push_back(i);
	for(char i='A';i<='Z';i++) TransCond.push_back(i);
	TransCond.push_back('_');
	SetJmpTable( 0,44,TransCond);
	SetJmpTable(44,45,TransCond);
	SetJmpTable(45,45,TransCond);
	for(char i='0';i<='9';i++) TransCond.push_back(i);
	for(int i=13;i<=43;i++) SetJmpTable(i,45,TransCond);
	
	/*Construct Other JT by DFAHelper*/
	TransCond.clear();
	TransCond.push_back('*');
	SetJmpTable(00, 5,TransCond);
	TransCond.clear();
	TransCond.push_back('+');
	SetJmpTable(00, 6,TransCond);
	TransCond.clear();
	TransCond.push_back('-');
	SetJmpTable(00, 7,TransCond);
	TransCond.clear();
	TransCond.push_back('/');
	SetJmpTable(00, 8,TransCond);
	TransCond.clear();
	TransCond.push_back(':');
	SetJmpTable(00, 9,TransCond);
	TransCond.clear();
	TransCond.push_back(';');
	SetJmpTable(00,10,TransCond);
	TransCond.clear();
	TransCond.push_back('<');
	SetJmpTable(00,11,TransCond);
	TransCond.clear();
	TransCond.push_back('=');
	SetJmpTable(00,12,TransCond);
	SetJmpTable( 9,19,TransCond);
	TransCond.clear();
	TransCond.push_back('a');
	SetJmpTable(23,29,TransCond);
	SetJmpTable(36,40,TransCond);
	TransCond.clear();
	TransCond.push_back('d');
	SetJmpTable(21,28,TransCond);
	SetJmpTable(29,35,TransCond);
	TransCond.clear();
	TransCond.push_back('e');
	SetJmpTable(00,13,TransCond);
	SetJmpTable(15,23,TransCond);
	SetJmpTable(24,31,TransCond);
	SetJmpTable(27,34,TransCond);
	SetJmpTable(30,36,TransCond);
	SetJmpTable(39,42,TransCond);
	TransCond.clear();
	TransCond.push_back('f');
	SetJmpTable(14,22,TransCond);
	TransCond.clear();
	TransCond.push_back('h');
	SetJmpTable(16,24,TransCond);
	TransCond.clear();
	TransCond.push_back('i');
	SetJmpTable(00,14,TransCond);
	SetJmpTable(26,33,TransCond);
	SetJmpTable(32,38,TransCond);
	TransCond.clear();
	TransCond.push_back('l');
	SetJmpTable(13,20,TransCond);
	SetJmpTable(38,41,TransCond);
	TransCond.clear();
	TransCond.push_back('n');
	SetJmpTable(13,21,TransCond);
	SetJmpTable(17,25,TransCond);
	SetJmpTable(31,37,TransCond);
	TransCond.clear();
	TransCond.push_back('p');
	SetJmpTable(23,30,TransCond);
	TransCond.clear();
	TransCond.push_back('r');
	SetJmpTable(00,15,TransCond);
	SetJmpTable(18,26,TransCond);
	TransCond.clear();
	TransCond.push_back('s');
	SetJmpTable(20,27,TransCond);
	TransCond.clear();
	TransCond.push_back('t');
	SetJmpTable(00,16,TransCond);
	SetJmpTable(25,32,TransCond);
	SetJmpTable(33,39,TransCond);
	SetJmpTable(40,43,TransCond);
	TransCond.clear();
	TransCond.push_back('u');
	SetJmpTable(00,17,TransCond);
	TransCond.clear();
	TransCond.push_back('w');
	SetJmpTable(00,18,TransCond);
}