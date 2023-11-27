#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// 파일 포인터 선언
FILE *astFile;                          // AST 파일
FILE *sourceFile;                       // 미니C 소스 프로그램 파일
FILE *ucodeFile;                        // ucode 파일

#include "Scanner.c"
#include "Parser.c"
#include "PrintTree.c"
// #include "src\EmitCode.c"
// #include "src\SymTab.c"

// 메인 함수
void main(int argc, char *argv[])
{
	char fileName[30]; // 파일 이름을 저장할 배열
	Node *root; // AST 루트 노드

	printf(" *** 미니 C 컴파일러 시작\n");
	if (argc != 2) { // 인자의 개수가 올바르지 않을 때 오류 처리
		exit(1);
	}
	strcpy(fileName, argv[1]); // 파일 이름 복사
	printf("   * 소스 파일 이름: %s\n", fileName);

	if ((sourceFile = fopen(fileName, "r")) == NULL) { // 소스 파일 열기 실패 시 오류 처리
		exit(1);
	}
	// AST 파일을 쓰기 모드로 열기, 파일 이름의 확장자를 .ast로 변경
	astFile = fopen(strcat(strtok(fileName, "."), ".ast"), "w");

	printf(" === 파서 시작\n");
    root = parser(); // 파싱 시작, AST 생성
	printTree(root, 0); // AST 출력
	printf(" *** 미니 C 컴파일러 종료\n");
} // 메인 함수 종료