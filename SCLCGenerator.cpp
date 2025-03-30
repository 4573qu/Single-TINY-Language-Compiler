#include "SingleCLanCompiler.h"
using namespace std;

vector<IRCode*> IRList;
string TMCode;
vector<char*> VarList;
int temp_offset;

string IRWord(IRType type){
	map<IRType,string> IRStrings={
		#define IRenum(IR,Code) {IR,Code},
		#define IRenumWithNum(IR,Code,Num) {IR,Code},
			#include "SCLCIRCodes.h"
		#undef IRenum
		#undef IRenumWithNum
	};
	return IRStrings[type];
}

vector<IRCode*> IRInherit(vector<vector<IRCode*>> OriginalIR) {
	vector<IRCode*> result;
	for (auto i:OriginalIR) {
		result.insert(result.end(),i.begin(),i.end());
	}
	return result;
}

vector<IRCode*> IRMergeStmt(vector<vector<IRCode*>> OriginalIR) {
	vector<IRCode*> result=OriginalIR[0];
	result.insert(result.end(),OriginalIR[2].begin(),OriginalIR[2].end());
	return result;
}

vector<IRCode*> IRIfStmt(vector<vector<IRCode*>> OriginalIR) {
	vector<IRCode*> result;
	vector<IRCode*> exp=OriginalIR[1];
	vector<IRCode*> stmt=OriginalIR[3];
	result.insert(result.end(),exp.begin(),exp.end());
	IRCode* if_ir=new IRCode(IR_JumpIfEqual,0,0,7);
	result.push_back(if_ir);
	result.insert(result.end(),stmt.begin(),stmt.end());
	IRCode* if_ir2=new IRCode(IR_LoadA,7,0,7);
	if_ir2->ExtraInfo.push_back((size_t)if_ir2);
	if_ir2->ExtraInfo.push_back(1);
	if_ir->ExtraInfo.push_back((size_t)if_ir2);
	if_ir->ExtraInfo.push_back(1);
	result.push_back(if_ir2);
	return result;
}

vector<IRCode*> IRIfElseStmt(vector<vector<IRCode*>> OriginalIR) {
	vector<IRCode*> result;
	vector<IRCode*> exp=OriginalIR[1];
	vector<IRCode*> stmt1=OriginalIR[3];
	vector<IRCode*> stmt2=OriginalIR[5];
	result.insert(result.end(),exp.begin(),exp.end());
	IRCode* if_ir=new IRCode(IR_JumpIfEqual,0,0,7);
	result.push_back(if_ir);
	result.insert(result.end(),stmt1.begin(),stmt1.end());
	IRCode* if_ir2=new IRCode(IR_LoadA,7,0,7);
	if_ir->ExtraInfo.push_back((size_t)if_ir2);
	if_ir->ExtraInfo.push_back(1);
	if_ir2->ExtraInfo.push_back((size_t)if_ir2);
	if_ir2->ExtraInfo.push_back(stmt2.size()+1);
	result.push_back(if_ir2);
	result.insert(result.end(),stmt2.begin(),stmt2.end());
	return result;
}

vector<IRCode*> IRRepeatStmt(vector<vector<IRCode*>> OriginalIR) {
	vector<IRCode*> result;
	vector<IRCode*> exp=OriginalIR[3];
	vector<IRCode*> stmt=OriginalIR[1];
	result.insert(result.end(),stmt.begin(),stmt.end());
	result.insert(result.end(),exp.begin(),exp.end());
	IRCode* if_ir=new IRCode(IR_JumpIfEqual,0,0,7);
	if_ir->ExtraInfo.push_back((size_t)result[0]);
	if_ir->ExtraInfo.push_back(0);
	result.push_back(if_ir);
	return result;
}

vector<IRCode*> IRAssignStmt(vector<vector<IRCode*>> OriginalIR) {
	vector<IRCode*> result;
	vector<IRCode*> exp=OriginalIR[2];
	result.insert(result.end(),exp.begin(),exp.end());
	WordExpression* temp_word=(WordExpression*)(OriginalIR[0][0]->ExtraInfo[0]);
	int loc=GetPlaceOfVar(temp_word->value);
	IRCode* assign=new IRCode(IR_Store,0,loc,5);//GetPlaceOfVar
	result.push_back(assign);
	return result;	
}

vector<IRCode*> IRReadStmt(vector<vector<IRCode*>> OriginalIR) {
	vector<IRCode*> result;
	IRCode* read=new IRCode(IR_In,0,0,0);
	result.push_back(read);
	WordExpression* temp_word=(WordExpression*)(OriginalIR[1][0]->ExtraInfo[0]);
	int loc=GetPlaceOfVar(temp_word->value);
	read=new IRCode(IR_Store,0,loc,5);//GetPlaceOfVar
	result.push_back(read);
	return result;
}

vector<IRCode*> IRWriteStmt(vector<vector<IRCode*>> OriginalIR) {
	vector<IRCode*> result;
	vector<IRCode*> exp=OriginalIR[1];
	result.insert(result.end(),exp.begin(),exp.end());
	IRCode* write=new IRCode(IR_Out,0,0,0);
	result.push_back(write);
	return result;
}

vector<IRCode*> IRLessthan(vector<vector<IRCode*>> OriginalIR) {
    vector<IRCode*> result;
    vector<IRCode*> stmt1=OriginalIR[0];
	vector<IRCode*> stmt2=OriginalIR[2];
	result.insert(result.end(),stmt1.begin(),stmt1.end());
	IRCode* oper=new IRCode(IR_Store,0,temp_offset--,6);
	result.push_back(oper);
	result.insert(result.end(),stmt2.begin(),stmt2.end());
	oper=new IRCode(IR_Load,1,++temp_offset,6);
	result.push_back(oper);
	oper=new IRCode(IR_Sub,0,1,0);
	result.push_back(oper);
	oper=new IRCode(IR_JumpIfLessThan,0,2,7);
	result.push_back(oper);
	oper=new IRCode(IR_LoadC,0,0,0);
	result.push_back(oper);
	oper=new IRCode(IR_LoadA,7,1,7);
	result.push_back(oper);
	oper=new IRCode(IR_LoadC,0,1,0);
	result.push_back(oper);
	return result;
}

vector<IRCode*> IREqual(vector<vector<IRCode*>> OriginalIR) {
    vector<IRCode*> result;
    vector<IRCode*> stmt1=OriginalIR[0];
	vector<IRCode*> stmt2=OriginalIR[2];
	result.insert(result.end(),stmt1.begin(),stmt1.end());
	IRCode* oper=new IRCode(IR_Store,0,temp_offset--,6);
	result.push_back(oper);
	result.insert(result.end(),stmt2.begin(),stmt2.end());
	oper=new IRCode(IR_Load,1,++temp_offset,6);
	result.push_back(oper);
	oper=new IRCode(IR_Sub,0,1,0);
	result.push_back(oper);
	oper=new IRCode(IR_JumpIfEqual,0,2,7);
	result.push_back(oper);
	oper=new IRCode(IR_LoadC,0,0,0);
	result.push_back(oper);
	oper=new IRCode(IR_LoadA,7,1,7);
	result.push_back(oper);
	oper=new IRCode(IR_LoadC,0,1,0);
	result.push_back(oper);
	return result;
}

vector<IRCode*> IRAddition(vector<vector<IRCode*>> OriginalIR) {
    vector<IRCode*> result;
    vector<IRCode*> stmt1=OriginalIR[0];
	vector<IRCode*> stmt2=OriginalIR[2];
	result.insert(result.end(),stmt1.begin(),stmt1.end());
	IRCode* oper=new IRCode(IR_Store,0,temp_offset--,6);
	result.push_back(oper);
	result.insert(result.end(),stmt2.begin(),stmt2.end());
	oper=new IRCode(IR_Load,1,++temp_offset,6);
	result.push_back(oper);
	oper=new IRCode(IR_Add,0,1,0);
	result.push_back(oper);
	return result;
}

vector<IRCode*> IRSubstract(vector<vector<IRCode*>> OriginalIR) {
    vector<IRCode*> result;
    vector<IRCode*> stmt1=OriginalIR[0];
	vector<IRCode*> stmt2=OriginalIR[2];
	result.insert(result.end(),stmt1.begin(),stmt1.end());
	IRCode* oper=new IRCode(IR_Store,0,temp_offset--,6);
	result.push_back(oper);
	result.insert(result.end(),stmt2.begin(),stmt2.end());
	oper=new IRCode(IR_Load,1,++temp_offset,6);
	result.push_back(oper);
	oper=new IRCode(IR_Sub,0,1,0);
	result.push_back(oper);
	return result;
}

vector<IRCode*> IRMultiple(vector<vector<IRCode*>> OriginalIR) {
    vector<IRCode*> result;
    vector<IRCode*> stmt1=OriginalIR[0];
	vector<IRCode*> stmt2=OriginalIR[2];
	result.insert(result.end(),stmt1.begin(),stmt1.end());
	IRCode* oper=new IRCode(IR_Store,0,temp_offset--,6);
	result.push_back(oper);
	result.insert(result.end(),stmt2.begin(),stmt2.end());
	oper=new IRCode(IR_Load,1,++temp_offset,6);
	result.push_back(oper);
	oper=new IRCode(IR_Mul,0,1,0);
	result.push_back(oper);
	return result;
}

vector<IRCode*> IRDivide(vector<vector<IRCode*>> OriginalIR) {
    vector<IRCode*> result;
    vector<IRCode*> stmt1=OriginalIR[0];
	vector<IRCode*> stmt2=OriginalIR[2];
	result.insert(result.end(),stmt1.begin(),stmt1.end());
	IRCode* oper=new IRCode(IR_Store,0,temp_offset--,6);
	result.push_back(oper);
	result.insert(result.end(),stmt2.begin(),stmt2.end());
	oper=new IRCode(IR_Load,1,++temp_offset,6);
	result.push_back(oper);
	oper=new IRCode(IR_Div,0,1,0);
	result.push_back(oper);
	return result;
}

vector<IRCode*> IRInheritWithoutParen(vector<vector<IRCode*>> OriginalIR) {
	vector<IRCode*> result=OriginalIR[1];
	return result;
}

vector<IRCode*> IRGetNumStmt(vector<vector<IRCode*>> OriginalIR) {
	vector<IRCode*> result;
	WordExpression* temp_word=(WordExpression*)(OriginalIR[0][0]->ExtraInfo[0]);
	int num=atoi(temp_word->value);
	IRCode* const_ir=new IRCode(IR_LoadC,0,num,0);//GetValOfVar
	result.push_back(const_ir);
	return result;
}

vector<IRCode*> IRGetIDStmt(vector<vector<IRCode*>> OriginalIR) {
	vector<IRCode*> result;
	WordExpression* temp_word=(WordExpression*)(OriginalIR[0][0]->ExtraInfo[0]);
	int loc=GetPlaceOfVar(temp_word->value);
	IRCode* const_ir=new IRCode(IR_Load,0,loc,5);//GetPlaceOfVar
	result.push_back(const_ir);
	return result;
}

int GetPlaceOfVar(char* name) {
	for (int i=0;i<VarList.size();i++) {
		if (!strcmp(VarList[i],name)) return i;
	}
	VarList.push_back(name);
	return VarList.size()-1;
}

void IRCompletion() {
	IRCode* initial=new IRCode(IR_Store,0,0,0);
	IRList.insert(IRList.begin(),initial);
	initial=new IRCode(IR_Load,6,0,0);
	IRList.insert(IRList.begin(),initial);
	IRCode* halt=new IRCode(IR_Halt,0,0,0);
	IRList.push_back(halt);
	for (int i=0;i<IRList.size();i++) {
		IRList[i]->line=i;
		if ((IRList[i]->Type==IR_JumpIfEqual || IRList[i]->Type==IR_JumpIfLessThan || IRList[i]->Type==IR_LoadA) && IRList[i]->ExtraInfo.size()>0) {
			auto it=find(IRList.begin(),IRList.end(),(IRCode*)IRList[i]->ExtraInfo[0]);
    		if (it!=IRList.end()) IRList[i]->s=distance(IRList.begin(),it)-i-1+IRList[i]->ExtraInfo[1];
    		else IRList[i]->s=IRList.size()-i-2+IRList[i]->ExtraInfo[1];
		}
	}
}