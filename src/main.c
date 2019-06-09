#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
    if(argc != 2){
        fprintf(stderr, "引数の個数が正しくありません。");
        return 1;
    }

    char* argPtr = argv[1];
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    printf("  mov rax, %ld\n", strtol(argPtr, &argPtr, 10));

    while(*argPtr){
        if(*argPtr == '+'){
            ++argPtr;
            printf("  add rax, %ld\n", strtol(argPtr, &argPtr, 10));
            continue;
        }else if(*argPtr == '-'){
            ++argPtr;
            printf("  sub rax, %ld\n", strtol(argPtr, &argPtr, 10));
            continue;
        }

        printf("想定してない文字です。コンパイルできません。文字:%c\n", *argPtr);
        return 1;
    }

    printf("  ret\n");
    return 0;
}