#include "SingleCLanCompiler.h"
using namespace std;

vector<ASTNode*> ASTNodeList;

//outer message dealing
void raise_dev_error(char* messagetext){
	cout<<endl<<"deverror:"<<messagetext<<endl;
}

void raise_error(int pos,char* messagetext){
	cout<<"error:"<<messagetext<<endl;
}

//global functions
char* StrToChar(string s){
	const char *tempchar=s.c_str();
	char *result=new char[strlen(tempchar)+1];
	strcpy(result,tempchar);
	return result;
}

template<class T>
bool hasVal(vector<T> stl,T target){
	return find(stl.begin(),stl.end(),target)!=stl.end();
}

//preprocess

//lex->SCLCLexer
//To abstract illegal string/char like "\", see YUYIFENXI
//WHEN YUYIFENXI JUST DEAL int+int when come into STR+STR FIND FOR OPERATOR

void SetupLexer(){
	ConstructDFANode();
	ConstructDFAJmpTable();
	vector<int> LengthList={
		#define TKenum(TK,STR) strlen(STR),
		#define TKenumWithNum(TK,STR,Num) strlen(STR),
			#include "SCLCToken.h"
		#undef TKenum
		#undef TKenumWithNum
	};
	vector<int>::iterator MaxLength=max_element(LengthList.begin(),LengthList.end());
	DFAWordMaxLength=*MaxLength;
}

void TranslateTKtoOP(){
	ASTNodeList.clear();
	for(vector<WordExpression*>::iterator i=LexWordExp.begin();i!=LexWordExp.end();i++){
		WordExpression* temp=*i;		
		ASTNode* tempAST=new ASTNode(temp);
		tempAST->OPstatus=(OPType)tempAST->LexWord->status;
		ASTNodeList.push_back(tempAST);
	}
}

void AddEOFNode(){
	WordExpression* temp=new WordExpression;
	char* endtext=new char[1];
	sprintf(endtext,"$");
	temp->lineNo=temp->startColNo=-1;
	temp->value=endtext;
	temp->status=TK_Err;
	ASTNode* tempAST=new ASTNode(temp);
	tempAST->OPstatus=OP_EOF;
	ASTNodeList.push_back(tempAST);
}

LRNode* GenerateNextList(OPType start,LRNode* StartNode,vector<Sentence*> SentenceList){
	LRNode* curr=new LRNode(LRNodeAmount++);
	vector<OPType> WaitList,WaitListCopy;
	WaitList.clear();
	WaitListCopy.clear();
	for(int i=0;i<StartNode->SentenceList.size();i++){
		for(int j=0;j<StartNode->SentenceList[i]->RightOp.size()-1;j++){
			if(StartNode->SentenceList[i]->RightOp[j]==OP_LRSeperator && StartNode->SentenceList[i]->RightOp[j+1]==start){
				Sentence* tempSentence=new Sentence;
				tempSentence->LeftOp=StartNode->SentenceList[i]->LeftOp;
				for(int k=0;k<StartNode->SentenceList[i]->RightOp.size();k++){
					tempSentence->RightOp.push_back(StartNode->SentenceList[i]->RightOp[k]);
				}
				tempSentence->RightOp[j]=StartNode->SentenceList[i]->RightOp[j+1];
				tempSentence->RightOp[j+1]=StartNode->SentenceList[i]->RightOp[j];
				curr->SentenceList.push_back(tempSentence);
				if(j+2<StartNode->SentenceList[i]->RightOp.size()){
					if(!hasVal(WaitList,StartNode->SentenceList[i]->RightOp[j+2])) WaitList.push_back(StartNode->SentenceList[i]->RightOp[j+2]);
					//vector<OPType>::iterator ite=unique(WaitList.begin(),WaitList.end());
					//WaitList.erase(ite,WaitList.end());
					if(!hasVal(WaitListCopy,StartNode->SentenceList[i]->RightOp[j+2])) WaitListCopy.push_back(StartNode->SentenceList[i]->RightOp[j+2]);
				}
				break;
			}
		}
	}
	//copy(WaitList.begin(),WaitList.end(),back_inserter(WaitListCopy));
	while(!WaitList.empty()){
		for(int i=0;i<SentenceList.size();i++){
			if(WaitList.front()==SentenceList[i]->LeftOp){
				Sentence* tempSentence=new Sentence;
				tempSentence->LeftOp=SentenceList[i]->LeftOp;
				tempSentence->RightOp.push_back(OP_LRSeperator);
				for(int j=0;j<SentenceList[i]->RightOp.size();j++){
					tempSentence->RightOp.push_back(SentenceList[i]->RightOp[j]);
				}
				curr->SentenceList.push_back(tempSentence);
				if(!hasVal(WaitList,tempSentence->RightOp[1])) WaitList.push_back(tempSentence->RightOp[1]);
				//vector<OPType>::iterator ite=unique(WaitList.begin(),WaitList.end());
				//WaitList.erase(ite,WaitList.end());
				if(!hasVal(WaitListCopy,tempSentence->RightOp[1])) WaitListCopy.push_back(tempSentence->RightOp[1]);
				//ite=unique(WaitListCopy.begin(),WaitListCopy.end());
				//WaitListCopy.erase(ite,WaitListCopy.end());
			}
		}
		WaitList.erase(WaitList.begin());
	}
//search if same
	for(auto i:LRNodeList){
		if(*i==*curr){
			/*cout<<"WHILE CONSTRUCTING "<<i->id<<" AT SYMBOL "<<start<<endl;
			for(auto j:i->SentenceList){
				cout<<j->LeftOp<<"->";
				for(auto k:j->RightOp){
					cout<<k<<" ";
				}
				cout<<endl;
			}
			for(auto j:i->NextNodeEdge){
				cout<<j.first<<"->"<<j.second->id;
				cout<<endl;
			}*/
			LRNodeAmount--;
			return i;
		}
	}
	LRNodeList.push_back(curr);
	curr->NextNodeEdge.clear();
	if(WaitListCopy.empty()) return curr;
	while(!WaitListCopy.empty()){
		LRNode* next=GenerateNextList(WaitListCopy.front(),curr,SentenceList);
		curr->NextNodeEdge[WaitListCopy.front()]=next;
		WaitListCopy.erase(WaitListCopy.begin());
	}
    return curr;
}

LRNode* CreateLRNode(OPType start,vector<Sentence*> SentenceList){
//Create SentenceList
	LRNode* curr=new LRNode(LRNodeAmount++);
	LRNodeList.push_back(curr);
	vector<OPType> WaitList,WaitListCopy;
	WaitList.clear();
	WaitListCopy.clear();
	WaitList.push_back(start);
	while(!WaitList.empty()){
		for(auto i:SentenceList){
			if(WaitList.front()==i->LeftOp){
				Sentence* tempSentence=new Sentence;
				tempSentence->LeftOp=i->LeftOp;
				tempSentence->RightOp.push_back(OP_LRSeperator);
				for(auto j:i->RightOp) tempSentence->RightOp.push_back(j);
				curr->SentenceList.push_back(tempSentence);
				if(!hasVal(WaitList,i->RightOp.front())) WaitList.push_back(i->RightOp.front());
				//vector<OPType>::iterator ite=unique(WaitList.begin(),WaitList.end());
				//WaitList.erase(ite,WaitList.end());
				if(!hasVal(WaitListCopy,i->RightOp.front())) WaitListCopy.push_back(i->RightOp.front());
				//ite=unique(WaitListCopy.begin(),WaitListCopy.end());
				//WaitListCopy.erase(ite,WaitListCopy.end());
			}
		}
		WaitList.erase(WaitList.begin());
	}
//Create NextEdgeList
	while(!WaitListCopy.empty()){
		LRNode* next=GenerateNextList(WaitListCopy.front(),curr,SentenceList);
		if(find(LRNodeList.begin(),LRNodeList.end(),next)!=LRNodeList.end()){
			curr->NextNodeEdge[WaitListCopy.front()]=next;
			WaitListCopy.erase(WaitListCopy.begin());
		}else{
			curr->NextNodeEdge[WaitListCopy.front()]=next;
			WaitListCopy.erase(WaitListCopy.begin());
			LRNodeList.push_back(next);
		}
	}
    return curr;
}

void ExamineStartSentence(){
	if(!StartSentence){
		char* error_text=new char[100];
		sprintf(error_text,"Lack Of StartSentence!");
		raise_dev_error(error_text);
		exit(0);
	}
}

void PrintLRNode(){
	cout<<"PrintLRNode:\n";
	for(auto i:LRNodeList){
		cout<<i->id<<":"<<endl;
		for(auto j:i->SentenceList){
			cout<<"Left:"<<j->LeftOp<<"\tRight:";
			for(auto k:j->RightOp){
				cout<<k<<" ";
			}
			cout<<endl;
		}
		for(auto j:i->NextNodeEdge){
			cout<<"Condition:"<<setw(5)<<j.first<<"\tTo Node Id:"<<j.second->id<<endl;
		}
		cout<<endl;
	}
}

void PrintFirstSetOfSentence(){
	cout<<"PrintFirstSetOfSentence:\n";
	for(auto i:SentenceList){
		cout<<i->LeftOp<<":";
		for(auto j:i->RightOp) cout<<j<<" ";
		cout<<endl;
		for(auto j:i->tempFirstSet) cout<<j<<" ";
		cout<<endl<<endl;
	}
}

void PrintFirstSet(){
	cout<<"PrintFirstSet:\n";
	for(map<OPType,vector<OPType>>::iterator i=FirstSet.begin();i!=FirstSet.end();i++){
		cout<<i->first<<":"<<endl;
		for(auto j:i->second){
			cout<<j<<" ";
		}
		cout<<endl<<endl;
	}
}

void PrintFollowSet(){
	cout<<"PrintFollowSet:\n";
	for(map<OPType,vector<OPType>>::iterator i=FollowSet.begin();i!=FollowSet.end();i++){
		cout<<i->first<<":"<<endl;
		for(auto j:i->second){
			cout<<j<<" ";
		}
		cout<<endl<<endl;
	}
}

void CalculateFirstSetOfSentence(Sentence* id){
	vector<OPType> TestList,SaveList;
	TestList.clear();
	SaveList.clear();
	SaveList.push_back(id->LeftOp);
	if(PtrMap[id->RightOp[0]]) TestList.push_back(id->RightOp[0]);
	else id->tempFirstSet.push_back(id->RightOp[0]);
	while(!TestList.empty()){
		for(auto i:SentenceList){
			if(i==id) continue;
			if(i->LeftOp==TestList.front()){
				if(!PtrMap[i->RightOp[0]]) id->tempFirstSet.push_back(i->RightOp[0]);
				else{
					if(!hasVal(SaveList,i->RightOp[0])){
						SaveList.push_back(i->RightOp[0]);
						if(!hasVal(TestList,i->RightOp[0])) TestList.push_back(i->RightOp[0]);
					}
				}
			}
		}
		TestList.erase(TestList.begin());
	}
}

void CalculateFirstSetOfPtr(OPType id){
	FirstSet[id].clear();
	if(!PtrMap[id]){
		FirstSet[id].push_back(id);
		return;
	}
	for(auto i:SentenceList){
		if(i->LeftOp==id){
			for(auto j:i->tempFirstSet){
				if(!hasVal(FirstSet[id],j)) FirstSet[id].push_back(j);
			}
		}
	}
	//vector<OPType>::iterator ite=unique(FirstSet[id].begin(),FirstSet[id].end());
	//FirstSet[id].erase(ite,FirstSet[id].end());
	//The Unique has no effect
}

void CalculateSpecificFollowSet(Sentence* i){
	if(PtrMap[i->RightOp[i->RightOp.size()-1]]){
		if(i->RightOp[i->RightOp.size()-1]!=i->LeftOp){
			for(auto j:FollowSet[i->LeftOp]){
				if(!hasVal(FollowSet[i->RightOp[i->RightOp.size()-1]],j)) FollowSet[i->RightOp[i->RightOp.size()-1]].push_back(j);
			}
		}
	}
	bool e_stand=true;
	int startindex=i->RightOp.size()-2,endindex=startindex+1;
	while(startindex>=0){
		if(PtrMap[i->RightOp[startindex]]){
			for(int j=startindex+1;j<=endindex;j++){
				for(auto k:FirstSet[i->RightOp[j]]){
					if(!hasVal(FollowSet[i->RightOp[startindex]],k) && k!=OP_epsilon) FollowSet[i->RightOp[startindex]].push_back(k);
				}
			}
			if(!hasVal(FirstSet[i->RightOp[startindex]],OP_epsilon)) endindex=startindex;
			if(e_stand && !hasVal(FirstSet[i->RightOp[startindex+1]],OP_epsilon)) e_stand=false;
			if(e_stand){
				for(auto j:FollowSet[i->LeftOp]){
					if(!hasVal(FollowSet[i->RightOp[startindex]],j)) FollowSet[i->RightOp[startindex]].push_back(j);
				}
			}
		}else{
			endindex=startindex;
			e_stand=false;
		}
		startindex--;
	}
}

void CalculateFollowSetOfPtr(Sentence* StartSentence){
	for(auto i:PtrMap) FollowSet[i.first].clear();
	vector<OPType> WaitPtrList;
	WaitPtrList.clear();
	for(auto i:StartSentence->RightOp){
		if(PtrMap[i]) WaitPtrList.push_back(i);
	}
	for(int it=0;it<WaitPtrList.size();it++){
		for(auto i:SentenceList){
			if(!hasVal(i->RightOp,WaitPtrList[it])) continue;
			for(auto j:i->RightOp){
    			if(PtrMap[j] && j!=WaitPtrList[it]){
    				if(!hasVal(WaitPtrList,j)) WaitPtrList.push_back(j);
				}
			}
			CalculateSpecificFollowSet(i);		
		}
		for(auto i:SentenceList){
			if(i->LeftOp!=WaitPtrList[it]) continue;
			for(auto j:i->RightOp){
    			if(PtrMap[j] && j!=WaitPtrList[it]){
    				if(!hasVal(WaitPtrList,j)) WaitPtrList.push_back(j);
				}
			}
			CalculateSpecificFollowSet(i);		
		}
	}
}

void ConstructLRTable(){
	//when parse and r -> tree construction in sentence
	LRSheet.clear();
	for(auto i:LRNodeList){
		for(auto j:i->NextNodeEdge){
			LRSheet[make_pair(j.first,i->id)].index.LR=j.second;
			if(!PtrMap[j.first]) LRSheet[make_pair(j.first,i->id)].type=LR_shift;
		}
	}
	for(auto i:LRNodeList){
		for(auto j:i->SentenceList){
			if(j->RightOp[j->RightOp.size()-1]==OP_LRSeperator){
				Sentence* tempSentence=new Sentence;
				tempSentence->LeftOp=j->LeftOp;
				for(auto k:j->RightOp){
					if(k!=OP_LRSeperator) tempSentence->RightOp.push_back(k);
				}
				for(auto k:SentenceList){
					if((*k)==(*tempSentence)){
						tempSentence=k;
						break;
					}
				}
				for(auto k:FollowSet[j->LeftOp]){
					LRSheet[make_pair(k,i->id)].type=LR_reduction;
					LRSheet[make_pair(k,i->id)].index.Sentence=tempSentence;
				}
			}
		}
		if(i->SentenceList.size()==1){
			Sentence* tempSentence=new Sentence;
			tempSentence->LeftOp=i->SentenceList[0]->LeftOp;
			for(auto j:i->SentenceList[0]->RightOp){
				if(j!=OP_LRSeperator) tempSentence->RightOp.push_back(j);
			}
			/*WRONG!!!
			bool find=false;
			for(auto j:SentenceList){
				if((*j)==(*tempSentence)){
					tempSentence=j;
					find=true;
				}
			}
			if(!find){*/
			if((*tempSentence)==(*StartSentence)){
				for(auto j:LRNodeList){
					for(auto k:j->NextNodeEdge){
						if(k.second->id==i->id){
							LRSheet[make_pair(OP_EOF,j->id)].type=LR_accept;
							//LRSheet[make_pair(OP_EOF,j->id)].index=(DWORD)NULL;
						}
					}
				}
			}
		}
	}
}

void SetupParser(){
	PtrMap.clear();
	LRNodeList.clear();
	SentenceAmount=0;
	LRNodeAmount=0;
	ConstructSentence();
	CreateLRNode(startPtr,SentenceList);
	FirstSet.clear();
	FollowSet.clear();
	for(auto i:SentenceList) CalculateFirstSetOfSentence(i);
	for(auto i:PtrMap) CalculateFirstSetOfPtr(i.first);
	ExamineStartSentence();
	CalculateFollowSetOfPtr(StartSentence);
	ConstructLRTable();
	#if (DEVPARSE==1)
		PrintLRNode();
		PrintFirstSetOfSentence();
		PrintFirstSet();
		PrintFollowSet();
		//PrintLRTable();
	#endif
}

void PrintParseNode(ASTNode* word){
	if(word->OPstatus==OP_EOF) printf("End Sign");
	else printf("text:'%s' at line:%d/col:%d",word->LexWord->value,word->LexWord->lineNo,word->LexWord->startColNo);
}

string OPWord(OPType Operator){
	map<OPType,string> OperatorStrings={
		#define OPenum(OP,STR) {OP,STR},
		#define OPenumWithNum(OP,STR,Num) {OP,STR},
			#include "SCLCOperator.h"
		#undef OPenum
		#undef OPenumWithNum
	};
	return OperatorStrings[Operator];
}

bool Parse(){
	#if (OUTTM==1)
		IRList.clear();	
		vector<vector<IRCode*>> codeStack;	
		codeStack.push_back(IRList);	
		VarList.clear();
		temp_offset=0;
	#endif
	vector<int> parseStack;
	parseStack.push_back(0);
	int current_wordId=0;
	ASTNode* current_word=ASTNodeList[current_wordId++];
	while(!parseStack.empty()){
		int current_state=parseStack.back();
		LRSheetNode current_symbol=LRSheet[make_pair(current_word->OPstatus,current_state)];
		switch(current_symbol.type){
			case LR_shift:{
				parseStack.push_back(current_symbol.index.LR->id);
				#if (DEVOUTPARSE==1)
					cout<<"indicator shift:"<<current_symbol.index.LR->id<<endl<<"The Stack is ";
					for(auto i:parseStack) cout<<i<<" ";
					cout<<endl<<"The past parse Node contains:";
					PrintParseNode(current_word);
					cout<<endl;
				#endif
				#if (OUTTM==1)
					vector<IRCode*> tempcode;
					IRCode* curr_code=new IRCode(IR_Temp,0,0,0);
					curr_code->ExtraInfo={(size_t)(current_word->LexWord)};
					tempcode.push_back(curr_code);
					codeStack.push_back(tempcode);
				#endif
				current_word=(current_wordId==ASTNodeList.size())?nullptr:ASTNodeList[current_wordId++];
				#if (DEVOUTPARSE==1)
					if(current_word){
						cout<<"The current parse Node contains:";
						PrintParseNode(current_word);
						cout<<endl;
					}else cout<<"No more parse Node!"<<endl;
					cout<<endl;
				#endif
				break;
			}
			case LR_reduction:{
				#if (DEVOUTPARSE==1)
					cout<<"indicator reduction:"<<OPWord(current_symbol.index.Sentence->LeftOp)<<" -> ";
					for(auto i:current_symbol.index.Sentence->RightOp) cout<<OPWord(i)<<" ";
					cout<<endl<<"The Stack is ";
					for(auto i:parseStack) cout<<i<<" ";
				#endif
				for(int i=0;i<current_symbol.index.Sentence->RightOp.size() && !parseStack.empty();i++) parseStack.pop_back();	
				current_state=parseStack.empty()?0:parseStack.back();
				parseStack.push_back(LRSheet[make_pair(current_symbol.index.Sentence->LeftOp,current_state)].index.LR->id);
				#if (DEVOUTPARSE==1)
					cout<<endl<<"The Stack(after reduction) is ";
					for(auto i:parseStack) cout<<i<<" ";
					cout<<endl<<"The current parse Node contains:";
					PrintParseNode(current_word);
					cout<<endl<<endl;
				#endif
				#if (OUTTM==1)
					vector<vector<IRCode*>> curr_codelist;
					for(int i=0;i<current_symbol.index.Sentence->RightOp.size() && !codeStack.empty();i++) {
						curr_codelist.insert(curr_codelist.begin(),codeStack.back());
						codeStack.pop_back();	
					}
					codeStack.push_back(current_symbol.index.Sentence->IRFuntion(curr_codelist));
				#endif
				break;
			}
			case LR_accept:{
				#if (DEVOUTPARSE==1)
					cout<<"indicator accpeted"<<endl;
				#endif
				#if (OUTTM==1)
					IRList=codeStack.back();
				#endif
				return true;
				break;
			}
			case LR_error:{
				char* errortext=new char[100];
				sprintf(errortext,"Parse Error with text:'%s' at line:%02d/col:%02d",(current_word->OPstatus==OP_EOF)?"EndSign":(current_word->LexWord->value),current_word->LexWord->lineNo,current_word->LexWord->startColNo);
				raise_dev_error(errortext);
				return false;
				break;
			}
			default:{
				char* errortext=new char[100];
				sprintf(errortext,"Invalid LR Type with text:'%s' at line:%d/col:%d",current_word->LexWord->value,current_word->LexWord->lineNo,current_word->LexWord->startColNo);
				raise_dev_error(errortext);
				exit(0);
			}
		}
	}
	cout<<"indicator invalid error!"<<endl;
	return false;
}

//toAST

//toIR
bool GenerateTM() {
	IRCompletion();
	TMCode="";
	for (auto i:IRList) TMCode+=i->ToString()+"\n";
	return true;
}

//tox86ASM

//toSPARC

//toELF

//toMIPS

//toJAVA

//mainFunc
void Setup(){
	SetupLexer();
	#if (DEVLEX==1)
		ExamineDFANode();
		//CANNOT TEST DFAjtlength<id
	#endif
	SetupParser();
}

template<typename... Args>
void ExaminedCompile(bool (*func)(Args...), Args... args){
	bool CompileStatus;
	CompileStatus=(*func)(args...);
	if(!CompileStatus){
		char* error_text=new char[20];
		sprintf(error_text,"Compile Failed!");
		raise_dev_error(error_text);
		exit(1);
	}
}

//mainConsole
int main(){
	Setup();
	string tempstr,str="";
	while(1){
		getline(cin,tempstr);
		if(tempstr=="quittest") break;
		str+=tempstr+"\n";
	}
	char* testtext=StrToChar(str);
	ExaminedCompile(Lex,testtext);
	
	SkipUselessToken();
	TranslateTKtoOP();
	AddEOFNode();
	ExaminedCompile(Parse);
	system("pause");
	ExaminedCompile(GenerateTM);
	cout<<TMCode;
}
//process:
/*
SetupLexer
SetupParser
SetupReg
*/