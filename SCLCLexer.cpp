#include "SingleCLanCompiler.h"
using namespace std;

int getNextStateInGraph(int state,unsigned char word){
	if(!DFAjt[state][word].data) return -1;//no raise_error;
	return DFAjt[state][word].data->nodeId;
}

bool getTypeInGraph(int state){
	if(DFAMap.find(state)==DFAMap.end()) return false;//add raise_error;
	return DFAMap[state]->isEndPtr;
}

DFAToken getCategoryInGraph(int state){
	if(DFAMap.find(state)==DFAMap.end()) return TK_Err;//add raise_error;
	return DFAMap[state]->status;
}

string TKWord(DFAToken Token){
	map<DFAToken,string> TokenStrings={
		#define TKenum(TK,STR) {TK,STR},
		#define TKenumWithNum(TK,STR,Num) {TK,STR},
			#include "SCLCToken.h"
		#undef TKenum
		#undef TKenumWithNum
	};
	return TokenStrings[Token];
}

bool Lex(char* input){
	LexWordExp.clear();
	int current_state=0,start_index=0,current_index=0,currline=0,currcolumn=0,line=0,column=0;
	while(current_index<strlen(input) || (current_index==strlen(input) && current_state!=0)){
		int next_state=getNextStateInGraph(current_state,input[current_index]);
		if(next_state==-1){
			if(getTypeInGraph(current_state)){
				DFAToken currToken=getCategoryInGraph(current_state);
				if(currToken!=TK_Err){
					#if (DEVOUTLEX==1)
						cout<<"Detected Word:"<<endl<<"'";
					#endif
					char* tempword=new char[current_index-start_index+1];
					for(int i=start_index;i<current_index;i++){
						#if (DEVOUTLEX==1)
							cout<<input[i];
						#endif
						tempword[i-start_index]=input[i];
					}
					tempword[current_index-start_index]='\0';
					WordExpression* currWordExp=new WordExpression(currToken,tempword,currline,currcolumn);
					#if (DEVOUTLEX==1)
						DFAWord=TKWord(getCategoryInGraph(current_state));
						printf("\'\nDFAnodeid=%03d->DFAtokenid=%*s at line=%02d/col=%02d\n\n",current_state,DFAWordMaxLength,DFAWord.c_str(),currline,currcolumn);
					#endif
					currline=line;
					currcolumn=column;
					LexWordExp.push_back(currWordExp);
					start_index=current_index;
					current_state=0;
				}else{
					cout<<"Undefined Word!"<<endl;//if raise_err? Or raise In getCategoryInGraph? I like the first one.
					start_index=current_index;
					current_state=0;
				}
			}else{
				if(current_index<strlen(input)){
			    	cout<<"Err:State at "<<current_state<<" with word as '"<<input[current_index]<<"' at line "<<line<<" & col "<<column<<endl;
			    	return false;
			    	//raise_error
			    }
				break;
			}
		}else{
			current_state=next_state;
			if(input[current_index]=='\n'){
				line++;
				column=-1;
			}
			current_index++;
			column++;
		}
    }
    return true;
}

void SkipUselessToken(){
	for(vector<WordExpression*>::iterator i=LexWordExp.begin();i!=LexWordExp.end();i++){
		WordExpression* temp=*i;
		if(temp->status==TK_UselessToken || temp->status==TK_Comment){
			LexWordExp.erase(i);		
			i--;	
		}
	}
}