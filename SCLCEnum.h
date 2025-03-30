#ifdef _ENUM_PREPROCESS_

#endif

#ifdef _ENUM_TOKEN_
	enum DFAToken{
		#define TKenum(TK,STR) TK,
		#define TKenumWithNum(TK,STR,Num) TK=Num,
			#include "SCLCToken.h"
		#undef TKenum
		#undef TKenumWithNum
	};
#endif

#ifdef _ENUM_OPERATOR_
	enum OPType{
		#define OPenum(OP,STR) OP,
		#define OPenumWithNum(OP,STR,Num) OP=Num,
			#include "SCLCOperator.h"
		#undef OPenum
		#undef OPenumWithNum
	};
#endif

#ifdef _ENUM_LRTYPE_
	enum LRSheetType{
		LR_reduction,
		LR_shift,
		LR_accept,
		LR_error
	};
#endif

#ifdef _ENUM_IRTYPE_
	enum IRType{
		#define IRenum(IR,Code) IR,
		#define IRenumWithNum(IR,Code,Num) IR=Num,
			#include "SCLCIRCodes.h"
		#undef IRenum
		#undef IRenumWithNum	
	};
#endif