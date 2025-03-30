#ifndef IRenum
	#error "You must define IRenum macro before include this file"
#endif

#ifndef IRenumWithNum
	#error "You must define IRenum macro before include this file"
#endif

#ifdef IRenum
#ifdef IRenumWithNum
	IRenum(IR_Load,					"LD")	//R=M[d+S]
	IRenum(IR_Store,				"ST")	//M[d+S]=R
	IRenum(IR_In,					"IN")	//R=In
	IRenum(IR_Out,					"OUT")	//Out=R
	IRenum(IR_Halt,					"HALT")
	IRenum(IR_Add,					"ADD")	//R=S+T
	IRenum(IR_Sub,					"SUB")	//R=S-T
	IRenum(IR_Mul,					"MUL")	//R=S*T
	IRenum(IR_Div,					"DIV")	//R=S/T
	IRenum(IR_LoadA,				"LDA")	//R=d+S
	IRenum(IR_LoadC,				"LDC")	//R=d
	IRenum(IR_JumpIfLessThan,		"JLT")	//R<0 R[7]=d+S
	IRenum(IR_JumpIfLessEqual,		"JLE")	//R<=0 R[7]=d+S
	IRenum(IR_JumpIfGreatersThan,	"JGT")	//R>0 R[7]=d+S
	IRenum(IR_JumpIfGreaterEqual,	"JGE")	//R>=0 R[7]=d+S
	IRenum(IR_JumpIfEqual,			"JEQ")	//R==0 R[7]=d+S
	IRenum(IR_JumpIfNotEqual,		"JNE")	//R!=0 R[7]=d+S
	IRenum(IR_Temp,					"TMP")
#endif
#endif