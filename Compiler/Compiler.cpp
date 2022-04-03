// Compiler.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include <iostream>
using namespace std;

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "SymbolTable.h"
#include "TokenList.h"
#include "production.h"
#include "Vts.h"
#include "analyzeStack.h"
#include "analyzeTable.h"
#include "ThreeaddressCode.h"

/*=========================================================================================
-----------------------------------------------变量及函数声明----------------------------------------------------
   =========================================================================================*/

/*-----------------------------------符号表----------------------------------------*/
SymbolTable symbolTable;
void visitSymbolItem(symbolItem item);
void showSymbolTable(SymbolTable s);


/*-----------------------------------输入模块-------------------------------------*/
char* code;   
void readSourceFile(char*); //读取文件内容到code指向的缓冲区 


/*-----------------------------------词法分析-------------------------------------*/
int row, column; 
char *lexemeBegin, *forwards;  
char keywords[][6] = { "int","float","if","else","do","while" };
//词法分析的token_id，也作为语法分析的终结符号
enum {ID = 1, NUM, INT, FLOAT, IF, ELSE, DO, WHILE, ADD, SUB, MUL, DIV, ASG, EQ, NE, LT, GT, LE, GE, LB, RP, COMMA};
char token_ids[][6] = { "ε","id","num","int","float","if","else","do","while","+","-","*","/","=","==","<>","<",">","<=",">=","(",")",";" };
//Token顺序表
TokenList tokenList;
void visitToken(Token token);
void showTokenList();
//辅助函数
int isDigit(char);		
int isLetter(char);
int isSeparator(char);
int isLogicalOp(char);
int isArithmeticOp(char);
int isBlank(char);
void fail(); //词法分析错误处理函数
//识别Token的函数
Token recognizeIDAndKey();
Token recognizeNum();
Token recognizeSp();
Token recognizeRelop();
Token recognizeArOp();
Token getNextToken();


/*-----------------------------------语法分析-------------------------------------*/
//非终结符
enum {A=100,P,X,D,Y,L,B,K,Z,S,M,N,C,E,T,F};
char Vn[] = { 'A','P','X','D','Y','L','B','K','Z','S','M','N','C','E','T','F'};
int vnNum = 16;
//原产生式
/*int produtionsNum = 23;
production productions[23] = { 
	{A,1,{P}},{P,2,{D,S}},{D,4,{L,ID,COMMA,D}},{D,1,{0}},{L,1,{INT}},{L,1,{FLOAT}},
	{S,4,{ID,ASG,E,COMMA}},{S,5,{IF,LB,C,RP,S}},{S,7,{IF,LB,C,RP,S,ELSE,S}},{S,5,{WHILE,LB,C,RP,S}},
	{S,2,{S,S}},{C,3,{E,GT,E}},{C,3,{E,LT,E}},{C,3,{E,EQ,E}},{E,3,{E,ADD,T}},{E,3,{E,SUB,T}},
	{E,1,{T}},{T,1,{F}},{T,3,{T,MUL,F}},{T,3,{T,DIV,F}},{F,3,{LB,E,RP}},{F,1,{ID} },{F,1,{NUM}}
};*/
//适合自底向上语法制导翻译的改进产生式
int produtionsNum = 33;
production productions[] = {
	{A,1,{P}},{P,3,{X,D,S}},{X,1,{0}}, {D,5,{L,ID,COMMA,Y,D}},{Y,1,{0}},{D,1,{0}},
	{L,3,{B,K,Z}},{B,1,{INT}},{B,1,{FLOAT}},{K,1,{0}},{Z,1,{0}},{S,4,{ID,ASG,E,COMMA}},
	{S,6,{IF,LB,C,RP,M,S}},{S,10,{IF,LB,C,RP,M,S,N,ELSE,M,S}},{S,7,{WHILE,M,LB,C,RP,M,S}},
	{S,3,{S,M,S}},{M,1,{0}},{N,1,{0}},{C,3,{E,GT,E}},{C,3,{E,LT,E}},{C,3,{E,EQ,E}},{C,3,{E,GE,E}},
	{C,3,{E,LE,E}},{C,3,{E,NE,E}},{E,3,{E,ADD,T}},{E,3,{E,SUB,T}},{E,1,{T}},{T,1,{F}},{T,3,{T,MUL,F}},
	{T,3,{T,DIV,F}},{F,3,{LB,E,RP}},{F,1,{ID} },{F,1,{NUM}}
};
void showPro(production* pro, int num);
void showPro(production p);

Clusters clu;				//项集簇
GoList goList;			//Goto函数
ActionList action;		//Action函数(和Goto组成分析表)
AnStack anStack;		//分析栈
Colletion prods;		//分析过程中依次使用的归约产生式

//求x的First集
Vts FIRST(int x);
Vts FIRST(Vts t);
//求x的FOLLOW集
Vts FOLLOW(int x);
void showVts(Vts t);
void showFirst(int x);
void showFollow(int x);
//求LR1项集规范簇
void CLOSURE(Colletion* col, int start);
Colletion GOTO(Colletion I, int X); //Goto(Ii,X)=Ij;
void calClu(int s);
void calculaterClu();
void showColletion(Colletion c);
void showClu(Clusters clus);
//求分析表
void buidAnTable();
void showGoto(GoList golist);
void showAction(ActionList actionlist);
//语法分析
void program();


/*----------------------------------------中间代码生成---------------------------------------------*/
ThrAddrCodes thrCodes;//三地址码
int offset,w,t;//属性
//语义分析
void  semanticAnalysis();
void showitemname(symbolItem t);
void VisitThrAddrCode(ThrAddrCode Tc);
void showThrAddrCodes();

/*=========================================================================================
-----------------------------------------------函数定义----------------------------------------------------------
   =========================================================================================*/

/*------------------------------------------输入处理---------------------------------------------*/
//输入处理：读取文件内容，并将所有内容传递到code缓冲区中
void readSourceFile() {
	FILE* fp;
	errno_t err;
	char filename[30];
	printf("Please enter the file name:");
	cin >> filename;
	//VS下使用fopen会报错，说不安全
	if ((err = fopen_s(&fp,filename, "r")) != 0) {
		printf("Unable to open this file!\n");
		exit(0);
	}

	if (fp)
	{
		fseek(fp, 0, SEEK_END);//指针指向末尾 
		int length = ftell(fp); //统计文件长度 
		code = (char*)malloc((length + 1) * sizeof(char));
		rewind(fp);//指针指向开头 
		if (code != NULL)
		{
			length = fread(code, 1, length, fp);
			code[length] = '\0';
		}
		fclose(fp);
	}
	
}


/*------------------------------------------词法分析--------------------------------------------*/
/**
 *辅助函数：判断字符类型
 */
 //判断数字
int isDigit(char c) {
	if (c >= '0' && c <= '9')
		return 1;
	return 0;
}
//判断字母 
int isLetter(char c) {
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return 1;
	return 0;
}
//判断是否为分隔符{ (、)、; } 
int isSeparator(char c) {
	if (c == '(' || c == ')' || c == ';')
		return 1;
	return 0;
}
//判断是否为算术运算符{ + - * /} 
int isArithmeticOp(char c) {
	if (c == '+' || c == '-' || c == '*' || c == '/')
		return 1;
	return 0;
}
//判断是否为逻辑运算符&赋值符号{> < = >= <= == <>} 
int isLogicalOp(char c) {
	if (c == '<' || c == '>' || c == '=')
		return 1;
	return 0;
}
//判断是否为空格或tab
int isBlank(char c) {
	if (c == ' ' || c == '\t')
		return 1;
	return 0;
}
//错误处理函数,控制台显示词法分析错误
void fail() {
	cout << "词法错误！第" << row << "行第" << column << "列出现非法字符：" << *forwards << endl;
}

/**
 *识别Token函数:注意识别过程中需要移动forwards指针,column，识别结束加入token_list以及移动lexemeBegin指针,可能建立符号表
 */
 //识别标识符和关键字
Token recognizeIDAndKey() {
	Token token = {1,-1};
	forwards++;
	column++;
	//找到不是数字或字母的字符(若为非法字符,这里是把它当作分割符) 
	while (isDigit(*forwards) || isLetter(*forwards)) {
		forwards++;
		column++;
	}
	//获取该标识符的名字,注意在末尾要加上结束符
	char name[20]=" ";
	int i = 0;
	for (; lexemeBegin < forwards; ) {
		name[i++] = *lexemeBegin++;
	}
	name[i] = '\0';
	//判断是否为关键字
	for (i = 0; i < 6; i++) {
		if (strcmp(name, keywords[i]) == 0) {
			token.token_id = i + 3;
			break;
		}
	}
	//若是标识符，则需要查找符号表，否则为关键字直接返回token 
	if (token.token_id == 1) {
		int res = LocateItem(symbolTable, name);
		if (res == 0) {
			//符号表不存在此标识符的信息,添加表项 
			symbolItem item = {"",ID,-1,-1,-1};
			strcpy_s(item.w_name, 20, name);
			int id = add(symbolTable, item);
			if (id == 0)
				printf("符号表插入操作失败!");
			token.table_id = id;
		}
		else {
			token.table_id = res;
		}
	}
	Append(&tokenList, token);
	return token;
}
//识别数字(常量)：计算输入的数字数值，将其加入到符号表中 
Token recognizeNum() {
	Token token = {NUM,-1};
	int val=0;
	//找到不是数字的字符(若为非法字符,这里是把它当作分割符了) 
	while (isDigit(*forwards)) {
		val = val * 10 + (*forwards) - '0';
		forwards++;
		column++;
	}
	//查找符号表，看是否已存在该数字常量，未找到则添加，找到则返回位置
	int res = LocateItem(symbolTable, val);
	if (res != 0) 
	{
		token.table_id = res; //找到
	}
	else
	{
		symbolItem item = {"",NUM,1,val,-1}; //数字默认为int型
		int id = add(symbolTable, item);
		token.table_id = id;
	}
	lexemeBegin = forwards;
	Append(&tokenList, token);
	return token;
}
//识别分隔符
Token recognizeSp() {
	Token token = {-1,-1};
	switch (*forwards++) {
	case '(':
		token.token_id = LB;
		break;
	case ')':
		token.token_id = RP;
		break;
	case ';':
		token.token_id = COMMA;
		break;
	default:
		printf("Recognize separator error!!!");
	}
	lexemeBegin++;
	column++;
	Append(&tokenList, token);
	return token;
}
//识别算术运算符
Token recognizeArOp() {
	Token token = {1,-1};
	switch (*forwards++) {
	case '+':
		token.token_id = ADD;
		break;
	case '-':
		token.token_id = SUB;
		break;
	case '*':
		token.token_id = MUL;
		break;
	case '/':
		token.token_id = DIV;
		break;
	default:
		printf("Recognize arithmetic operators error!!!");
	}
	lexemeBegin++;
	column++;
	Append(&tokenList, token);
	return token;
}
//识别逻辑运算(包括赋值符号) 
Token recognizeRelop() {
	Token token = { 1,-1};
	int state = 0;
	switch (*forwards++)
	{
	case '>':
		state = 1;
		break;
	case '<':
		state = 2;
		break;
	case '=':
		state = 3;
		break;
	default:
		cout << "Error" << endl;
		break;
	}
	column++;
	char c = *forwards;
	switch (state)
	{
	case 1:
		if (c == '=')
		{
			token.token_id = GE;
			forwards++; //指向下一个单词的开头
			column++;
		}
		else
		{
			token.token_id = GT;
		}
		break;
	case 2:
		if (c == '>')
		{
			token.token_id = NE;
			forwards++; //指向下一个单词的开头
			column++;
		}
		else if (c == '=')
		{
			token.token_id = LE;
			forwards++;
			column++;
		}
		else
		{
			token.token_id = LT;
		}
		break;
	case 3:
		if (c == '=')
		{
			token.token_id = EQ;
			forwards++; //指向下一个单词的开头
			column++;
		}
		else
		{
			token.token_id = ASG;
		}
		break;
	default:
		cout << "Error!!!" << endl;
		break;
	}
	//识别完，移动lexemeBegin指针,添加token到toekn列表
	lexemeBegin = forwards;
	Append(&tokenList, token);
	return  token;
}
//识别下一个Token
Token getNextToken() {
	Token token = { -1,-1 };
Begin:
	while (isBlank(*forwards)) {		//过滤空格和tab 
		lexemeBegin++;
		forwards++;
		column++;
	}
	while ((*forwards) == '\n') {		//过滤换行符 
		lexemeBegin++;
		forwards++;
		row++;
		column = 1;
	}
	if (*forwards == '\0') {			//文件读取结束
		token.token_id = -1; 
		return token;
	}

	//词法单元识别 
	if (isLetter(*forwards)) {
		token = recognizeIDAndKey();	 //字母开头，进入标识符&关键字识别模块
	}
	else if (isDigit(*forwards)) {
		token = recognizeNum();			//数字开头，进入数字识别模块
	}
	else if (isSeparator(*forwards)) {
		token = recognizeSp();
	}
	else if (isArithmeticOp(*forwards)) {
		token = recognizeArOp();
	}
	else if (isLogicalOp(*forwards)) {
		token = recognizeRelop();
	}
	else {		//单词开头出现非法字符，报错并直接跳过接着识别单词 
		fail();
		forwards++;
		lexemeBegin++;
		column++;
		goto Begin;
	}
	return token;
}


/*----------------------------------------语法分析---------------------------------------------*/
//求x的First集合(x可为非终结符和终结符,t为字符串)
Vts FIRST(int x) {
	Vts s;
	InitVts(&s); //初始化
	if (x>=-1 && x<=COMMA)  //x为终结符或空,结束符，First集合为自身
	{
		addVt(&s, x);
	}
	else if (x>=A && x<=A+vnNum-1)	  //x为非终结符
	{
		for (int i = 0; i < produtionsNum; i++)  //遍历所有产生式,找到左部为x的产生式
		{
			production p = productions[i];
			if (p.left == x)  
			{
				if (p.right[0] == x)	//左递归则跳过,否则会死循环
				{
					continue;
				}
				else if (p.right[0] == 0) //空产生式
				{
					addVt(&s, 0);
				}
				else  //否则为一般情况,x -> x0x1...xn，分三步处理
				{
					int n = p.num, k = 0;
					//1. 初值
					Vts t = FIRST(p.right[k++]);
					SubNul(&t); //去空
					UnionVts(&s, t);
					//2. 循环，若Xk-1包含空，则继续加下一个
					while (includeNul(FIRST(p.right[k-1])) != 0 && k < n)
					{
						t = FIRST(p.right[k++]);
						SubNul(&t);
						UnionVts(&s, t);
					}
					//3. 结束处理，即若分析到最后一个，且最后也包含空;则加上空
					if (k == n && includeNul(FIRST(p.right[k-1])) != 0)
					{
						AddNul(&s);
					}
				}
			}
		}
	}
	else
	{
		cout << "错误!" << x << "不是终结符也不是非终结符" << endl;
	}
	return s;
}
Vts FIRST(Vts t) {
	Vts s;
	InitVts(&s);
	if (t.len < 1)
	{
		cout << "Error!" << endl;
		return s;
	}
	Vts temp = FIRST(t.elem[0]);
	SubNul(&temp);
	UnionVts(&s, temp);
	int k = 1;
	while (k<t.len && includeNul(FIRST(t.elem[k-1])) != 0)
	{
		temp = FIRST(t.elem[k++]);
		SubNul(&temp);
		UnionVts(&s, temp);
	}
	if (k == t.len && includeNul(FIRST(t.elem[k-1])) != 0)
	{
		AddNul(&s);
	}

	return s;
}
//求A的Follow集(x为非终结符)
Vts FOLLOW(int x) {
	Vts s;
	InitVts(&s);
	if (x>=A && x<=A+vnNum-1)
	{
		if (x == A) //为开始符号，则加上$结束符
		{
			addVt(&s, -1);
		}
		for (int i = 0; i < produtionsNum; i++) //遍历所有产生式
		{
			production p = productions[i];
			int j,num = p.num;
			for (j=0; j < num; j++)
			{
				if (p.right[j] == x) 
				{
					break;
				}
			}
			if (j != num)//找到右部包含x的产生式
			{
				int k;
				for ( k = j+1; k < num; k++) //遍历x后面的符号
				{
					Vts t = FIRST(p.right[k]);
					SubNul(&t);
					UnionVts(&s, t);  //下一元素的FIRST-{0}是FOLLOW的一部分
					if (includeNul(FIRST(p.right[k])) == 0) //不包含空，则结束
					{
						break;
					}
				}
				if (k == num) //遍历到最后一个了
				{
					if (p.left != p.right[num-1]) //若不是S -> ABxS这种情况，则将FOLLOW(S)加上
					{
						Vts t = FOLLOW(p.left);
						UnionVts(&s, t);
					}
				}
			}
		}
	}
	else
	{
		cout << "错误!" << x << "不是非终结符!" << endl;
	}
	return s;
}
//求LR1项集族
void CLOSURE(Colletion* col,int start) {
	int len = (*col).len;
	if (start == len) //递归结束
	{
		return;
	}

	for (int i = start; i < len; i++) //从第start项开始
	{
		production p = (*col).I[i];
		int num = p.num, point = p.point,x=p.right[point];
		//处理非空、非可规约、且待接受字符为非终结符的产生式
		if (ProIsNul(p) == 0 && point != num && x >= A && x <= A+vnNum-1) 
		{
			Vts vts;
			InitVts(&vts);
			while (++point < num)
			{
				addVt(&vts, p.right[point]);
			}
			addVt(&vts, p.a);
			//遍历所有产生式，找到左部为x的产生式
			for (int j = 0; j < produtionsNum; j++) 
			{
				production q = productions[j];
				if (q.left ==x)
				{
					Vts t = FIRST(vts);
					for (int k = 0; k < t.len; k++)
					{
						if (t.elem[k] >= -1 && t.elem[k]<=COMMA)
						{
							q.point = 0;
							q.a = t.elem[k];
							addPro(col, q);
						}
					}
				}
			}
		}
	}
	CLOSURE(col, len);
}
Colletion GOTO(Colletion I, int X) {
	Colletion col;
	InitColletion(&col);
	for (int i = 0; i < I.len; i++)
	{
		production p = I.I[i];
		if (p.point != p.num && p.right[p.point] == X)
		{
			p.point++;
			addPro(&col, p);
		}
	}
	CLOSURE(&col, 0);
	return col;
}
void calculaterClu() {
	/*先求出I0项集，然后加入到项集簇中*/
	production p0 = productions[0];
	p0.point = 0;
	p0.a = -1;
	Colletion col;
	InitColletion(&col);
	addPro(&col, p0);
	CLOSURE(&col, 0);
	addCol(&clu, col);
	/*递归计算项集闭包*/
	calClu(0);
}
void calClu(int s) {
	int length = clu.len;
	/*结束条件*/
	if (s == length)
	{
		return;
	}
	//遍历每一个项集
	for (int i = s; i < length; i++)
	{
		Colletion c = clu.cluster[i];
		//找到这个项集可接收的字符的集合
		int acceptV[25];
		int accindex = 0;
		for (int j = 0; j < c.len; j++)
		{
			production p = c.I[j];
			if (p.point != p.num && ProIsNul(p) == 0)	//不是可规约且非空
			{
				int flag = 0;
				for (int k = 0; k < accindex; k++)
				{
					if (p.right[p.point] == acceptV[k])
					{
						flag = 1;
						break;
					}
				}
				if (flag == 0)
				{
					acceptV[accindex++] = p.right[p.point];
				}
			}
		}

		for (int j = 0; j < accindex; j++)
		{
			Colletion c1 = GOTO(clu.cluster[i], acceptV[j]);
			if (c1.len != 0 && queryCol(clu, c1) == -1)
			{
				addCol(&clu, c1);
			}
		}

	}
	calClu(length);
}
//构造LR1的分析表
void buidAnTable()
{
	//遍历所有的状态
	for (int i = 0; i < clu.len; i++)
	{
		Colletion col = clu.cluster[i];
		//遍历所有项
		for (int j = 0; j < col.len; j++)
		{
			production p = col.I[j];
			if (p.num == p.point && p.left == A && p.a == -1)
			{
				//接受状态,使用0号产生式归约
				addAction(&action, { i,-1,3,0 });	
			}
			else if (ProIsNul(p) == 1 || p.num == p.point) //空产生式或可规约项,归约
			{
				/*SLR实现*/
				/*Vts t = FOLLOW(p.left);
				int j = queryProNum(productions, produtionsNum, p); //使用的产生式编号
				for (int k = 0; k < t.len; k++)
				{
					int a = t.elem[k];
					addAction(&action, { i,a,2,j });
				}*/
				int j1 = queryProNum(productions, produtionsNum, p);
				addAction(&action, { i,p.a,2,j1 });
			}
			else if (p.num != p.point && p.right[p.point] > 0 && p.right[p.point] <= COMMA)
			{
				//后面为终结符,移入
				Colletion c = GOTO(col, p.right[p.point]);
				int j2 = queryCol(clu, c);
				addAction(&action, { i,p.right[p.point],1,j2 });
			}
			else if (p.num != p.point && p.right[p.point] >= A && p.right[p.point] <= A + vnNum - 1) {
				//后面为非终结符，构造goto函数
				Colletion c = GOTO(col, p.right[p.point]);
				int j3 = queryCol(clu, c);
				addGo(&goList, { i,p.right[p.point],j3 });
			}
		}
	}
}
//语法分析：输出使用的产生式顺序
void program() {
	//计算项集族和分析表
	calculaterClu();
	buidAnTable();
	Token token = {-1,-1};
	do
	{
		//获取下一个Token
		token = getNextToken();
		int a = token.token_id;
		while (true)
		{
			//获取栈顶状态
			int s = getTopSta(anStack);
			//根据当前状态和输入符号，查询分析表
			Action item = queryActionList(action, s, a);
			if (item.status == 1)//移入操作
			{
				Push(&anStack, { item.j,a});
				break;
			}
			else if (item.status == 2)//归约操作,使用的产生式为p,非空产生式才进行弹栈
			{
				production p = productions[item.j];
				if (ProIsNul(p) == 0)
				{
					for (int i = 0; i < p.num; i++)//分析栈弹出num个
					{
						stackItem iteM;
						Pop(&anStack, &iteM);
					}
				}
				//获取栈顶状态,将GOTO[s,A]压入栈
				s = getTopSta(anStack);
				stackItem iteX;
				iteX.status = gotoJ(goList, s, p.left);
				iteX.symbol = p.left;
				Push(&anStack,iteX);
				
				addPro(&prods, p);
			}
			else if (item.status == 3) //结束
			{
				addPro(&prods, productions[0]);
				cout << "语法分析成功!" << endl;
				break;
			}
			else//语法分析出错,采用恐慌模式
			{
				cout << "语法错误!,位于第" << row << "行第" << column << "列前面的短语不符合语法要求" << endl;
				cout << "该Token单元为:" << endl;
				visitToken(token);
				break;
			}
		}
		
	} while (token.token_id != -1);
}


/*----------------------------------------中间代码生成---------------------------------------------*/
void semanticAnalysis()
{
	//计算项集族和分析表
	calculaterClu();
	buidAnTable();
	Token token = { -1,-1 };
	do
	{
		//获取下一个Token
		token = getNextToken();
		int a = token.token_id;
		while (true)
		{
			//获取栈顶状态
			int s = getTopSta(anStack);
			//根据当前状态和输入符号，查询分析表
			Action item = queryActionList(action, s, token.token_id);
			if (item.status == 1)//移入操作
			{
				stackItem stackI = {item.j,a};
				if (token.token_id == ID || token.token_id == NUM)
				{
					stackI.addr = token.table_id;
				}
				Push(&anStack, stackI);
				break;
			}
			else if (item.status == 2)//归约操作,使用的产生式为p,非空产生式才进行弹栈
			{
				stackItem si;
				list l1, l2;
				ThrAddrCode code1;
				/*根据所使用的产生式进行相应的语义分析*/
				switch (item.j)
				{
				case 2:     //X -> ε
					offset = 0;
					break;
				case 4:		//Y -> ε， 根据D->L id;YD确定位置
					setSymbolItem(symbolTable, (anStack.top - 2)->addr, (anStack.top - 3)->type, offset);
					offset += (anStack.top - 3)->width;
					break;
				case 6:     //L -> BKZ
					si.type = (anStack.top-1)->type;
					si.width = (anStack.top - 1)->width;
					break;
				case 7:     //B -> int
					si.type = 1;
					si.width = 4;
					break;
				case 8:    //B -> float
					si.type = 2;
					si.width = 8;
					break;
				case 9:    //K -> ε
					t = (anStack.top-1)->type;
					w = (anStack.top-1)->width;
					break;
				case 10:   //Z -> ε
					si.type = t;
					si.width = w;
					break;
				case 11:	    //S -> id=E;
					code1.type = 2;//赋值语句,x=y
					code1.x = (anStack.top - 4)->addr;
					code1.y = (anStack.top - 2)->addr;
					AddCode(&thrCodes, code1);
					break;
				case 12:   //S -> if(C)MS
					backpatch(&thrCodes, (anStack.top - 4)->truelist, (anStack.top - 2)->instr);
					si.nextlist = merge((anStack.top - 4)->falselist, (anStack.top - 1)->nextlist);
					break;
				case 13:   //S->if(C)MSNelseMS
					backpatch(&thrCodes,(anStack.top- 8)->truelist, (anStack.top - 6)->instr);
					backpatch(&thrCodes, (anStack.top - 8)->falselist, (anStack.top - 2)->instr);
					si.nextlist = merge((anStack.top - 1)->nextlist, merge((anStack.top - 4)->nextlist, (anStack.top - 5)->nextlist));
					break;
				case 14:   //S -> whileM(C)MS
					backpatch(&thrCodes, (anStack.top - 1)->nextlist, (anStack.top - 6)->instr);
					backpatch(&thrCodes, (anStack.top - 4)->truelist, (anStack.top - 2)->instr);
					si.nextlist = (anStack.top - 4)->falselist;
					ThrAddrCode code1;
					code1.type = 3;//无条件转移goto L
					code1.L = (anStack.top - 6)->instr;
					AddCode(&thrCodes, code1);
					break;
				case 15:    //S -> SMS
					backpatch(&thrCodes, (anStack.top - 3)->nextlist, (anStack.top - 2)->instr);
					si.nextlist = (anStack.top - 1)->nextlist;
					break;
				case 16:     //M -> ε
					si.instr = getNextInstr(thrCodes);
					break;
				case 17:     //N -> ε
					makelist(&l1, getNextInstr(thrCodes));
					si.nextlist = l1;
					AddCode(&thrCodes, { 3 });//goto -
					break;
				case 18:    //C -> E>E
					makelist(&l1, getNextInstr(thrCodes));
					si.truelist = l1;
					makelist(&l1, getNextInstr(thrCodes) + 1);
					si.falselist = l1;
					code1.type = 4;
					code1.x = (anStack.top - 3)->addr;
					code1.y = (anStack.top - 1)->addr;
					code1.op = GT;
					AddCode(&thrCodes, code1); //if E1.addr>E2.addr goto -
					AddCode(&thrCodes, { 3 });//goto -
					break;
				case 19:    //C -> E<E
					makelist(&l1, getNextInstr(thrCodes));
					si.truelist = l1;
					makelist(&l1, getNextInstr(thrCodes) + 1);
					si.falselist = l1;
					code1.type = 4;
					code1.x = (anStack.top - 3)->addr;
					code1.y = (anStack.top - 1)->addr;
					code1.op = LT;
					AddCode(&thrCodes, code1); //if E1.addr < E2.addr goto -
					AddCode(&thrCodes, { 3 });//goto -
					break;
				case 20:    //C -> E==E
					makelist(&l1, getNextInstr(thrCodes));
					si.truelist = l1;
					makelist(&l2, getNextInstr(thrCodes) + 1);
					si.falselist = l2;
					code1.type = 4;
					code1.x = (anStack.top - 3)->addr;
					code1.y = (anStack.top - 1)->addr;
					code1.op = EQ;
					AddCode(&thrCodes, code1); //if E1.addr==E2.addr goto -
					AddCode(&thrCodes, { 3 });//goto -
					break;
				case 21:    //C -> E>=E
					list l1, l2;
					makelist(&l1, getNextInstr(thrCodes));
					si.truelist = l1;
					makelist(&l2, getNextInstr(thrCodes) + 1);
					si.falselist = l2;
					code1.type = 4;
					code1.x = (anStack.top - 3)->addr;
					code1.y = (anStack.top - 1)->addr;
					code1.op = GE;
					AddCode(&thrCodes, code1); //if E1.addr>=E2.addr goto -
					AddCode(&thrCodes, { 3 });//goto -
					break;
				case 22:    //C -> E<=E
					makelist(&l1, getNextInstr(thrCodes));
					si.truelist = l1;
					makelist(&l2, getNextInstr(thrCodes) + 1);
					si.falselist = l2;
					code1.type = 4;
					code1.x = (anStack.top - 3)->addr;
					code1.y = (anStack.top - 1)->addr;
					code1.op = LE;
					AddCode(&thrCodes, code1); //if E1.addr<=E2.addr goto -
					AddCode(&thrCodes, { 3 });//goto -
					break;
				case 23:    //C -> E<>E
					makelist(&l1, getNextInstr(thrCodes));
					si.truelist = l1;
					makelist(&l2, getNextInstr(thrCodes) + 1);
					si.falselist = l2;
					code1.type = 4;
					code1.x = (anStack.top - 3)->addr;
					code1.y = (anStack.top - 1)->addr;
					code1.op = NE;
					AddCode(&thrCodes, code1); //if E1.addr<>E2.addr goto -
					AddCode(&thrCodes, { 3 });//goto -
					break;
				case 24:    //E -> E + T
					si.addr = newtemp(symbolTable);   //新增一个中间变量
					code1.type = 1;
					code1.x = si.addr;
					code1.y = (anStack.top - 3)->addr;
					code1.op = ADD;
					code1.z = (anStack.top - 1)->addr;
					AddCode(&thrCodes, code1);
					break;
				case 25:    //E -> E - T
					si.addr = newtemp(symbolTable);   //新增一个中间变量
					code1.type = 1;
					code1.x = si.addr;
					code1.y = (anStack.top - 3)->addr;
					code1.op = SUB;
					code1.z = (anStack.top - 1)->addr;
					AddCode(&thrCodes, code1);
					break;
				case 26:    //E -> T
					si.addr = (anStack.top - 1)->addr;
					break;
				case 27:    //T -> F
					si.addr = (anStack.top - 1)->addr;
					break;
				case 28:    //T -> T*F
					si.addr = newtemp(symbolTable);   //新增一个中间变量
					code1.type = 1;
					code1.x = si.addr;
					code1.y = (anStack.top - 3)->addr;
					code1.op = MUL;
					code1.z = (anStack.top - 1)->addr;
					AddCode(&thrCodes, code1);
					break;
				case 29:    //T -> T/F
					si.addr = newtemp(symbolTable);   //新增一个中间变量
					code1.type = 1;
					code1.x = si.addr;
					code1.y = (anStack.top - 3)->addr;
					code1.op = DIV;
					code1.z = (anStack.top - 1)->addr;
					AddCode(&thrCodes, code1);
					break;
				case 30:    //F -> (E)
					si.addr = (anStack.top - 2)->addr;
					break;
				case 31:     //F -> id
					si.addr = (anStack.top - 1)->addr;
					break;
				case 32:    //F -> num
					si.addr = (anStack.top - 1)->addr;
					break;
				default:
					break;
				}
				production p = productions[item.j];
				if (ProIsNul(p) == 0)
				{
					for (int i = 0; i < p.num; i++)//分析栈弹出num个
					{
						stackItem iteM;
						Pop(&anStack, &iteM);
					}
				}
				//获取栈顶状态,将GOTO[s,A]压入栈
				s = getTopSta(anStack);
				si.status = gotoJ(goList, s, p.left);
				si.symbol = p.left;
				Push(&anStack, si);

				addPro(&prods, p);
			}
			else if (item.status == 3) //结束
			{
				addPro(&prods, productions[0]);
				cout << "编译成功！" << endl;
				break;
			}
			else//采用恐慌模式
			{
				cout << "语法错误!,位于第" << row << "行第" << column << "列前面的短语不符合语法要求" << endl;
				cout << "该Token单元为:" << endl;
				visitToken(token);
				break;
			}
		}
	} while (token.token_id != -1);
}


/*------------------------------------------输出函数---------------------------------------------*/
//显示符号表
void visitSymbolItem(symbolItem item) {
	cout << "\t" << item.w_name << "\t" << item.w_kind << "\t" << item.w_type << "\t" << item.w_val << "\t" << item.w_addr << endl;
}
void showSymbolTable(SymbolTable s) {
	cout << "--------------------------------------符号表-----------------------------------------------" << endl;
	cout << "\tName\tKind\tType\tVal\tAddr" << endl;
	TableTraverse(s, visitSymbolItem);
}
//显示Token序列
void visitToken(Token token) {
	cout << "{ " << token_ids[token.token_id] << ", " << token.table_id << " }" << endl;
}
void showTokenList() {
	cout << "说明：用二元式表示token，第二个参数为指向字符表的指针(这里存储所在位置)" << endl;
	cout << "--------------------------------------Token序列------------------------------------------" << endl;
	ListTraverse(tokenList, visitToken);
}
//显示产生式
void showPro(production* pro, int num) {
	for (int i = 0; i < num; i++)
	{
		production p = pro[i];
		cout << "\t";
		cout << Vn[p.left - 100] << " —> ";
		for (int j = 0; j < p.num; j++)
		{
			int x = p.right[j];
			if (x >= 100)
			{
				cout << Vn[x - 100];
			}
			else if (x >= 0 && x <= 22)
			{
				cout << token_ids[x];
			}
		}
		cout << endl;
	}
}
void showPro(production p) {
	if (p.left < A || p.left > A + vnNum - 1)
	{
		return;
	}
	cout << "\t";
	cout << Vn[p.left - 100] << " —> ";
	for (int j = 0; j < p.num; j++)
	{
		int x = p.right[j];
		if (x >= A && x <= A + vnNum - 1)
		{
			cout << Vn[x - 100];
		}
		else if (x >= 0 && x <= 22)
		{
			cout << token_ids[x];
		}
	}
	cout << endl;
}
//显示First集和Follow集
void showVts(Vts t) {
	int i = 0;
	cout << "{";
	for (; i < t.len - 1; i++)
	{
		if (t.elem[i] == -1)
		{
			cout << "$,";
		}
		else {
			cout << token_ids[t.elem[i]] << ",";
		}
	}
	if (t.elem[i] == -1)
	{
		cout << "$" << "}" << endl;
	}
	else
	{
		cout << token_ids[t.elem[i]] << "}" << endl;
	}
}
void showFirst(int x) {
	cout << "\t";
	cout << "FIRST(";
	if (x >= 0 && x <= COMMA)
	{
		cout << token_ids[x] << ") = ";
	}
	else if (x >= A && x <= A + vnNum - 1)
	{
		cout << Vn[x - 100] << ") = ";
	}
	Vts t = FIRST(x);
	showVts(t);
}
void showFollow(int x) {
	if (x >= A && x <= A + vnNum - 1)
	{
		cout << "\t";
		cout << "FOLLOW(" << Vn[x - 100] << ") = ";
		Vts t = FOLLOW(x);
		showVts(t);
	}
}
//显示项集族、项集闭包
void showColletion(Colletion c) {
	for (int i = 0; i < c.len; i++)
	{
		production p = c.I[i];
		if (ProIsNul(p) == 1)
		{
			cout << "	" << Vn[p.left - 100] << " —> .,";
		}
		else
		{
			cout << "	" << Vn[p.left - 100] << " —> ";
			for (int j = 0; j < p.num; j++)
			{
				if (j == p.point)
				{
					cout << ".";
				}
				int x = p.right[j];
				if (x >= 100)
				{
					cout << Vn[x - 100];
				}
				else if (x >= 0 && x <= 22)
				{
					cout << token_ids[x];
				}
			}
			if (p.num == p.point)
			{
				cout << ".";
			}
			cout << ",";
		}
		if (p.a == -1)
		{
			cout << "$" << endl;
		}
		else if (p.a >= 0 && p.a <= COMMA)
		{
			cout << token_ids[p.a] << endl;
		}
		else if (p.a >= A && p.a <= A + vnNum - 1)
		{
			cout << Vn[p.a - 100] << endl;
		}
	}
}
void showClu(Clusters clus) {
	cout << "-------------------------------------LR1项集族----------------------------------------------" << endl;
	for (int i = 0; i < clus.len; i++)
	{
		cout << "I" << i << ":" << endl;
		showColletion(clus.cluster[i]);
	}
}
//显示分析表的goto表和action表
void showGoto(GoList golist) {
	cout << "-------------------------------------GOTO函数----------------------------------------------" << endl;
	for (int i = 0; i < clu.len; i++)
	{
		int num = 0;
		for (int x = A; x <= A + vnNum - 1; x++)
		{
			int j = gotoJ(golist, i, x);
			if (j != -1)
			{
				cout << "goto(" << i << "," << Vn[x - 100] << ") = " << j << ";\t";
				num++;
			}
		}
		for (int x = 1; x <= COMMA; x++)
		{
			int j = gotoJ(golist, i, x);
			if (j != -1)
			{
				cout << "goto(" << i << "," << token_ids[x] << ") = " << j << ";\t";
				num++;
			}
		}
		if (num != 0)
		{
			cout << endl;
		}
	}
}
void showAction(ActionList actionlist) {
	cout << "------------------------------------ACTION函数---------------------------------------------" << endl;
	int status = 0;
	for (int i = 0; i < actionlist.len; i++)
	{
		Action item = actionlist.base[i];
		if (status != item.i)
		{
			status = item.i;
			cout << endl;
		}
		cout << "action(" << item.i << ",";
		if (item.a != -1)
		{
			cout << token_ids[item.a] << ")=";
		}
		else
		{
			cout << "$)=";
		}

		switch (item.status)
		{
		case 1:
			cout << "s" << item.j << ";	";
			break;
		case 2:
			cout << "r" << item.j << ";	";
			break;
		case 3:
			cout << "acc" << ";	";
		default:
			break;
		}
	}
	cout << endl;
}
//显示生成的三地址代码
void showitemname(symbolItem t) {
	if (t.w_kind == 1 || t.w_kind == 3)
	{
		cout << t.w_name;
	}
	else if (t.w_kind == 2)
	{
		cout << t.w_val;
	}
}
void VisitThrAddrCode(ThrAddrCode Tc)
{
	symbolItem t;
	switch (Tc.type)
	{
	case 1:  //算术运算
		t = searchItem(symbolTable, Tc.x);
		showitemname(t);
		cout << " = ";
		t = searchItem(symbolTable, Tc.y);
		showitemname(t);
		cout << " " << token_ids[Tc.op] << " ";
		t = searchItem(symbolTable, Tc.z);
		showitemname(t);
		cout << endl;
		break;
	case 2:  //赋值
		t = searchItem(symbolTable, Tc.x);
		showitemname(t);
		cout << " = ";
		t = searchItem(symbolTable, Tc.y);
		showitemname(t);
		cout << endl;
		break;
	case 3:  //无条件跳转
		cout << "goto " << Tc.L << endl;
		break;
	case 4:  //条件跳转
		cout << "if ";
		t = searchItem(symbolTable, Tc.x);
		showitemname(t);
		cout << " " << token_ids[Tc.op] << " ";
		t = searchItem(symbolTable, Tc.y);
		showitemname(t);
		cout << " goto " << Tc.L << endl;
		break;
	default:
		break;
	}
}
void showThrAddrCodes() {
	cout << "-------------------------------------中间代码----------------------------------------------" << endl;
	TraverseAddrCodes(thrCodes, VisitThrAddrCode);
}
//运行显示功能界面
void show() {
	cout << "\t---------------------功能菜单(输入序号进行相应操作)--------------------" << endl;
	cout << "\t0. 显示功能菜单" << endl;
	cout << "\t1. 查看语法分析使用的文法" << endl;
	cout << "\t2. 查看非终结符的FIRST集" << endl;
	cout << "\t3. 查看非终结符的FOLLOW集" << endl;
	cout << "\t4. 查看生成的LR1项集闭包" << endl;
	cout << "\t5. 查看分析表的Goto函数" << endl;
	cout << "\t6. 查看分析表的Action函数" << endl;
	cout << "\t7. 查看Token序列" << endl;
	cout << "\t8. 查看语法分析树(依次使用的归约产生式)" << endl;
	cout << "\t9. 查看生成的中间代码" << endl;
	cout << "\t10. 查看符号表" << endl;
	cout << "\t11. 结束运行" << endl;
}

/*============================================================================================
---------------------------------------------main主函数--------------------------------------------------------------
   ============================================================================================*/

int main(int argc, char* argv[]) {
	/*全局变量的初始化*/
	Init_Table(&symbolTable);		//符号表
	InitList(&tokenList);				//Token序列
	row = column = 1;						//行列信息
	readSourceFile();				/*读取源代码,初始化code指针*/
	forwards = code;  //词法分析两指针
	lexemeBegin = code;
	InitCluters(&clu);						//LR1项集族
	InitGoList(&goList);					//GOTO表
	InitActionList(&action);			//ACTION表
	InitStack(&anStack);				//分析栈，初始化为状态0，$符号
	Push(&anStack, { 0,-1 });			
	InitColletion(&prods);				//归约使用的产生式顺序
	InitCodes(&thrCodes);				//三地址码序列

	/*编译：语法制导翻译*/
	semanticAnalysis();
	//program();      //语法分析

	/*功能界面*/
	show();
	int n;
	while (true)
	{
		cout << "请输入功能序号：";
		cin >> n;
		switch (n)
		{
		case 0:
			show();
			break;
		case 1:
			showPro(productions, produtionsNum);
			break;
		case 2:
			cout << "-------------------FIRST集-------------------" << endl;
			for (int i = A; i < A+vnNum; i++)
			{
				showFirst(i);
			}
			break;
		case 3:
			cout << "-------------------FOLLOW集-------------------" << endl;
			for (int i = A; i < A + vnNum; i++)
			{
				showFollow(i);
			}
			break;
		case 4:
			showClu(clu);
			break;
		case 5:
			showGoto(goList);
			break;
		case 6:
			showAction(action);
			break;
		case 7:
			showTokenList();
			break;
		case 8:
			showPro(prods.I, prods.len);
			break;
		case 9:
			showThrAddrCodes();
			break;
		case 10:
			showSymbolTable(symbolTable);
			break;
		case 11:
			return 0; //结束
		default:
			cout << "输入错误，未找到相应的功能" << endl;
			break;
		}
	}
	return 1;
}