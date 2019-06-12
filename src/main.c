#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


//-----------------------------------
//マクロ
//-----------------------------------

#define CAST_TOKEN_PTR(ptr) ((TOKEN*)(ptr))

//-----------------------------------
//前方宣言
//-----------------------------------


//-----------------------------------
//enum
//-----------------------------------

//トークンの型を表す値
enum{
    TK_NUM=256, //整数
    TK_EQ,      //==
    TK_NE,      //!=
    TK_LE,      //<=
    TK_GE,      //>=
    TK_EOF      //入力の終わり
};
//ノードの型を表す値
enum{
    ND_NUM=256, //整数のノード型
};


//-----------------------------------
//構造体
//-----------------------------------

//トークンの型
typedef struct TOKEN{
    int tokenType;  //トークンの型
    int value;      //tokenTypeがTK_NUMの場合の値
    char* input;    //トークン文字列（エラーメッセージ用）
} TOKEN;
//ノード
typedef struct NODE{
    int nodeType;       //ノード型
    struct NODE* lhs;   //左辺
    struct NODE* rhs;   //右辺
    int value;          //nodeTypeがND_NUMの場合の値
} NODE;
//ベクタ
typedef struct VECTOR{
    void** data;    //データ
    int capacity;   //バッファサイズ
    int len;        //要素数
} VECTOR;


//-----------------------------------
//グローバル
//-----------------------------------

//入力プログラム
char* userInput;

//トークナイズした結果のトークン
VECTOR* tokenVec;
int tokenIdx;


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

//トークンの作成
TOKEN* CreateToken(void){
    TOKEN* token = malloc(sizeof(TOKEN));
    memset(token, 0, sizeof(TOKEN));
    return token;
}


//ノード作成
NODE* CreateNode(int nodeType, NODE* lhs, NODE* rhs){
    NODE* node = malloc(sizeof(NODE));

    node->nodeType = nodeType;
    node->lhs = (struct NODE*)lhs;
    node->rhs = (struct NODE*)rhs;
    node->value = 0;

    return node;
}
//数値のノード作成
NODE* CreateNumNode(int value){
    NODE* node = malloc(sizeof(NODE));

    node->nodeType = ND_NUM;
    node->lhs = NULL;
    node->rhs = NULL;
    node->value = value;

    return node;
}

bool Consume(int tokenType){
    if(CAST_TOKEN_PTR(tokenVec->data[tokenIdx])->tokenType != tokenType){
        return false;
    }
    ++tokenIdx;
    if(tokenIdx > tokenVec->len){
        Error("トークンの最大値を超えました。ロジックエラーの可能性があります");
    }
    return true;
}

NODE* Expr(void);
NODE* Term(void){
    //次のトークンが'('ならば、'(' expr ')' のはず
    if(Consume('(')){
        NODE* node = Expr();
        if(!Consume(')')){
            ErrorAt(CAST_TOKEN_PTR(tokenVec->data[tokenIdx])->input, "開きカッコに対応する閉じカッコがありません");
        }
        return node;
    }

    //そうでなければ数値のハズ
    if(CAST_TOKEN_PTR(tokenVec->data[tokenIdx])->tokenType == TK_NUM){
        return CreateNumNode(CAST_TOKEN_PTR(tokenVec->data[tokenIdx++])->value);
    }

    ErrorAt(CAST_TOKEN_PTR(tokenVec->data[tokenIdx])->input, "数値でも閉じカッコでもないトークンです");
}
NODE* Unary(void){
    if(Consume('+')){
        return Term();
    }
    if(Consume('-')){
        return CreateNode('-', CreateNumNode(0), Term());
    }
    return Term();
}
//* /
NODE* Mul(void){
    NODE* node = Unary();

    for(;;){
        if(Consume('*')){
            node = CreateNode('*', node, Term());
        }else if(Consume('/')){
            node = CreateNode('/', node, Term());
        }else{
            return node;
        }
    }
}
//+ -
NODE* Add(void){
    NODE* node = Mul();

    for(;;){
        if(Consume('+')){
            node = CreateNode('+', node, Mul());
        }else if(Consume('-')){
            node = CreateNode('-', node, Mul());
        }else{
            return node;
        }
    }
}
//> >= <= <
NODE* Relational(void){
    NODE* node = Add();

    for(;;){
        if(Consume('>')){
            node = CreateNode('>', node, Add());
        }else if(Consume('<')){
            node = CreateNode('<', node, Add());
        }else if(Consume(TK_LE)){
            node = CreateNode(TK_LE, node, Add());
        }else if(Consume(TK_GE)){
            node = CreateNode(TK_GE, node, Add());
        }else{
            return node;
        }
    }
}
//== !=
NODE* Equality(void){
    NODE* node = Relational();

    for(;;){
        if(Consume(TK_EQ)){
            node = CreateNode(TK_EQ, node, Relational());
        }else if(Consume(TK_NE)){
            node = CreateNode(TK_NE, node, Relational());
        }else{
            return node;
        }
    }
}
NODE* Expr(void){
    return Equality();
}

//userInputが指してる文字列をトークンに分割してtokensに保存をする
void Tokenize(void){
    char* top = userInput;

    while(*top){
        //空白文字をスキップ
        if(isspace(*top)){
            ++top;
            continue;
        }

        //+ - * / ( ) 記号
        if(*top == '+' || *top == '-' || *top == '(' || *top == ')' || *top == '*' || *top == '/'){
            TOKEN* token = CreateToken();
            token->tokenType = *top;
            token->input = top;
            PushVector(tokenVec, (void*)token);
            ++top;
            continue;
        }

        //==
        if(*top == '='){
            if(++top){
                if(*top == '='){
                    TOKEN* token = CreateToken();
                    token->tokenType = TK_EQ;
                    token->input = top;
                    PushVector(tokenVec, (void*)token);
                    ++top;
                    continue;
                }else{
                    //=はまだ未対応
                    ErrorAt(top, "=のトークナイズは未対応です");
                }
            }else{
                ErrorAt(--top, "トークナイズできません");
            }
        }

        //!=
        if(*top == '!'){
            if(++top){
                if(*top == '='){
                    TOKEN* token = CreateToken();
                    token->tokenType = TK_NE;
                    token->input = top;
                    PushVector(tokenVec, (void*)token);
                    ++top;
                    continue;
                }else{
                    //!はまだ未対応
                    ErrorAt(top, "!のトークナイズは未対応です");
                }
            }else{
                ErrorAt(--top, "トークナイズできません");
            }
        }

        //< <=
        if(*top == '<'){
            ++top;
            if(*top){
                if(*top == '='){
                    TOKEN* token = CreateToken();
                    token->tokenType = TK_LE;
                    token->input = top;
                    PushVector(tokenVec, (void*)token);
                    ++top;
                    continue;
                }else{
                    --top;
                    TOKEN* token = CreateToken();
                    token->tokenType = *top;
                    token->input = top++;
                    PushVector(tokenVec, (void*)token);
                    continue;
                }
            }else{
                //<の後がないってことはありえないはず
                ErrorAt(--top, "トークナイズできません");
            }
        }
        //> >=
        if(*top == '>'){
            ++top;
            if(*top){
                if(*top == '='){
                    TOKEN* token = CreateToken();
                    token->tokenType = TK_GE;
                    token->input = top;
                    PushVector(tokenVec, (void*)token);
                    ++top;
                    continue;
                }else{
                    --top;
                    TOKEN* token = CreateToken();
                    token->tokenType = *top;
                    token->input = top;
                    PushVector(tokenVec, (void*)token);
                    ++top;
                    continue;
                }
            }else{
                //>の後がないってことはありえないはず
                ErrorAt(--top, "トークナイズできません");
            }
        }

        //数値
        if(isdigit(*top)){
            TOKEN* token = CreateToken();
            token->tokenType = TK_NUM;
            token->input = top;
            token->value = strtol(top, &top, 10);
            PushVector(tokenVec, (void*)token);
            continue;
        }

        ErrorAt(top, "トークナイズできません");
    }

    TOKEN* token = CreateToken();
    token->tokenType = TK_EOF;
    token->input = top;
    PushVector(tokenVec, (void*)token);
}

void Gen(NODE* node){
    if(node->nodeType == ND_NUM){
        printf("  push %d\n", node->value);
        return;
    }

    Gen(node->lhs);
    Gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch(node->nodeType){
    case '+':
        printf("  add rax, rdi\n");
        break;
    case '-':
        printf("  sub rax, rdi\n");
        break;
    case '*':
        printf("  imul rdi\n");
        break;
    case '/':
        printf("  cqo\n");
        printf("  idiv rdi\n");
        break;
    case TK_EQ:// ==
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        break;
    case TK_NE:// !=
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
        break;
    case '<':
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;
    case TK_LE:// <=
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;
    case '>':
        printf("  cmp rdi, rax\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;
    case TK_GE:// >=
        printf("  cmp rdi, rax\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;

    default:
        Error("予想外のノードタイプが存在します NodeType:%d", node->nodeType);
    }

    printf("  push rax\n");
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