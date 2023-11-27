#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NO_KEYWORD 14
#define ID_LENGTH 12

// 토큰의 종류를 나타내는 enum 선언
enum tsymbol {
	tnull = -1,
	tnot,		tnotequ,	tremainder, tremAssign, tident,		tnumber,
	/* 0          1           2          3            4           5      */
	tand,		tlparen,	trparen,	tmul,		tmulAssign, tplus,
	/* 6          7           8          9           10          11     */
	tinc,		taddAssign, tcomma,		tminus,		tdec,		tsubAssign,
	/* 12         13          14         15          16          17     */
	tdiv,		tdivAssign, tcolon,		tsemicolon, tless,		tlesse,
	/* 18         19          20         21          22          23     */
	tassign,	tequal,		tgreat,		tgreate,	tlbracket, trbracket,
	/* 24         25          26         27          28          29     */
	teof,		tbreak,		tcase,		tconst,		tcontinue, tdefault,
	/* 30         31          32         33          34          35     */
	tdo,		telse,		tfor,		tif,		tint,		treturn,
	/* 36         37          38         39          40          41     */
	tswitch,	tvoid,		twhile,		tlbrace,	tor,		trbrace
	/* 42         43          44         45          46          47     */
};

// 토큰 이름 배열 선언
extern char *tokenName[];

// 키워드 배열 선언
extern char *keyword[NO_KEYWORD];

// 키워드에 해당하는 토큰 번호 배열 선언
extern enum tsymbol tnum[NO_KEYWORD];

// tokenType 구조체 선언
struct tokenType {
    int number;
    union {
        char id[ID_LENGTH];
        int num;
    } value;
};

// scanner 함수 선언
struct tokenType scanner();
extern FILE *sourceFile;

#endif // SCANNER_H