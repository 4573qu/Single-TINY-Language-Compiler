//Syntaxer Director Emitter
//Interpreter
#ifndef _SCLC_SYNTAXER_
#define _SCLC_SYNTAXER_

#include <bits/stdc++.h>
using namespace std;

enum SheetOperation {
	SO_CREATE,
	SO_DELETE,
	SO_UPDATE,
	SO_SELECT,
};

bool TypeDefinite();

bool VariableDefinite();

#endif