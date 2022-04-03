#pragma once

#include<stdio.h>
#include<stdlib.h>
#include "ThreeaddressCode.h"

#define STACK_INIT_SIZE  50
#define STACK_INCREMENT 10

typedef struct {
	int l[20];
	int num;
}list;

typedef struct {
	int status; //状态
	int symbol; //符号
	int type,width;//声明语句的两个S属性
	int instr;     //跳转、顺序语句中使用到的表明三地址码位置的属性
	list truelist, falselist, nextlist;//Bool表达式的三个属性
	int addr;	//表示对应符号表的位置的属性,进而得到变量的lexname和常量的val
}stackItem;

//分析栈
typedef struct {
	stackItem* base;
	stackItem* top;
	int stackSize;
}AnStack;

/*初始化*/
void InitStack(AnStack* S);

/*入栈*/
void Push(AnStack* S, stackItem e);

/*出栈*/
void Pop(AnStack* S, stackItem* e);

/*获取栈顶状态值*/
int getTopSta(AnStack S);

/*获取栈顶数据*/
stackItem getTop(AnStack S);

/*遍历*/
void Traverse(AnStack S, void(Visit)(stackItem));

/*初始化list*/
void makelist(list* list1, int instr);

/*连接2个list*/
list merge(list list1, list list2);

/*回填:用instr填充list1中各三地址码的L属性*/
void backpatch(ThrAddrCodes *codes,list list1, int instr);