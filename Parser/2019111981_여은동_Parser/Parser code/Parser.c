#include "MiniC.tbl"                 /* Mini C table for appendix A */
//#define  NO_RULES    97            /* number of rules */
//#define  GOAL_RULE  (NO_RULES+1)   /* accept rule */
//#define  NO_SYMBOLS  85            /* number of grammar symbols */
//#define  NO_STATES  153            /* number of states */
#define  PS_SIZE    100              /* size of parsing stack */
#include <stdio.h>
#include <stdlib.h>
#include "Scanner.h"

void semantic(int); // 의미 분석 함수
void printToken(struct tokenType token); // 토큰 출력 함수
void dumpStack(); // 스택 덤프 함수
void errorRecovery(); // 오류 복구 함수

int errcnt = 0; // 오류 개수
int sp; // 스택 포인터
int stateStack[PS_SIZE]; // 상태 스택
int symbolStack[PS_SIZE]; // 심볼 스택

void parser()
{
	extern int parsingTable[NO_STATES][NO_SYMBOLS + 1];  // 구문 분석 테이블
    extern int leftSymbol[NO_RULES + 1], rightLength[NO_RULES + 1];  // 규칙 정보
    int entry, ruleNumber, lhs;  // 테이블 항목, 규칙 번호, 왼쪽 심볼
    int currentState;  // 현재 상태
    struct tokenType token;  // 토큰 타입

    sp = 0; stateStack[sp] = 0;  // 초기 상태 설정
    token = scanner();  // 첫 번째 토큰 읽기
    while (1) {
        currentState = stateStack[sp];  // 현재 상태 가져오기
        entry = parsingTable[currentState][token.number];  // 액션 결정
		if (entry > 0)  // 이동(shifting) 액션
		{
			sp++;
			if (sp > PS_SIZE) {
				printf("critical compiler error: parsing stack overflow");
				exit(1);
			}
			symbolStack[sp] = token.number;
			stateStack[sp] = entry;
			token = scanner();
		}
		else if (entry < 0) // 축소(reducing) 액션
		{
			ruleNumber = -entry;
			if (ruleNumber == GOAL_RULE) /* accept action */
			{
				if (errcnt == 0) printf(" *** valid source ***\n");
				else  printf(" *** error in source : %d\n", errcnt);
				return;
			}
			semantic(ruleNumber);
			sp = sp - rightLength[ruleNumber];
			lhs = leftSymbol[ruleNumber];
			currentState = parsingTable[stateStack[sp]][lhs];
			sp++;
			symbolStack[sp] = lhs;
			stateStack[sp] = currentState;
		}
		else {  // 오류 발생
            printf(" === error in source ===\n");  // 오류 메시지 출력
            errcnt++;  // 오류 카운트 증가
            printToken(token);  // 오류 토큰 출력
            dumpStack();  // 스택 상태 출력
            errorRecovery();  // 오류 복구 시도
            token = scanner();  // 다음 토큰 읽기
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