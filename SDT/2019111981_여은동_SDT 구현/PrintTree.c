// AST를 출력하는 함수들
void printNode(Node *pt, int indent);  // 노드를 출력하는 함수
void printTree(Node *pt, int indent);  // 트리를 출력하는 함수

// 트리를 출력하는 함수의 구현
void printTree(Node *pt, int indent)
{
       Node *p = pt;
       while (p != NULL) {
           printNode(p, indent);  // 현재 노드를 출력
           if (p->noderep == nonterm) printTree(p->son, indent+5);  // 비터미널 노드의 자식도 출력
           p = p->brother;  // 형제 노드로 이동
       }
}

// 노드를 출력하는 함수의 구현
void printNode(Node *pt, int indent)
{
       extern FILE * astFile;  // AST를 기록할 파일
       int i;

       for (i=1; i<=indent; i++) fprintf(astFile," ");  // 들여쓰기를 파일에 출력
       if (pt->noderep == terminal) {  // 터미널 노드인 경우
          if (pt->token.number == tident)
            fprintf(astFile," Terminal: %s", pt->token.value.id);  // 식별자 출력
          else if (pt->token.number == tnumber)
            fprintf(astFile," Terminal: %d", pt->token.value.num);  // 숫자 출력
       }
       else { // 비터미널 노드인 경우
              i = (int) (pt->token.number);
              fprintf(astFile," Nonterminal: %s", nodeName[i]);  // 비터미널 노드 이름 출력
       }
       fprintf(astFile, "\n");  // 줄바꿈 출력
}
