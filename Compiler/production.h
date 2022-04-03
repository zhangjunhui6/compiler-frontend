#pragma once
#include<stdio.h>
#include<stdlib.h>

//文法产生式(项)结构体
typedef struct {
	int left;   //产生式左部
	int num;   //右部符号数量
	int right[12]; //产生式右部
	int point;//项中‘点’的所在位置，point ∈ [0,num]
	int a; //展望符,LR(1)文法的展望符,表示归约项只有遇到展望符才能归约
}production;

//项集闭包结构体
typedef struct {
	production* I;
	int len;
	int size;
}Colletion;

//项集簇
typedef struct {
	Colletion* cluster;
	int len;
	int size;
}Clusters;

//判断产生式是否为空产生式
int ProIsNul(production p);

//查询产生式编号
int queryProNum(production* s, int num, production p);

//初始化项集闭包
void InitColletion(Colletion *col);

//项集闭包col里面是否存在项p
int ProIsExist(Colletion col, production p);

//向项集闭包中添加项
void addPro(Colletion* col, production p);

//初始化项集簇
void InitCluters(Clusters* clu);

//查询项集簇中的项集闭包
int queryCol(Clusters clu, Colletion col);

//向Cluters中添加Colletion,返回下标
int addCol(Clusters* clu, Colletion col);