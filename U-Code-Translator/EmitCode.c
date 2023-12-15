/********************************************************
 *         Code emitting functions                      *
 *                                       2003. 3. 18.   *
 ********************************************************/

#define numberOfOpcodes 40           // 35 + 4 + 1

typedef enum {
     notop, neg,   incop, decop, dup,  swp, add,  sub,   mult, divop,
	 modop, andop, orop,  gt,    lt,   ge,  le,   eq,    ne,
	 lod,   ldc,   lda,   ldi,   ldp,  str, sti,  ujp,   tjp,  fjp,
	 call,  ret,   retv,  chkh,  chkl, nop, proc, endop, bgn,  sym,
	 none
} opcode;

char *mnemonic[numberOfOpcodes] = {
     "notop", "neg",  "inc", "dec",  "dup", "swp",  "add", "sub",
	 "mult",  "div",  "mod", "and",  "or",  "gt",   "lt",  "ge",
	 "le",    "eq",   "ne",	 "lod",  "ldc", "lda",  "ldi", "ldp",
	 "str",   "sti",  "ujp", "tjp",  "fjp", "call", "ret", "retv",
	 "chkh",  "chkl", "nop", "proc", "end", "bgn",  "sym", "none"
};

void emit0(opcode op) {
    fprintf(ucodeFile, "           "); // 고정된 공백 출력
    fprintf(ucodeFile, "%-10s\n", mnemonic[op]); // 연산자의 문자열을 출력
}

void emit1(opcode op, int num) {
    fprintf(ucodeFile, "           "); // 고정된 공백 출력
    fprintf(ucodeFile, "%-10s %5d\n", mnemonic[op], num); // 연산자 문자열과 정수 출력
}

void emit2(opcode op, int base, int offset) {
    fprintf(ucodeFile, "           "); // 고정된 공백 출력
    fprintf(ucodeFile, "%-10s %5d %5d\n", mnemonic[op], base, offset); // 연산자 문자열과 두 정수 출력
}
void emit3(opcode op, int p1, int p2, int p3) {
    fprintf(ucodeFile, "           "); // 고정된 공백 출력
    fprintf(ucodeFile, "%-10s %5d %5d %5d\n", mnemonic[op], p1, p2, p3); // 연산자 문자열과 세 정수 출력
}

void emitLabel(char *label) {
    fprintf(ucodeFile, "%s", label); // 레이블 출력
    // 레이블 뒤의 공백 계산 및 출력
    int noBlanks = 12 - strlen(label);
    for (int i = 1; i < noBlanks; ++i)
        fprintf(ucodeFile, " ");
    fprintf(ucodeFile, "%-10s\n", mnemonic[nop]); // 'nop' 연산자 출력
}

void emitJump(opcode op, char *label) {
    fprintf(ucodeFile, "           %-10s %-10s\n", mnemonic[op], label); // 연산자와 레이블 출력
}


void emitSym(int base, int offset, int size) {
    fprintf(ucodeFile, "           %-10s %5d %5d %5d\n", mnemonic[sym], base, offset, size); // 'sym' 연산자와 정수 인자들 출력
}


void emitFunc(char *label, int base, int offset, int size) {
    fprintf(ucodeFile, "%s", label); // 함수 레이블 출력
    // 레이블 뒤의 공백 계산 및 출력
    int noBlanks = 12 - strlen(label);
    for (int i = 1; i < noBlanks; ++i)
        fprintf(ucodeFile, " ");
    fprintf(ucodeFile, "%-10s %5d %5d %5d\n", mnemonic[proc], base, offset, size); // 'proc' 연산자와 정수 인자들 출력
}

