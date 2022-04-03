#pragma once

#include<stdio.h>
#include<stdlib.h>

typedef struct {
	int i;		//状态i
	int x;	//符号x，可以为终结符或非终结符
	int j;		//状态j
}Go;

typedef struct {
	Go* elem;
	int len;
	int size;
}GoList;

typedef struct {
	int i;				//状态i
	int a;				//终结符号a
	int status;	//移入1，归约2，接受3，报错4
	int j;				//移入后的状态、归约使用的产生式编号、接受使用的归约产生式编号
}Action;

typedef struct {
	Action* base;
	int len;
	int size;
}ActionList;

//初始化Go函数
void InitGoList(GoList* l);

//查询goto函数：找到则返回j的值，否则返回-1
int gotoJ(GoList l, int i, int x);

//添加Go项
void addGo(GoList* l, Go g);

/*初始化Action函数*/
void InitActionList(ActionList* T);

/*查重:根据i,x,status查重*/
int actionIsExist(ActionList T, Action item);

/*添加Action项*/
void addAction(ActionList* T, Action item);

/*根据i和a查询Action*/
Action queryActionList(ActionList T, int i, int a);
