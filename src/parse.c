#include "ccpr.h"



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