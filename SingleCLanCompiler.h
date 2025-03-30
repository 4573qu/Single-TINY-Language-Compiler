#ifndef _SCLC_H_
#define _SCLC_H_

#include <bits/stdc++.h>
#include "SCLCLexer.h"
#include "SCLCParser.h"

using namespace std;
#define DEV 0
#define DEVLEX 0
#define DEVPARSE 0
#define DEVOUT 1
#define DEVOUTLEX 1
#define DEVOUTPARSE 1
#define OUTTM 1
#define startPtr OP_S

#if (DEV==1)
	#undef DEVLEX
	#undef DEVPARSE
	#define DEVLEX 1
	#define DEVPARSE 1
#endif
#if (DEVOUT==1)
	#undef DEVOUTLEX
	#undef DEVOUTPARSE
	#define DEVOUTLEX 1
	#define DEVOUTPARSE 1
#endif

//enums
enum PreprocessType{
	PR_define,
	PR_include,
	PR_pragma,
	PR_error,
	PR_ifdef,
	PR_endif,
	PR_ifndef,
	PR_elif,
	PR_undef,
	PR_line,
	PR_if,
	PR_else,
	PR_defined,
	PR_lineno,
	PR_file,
	PR_date,
	PR_time,
	PR_stdc,
	PR_eval
};
/*preprocess:(preprocess_file(file))
clear whitespace(or just skip tab and space at DFA?
link multi-line
test if start with “#”
*/

//global variables
extern vector<ASTNode*> ASTNodeList;
extern vector<IRCode*> IRList;
extern string TMCode;
extern vector<char*> VarList;
extern int temp_offset;

void raise_dev_error(char* messagetext);
void raise_error(int pos,char* messagetext);
char* StrToChar(string s);
void ConstructSentence();

template<class T>
bool hasVal(vector<T> stl,T target);
#endif