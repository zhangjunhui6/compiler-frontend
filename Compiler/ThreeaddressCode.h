#pragma once
#include<iostream>
using namespace std;
#include<cstdio>
#include<cstdlib>

/*三地址码结构体*/
typedef struct {
	int type;   //类型：1:运算x = y op z 2:赋值:x = y 
					//3:无条件转移: goto L4.条件转移: if x relop y goto L
	int x, y, z; //这里的x,y,z存放的都是符号表的地址
	int op; //op/relop,存放token_id
	int L; //存放要跳转的三地址吗的坐标
}ThrAddrCode;

/*三地址码链表*/
typedef struct {
	ThrAddrCode* base;
	int len;
	int size;
}ThrAddrCodes;

/*初始化*/
void InitCodes(ThrAddrCodes* codes);

/*添加三地址码*/
void AddCode(ThrAddrCodes* codes, ThrAddrCode code);

/*获取某一条三地址码*/
ThrAddrCode getCode(ThrAddrCodes codes, int i);

/*设置某一条三地址码*/
void setCode(ThrAddrCodes* codes, int i, ThrAddrCode code);

/*修改某一条三地址码的L属性*/
void setCodeL(ThrAddrCodes* codes, int i, int L);

/*获取下一条要生成的三地址代码的下标*/
int getNextInstr(ThrAddrCodes codes);

/*遍历输出*/
void TraverseAddrCodes(ThrAddrCodes codes, void(Visit)(ThrAddrCode));