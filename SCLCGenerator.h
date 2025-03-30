#ifndef _SCLC_IR_
#define _SCLC_IR_

#include <bits/stdc++.h>
using namespace std;

//IRType
#define _ENUM_IRTYPE_
#include "SCLCEnum.h"
#undef _ENUM_IRTYPE_

string IRWord(IRType IR);

class IRCode{
	public:
		IRType Type;
		int r;
		int s;
		int d;
		int line;
		vector<size_t> ExtraInfo;
		IRCode(IRType type,int R,int S,int D) : Type(type) , r(R) , s(S) , d(D) {}
		string ToString() {
			char* result_c=new char[50];
			string IRName=IRWord(Type);
			if (Type>=IR_In && Type<=IR_Div) sprintf(result_c,"%3d:  %5s  %d,%d,%d ",line,IRName.c_str(),r,s,d);
			else sprintf(result_c,"%3d:  %5s  %d,%d(%d) ",line,IRName.c_str(),r,s,d);
			string result=result_c;
			delete result_c;
			return result;
		}
};

vector<IRCode*> IRInherit(vector<vector<IRCode*>> OriginalIR);
vector<IRCode*> IRMergeStmt(vector<vector<IRCode*>> OriginalIR);
vector<IRCode*> IRIfStmt(vector<vector<IRCode*>> OriginalIR);
vector<IRCode*> IRIfElseStmt(vector<vector<IRCode*>> OriginalIR);
vector<IRCode*> IRRepeatStmt(vector<vector<IRCode*>> OriginalIR);
vector<IRCode*> IRAssignStmt(vector<vector<IRCode*>> OriginalIR);
vector<IRCode*> IRReadStmt(vector<vector<IRCode*>> OriginalIR);
vector<IRCode*> IRWriteStmt(vector<vector<IRCode*>> OriginalIR);
vector<IRCode*> IRLessthan(vector<vector<IRCode*>> OriginalIR);
vector<IRCode*> IREqual(vector<vector<IRCode*>> OriginalIR);
vector<IRCode*> IRAddition(vector<vector<IRCode*>> OriginalIR);
vector<IRCode*> IRSubstract(vector<vector<IRCode*>> OriginalIR);
vector<IRCode*> IRMultiple(vector<vector<IRCode*>> OriginalIR);
vector<IRCode*> IRDivide(vector<vector<IRCode*>> OriginalIR);
vector<IRCode*> IRInheritWithoutParen(vector<vector<IRCode*>> OriginalIR);
vector<IRCode*> IRGetNumStmt(vector<vector<IRCode*>> OriginalIR);
vector<IRCode*> IRGetIDStmt(vector<vector<IRCode*>> OriginalIR);
int GetPlaceOfVar(char* name);
void IRCompletion();

#endif