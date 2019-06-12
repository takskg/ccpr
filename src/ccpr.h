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
//関数宣言
//-----------------------------------

//エラー出力
extern void Error(char *fmt, ...);
extern void ErrorAt(char *loc, char *msg);

//トークナイズ
extern void Tokenize(void);

//構文解析
extern NODE* Expr(void);

//コード生成
extern void Gen(NODE* node);

//コンテナ
extern VECTOR* CreateVector(void);
extern void PushVector(VECTOR* vec, void* elem);
extern void RunVectorTest(void);