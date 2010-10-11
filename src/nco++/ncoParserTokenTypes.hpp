#ifndef INC_ncoParserTokenTypes_hpp_
#define INC_ncoParserTokenTypes_hpp_

/* $ANTLR 2.7.7 (20100807): "ncoGrammer.g" -> "ncoParserTokenTypes.hpp"$ */

#ifndef CUSTOM_API
# define CUSTOM_API
#endif

#ifdef __cplusplus
struct CUSTOM_API ncoParserTokenTypes {
#endif
	enum {
		EOF_ = 1,
		NULL_NODE = 4,
		BLOCK = 5,
		ARG_LIST = 6,
		DMN_LIST = 7,
		DMN_ARG_LIST = 8,
		LMT_LIST = 9,
		VALUE_LIST = 10,
		FUNC_ARG = 11,
		LMT = 12,
		EXPR = 13,
		FEXPR = 14,
		POST_INC = 15,
		POST_DEC = 16,
		UTIMES = 17,
		PROP = 18,
		FOR2 = 19,
		NORET = 20,
		ATAN2 = 21,
		WHERE_ASSIGN = 22,
		MISS2ZERO = 23,
		VSORT = 24,
		VABS = 25,
		VSQR2 = 26,
		SEMI = 27,
		DEFDIM = 28,
		LPAREN = 29,
		NSTRING = 30,
		COMMA = 31,
		RPAREN = 32,
		WHILE = 33,
		BREAK = 34,
		CONTINUE = 35,
		IF = 36,
		ELSE = 37,
		WHERE = 38,
		ELSEWHERE = 39,
		PRINT = 40,
		VAR_ID = 41,
		ATT_ID = 42,
		LCURL = 43,
		RCURL = 44,
		FOR = 45,
		COLON = 46,
		LSQUARE = 47,
		DIM_ID = 48,
		RSQUARE = 49,
		DIVIDE = 50,
		DIM_MTD_ID = 51,
		CALL_REF = 52,
		DOT = 53,
		FUNC = 54,
		INC = 55,
		DEC = 56,
		LNOT = 57,
		PLUS = 58,
		MINUS = 59,
		TIMES = 60,
		CARET = 61,
		MOD = 62,
		FLTHAN = 63,
		FGTHAN = 64,
		LTHAN = 65,
		GTHAN = 66,
		GEQ = 67,
		LEQ = 68,
		EQ = 69,
		NEQ = 70,
		LAND = 71,
		LOR = 72,
		QUESTION = 73,
		ASSIGN = 74,
		PLUS_ASSIGN = 75,
		MINUS_ASSIGN = 76,
		TIMES_ASSIGN = 77,
		DIVIDE_ASSIGN = 78,
		FLOAT = 79,
		DOUBLE = 80,
		INT = 81,
		BYTE = 82,
		UBYTE = 83,
		SHORT = 84,
		USHORT = 85,
		UINT = 86,
		INT64 = 87,
		UINT64 = 88,
		N4STRING = 89,
		DIM_ID_SIZE = 90,
		NRootAST = 91,
		SHIFTL = 92,
		SHIFTR = 93,
		QUOTE = 94,
		DGT = 95,
		LPH = 96,
		LPHDGT = 97,
		XPN = 98,
		VAR_NM_QT = 99,
		BLASTOUT = 100,
		UNUSED_OPS = 101,
		WS = 102,
		CXX_COMMENT = 103,
		C_COMMENT = 104,
		NUMBER_DOT = 105,
		NUMBER = 106,
		VAR_ATT = 107,
		VAR_ATT_QT = 108,
		DIM_QT = 109,
		DIM_VAL = 110,
		INCLUDE = 111,
		LMT_DMN = 112,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
#endif /*INC_ncoParserTokenTypes_hpp_*/
