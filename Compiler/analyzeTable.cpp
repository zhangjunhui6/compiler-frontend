#include "analyzeTable.h"

//初始化Go函数
void InitGoList(GoList* l) {
	(*l).elem = (Go*)malloc(50 * sizeof(Go));
	(*l).len = 0;
	(*l).size = 50;
}

//查询goto函数
int gotoJ(GoList l, int i, int x) {
	for (int k = 0; k < l.len; k++)
	{
		Go g = l.elem[k];
		if (g.i == i && g.x == x)
		{
			return g.j;
		}
	}
	return -1;
}

//添加Go项
void addGo(GoList* l, Go g) {
	if (l == NULL || (*l).elem == NULL)
	{
		return;
	}

	int j = gotoJ(*l, g.i, g.x); //查重,若有冲突，以最早加入的为主
	if (j != -1)
	{
		return;
	}

	int len = (*l).len;
	int size = (*l).size;
	if (len >= size)
	{
		Go* x;
		x = (Go*)realloc((*l).elem, (size + 5) * sizeof(Go));
		(*l).elem = x;
		(*l).size += 5;
	}

	(*l).elem[len] = g;
	(*l).len++;
}

//初始化Action函数
void InitActionList(ActionList* T)
{
	if (T == NULL)
	{
		return;
	}
	(*T).base = (Action*)malloc(50 * sizeof(Action));
	(*T).len = 0;
	(*T).size = 50;
}

int actionIsExist(ActionList T, Action item)
{
	for (int i = 0; i < T.len; i++)
	{
		Action a = T.base[i];
		if (a.i == item.i && a.a==item.a && a.status == item.status && a.j == item.j)
		{
			return 1;
		}
	}
	return 0;
}

//添加Action项
void addAction(ActionList* T, Action item)
{
	if (T == NULL || (*T).base == NULL)
	{
		return;
	}

	if (actionIsExist(*T,item) == 1)
	{
		return;
	}

	if ((*T).len >= (*T).size)
	{
		Action* x = (Action*)realloc(T->base, ((*T).size + 20) * sizeof(Action));
		(*T).base = x;
		(*T).size += 20;
	}

	(*T).base[(*T).len] = item;
	(*T).len++;
}

//查询action函数
Action queryActionList(ActionList T, int i, int a)
{
	for (int j= 0; j < T.len; j++)
	{
		Action item = T.base[j];
		if (item.i == i && item.a == a)
		{
			return item;
		}
	}
	return Action({i, a, 4, -1}); //找不到则是error
}