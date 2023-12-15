#include <stddef.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <ctype.h>

#define SIZE 97
#define LABEL_SIZE 7
#define PS_SIZE 100  

int base = 1;
int offset = 1;
int width = 1;
int lvalue, rvalue;
int levelTable[PS_SIZE];
int avail = 0;
int top;
int stIdx = 0;

enum returnType {
	CONST_TYPE, VAR_TYPE, INT_TYPE, VOID_TYPE, FUNC_TYPE
};

typedef struct Table {
	char * symname;
	int index;
	int base;
	int offset;
	int width;
	int typeQualifier;
	int typeSpecifier;
	int initialValue;
} symt;

void delSymbol(symt *stptr);
symt symbolTable[SIZE];


void initSymbolTable()
{
	symt * hashTable;
	hashTable = (symt*)malloc(sizeof(symt)*SIZE);
	for (int i = 0; i < SIZE; i++)
	{
		hashTable[i].index = i;
	}
}

int lookup(char *name)
{
	for (int i = avail - 1; i >= 0; i--) {
		if (strcmp(symbolTable[i].symname, name)==0)
		{
			return i;
		}
	}
	return -1;
}

int insert(char *tokenValue, int typeSpecifier, int typeQualifier, int base, int offset, int width, int initialValue)
{
	int hashValue = 0, tokenSize;
	symt *stptr = NULL;
	stptr = &symbolTable[avail];

	stptr->symname = tokenValue;
	stptr->base = base;
	stptr->offset = offset;
	stptr->width = width;
	stptr->initialValue = initialValue;
	stptr->typeSpecifier = typeSpecifier;
	stptr->typeQualifier = typeQualifier;

	avail++;
	return hashValue;
}

void set()
{
	top++; base++;
	levelTable[top] = avail;
}

void reset()
{
	int start = 0;
	int end = 0;
	start = levelTable[top];
	end = levelTable[top - 1];
	for (int i = start; i > end; i--) {
		delSymbol(&symbolTable[i]);
		avail--;
	}
	top--;
	base--;
}

void delSymbol(symt *stptr)
{
	stptr->symname = NULL;
	stptr->base = 0;
	stptr->offset = 0;
	stptr->width = 0;
	stptr->typeQualifier = 0;
	stptr->typeSpecifier = 0;
	stptr->initialValue = 0;
}