#pragma once
#include<iostream>
using namespace std;
#include<cstdio>
#include<cstdlib>

//First、FOLLOW集结构体
typedef struct {
	int* elem; //数组基址  
	int len;  
	int size;
}Vts;

//初始化
void InitVts(Vts* t);

//是否包含空,是则返回从1开始的下标，否则返回0
int includeNul(Vts t);

//是否包含$,是则返回从1开始的下标，否则返回0
int includeEof(Vts t);

//添加元素，从末尾添加
void addVt(Vts *t,int i);

//求并集，s = s ∪ t
void UnionVts(Vts *s, Vts t);

//去空,t = t - {0}：后面元素前移一个单位
void SubNul(Vts* t);

//加空,t = t ∪ {0}
void AddNul(Vts* t);