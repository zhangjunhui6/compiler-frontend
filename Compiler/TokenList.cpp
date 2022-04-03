#include "TokenList.h"

/*
 *初始化
 */
void InitList(TokenList* L) {
	(*L).token = (Token*)malloc(LIST_INIT_SIZE * sizeof(Token));
	if ((*L).token == NULL) {
		exit(-2); //内存分配失败 
	}

	(*L).length = 0;
	(*L).listsize = LIST_INIT_SIZE;
}

/*
 *计数
 */
int ListLength(TokenList L) {
	return L.length;
}

/*
 *添加Token
 */
int Append(TokenList* L, Token t) {
	Token* newbase;
	//确保顺序表存在
	if (L == NULL || (*L).token == NULL) {
		return 0;
	}

	//若存储空间满，则增加新空间
	if ((*L).length >= (*L).listsize) {
		newbase = (Token*)realloc((*L).token, ((*L).listsize + LIST_INCREMENT) * sizeof(Token));
		if (newbase == NULL) {
			exit(-2); //内存重新分配失败 
		}

		(*L).token = newbase;
		(*L).listsize += LIST_INCREMENT;
	}
	int len = (*L).length;

	(*L).token[len] = t;
	(*L).length++;

	return 1;
}

/*
 *遍历
 */
void ListTraverse(TokenList L, void(Visit)(Token)) {
	int i;

	for (i = 0; i < L.length; i++) {
		Visit(L.token[i]);
	}
}