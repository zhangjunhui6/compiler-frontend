#include "analyzeStack.h"
/*初始化分析栈*/
void InitStack(AnStack* S) {
	if (S == NULL)
	{
		return;
	}
	(*S).base = (stackItem*)malloc(STACK_INIT_SIZE * sizeof(stackItem));
	(*S).top = (*S).base;
	(*S).stackSize = STACK_INIT_SIZE;
}

/*e入栈*/
void Push(AnStack* S, stackItem e) {
	if (S == NULL || (*S).base == NULL)
	{
		return;
	}

	//栈满则加空间
	if ((*S).top - (*S).base >= (*S).stackSize )
	{
		(*S).base = (stackItem*)realloc((*S).base, ((*S).stackSize + STACK_INCREMENT) * sizeof(stackItem));
		if ((*S).base == NULL)
		{
			exit(-2);
		}
		(*S).top = (*S).base + (*S).stackSize;
		(*S).stackSize += STACK_INCREMENT;
	}

	*(S->top++) = e;
}

/*出栈*/
void Pop(AnStack* S, stackItem* e) {
	if (S == NULL || (*S).base == NULL) {
		return;
	}

	if ((*S).top == (*S).base) {
		return;
	}

	*e = *(--(*S).top);
}

/*获取栈顶的状态*/
int getTopSta(AnStack S)
{
	if (S.base == S.top)
	{
		return -1;
	}

	return (S.top-1)->status;
}

/*获取栈顶数据*/
stackItem getTop(AnStack S)
{
	if (S.base == S.top)
	{
		return stackItem({-1});
	}

	return *(S.top-1);
}

/*遍历*/
void Traverse(AnStack S, void(Visit)(stackItem)) {
	stackItem* p = S.base;
	if (S.base == NULL)
	{
		return;
	}
	while (p < S.top)
	{
		Visit(*p++);
	}
}

void makelist(list* list1, int instr)
{
	list1->l[0] = instr;
	list1->num = 1;
}

list merge(list list1, list list2)
{
	list l;
	l.num = 0;
	for (int i = 0; i < list1.num; i++)
	{
		l.l[l.num++] = list1.l[i];
	}
	for (int i = 0; i < list2.num; i++)
	{
		int flag = 0;
		for (int j = 0; j < list1.num; j++)
		{
			if (list2.l[i] == list2.l[j])
			{
				flag = 1;
				break;
			}
		}
		if (flag == 0)
		{
			l.l[l.num++] = list2.l[i];
		}
	}
	return l;
}

void backpatch(ThrAddrCodes* codes, list list1, int instr)
{
	for (int i = 0; i < list1.num; i++)
	{
		setCodeL(codes, list1.l[i], instr);
	}
}

