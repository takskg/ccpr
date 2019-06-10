#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//トークンの型を表す値
enum{
    TK_NUM=256, //整数
    TK_EOF      //入力の終わり
};

//トークンの型
typedef struct{
    int tokenType;  //トークンの型
    int value;      //tokenTypeがTK_NUMの場合の値
    char* input;    //トークン文字列（エラーメッセージ用）
} TOKEN;

//入力プログラム
char* userInput;

//トークナイズした結果のトークン配列
//とりあえずトークンは100個まで
TOKEN tokens[100];

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

//userInputが指してる文字列をトークンに分割してtokensに保存をする
void Tokenize(void){
    char* top = userInput;

    int idx=0;
    while(*top){
        //空白文字をスキップ
        if(isspace(*top)){
            ++top;
            continue;
        }

        //+ - 記号
        if(*top == '+' || *top == '-'){
            tokens[idx].tokenType = *top;
            tokens[idx].input = top;
            ++idx;
            ++top;
            continue;
        }

        //数値
        if(isdigit(*top)){
            tokens[idx].tokenType = TK_NUM;
            tokens[idx].input = top;
            tokens[idx].value = strtol(top, &top, 10);
            ++idx;
            continue;
        }

        ErrorAt(top, "トークナイズできません");
    }

    tokens[idx].tokenType = TK_EOF;
    tokens[idx].input = top;
}

int main(int argc, char **argv){
    if(argc != 2){
        Error("引数の個数が正しくありません");
        return 1;
    }

    //トークナイズする
    userInput = argv[1];
    Tokenize();

    //アセンブリの前半部分出力
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    //式の最初は数値でないとダメ
    if(tokens[0].tokenType != TK_NUM){
        ErrorAt(tokens[0].input, "数ではありません");
    }
    printf("  mov rax, %d\n", tokens[0].value);

    // `+ <数>`あるいは`- <数>`というトークンの並びを消費しつつ
    // アセンブリを出力
    int idx=1;
    while(tokens[idx].tokenType != TK_EOF){
        if(tokens[idx].tokenType == '+'){
            ++idx;
            if(tokens[idx].tokenType == TK_NUM){
                printf("  add rax, %d\n", tokens[idx].value);
            }else{
                ErrorAt(tokens[idx].input, "数ではありません");
            }
            ++idx;
            continue;
        }else if(tokens[idx].tokenType == '-'){
            ++idx;
            if(tokens[idx].tokenType == TK_NUM){
                printf("  sub rax, %d\n", tokens[idx].value);
            }else{
                ErrorAt(tokens[idx].input, "数ではありません");
            }
            ++idx;
            continue;
        }

        ErrorAt(tokens[idx].input, "予期しないトークンです");
    }

    printf("  ret\n");
    return 0;
}