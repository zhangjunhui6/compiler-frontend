#pragma once
#include<stdio.h>
#include<stdlib.h>

#define LIST_INIT_SIZE 30 
#define LIST_INCREMENT 10

typedef struct Token {
    int token_id;
    int table_id;//存放位于符号表的位置，从1开始 
}Token;

typedef struct TokenList {
    Token* token; //顺序表存储空间的基址
    int length;
    int listsize;
}TokenList;

/*
 *初始化
 */
void InitList(TokenList* L);

/*
 *计数
 */
int ListLength(TokenList L);

/*
 *添加Token
 */
int Append(TokenList* L, Token t);

/*
 *遍历
 */
void ListTraverse(TokenList L, void(Visit)(Token));