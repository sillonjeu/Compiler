#define _CRT_SECURE_NO_WARNINGS

#include "MiniC.tbl"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define NO_KEYWORD 14
#define ID_LENGTH 12
#define  PS_SIZE    100              /* size of parsing stack */
#define numberOfOpcodes 40 
#define LABEL_SIZE 7


FILE* astFile;                          // AST file
FILE* sourceFile;                       // miniC source program
FILE* ucodeFile;                        // ucode file

int labelCount = 0;
int returnWithValue, lvalue;

struct tokenType {
	int number;
	union {
		char id[ID_LENGTH];
		int num;
	} value;
};

typedef struct nodeType {
	struct tokenType token;            	// ��ū ����
	enum { terminal, nonterm } noderep; // ����� ����
	struct nodeType* son;             	// ���� ��ũ
	struct nodeType* brother;         	// ������ ��ũ
	struct nodeType* father;		   	// ���� ��ũ
} Node;

typedef enum {
	notop, neg, incop, decop, dup, swp, add, sub, mult, divop,
	modop, andop, orop, gt, lt, ge, le, eq, ne,
	lod, ldc, lda, ldi, ldp, str, sti, ujp, tjp, fjp,
	call, ret, retv, chkh, chkl, nop, proc, endop, bgn, sym,
	none
} opcode;

char* mnemonic[numberOfOpcodes] = {
	 "notop", "neg",  "inc", "dec",  "dup", "swp",  "add", "sub",
	 "mult",  "div",  "mod", "and",  "or",  "gt",   "lt",  "ge",
	 "le",    "eq",   "ne",	 "lod",  "ldc", "lda",  "ldi", "ldp",
	 "str",   "sti",  "ujp", "tjp",  "fjp", "call", "ret", "retv",
	 "chkh",  "chkl", "nop", "proc", "end", "bgn",  "sym", "none"
};

// Symbol Table
int offset = 1;
int base = 1;
int sym_next = 0;
int level_top = 0;

enum TYPE {
	CONST_TYPE, VAR_TYPE, FUNC_TYPE
};

typedef struct SymbolTalbe {
	char* name;
	int typeSpecifier;
	int typeQualifier;
	int initialValue;
	int width;
	int base;
	int offset;
} SymbolTable;

SymbolTable symbolTable[PS_SIZE];
int levelTable[PS_SIZE];

void codeGen(Node* ptr);
void processDeclaration(Node* ptr);
void processFuncHeader(Node* ptr);
void processFunction(Node* ptr);
void icg_error(int n);
void processSimpleVariable(Node* ptr, int typeSpecifier, int typeQualifier);
void processArrayVariable(Node* ptr, int typeSpecifier, int typeQualifier);
void processStatement(Node* ptr);
void processOperator(Node* ptr);
void processCondition(Node* ptr);
void rv_emit(Node* ptr);
void genLabel(char* label);
int checkPredefined(Node* ptr);

void semantic(int);
void printToken(struct tokenType token);
void dumpStack();
void errorRecovery();

void printNode(Node* pt, int indent);
void printTree(Node* pt, int indent);

Node* buildNode(struct tokenType token);
Node* buildTree(int nodeNumber, int rhsLength);
int meaningfulToken(struct tokenType token);

int superLetter(char ch);
int superLetterOrDigit(char ch);
int getNumber(char firstCharacter);
int hexValue(char ch);
void lexicalError(int n);

void emit0(opcode op);
void emit1(opcode op, int num);
void emit2(opcode op, int base, int offset);
void emit3(opcode op, int p1, int p2, int p3);
void emitLabel(char* label);
void emitJump(opcode op, char* label);
void emitSym(int base, int offset, int size);
void emitFunc(char* label, int base, int offset, int size);

void initSymbolTable();
int lookup(char* name);
int insert(char* tokenValue, int typeSpecifier, int typeQualifier, int base, int offset, int width, int initialValue);
void set();
void reset();
void delSymbol(SymbolTable* ptr);

enum tsymbol {
	tnull = -1,
	tnot, tnotequ, tremainder, tremAssign, tident, tnumber,
	/* 0          1           2          3            4           5      */
	tand, tlparen, trparen, tmul, tmulAssign, tplus,
	/* 6          7           8          9           10          11     */
	tinc, taddAssign, tcomma, tminus, tdec, tsubAssign,
	/* 12         13          14         15          16          17     */
	tdiv, tdivAssign, tcolon, tsemicolon, tless, tlesse,
	/* 18         19          20         21          22          23     */
	tassign, tequal, tgreat, tgreate, tlbracket, trbracket,
	/* 24         25          26         27          28          29     */
	teof, tbreak, tcase, tconst, tcontinue, tdefault,
	/* 30         31          32         33          34          35     */
	tdo, telse, tfor, tif, tint, treturn,
	/* 36         37          38         39          40          41     */
	tswitch, tvoid, twhile, tlbrace, tor, trbrace
	/* 42         43          44         45          46          47     */
};



char* tokenName[] = {
	"!",        "!=",      "%",       "%=",     "%ident",   "%number",
	/* 0          1           2         3          4          5        */
	"&&",       "(",       ")",       "*",      "*=",       "+",
	/* 6          7           8         9         10         11       */
	"++",       "+=",      ",",       "-",      "--",      "-=",
	/* 12         13         14        15         16         17       */
	"/",        "/=",      ":",       ";",      "<",       "<=",
	/* 18         19         20        21         22         23       */
	"=",        "==",      ">",       ">=",     "[",       "]",
	/* 24         25         26        27         28         29       */
	"_|_",      "break",   "case",    "const",  "continue", "default",
	/* 30         31         32        33         34         35       */
	"do",       "else",    "for",     "if",     "int",      "return",
	/* 36         37         38        39         40         41       */
	"switch",   "void",    "while",   "{",      "||",       "}"
	/* 42         43         44        45         46         47       */
};

char* keyword[NO_KEYWORD] = {
	"break",   "case",    "const",  "continue", "default", "do",   "else",
	"for",     "if",      "int",    "return",   "switch",  "void", "while"
};


enum tsymbol tnum[NO_KEYWORD] = {
	tbreak,    tcase,     tconst,   tcontinue,  tdefault,  tdo,     telse,
	tfor,      tif,       tint,     treturn,    tswitch,   tvoid,   twhile
};


enum nodeNumber {
	ERROR_NODE, 
	ACTUAL_PARAM, ADD, ADD_ASSIGN, ARRAY_VAR, ASSIGN_OP, BREAK_ST, CALL,
	CASE_ST, COMPOUND_ST, CONDITION_PART, CONST_NODE, CONTINUE_ST, DCL,
	DCL_ITEM, DCL_LIST, DCL_SPEC, DEFAULT_ST, DIV, DIV_ASSIGN, DO_WHILE_ST,
	EQ, ERROR, EXP_ST, FORMAL_PARA, FOR_ST, FUNC_DEF, FUNC_HEAD, GE, GT, IDENT,
	IF_ELSE_ST, IF_ST, INDEX, INIT_PART, INT_TYPE, LE, LOGICAL_AND,
	LOGICAL_NOT, LOGICAL_OR, LT, MOD_ASSIGN, MUL, MUL_ASSIGN, NE, NUMBER,
	PARAM_DCL, POST_DEC, POST_INC, POST_PART, PRE_DEC, PRE_INC, PROGRAM,
	REMAINDER, RETURN_ST, SIMPLE_VAR, STAT_LIST, SUB, SUB_ASSIGN, SWITCH_ST,
	UNARY_MINUS, VOID_TYPE, WHILE_ST
};

char* nodeName[] = {
   "ERROR_NODE",
  "ACTUAL_PARAM",  "ADD",  "ADD_ASSIGN",  "ARRAY_VAR",  "ASSIGN_OP",  "BREAK_ST",  "CALL",
  "CASE_ST",  "COMPOUND_ST",  "CONDITION_PART",  "CONST_NODE",  "CONTINUE_ST",  "DCL",
  "DCL_ITEM",  "DCL_LIST",  "DCL_SPEC",  "DEFAULT_ST",  "DIV",  "DIV_ASSIGN",  "DO_WHILE_ST",
  "EQ",  "ERROR",  "EXP_ST",  "FORMAL_PARA",  "FOR_ST",  "FUNC_DEF",  "FUNC_HEAD",  "GE",  "GT",  "IDENT",
  "IF_ELSE_ST",  "IF_ST",  "INDEX",  "INIT_PART",  "INT_TYPE",  "LE",  "LOGICAL_AND",
  "LOGICAL_NOT",  "LOGICAL_OR",  "LT",  "MOD_ASSIGN",  "MUL",  "MUL_ASSIGN",  "NE",  "NUMBER",
  "PARAM_DCL",  "POST_DEC",  "POST_INC",  "POST_PART",  "PRE_DEC",  "PRE_INC",  "PROGRAM",
  "REMAINDER",  "RETURN_ST",  "SIMPLE_VAR",  "STAT_LIST",  "SUB",  "SUB_ASSIGN",  "SWITCH_ST",
  "UNARY_MINUS",  "VOID_TYPE",  "WHILE_ST" };

// ������ Ȯ��Ǿ��� ��� ruleName ���� Ȯ��� ������ �ݿ��Ǿ�� ��.
int ruleName[] = {
	/* 0            1            2            3           4           */
	   0,           PROGRAM,     0,           0,          0,
	   /* 5            6            7            8           9           */
		  0,           FUNC_DEF,    FUNC_HEAD,   DCL_SPEC,   0,
		  /* 10           11           12           13          14          */
			 0,           0,           0,           CONST_TYPE, INT_TYPE,
			 /* 15           16           17           18          19          */
				VOID_TYPE,   0,           FORMAL_PARA, 0,          0,
				/* 20           21           22           23          24          */
				   0,           0,           PARAM_DCL,   COMPOUND_ST, DCL_LIST,
				   /* 25           26           27           28          29          */
					  DCL_LIST,    0,           0,           DCL,        0,
					  /* 30           31           32           33          34          */
						 0,           DCL_ITEM,    DCL_ITEM,    SIMPLE_VAR, ARRAY_VAR,
						 /* 35           36           37           38          39          */
							0,           0,           STAT_LIST,   0,          0,
							/* 40           41           42           43          44          */
							   0,           0,           0,           0,          0,
							   /* 45           46           47           48          49          */
								  0,           0,      0,           0,          0,
								  /* 50           51           52           53          54          */
									 0,			 EXP_ST,		0,			0,          CASE_ST,
									 /* 55           56           57           58          59          */
										DEFAULT_ST,   CONTINUE_ST,  BREAK_ST,  IF_ST,	IF_ELSE_ST,
										/* 60           61           62           63          64          */
										   WHILE_ST,  DO_WHILE_ST,  SWITCH_ST,	FOR_ST,		INIT_PART,
										   /* 65           66           67           68          69          */
										CONDITION_PART,   POST_PART,  RETURN_ST,      0,          0,
										/* 70           71           72           73          74          */
										   ASSIGN_OP,          ADD_ASSIGN,          SUB_ASSIGN,          MUL_ASSIGN,DIV_ASSIGN,
										   /* 75           76           77           78          79          */
											  MOD_ASSIGN,         0,           LOGICAL_OR,         0,        LOGICAL_AND,
											  /* 80           81           82           83          84          */
												 0,           EQ,			 NE,			0,			GT,
												 /* 85           86           87           88          89          */
													LT,          GE,			 LE,		0,			ADD,
													/* 90           91           92           93          94          */
													   SUB,           0,           MUL		,DIV,	REMAINDER,
													   /* 95           96           97                                   */
														  0,			UNARY_MINUS,			LOGICAL_NOT,			PRE_INC,			PRE_DEC, // 99
														  0,			INDEX,				CALL,					POST_INC,			POST_DEC, // 104
														  0,			0,						ACTUAL_PARAM,			0,					0,		  // 109
														  0, 0, 0

};


int sp;                               // stack pointer
int stateStack[PS_SIZE];              // state stack
int symbolStack[PS_SIZE];             // symbol stack
Node* valueStack[PS_SIZE];            // value stack

struct tokenType scanner()
{
	struct tokenType token;
	int i, index;
	char ch, id[ID_LENGTH];

	token.number = tnull;

	do {
		while (isspace(ch = fgetc(sourceFile)));	// state 1: skip blanks
		if (superLetter(ch)) { // identifier or keyword
			i = 0;
			do {
				if (i < ID_LENGTH) id[i++] = ch;
				ch = fgetc(sourceFile);
			} while (superLetterOrDigit(ch));
			if (i >= ID_LENGTH) lexicalError(1);
			id[i] = '\0';
			ungetc(ch, sourceFile);  //  retract
			// find the identifier in the keyword table
			for (index = 0; index < NO_KEYWORD; index++)
				if (!strcmp(id, keyword[index])) break;
			if (index < NO_KEYWORD)    // found, keyword exit
				token.number = tnum[index];
			else {                     // not found, identifier exit
				token.number = tident;
				strcpy(token.value.id, id);
			}
		}  // end of identifier or keyword
		else if (isdigit(ch)) {  // number
			token.number = tnumber;
			token.value.num = getNumber(ch);
		}
		else switch (ch) {  // special character
		case '/':
			ch = fgetc(sourceFile);
			if (ch == '*')			// text comment
				do {
					while (ch != '*') ch = fgetc(sourceFile);
					ch = fgetc(sourceFile);
				} while (ch != '/');
			else if (ch == '/')		// line comment
				while (fgetc(sourceFile) != '\n');
			else if (ch == '=')  token.number = tdivAssign;
			else {
				token.number = tdiv;
				ungetc(ch, sourceFile); // retract
			}
			break;
		case '!':
			ch = fgetc(sourceFile);
			if (ch == '=')  token.number = tnotequ;
			else {
				token.number = tnot;
				ungetc(ch, sourceFile); // retract
			}
			break;
		case '%':
			ch = fgetc(sourceFile);
			if (ch == '=') {
				token.number = tremAssign;
			}
			else {
				token.number = tremainder;
				ungetc(ch, sourceFile);
			}
			break;
		case '&':
			ch = fgetc(sourceFile);
			if (ch == '&')  token.number = tand;
			else {
				lexicalError(2);
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '*':
			ch = fgetc(sourceFile);
			if (ch == '=')  token.number = tmulAssign;
			else {
				token.number = tmul;
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '+':
			ch = fgetc(sourceFile);
			if (ch == '+')  token.number = tinc;
			else if (ch == '=') token.number = taddAssign;
			else {
				token.number = tplus;
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '-':
			ch = fgetc(sourceFile);
			if (ch == '-')  token.number = tdec;
			else if (ch == '=') token.number = tsubAssign;
			else {
				token.number = tminus;
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '<':
			ch = fgetc(sourceFile);
			if (ch == '=') token.number = tlesse;
			else {
				token.number = tless;
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '=':
			ch = fgetc(sourceFile);
			if (ch == '=')  token.number = tequal;
			else {
				token.number = tassign;
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '>':
			ch = fgetc(sourceFile);
			if (ch == '=') token.number = tgreate;
			else {
				token.number = tgreat;
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '|':
			ch = fgetc(sourceFile);
			if (ch == '|')  token.number = tor;
			else {
				lexicalError(3);
				ungetc(ch, sourceFile);  // retract
			}
			break;

		case '_':
			ch = fgetc(sourceFile);
			if (ch == '|') {
				ch = fgetc(sourceFile);
				if (ch == '_') token.number = teof;
				else lexicalError(5);
			}
			else {
				ungetc(ch, sourceFile); // retract
				lexicalError(4);
			}
			break;

		case '(': token.number = tlparen;         break;
		case ')': token.number = trparen;         break;
		case ',': token.number = tcomma;          break;
		case ':': token.number = tcolon;          break;
		case ';': token.number = tsemicolon;      break;
		case '[': token.number = tlbracket;       break;
		case ']': token.number = trbracket;       break;
		case '{': token.number = tlbrace;         break;
		case '}': token.number = trbrace;         break;
		case EOF: token.number = teof;            break;
		default: {
			printf("Current character : %c", ch);
			lexicalError(4);
			break;
		}

		} // switch end
	} while (token.number == tnull);
	return token;
} // end of scanner

void lexicalError(int n)
{
	printf(" *** Lexical Error : ");
	switch (n) {
	case 1: printf("an identifier length must be less than 12.\n");
		break;
	case 2: printf("next character must be &\n");
		break;
	case 3: printf("next character must be |\n");
		break;
	case 4: printf("invalid character\n");
		break;
	}
}

int superLetter(char ch)
{
	if (isalpha(ch) || ch == '_') return 1;
	else return 0;
}

int superLetterOrDigit(char ch)
{
	if (isalnum(ch) || ch == '_') return 1;
	else return 0;
}

int getNumber(char firstCharacter)
{
	int num = 0;
	int value;
	char ch;

	if (firstCharacter == '0') {
		ch = fgetc(sourceFile);
		if ((ch == 'X') || (ch == 'x')) {		// hexa decimal
			while ((value = hexValue(ch = fgetc(sourceFile))) != -1)
				num = 16 * num + value;
		}
		else if ((ch >= '0') && (ch <= '7'))	// octal
			do {
				num = 8 * num + (int)(ch - '0');
				ch = fgetc(sourceFile);
			} while ((ch >= '0') && (ch <= '7'));
		else num = 0;						// zero
	}
	else {									// decimal
		ch = firstCharacter;
		do {
			num = 10 * num + (int)(ch - '0');
			ch = fgetc(sourceFile);
		} while (isdigit(ch));
	}
	ungetc(ch, sourceFile);  /*  retract  */
	return num;
}

int hexValue(char ch)
{
	switch (ch) {
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		return (ch - '0');
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
		return (ch - 'A' + 10);
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
		return (ch - 'a' + 10);
	default: return -1;
	}
}

Node* parser()
{
	extern int parsingTable[NO_STATES][NO_SYMBOLS + 1];
	extern int leftSymbol[NO_RULES + 1], rightLength[NO_RULES + 1];
	int entry, ruleNumber, lhs;
	int currentState;
	struct tokenType token;
	Node* ptr;

	sp = 0; stateStack[sp] = 0;  // initial state
	token = scanner();
	while (1) {
		currentState = stateStack[sp];
		entry = parsingTable[currentState][token.number];
		if (entry > 0)                          /* shift action */
		{
			sp++;
			if (sp > PS_SIZE) {
				printf("critical compiler error: parsing stack overflow");
				exit(1);
			}
			symbolStack[sp] = token.number;
			stateStack[sp] = entry;
			valueStack[sp] = meaningfulToken(token) ? buildNode(token) : NULL;
			token = scanner();
		}
		else if (entry < 0)                   /* reduce action */
		{
			ruleNumber = -entry;
			if (ruleNumber == GOAL_RULE) /* accept action */
			{
				//                      printf(" *** valid source ***\n");
				return valueStack[sp - 1];
			}
			//                 semantic(ruleNumber);
			ptr = buildTree(ruleName[ruleNumber], rightLength[ruleNumber]);
			sp = sp - rightLength[ruleNumber];
			lhs = leftSymbol[ruleNumber];
			currentState = parsingTable[stateStack[sp]][lhs];
			sp++;
			symbolStack[sp] = lhs;
			stateStack[sp] = currentState;
			valueStack[sp] = ptr;
		}
		else                               /* error action */
		{
			printf(" === error in source ===\n");
			printf("Current Token : ");
			printToken(token);
			dumpStack();
			errorRecovery();
			token = scanner();
		}
	} /* while (1) */
} /* parser */
void semantic(int n)
{
	printf("reduced rule number = %d\n", n);
}

void dumpStack()
{
	int i, start;

	if (sp > 10) start = sp - 10;
	else start = 0;

	printf("\n *** dump state stack :");
	for (i = start; i <= sp; ++i)
		printf(" %d", stateStack[i]);

	printf("\n *** dump symbol stack :");
	for (i = start; i <= sp; ++i)
		printf(" %d", symbolStack[i]);
	printf("\n");
}

void printToken(struct tokenType token)
{
	if (token.number == tident)
		printf("%s", token.value.id);
	else if (token.number == tnumber)
		printf("%d", token.value.num);
	else
		printf("%s", tokenName[token.number]);

}

void errorRecovery()
{
	struct tokenType tok;
	int parenthesisCount, braceCount;
	int i;

	// step 1: skip to the semicolon
	parenthesisCount = braceCount = 0;
	while (1) {
		tok = scanner();
		if (tok.number == teof) exit(1);
		if (tok.number == tlparen) parenthesisCount++;
		else if (tok.number == trparen) parenthesisCount--;
		if (tok.number == tlbrace) braceCount++;
		else if (tok.number == trbrace) braceCount--;
		if ((tok.number == tsemicolon) && (parenthesisCount <= 0) && (braceCount <= 0))
			break;
	}

	// step 2: adjust state stack
	for (i = sp; i >= 0; i--) {
		// statement_list ->  statement_list .  statement
		if (stateStack[i] == 36) break;	 // second statement part

		// statement_list ->  .  statement
		// statement_list ->  .  statement_list statement
		if (stateStack[i] == 24) break;	 // first statement part

		// declaration_list ->  declaration_list .  declaration
		if (stateStack[i] == 25) break;  // second internal dcl

		// declaration_list ->  .  declaration
		// declaration_list ->  .  declaration_list declaration
		if (stateStack[i] == 17) break;  // internal declaration

		// external declaration
		// external_dcl ->  .  declaration
		if (stateStack[i] == 2) break;	// after first external dcl
		if (stateStack[i] == 0) break;	// first external declaration
	}
	sp = i;
}

int meaningfulToken(struct tokenType token)
{
	if ((token.number == tident) || (token.number == tnumber))
		return 1;
	else return 0;
}

Node* buildNode(struct tokenType token)
{
	Node* ptr;
	ptr = (Node*)malloc(sizeof(Node));
	if (!ptr) {
		printf("malloc error in buildNode()\n");
		exit(1);
	}
	ptr->token = token;
	ptr->noderep = terminal;
	ptr->son = ptr->brother = NULL;
	return ptr;
}

Node* buildTree(int nodeNumber, int rhsLength)
{
	int i, j, start;
	Node* first, * ptr;

	i = sp - rhsLength + 1;
	/* step 1: find a first index with node in value stack */
	while (i <= sp && valueStack[i] == NULL) i++;
	if (!nodeNumber && i > sp) return NULL;
	start = i;
	/* step 2: linking brothers */
	while (i <= sp - 1) {
		j = i + 1;
		while (j <= sp && valueStack[j] == NULL) j++;
		if (j <= sp) {
			ptr = valueStack[i];
			while (ptr->brother) ptr = ptr->brother;
			ptr->brother = valueStack[j];
		}
		i = j;
	}
	first = (start > sp) ? NULL : valueStack[start];
	/* step 3: making subtree root and linking son */
	if (nodeNumber) {
		ptr = (Node*)malloc(sizeof(Node));
		if (!ptr) {
			printf("malloc error in buildTree()\n");
			exit(1);
		}
		ptr->token.number = nodeNumber;
		ptr->noderep = nonterm;
		ptr->son = first;
		ptr->brother = NULL;
		return ptr;
	}
	else return first;
}

void printTree(Node* pt, int indent)
{
	Node* p = pt;
	while (p != NULL) {
		printNode(p, indent);
		if (p->noderep == nonterm) printTree(p->son, indent + 5);
		p = p->brother;
	}
}

void printNode(Node* pt, int indent)
{
	extern FILE* astFile;
	int i;

	for (i = 1; i <= indent; i++) fprintf(astFile, " ");
	if (pt->noderep == terminal) {
		if (pt->token.number == tident)
			fprintf(astFile, " Terminal: %s", pt->token.value.id);
		else if (pt->token.number == tnumber)
			fprintf(astFile, " Terminal: %d", pt->token.value.num);
	}
	else { // nonterminal node
		i = (int)(pt->token.number);
		fprintf(astFile, " Nonterminal: %s", nodeName[i]);
	}
	fprintf(astFile, "\n");
}

void main(int argc, char* argv[])
{
	char fileName[30];
	Node* root;

	printf(" *** start of Mini C Compiler\n");
	if (argc != 2) {
		icg_error(1);
		exit(1);
	}
	strcpy(fileName, argv[1]);
	printf("   * source file name: %s\n", fileName);

	if ((sourceFile = fopen(fileName, "r")) == NULL) {
		icg_error(2);
		exit(1);
	}
	astFile = fopen(strcat(strtok(fileName, "."), ".ast"), "w");
	ucodeFile = fopen(strcat(strtok(fileName, "."), ".uco"), "w");

	printf(" === start of Parser\n");
	root = parser();
	printTree(root, 0);
	printf(" === start of ICG\n");
	codeGen(root);
	printf(" *** end   of Mini C Compiler\n");
} // end of main

//void main()
//{
//	char fileName[30] = "ext.mc";  // 고정 파일 이름
//	Node* root;
//
//	printf(" *** start of Mini C Compiler\n");
//	printf("   * source file name: %s\n", fileName);
//
//	if ((sourceFile = fopen(fileName, "r")) == NULL) {
//		icg_error(2);
//		exit(1);
//	}
//	astFile = fopen(strcat(strtok(fileName, "."), ".ast"), "w");
//	ucodeFile = fopen(strcat(strtok(fileName, "."), ".uco"), "w");
//
//	printf(" === start of Parser\n");
//	root = parser();
//	printTree(root, 0);
//	printf(" === start of ICG\n");
//	codeGen(root);
//	printf(" *** end   of Mini C Compiler\n");
//} // end of main


void initSymbolTable() {
	for (int i = 0; i < PS_SIZE; i++) {
		symbolTable[i].name = NULL;
		symbolTable[i].typeSpecifier = 0;
		symbolTable[i].typeQualifier = 0;
		symbolTable[i].initialValue = 0;
		symbolTable[i].width = 0;
		symbolTable[i].base = 0;
		symbolTable[i].offset = 0;
	}

	sym_next = 0;
}

int lookup(char* name) {
	for (int i = 0; i < PS_SIZE; i++) {
		if (strcmp(symbolTable[i].name, name) == 0)
			return i;
	}
	return -1;
}

int insert(char* tokenValue, int typeSpecifier, int typeQualifier, int base, int offset, int width, int initialValue) {
	SymbolTable* ptr = &symbolTable[sym_next];

	ptr->name = tokenValue;
	ptr->typeSpecifier = typeSpecifier;
	ptr->typeQualifier = typeQualifier;
	ptr->base = base;
	ptr->offset = offset;
	ptr->width = width;
	ptr->initialValue = initialValue;

	return sym_next++;
}

void set() {
	level_top++;
	base++;
	levelTable[level_top] = sym_next;
}

void reset() {
	if (level_top > 0) {
		int startLevelIndex = levelTable[level_top];
		int endLevelIndex = levelTable[level_top - 1];

		for (int i = startLevelIndex; i > endLevelIndex; i--) {
			sym_next--;
			delSymbol(&symbolTable[i]);
		}

		level_top--;
		base--;
	}
}

void delSymbol(SymbolTable* ptr) {
	ptr->name = NULL;
	ptr->typeSpecifier = 0;
	ptr->typeQualifier = 0;
	ptr->initialValue = 0;
	ptr->width = 0;
	ptr->base = 0;
	ptr->offset = 0;
}


void codeGen(Node* ptr)
{
	Node* p;
	int globalSize;

	initSymbolTable();
	// first, process the declaration part
	for (p = ptr->son; p; p = p->brother) {
		if (p->token.number == DCL) processDeclaration(p->son);
		else if (p->token.number == FUNC_DEF) processFuncHeader(p->son);
		else icg_error(3);
	}

	//	dumpSymbolTable();
	globalSize = offset - 1;
	//	printf("size of global variables = %d\n", globalSize);

	// genSym(base);

	// second, process the function part
	for (p = ptr->son; p; p = p->brother)
		if (p->token.number == FUNC_DEF) processFunction(p);
	//	if (!mainExist) warningmsg("main does not exist");

		// generate codes for start routine
		//          bgn    globalSize
		//			ldp
		//          call    main
		//          end
	emit1(bgn, globalSize);
	emit0(ldp);
	emitJump(call, "main");
	emit0(endop);
}

void icg_error(int n)
{
	printf("icg_error: %d\n", n);
	//3:printf("A Mini C Source file must be specified.!!!\n");
	//"error in DCL_SPEC"
	//"error in DCL_ITEM"
}

void processDeclaration(Node* ptr)
{
	int typeSpecifier, typeQualifier;
	Node* p, * q;

	if (ptr->token.number != DCL_SPEC)
		icg_error(4);

	//	printf("processDeclaration\n");
		// 1. process DCL_SPEC
	typeSpecifier = INT_TYPE;		// default type
	typeQualifier = VAR_TYPE;
	p = ptr->son;
	while (p) {
		if (p->token.number == INT_TYPE)    typeSpecifier = INT_TYPE;
		else if (p->token.number == CONST_NODE)  typeQualifier = CONST_TYPE;
		else { // AUTO, EXTERN, REGISTER, FLOAT, DOUBLE, SIGNED, UNSIGNED
			printf("not yet implemented\n");
			return;
		}
		p = p->brother;
	}

	// 2. process DCL_ITEM
	p = ptr->brother;
	//if (p->token.number != DCL_ITEM)
	//	icg_error(5);

	while (p) {
		q = p->son;    // SIMPLE_VAR or ARRAY_VAR

		switch (q->token.number) {
		case SIMPLE_VAR: {		// simple variable
			processSimpleVariable(q, typeSpecifier, typeQualifier);
			break;
		}
		case ARRAY_VAR: {		// one dimensional array
			processArrayVariable(q, typeSpecifier, typeQualifier);
			break;
		}
		default: printf("error in SIMPLE_VAR or ARRAY_VAR\n");
			break;
		} // end switch
		p = p->brother;
	} // end while
}

void processSimpleVariable(Node* ptr, int typeSpecifier, int typeQualifier)
{
	int stIndex, width, initialValue;
	int sign = 1;
	Node* p = ptr->son;          // variable name(=> identifier)
	Node* q = ptr->brother;      // initial value part

	if (ptr->token.number != SIMPLE_VAR)
		printf("error in SIMPLE_VAR\n");

	if (typeQualifier == CONST_TYPE) {		// constant type
		if (q == NULL) {
			printf("%s must have a constant value\n", ptr->son->token.value.id);
			return;
		}
		if (q->token.number == UNARY_MINUS) {
			sign = -1;
			q = q->son;
		}
		initialValue = sign * q->token.value.num;

		stIndex = insert(p->token.value.id, typeSpecifier, typeQualifier,
			0/*base*/, 0/*offset*/, 0/*width*/, initialValue);
	}
	else {  // variable type
		width = 1;
		stIndex = insert(p->token.value.id, typeSpecifier, typeQualifier,
			base, offset, width, 0);
		offset++;
	}
}

void processArrayVariable(Node* ptr, int typeSpecifier, int typeQualifier)
{
	int stIndex, size;
	Node* p = ptr->son;          // variable name(=> identifier)

	if (ptr->token.number != ARRAY_VAR) {
		printf("error in ARRAY_VAR\n");
		return;
	}
	if (p->brother == NULL)			// no size
		printf("array size must be specified\n");
	else size = p->brother->token.value.num;

	stIndex = insert(p->token.value.id, typeSpecifier, typeQualifier,
		base, offset, size, 0);
	offset += size;
}

void processFuncHeader(Node* ptr)
{
	int noArguments, returnType;
	int stIndex;
	Node* p;

	//	printf("processFuncHeader\n");
	if (ptr->token.number != FUNC_HEAD)
		printf("error in processFuncHeader\n");

	// 1. process the function return type
	p = ptr->son->son;
	while (p) {
		if (p->token.number == INT_TYPE) returnType = INT_TYPE;
		else if (p->token.number == VOID_TYPE) returnType = VOID_TYPE;
		else printf("invalid function return type\n");
		p = p->brother;
	}

	// 2. process formal parameters
	p = ptr->son->brother->brother;		// FORMAL_PARA
	p = p->son;							// PARAM_DCL

	noArguments = 0;
	while (p) {
		noArguments++;
		p = p->brother;
	}

	// 3. insert the function name
	stIndex = insert(ptr->son->brother->token.value.id, returnType, FUNC_TYPE,
		1/*base*/, 0/*offset*/, noArguments/*width*/, 0/*initialValue*/);
	//	if (!strcmp("main", functionName)) mainExist = 1;

}

void processFunction(Node* ptr)
{
	int paraType, noArguments;
	int typeSpecifier, returnType;
	int p1, p2, p3;
	int stIndex;
	Node* p, * q;
	char* functionName;
	//	int i, j;

	//	printf("processFunction\n");
	if (ptr->token.number != FUNC_DEF)
		printf("error in processFunction\n");

	// set symbol table for the function
	set();

	// 1. process formal parameters
	p = ptr->son->son->brother->brother->son; 
	for (; p; p = p->brother) { 
		if (p->token.number == PARAM_DCL) {
			processDeclaration(p->son);
		}
	}

	// 2. process the declaration part in function body
	p = ptr->son->brother->son->son;		
	for (; p; p = p->brother) { 
		if (p->token.number == DCL) {
			processDeclaration(p->son);
		}
	}

	// 3. emit the function start code
		// fname       proc      p1 p2 p3
		// p1 = size of local variables + size of arguments
		// p2 = block number
		// p3 = lexical level
	p1 = offset - 1;
	p2 = p3 = base;
	functionName = ptr->son->son->brother->token.value.id;
	emitFunc(functionName, p1, p2, p3);


	//	dumpSymbolTable();
	// genSym(base);

	// 4. process the statement part in function body
	p = ptr->son->brother->son->brother;	// STAT_LIST
	returnWithValue = 0;
	processStatement(p);

	// 5. check if return type and return value
	stIndex = lookup(functionName);
	if (stIndex == -1) return;
	returnType = symbolTable[stIndex].typeSpecifier;
	if ((returnType == VOID_TYPE) && returnWithValue)
		printf("void return type must not return a value\n");
	if ((returnType == INT_TYPE) && !returnWithValue)
		printf("int return type must return a value\n");

	// 6. generate the ending codes
	if (!returnWithValue) emit0(ret);
	emit0(endop);

	// reset symbol table
	reset();
}

void processStatement(Node* ptr)
{
	Node* p;

	if (ptr == NULL) return;		// null statement

	switch (ptr->token.number) {
	case COMPOUND_ST:
		p = ptr->son->brother;		// STAT_LIST
		p = p->son;
		while (p) {
			processStatement(p);
			p = p->brother;
		}
		break;
	case EXP_ST:
		if (ptr->son != NULL) processOperator(ptr->son);
		break;
	case RETURN_ST:
		if (ptr->son != NULL) {
			returnWithValue = 1;
			p = ptr->son;
			if (p->noderep == nonterm)
				processOperator(p); // return value
			else rv_emit(p);
			emit0(retv);
		}
		else
			emit0(ret);
		break;
	case IF_ST:
	{
		char label[LABEL_SIZE];

		genLabel(label);
		processCondition(ptr->son);				// condition
		emitJump(fjp, label);
		processStatement(ptr->son->brother);	// true part
		emitLabel(label);
	}
	break;
	case IF_ELSE_ST:
	{
		char label1[LABEL_SIZE], label2[LABEL_SIZE];

		genLabel(label1);
		genLabel(label2);
		processCondition(ptr->son);				// condition
		emitJump(fjp, label1);
		processStatement(ptr->son->brother);	// true part
		emitJump(ujp, label2);
		emitLabel(label1);
		processStatement(ptr->son->brother->brother);	// false part
		emitLabel(label2);
	}
	break;
	case WHILE_ST:
	{
		char label1[LABEL_SIZE], label2[LABEL_SIZE];

		genLabel(label1);
		genLabel(label2);
		emitLabel(label1);
		processCondition(ptr->son);				// condition
		emitJump(fjp, label2);
		processStatement(ptr->son->brother);	// loop body
		emitJump(ujp, label1);
		emitLabel(label2);
	}
	break;
	default:
		printf("not yet implemented.\n");
		break;
	} //end switch
}

void genLabel(char* label)
{
	sprintf(label, "$$%d", labelCount++);
}

void processCondition(Node* ptr)
{
	if (ptr->noderep == nonterm) processOperator(ptr);
	else rv_emit(ptr);
}

void rv_emit(Node* ptr)
{
	int stIndex;

	if (ptr->token.number == tnumber)		// number
		emit1(ldc, ptr->token.value.num);
	else {									// identifier
		stIndex = lookup(ptr->token.value.id);
		if (stIndex == -1) return;
		if (symbolTable[stIndex].typeQualifier == CONST_TYPE)		// constant
			emit1(ldc, symbolTable[stIndex].initialValue);
		else if (symbolTable[stIndex].width > 1)					// array var
			emit2(lda, symbolTable[stIndex].base, symbolTable[stIndex].offset);
		else														// simple var
			emit2(lod, symbolTable[stIndex].base, symbolTable[stIndex].offset);
	}
}

void processOperator(Node* ptr)
{
	int stIndex;

	if (ptr->noderep == terminal) {
		printf("illegal expression\n");
		return;
	}

	switch (ptr->token.number) {
	case ASSIGN_OP:
	{
		Node* lhs = ptr->son, * rhs = ptr->son->brother;

		// generate instructions for left-hane side if INDEX node.
		if (lhs->noderep == nonterm) {		// index variable
			lvalue = 1;
			processOperator(lhs);
			lvalue = 0;
		}

		// generate instructions for right-hane side
		if (rhs->noderep == nonterm) processOperator(rhs);
		else rv_emit(rhs);

		// generate a store instruction
		if (lhs->noderep == terminal) {		// simple variable
			stIndex = lookup(lhs->token.value.id);
			if (stIndex == -1) {
				printf("undefined variable : %s\n", lhs->token.value.id);
				return;
			}
			emit2(str, symbolTable[stIndex].base, symbolTable[stIndex].offset);
		}
		else								// array variable
			emit0(sti);
		break;
	}
	case ADD_ASSIGN: case SUB_ASSIGN: case MUL_ASSIGN: case DIV_ASSIGN:
	case MOD_ASSIGN:
	{
		Node* lhs = ptr->son, * rhs = ptr->son->brother;
		int nodeNumber = ptr->token.number;

		ptr->token.number = ASSIGN_OP;
		if (lhs->noderep == nonterm) {	// code generation for left hand side
			lvalue = 1;
			processOperator(lhs);
			lvalue = 0;
		}
		ptr->token.number = nodeNumber;
		if (lhs->noderep == nonterm)	// code generation for repeating part
			processOperator(lhs);
		else rv_emit(lhs);
		if (rhs->noderep == nonterm) 	// code generation for right hand side
			processOperator(rhs);
		else rv_emit(rhs);

		switch (ptr->token.number) {
		case ADD_ASSIGN: emit0(add);	break;
		case SUB_ASSIGN: emit0(sub);	break;
		case MUL_ASSIGN: emit0(mult);	break;
		case DIV_ASSIGN: emit0(divop);	break;
		case MOD_ASSIGN: emit0(modop);	break;
		}
		if (lhs->noderep == terminal) {	// code generation for store code
			stIndex = lookup(lhs->token.value.id);
			if (stIndex == -1) {
				printf("undefined variable : %s\n", lhs->son->token.value.id);
				return;
			}
			emit2(str, symbolTable[stIndex].base, symbolTable[stIndex].offset);
		}
		else
			emit0(sti);
		break;
	}
	/*
		// logical operators(new computation of and/or operators: 2001.10.21)
		case AND: case OR:
			{
				Node *lhs = ptr->son, *rhs = ptr->son->brother;
				char label[LABEL_SIZE];

				genLabel(label);

				if (lhs->noderep == nonterm) processOperator(lhs);
					else rv_emit(lhs);
				emit0(dup);

				if (ptr->token.number == AND) emitJump(fjp, label);
				else if (ptr->token.number == OR) emitJump(tjp, label);

				// pop the first operand and push the second operand
				emit1(popz, 15);	// index 15 => swReserved7(dummy)
				if (rhs->noderep == nonterm) processOperator(rhs);
					else rv_emit(rhs);

				emitLabel(label);
				break;
			}
	*/
	// arithmetic operators
	case ADD: case SUB: case MUL: case DIV: case REMAINDER:
		// relational operators
	case EQ:  case NE: case GT: case LT: case GE: case LE:
		// logical operators
	case LOGICAL_AND: case LOGICAL_OR:
	{
		Node* lhs = ptr->son, * rhs = ptr->son->brother;

		if (lhs->noderep == nonterm) processOperator(lhs);
		else rv_emit(lhs);
		if (rhs->noderep == nonterm) processOperator(rhs);
		else rv_emit(rhs);
		switch (ptr->token.number) {
		case ADD: emit0(add);	break;			// arithmetic operators
		case SUB: emit0(sub);	break;
		case MUL: emit0(mult);	break;
		case DIV: emit0(divop);	break;
		case REMAINDER: emit0(modop);	break;
		case EQ:  emit0(eq);	break;			// relational operators
		case NE:  emit0(ne);	break;
		case GT:  emit0(gt);	break;
		case LT:  emit0(lt);	break;
		case GE:  emit0(ge);	break;
		case LE:  emit0(le);	break;
		case LOGICAL_AND: emit0(andop);	break;	// logical operators
		case LOGICAL_OR: emit0(orop);	break;
		}
		break;
	}
	// unary operators
	case UNARY_MINUS: case LOGICAL_NOT:
	{
		Node* p = ptr->son;

		if (p->noderep == nonterm) processOperator(p);
		else rv_emit(p);
		switch (ptr->token.number) {
		case UNARY_MINUS: emit0(neg);
			break;
		case LOGICAL_NOT: emit0(notop);
			break;
		}
		break;
	}
	// increment/decrement operators
	case PRE_INC: case PRE_DEC: case POST_INC: case POST_DEC:
	{
		Node* p = ptr->son;
		Node* q;
		int stIndex;
		int amount = 1;

		if (p->noderep == nonterm) processOperator(p);		// compute value
		else rv_emit(p);

		q = p;
		while (q->noderep != terminal) q = q->son;
		if (!q || (q->token.number != tident)) {
			printf("increment/decrement operators can not be applied in expression\n");
			break;
		}
		stIndex = lookup(q->token.value.id);
		if (stIndex == -1) return;

		switch (ptr->token.number) {
		case PRE_INC: emit0(incop);
			//							  if (isOperation(ptr)) emit0(dup);
			break;
		case PRE_DEC: emit0(decop);
			//							  if (isOperation(ptr)) emit0(dup);
			break;
		case POST_INC:
			//							   if (isOperation(ptr)) emit0(dup);
			emit0(incop);
			break;
		case POST_DEC:
			//							   if (isOperation(ptr)) emit0(dup);
			emit0(decop);
			break;
		}
		if (p->noderep == terminal) {
			stIndex = lookup(p->token.value.id);
			if (stIndex == -1) return;
			emit2(str, symbolTable[stIndex].base, symbolTable[stIndex].offset);
		}
		else if (p->token.number == INDEX) {				// compute index
			lvalue = 1;
			processOperator(p->son);
			lvalue = 1;
			emit0(swp);
			emit0(sti);
		}
		else printf("error in prefix/postfix operator\n");
		break;
	}
	case INDEX:
	{
		Node* indexExp = ptr->son->brother;

		if (indexExp->noderep == nonterm) processOperator(indexExp);
		else rv_emit(indexExp);
		stIndex = lookup(ptr->son->token.value.id);
		if (stIndex == -1) {
			printf("undefined variable : %s\n", ptr->son->token.value.id);
			return;
		}
		emit2(lda, symbolTable[stIndex].base, symbolTable[stIndex].offset);
		emit0(add);
		if (!lvalue) emit0(ldi);	// rvalue
		break;
	}
	case CALL:
	{
		Node* p = ptr->son;		// function name
		char* functionName;
		int stIndex;
		int noArguments;

		if (checkPredefined(p)) // library functions
			break;

		// handle for user function
		functionName = p->token.value.id;
		stIndex = lookup(functionName);
		if (stIndex == -1) break;			// undefined function !!!
		noArguments = symbolTable[stIndex].width;

		emit0(ldp);
		p = p->brother;			// ACTUAL_PARAM
		while (p) {				// processing actual arguments
			if (p->noderep == nonterm) processOperator(p);
			else rv_emit(p);
			noArguments--;
			p = p->brother;
		}
		if (noArguments > 0) printf("%s: too few actual arguments", functionName);
		if (noArguments < 0) printf("%s: too many actual arguments", functionName);
		emitJump(call, ptr->son->token.value.id);
		break;
	}
	} //end switch
}

int checkPredefined(Node* ptr)
{
	char* functionName;
	int stIndex;

	functionName = ptr->token.value.id;

	if (!strcmp(functionName, "read")) {	// read procedure : call by address
		stIndex = lookup(ptr->brother->token.value.id);
		if (stIndex == -1) return 1;
		emit0(ldp);
		emit2(lda, symbolTable[stIndex].base, symbolTable[stIndex].offset);
		emitJump(call, "read");
		return 1;
	}
	if (!strcmp(functionName, "write")) {	// write procedure
		emit0(ldp);
		if (ptr->brother->noderep == nonterm) processOperator(ptr->brother);
		else rv_emit(ptr->brother);
		emitJump(call, "write");
		return 1;
	}
	return 0;
}

void emit0(opcode op)
{
	fprintf(ucodeFile, "           ");
	fprintf(ucodeFile, "%-10s\n", mnemonic[op]);
}

void emit1(opcode op, int num)
{
	fprintf(ucodeFile, "           ");
	fprintf(ucodeFile, "%-10s %5d\n", mnemonic[op], num);
}

void emit2(opcode op, int base, int offset)
{
	fprintf(ucodeFile, "%-10s %5d %5d\n", mnemonic[op], base, offset);
}
void emit3(opcode op, int p1, int p2, int p3)
{
	fprintf(ucodeFile, "           %-10s %5d %5d %5d\n", mnemonic[op], p1, p2, p3);
}

void emitLabel(char* label)
{
	int i, noBlanks;

	fprintf(ucodeFile, "%s", label);
	noBlanks = 12 - strlen(label);
	for (i = 1; i < noBlanks; ++i)
		fprintf(ucodeFile, " ");
	fprintf(ucodeFile, "%-10s\n", mnemonic[nop]);

}

void emitJump(opcode op, char* label)
{
	fprintf(ucodeFile, "           %-10s %-10s\n", mnemonic[op], label);
}

void emitSym(int base, int offset, int size)
{
	fprintf(ucodeFile, "           ");
	fprintf(ucodeFile, "%-10s %5d %5d %5d\n", mnemonic[sym], base, offset, size);
}

void emitFunc(char* label, int base, int offset, int size)
{
	int i, noBlanks;

	fprintf(ucodeFile, "%s", label);
	noBlanks = 12 - strlen(label);
	for (i = 1; i < noBlanks; ++i)
		fprintf(ucodeFile, " ");
	fprintf(ucodeFile, "%-10s", mnemonic[proc]);
	fprintf(ucodeFile, " ");
	fprintf(ucodeFile, "%5d %5d %5d\n", base, offset, size);
}

