#ifndef OPenum
	#error "You must define OPenum macro before include this file"
#endif

#ifndef OPenumWithNum
	#error "You must define OPenum macro before include this file"
#endif

#ifdef OPenum
#ifdef OPenumWithNum
	//literal
	OPenumWithNum(OP_NumberLiteral,	"NUM",1)
	
	OPenum(OP_less,					"LT")
	
	OPenum(OP_add,					"PLUS")
	OPenum(OP_sub,					"MINUS")
	OPenum(OP_mul,					"TIMES")
	OPenum(OP_div,					"PVER")
	OPenum(OP_equal,				"EQ")
	OPenum(OP_assign,				"ASSIGN")
	
	OPenum(OP_leftparen,			"LPAREN")
	OPenum(OP_rightparen,			"RPAREN")
	
	OPenum(OP_semicolon,			"SEMI")
	
	OPenum(OP_if,					"IF")
	OPenum(OP_then,					"THEN")
	OPenum(OP_else,					"ELSE")
	OPenum(OP_end,					"END")
	OPenum(OP_repeat,				"REPEAT")
	OPenum(OP_until,				"UNTIL")
	OPenum(OP_read,					"READ")
	OPenum(OP_write,				"WRITE")
	
	OPenum(OP_id,					"ID")


	
	//NextStatus
	OPenumWithNum(OP_S,				"_$Start",1024)
	OPenum(OP_STMTSEQ,				"_stmt_seq")
	OPenum(OP_STMT,					"_stmt")
	OPenum(OP_IFSTMT,				"_if_stmt")
	OPenum(OP_REPEATSTMT,			"_repeat_stmt")
	OPenum(OP_ASSIGNSTMT,			"_assign_stmt")
	OPenum(OP_READSTMT,				"_read_stmt")
	OPenum(OP_WRITESTMT,			"_write_stmt")
	OPenum(OP_EXP,					"_exp")
	OPenum(OP_SIMPLEEXP,			"_simple_exp")
	OPenum(OP_TERM,					"_term")
	OPenum(OP_FACTOR,				"_factor")
	
	//LR & Parse
	OPenum(OP_LRSeperator,"")
	OPenum(OP_epsilon,"")
	OPenum(OP_EOF,"")
#endif
#endif