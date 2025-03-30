#ifndef TKenum
	#error "You must define TKenum macro before include this file"
#endif

#ifndef TKenumWithNum
	#error "You must define TKenum macro before include this file"
#endif

#ifdef TKenum
#ifdef TKenumWithNum
	TKenumWithNum(TK_Start,				"TK_Start",0)
	
	TKenum(TK_NumberLiteral,			"NUM")
	
	TKenum(TK_less,						"LT")
	
	TKenum(TK_add,						"PLUS")
	TKenum(TK_sub,						"MINUS")
	TKenum(TK_mul,						"TIMES")
	TKenum(TK_div,						"PVER")
	TKenum(TK_equal,					"EQ")
	TKenum(TK_assign,					"ASSIGN")
	
	TKenum(TK_leftparen,				"LPAREN")
	TKenum(TK_rightparen,				"RPAREN")
	
	TKenum(TK_semicolon,				"SEMI")
	
	TKenum(TK_if,						"IF")
	TKenum(TK_then,						"THEN")
	TKenum(TK_else,						"ELSE")
	TKenum(TK_end,						"END")
	TKenum(TK_repeat,					"REPEAT")
	TKenum(TK_until,					"UNTIL")
	TKenum(TK_read,						"READ")
	TKenum(TK_write,					"WRITE")
	
	TKenum(TK_id,						"ID")
	
	TKenumWithNum(TK_incompleteId,		"ERROR",512)
	
	TKenumWithNum(TK_UselessToken,		"USELESS",1024)//space tab enter//skip comment as well
	TKenum(TK_Comment,					"COMMENT")
	
	TKenumWithNum(TK_Err,				"ERROR",2048)
#endif
#endif