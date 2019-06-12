#include "ccpr.h"


//-----------------------------------
//関数
//-----------------------------------

// エラーを報告するための関数
// printfと同じ引数を取る
void Error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// エラー箇所を報告するための関数
void ErrorAt(char *loc, char *msg) {
    int pos = loc - userInput;
    fprintf(stderr, "%s\n", userInput);
    fprintf(stderr, "%*s", pos, ""); // pos個の空白を出力
    fprintf(stderr, "^ %s\n", msg);
    exit(1);
}



int main(int argc, char **argv){
    if(argc != 2){
        Error("引数の個数が正しくありません");
        return 1;
    }

    userInput = argv[1];

    //テスト実行
    if(strcmp(userInput, "-t") == 0 || strcmp(userInput, "--test") == 0){
        RunVectorTest();
        exit(0);
    }

    //トークンベクタ生成
    tokenVec = CreateVector();

    //トークナイズしてパースする
    Tokenize();
    tokenIdx = 0;
    NODE* node = Expr();

    //アセンブリの前半部分出力
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    //抽象構文木を下りながらコード生成
    Gen(node);

    //スタックトップに式全体の値が残ってるはずなので
    //RAXにロードして関数からの戻り値とする
    printf("  pop rax\n");
    printf("  ret\n");
    return 0;
}