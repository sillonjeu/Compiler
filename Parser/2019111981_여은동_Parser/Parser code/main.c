#include <stdio.h>   
#include <stdlib.h>  
#include "Scanner.h" 

FILE *sourceFile;    // 파싱할 파일을 가리키는 파일 포인터

int main(int argc, char *argv[]) {
    // 프로그램이 실행될 때 전달되는 명령줄 인수의 개수(argc)와 그 값을 저장하는 배열(argv)
    
    if (argc != 2) {
        // 사용자가 파일 이름을 정확히 하나만 입력했는지 확인
        printf("Usage: %s <filename>\n", argv[0]);  
        return 1;  
    }

    sourceFile = fopen(argv[1], "r");  // 사용자가 지정한 파일을 읽기 모드로 열기
    if (sourceFile == NULL) {
        
        perror("Error opening file");  
        return 1;  
    }

    parser();  // 파일 파싱 수행 

    fclose(sourceFile);  
    return 0;  
}