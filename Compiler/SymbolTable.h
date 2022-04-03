#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//表项数据 
typedef struct symbolItem {
    char w_name[20];  	//名字NAME
    int w_kind;			//种属KIND
    int w_type;			//类型TYPE 
    int w_val;			//数值VAL 
    int w_addr;			//地址ADDR
}symbolItem;

//符号表结点 
typedef struct TNode {
    symbolItem data;
    struct TNode* next;
}TNode;

//符号表
typedef TNode* SymbolTable;

/*
 * 初始化
 */
void Init_Table(SymbolTable* T);

/*
 * 计数：获取符号表的长度
 */
int TableLength(SymbolTable T);

/*
 *查找:根据w_name查找符号项(标识符)，返回该项在符号表中的位置
 */
int LocateItem(SymbolTable T, char w_name[20]);

/*
 *查找:根据数值大小查找符号项(数字)，返回该项在符号表中的位置
 */
int LocateItem(SymbolTable T,int val);

/*
 * 插入:向符号表插入一项
 */
int TableInsert(SymbolTable T, int i, symbolItem item);

/**
 *在表格后面添加数据
 */
int add(SymbolTable T, symbolItem item);

/*更新符号表中第index项的数据*/
void setSymbolItem(SymbolTable T, int index, int type, int addr);

/*
 *遍历:用visit函数访问符号表
 */
void TableTraverse(SymbolTable T, void(Visit)(symbolItem));

/*生成中间变量，返回在符号表中位置*/
int newtemp(SymbolTable T);

/*查找符号项*/
symbolItem searchItem(SymbolTable T, int i);