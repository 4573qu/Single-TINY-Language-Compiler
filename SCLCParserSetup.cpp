#include "SingleCLanCompiler.h"
using namespace std;

int SentenceAmount=0;
int LRNodeAmount=0;
Sentence* StartSentence=NULL;
vector<Sentence*> SentenceList;
vector<LRNode*> LRNodeList;
map<OPType,int> PtrMap;//int=0 EndPtr int=1 NotEndPtr
map<OPType,vector<OPType>> FirstSet;
map<OPType,vector<OPType>> FollowSet;
map<pair<OPType,int>,LRSheetNode> LRSheet;

bool operator == (const Sentence& s1,const Sentence& s2){
    if(s1.LeftOp!=s2.LeftOp) return false;
    vector<OPType> s1ROp=s1.RightOp,s2ROp=s2.RightOp;
    return s1ROp==s2ROp;
}

bool operator != (const Sentence& s1,const Sentence& s2){
    if(s1.LeftOp!=s2.LeftOp) return true;
    vector<OPType> s1ROp=s1.RightOp,s2ROp=s2.RightOp;
    return !(s1ROp==s2ROp);
}

bool operator < (const Sentence& s1,const Sentence& s2){
	if(s1.LeftOp!=s2.LeftOp) return s1.LeftOp<s2.LeftOp;
	if(s1.RightOp.size()!=s2.RightOp.size()) return s1.RightOp.size()<s2.RightOp.size();
	for(int i=0;i<s1.RightOp.size();i++){
		if(s1.RightOp[i]!=s2.RightOp[i]) return s1.RightOp[i]<s2.RightOp[i];
	}
	return true;
}

bool operator > (const Sentence& s1,const Sentence& s2){
	return !(s1<s2);
}

bool operator == (const LRNode& s1,const LRNode& s2){
	if(s1.SentenceList.size()!=s2.SentenceList.size()) return false;
	vector<Sentence*> s1copy(s1.SentenceList.size()),s2copy(s2.SentenceList.size());
	copy(s1.SentenceList.begin(),s1.SentenceList.end(),s1copy.begin());
	copy(s2.SentenceList.begin(),s2.SentenceList.end(),s2copy.begin());
	sort(s1copy.begin(),s1copy.end(),[](Sentence* se1,Sentence* se2){
		return (*se1)<(*se2);
	});
	sort(s2copy.begin(),s2copy.end(),[](Sentence* se1,Sentence* se2){
		return (*se1)<(*se2);
	});
    for(int i=0;i<s1copy.size();i++){
    	if(*(s1copy[i])!=*(s2copy[i])) return false;
	}
	return true;
}

bool operator != (const LRNode& s1,const LRNode& s2){
	return !(s1==s2);
}

ASTNode* ParseTranslation(vector<ASTNode*>){
	ASTNode* ReturnResult=new ASTNode;
	return ReturnResult;
}

ASTNode* NoOperation(vector<ASTNode*>){
	ASTNode* ReturnResult=new ASTNode;
	return ReturnResult;
}

Sentence* SetSentence(OPType LeftExpression,vector<OPType> RightExpression,ASTNode* (*TreeConstruction)(vector<ASTNode*>),vector<IRCode*> (*IRConstruction)(vector<vector<IRCode*>>)){
	Sentence* tempSentence=new Sentence;
	tempSentence->LeftOp=LeftExpression;
	tempSentence->RightOp=RightExpression;
	tempSentence->ParseFunction=TreeConstruction;
	tempSentence->IRFuntion=IRConstruction;
	SentenceList.push_back(tempSentence);
	SentenceAmount++;
	PtrMap[LeftExpression]|=1;
	for(auto i:RightExpression) PtrMap[i]|=0;
	return tempSentence;
}

//parse
//First Translate TK to OP
//for example, long long int->op_long op_long op_int->(reduce)op_D->(rename) op_declarator
//for example when id->E, we actually construct tree E -> id锛宻o the construction of the first translate(op_int->op_D) is also in the construction
void ConstructSentence(){
	StartSentence=nullptr;
	vector<OPType> RightSentence;
	//stmt_seq -> stmt_seq SEMI stmt | stmt
	{
		RightSentence={OP_STMT};
		SetSentence(OP_STMTSEQ,RightSentence,ParseTranslation,IRInherit);
		RightSentence={OP_STMTSEQ,OP_semicolon,OP_STMT};
		SetSentence(OP_STMTSEQ,RightSentence,ParseTranslation,IRMergeStmt);
	}
	//stmt -> if_stmt | repeat_stmt | assign_stmt | read_stmt | write_stmt
	{
		RightSentence={OP_IFSTMT};
		SetSentence(OP_STMT,RightSentence,ParseTranslation,IRInherit);
		RightSentence={OP_REPEATSTMT};
		SetSentence(OP_STMT,RightSentence,ParseTranslation,IRInherit);
		RightSentence={OP_ASSIGNSTMT};
		SetSentence(OP_STMT,RightSentence,ParseTranslation,IRInherit);
		RightSentence={OP_READSTMT};
		SetSentence(OP_STMT,RightSentence,ParseTranslation,IRInherit);
		RightSentence={OP_WRITESTMT};
		SetSentence(OP_STMT,RightSentence,ParseTranslation,IRInherit);
	}
	//if_stmt -> IF exp THEN stmt_seq END | IF exp THEN stmt_seq ELSE stmt_seq END
	{
		RightSentence={OP_if,OP_EXP,OP_then,OP_STMTSEQ,OP_end};
		SetSentence(OP_IFSTMT,RightSentence,ParseTranslation,IRIfStmt);
		RightSentence={OP_if,OP_EXP,OP_then,OP_STMTSEQ,OP_else,OP_STMTSEQ,OP_end};
		SetSentence(OP_IFSTMT,RightSentence,ParseTranslation,IRIfElseStmt);
	}
	//repeat_stmt -> REPEAT stmt_seq UNTIL exp
	{
		RightSentence={OP_repeat,OP_STMTSEQ,OP_until,OP_EXP};
		SetSentence(OP_REPEATSTMT,RightSentence,ParseTranslation,IRRepeatStmt);
	}
	//assign_stmt -> ID ASSIGN exp
	{
		RightSentence={OP_id,OP_assign,OP_EXP};
		SetSentence(OP_ASSIGNSTMT,RightSentence,ParseTranslation,IRAssignStmt);
	}
	//read_stmt -> READ ID
	{
		RightSentence={OP_read,OP_id};
		SetSentence(OP_READSTMT,RightSentence,ParseTranslation,IRReadStmt);;
	}
	//write_stmt -> WRITE exp
	{
		RightSentence={OP_write,OP_EXP};
		SetSentence(OP_WRITESTMT,RightSentence,ParseTranslation,IRWriteStmt);
	}
	//exp -> simple_exp LT simple_exp | simple_exp EQ simple_exp | simple_exp
	{
		RightSentence={OP_SIMPLEEXP,OP_less,OP_SIMPLEEXP};
		SetSentence(OP_EXP,RightSentence,ParseTranslation,IRLessthan);
		RightSentence={OP_SIMPLEEXP,OP_equal,OP_SIMPLEEXP};
		SetSentence(OP_EXP,RightSentence,ParseTranslation,IREqual);
		RightSentence={OP_SIMPLEEXP};
		SetSentence(OP_EXP,RightSentence,ParseTranslation,IRInherit);
	}
	//sinple_exp -> simple_exp PLUS term | simple_exp MINUS term | term
	{
		RightSentence={OP_SIMPLEEXP,OP_add,OP_TERM};
		SetSentence(OP_SIMPLEEXP,RightSentence,ParseTranslation,IRAddition);
		RightSentence={OP_SIMPLEEXP,OP_sub,OP_TERM};
		SetSentence(OP_SIMPLEEXP,RightSentence,ParseTranslation,IRSubstract);
		RightSentence={OP_TERM};
		SetSentence(OP_SIMPLEEXP,RightSentence,ParseTranslation,IRInherit);
	}
	//term -> term TIMES factor | term OVER factor | factor
	{
		RightSentence={OP_TERM,OP_mul,OP_FACTOR};
		SetSentence(OP_TERM,RightSentence,ParseTranslation,IRMultiple);
		RightSentence={OP_TERM,OP_div,OP_FACTOR};
		SetSentence(OP_TERM,RightSentence,ParseTranslation,IRDivide);
		RightSentence={OP_FACTOR};
		SetSentence(OP_TERM,RightSentence,ParseTranslation,IRInherit);
	}
	//factor -> LPAREN exp RPAREN | NUM | ID
	{
		RightSentence={OP_leftparen,OP_EXP,OP_rightparen};
		SetSentence(OP_FACTOR,RightSentence,ParseTranslation,IRInheritWithoutParen);
		RightSentence={OP_NumberLiteral};
		SetSentence(OP_FACTOR,RightSentence,ParseTranslation,IRGetNumStmt);
		RightSentence={OP_id};
		SetSentence(OP_FACTOR,RightSentence,ParseTranslation,IRGetIDStmt);
	}
	
	RightSentence.clear();
	RightSentence.push_back(OP_STMTSEQ);
	RightSentence.push_back(OP_EOF);
	StartSentence=SetSentence(startPtr,RightSentence,ParseTranslation,IRInherit);

	
	/*For Example:E->E+T
		RightSentence.clear();
		RightSentence.push_back(OP_E);
		RightSentence.push_back(OP_add);
		RightSentence.push_back(OP_T);
		SetSentence(OP_E,RightSentence,Tree1to3,Add);
		OP_E->OP_E OP_add OP_T
		ConstructTree:Tree1to3(OP_E,RightSentence)
		ConstructIR:Add(ASTNodeLeft*,ASTNodeRight*);
	*/
}