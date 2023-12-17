#include <stddef.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <ctype.h>

#define SIZE 97 // 심볼 테이블의 크기
#define LABEL_SIZE 7 // 레이블의 크기
#define PS_SIZE 100  

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
