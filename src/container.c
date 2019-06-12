#include "ccpr.h"


//VECTOR生成
VECTOR* CreateVector(void){
    VECTOR* vec = malloc(sizeof(VECTOR));
    if(vec){
        vec->data = malloc(sizeof(void*) * 16);
        vec->capacity = 16;
        vec->len = 0;
    }else{
        Error("VECTORのメモリ確保に失敗しました。メモリ不足かも");
    }
    return vec;
}
//要素を末尾に追加
void PushVector(VECTOR* vec, void* elem){
    if(vec){
        if(vec->capacity == vec->len){
            vec->capacity *= 2;
            vec->data = realloc(vec->data, sizeof(void*) * vec->capacity);
        }
        vec->data[vec->len++] = elem;
    }else{
        Error("VECTORがNULLです。メモリ不足かも");
    }
}

void Expect(int line, int expected, int actual){
    if(expected == actual){
        return;
    }else{
        fprintf(stderr, "%d: %d expected, but got %d\n", line, expected, actual);
        exit(1);
    }
}
void RunVectorTest(void){
    VECTOR* vec = CreateVector();
    Expect(__LINE__, 0, vec->len);

    for(long idx=0; idx < 100; ++idx){
        PushVector(vec, (void*)idx);
    }

    Expect(__LINE__, 100, vec->len);
    Expect(__LINE__, 128, vec->capacity);
    Expect(__LINE__, 0, (long)vec->data[0]);
    Expect(__LINE__, 50, (long)vec->data[50]);
    Expect(__LINE__, 99, (long)vec->data[99]);

    printf("OK\n");
}