#include "ThreeaddressCode.h"

void InitCodes(ThrAddrCodes* codes)
{
	(*codes).base = (ThrAddrCode*)malloc(50 * sizeof(ThrAddrCode));
	(*codes).len = 0;
	(*codes).size = 50;
}

void AddCode(ThrAddrCodes* codes, ThrAddrCode code)
{
	if (codes == NULL || (*codes).base == NULL)
	{
		return;
	}
	if ((*codes).len >= (*codes).size)
	{
		ThrAddrCode* newbase;
		newbase = (ThrAddrCode*)realloc(codes->base, (codes->size + 10) * sizeof(ThrAddrCode));
		codes->base = newbase;
	}

	codes->base[codes->len++] = code;
}

ThrAddrCode getCode(ThrAddrCodes codes, int i)
{
	if (codes.base == NULL || codes.len < i || i < 0)
	{
		return ThrAddrCode({-1});
	}
	
	return codes.base[i];
}

void setCode(ThrAddrCodes* codes, int i, ThrAddrCode code)
{
	if (codes == NULL || codes->base == NULL || codes->len < i || i<0)
	{
		return;
	}

	codes->base[i] = code;
}

void setCodeL(ThrAddrCodes* codes, int i, int L)
{
	if (codes == NULL || codes->base == NULL || codes->len < i || i < 0)
	{
		return;
	}

	codes->base[i].L = L;
}

int getNextInstr(ThrAddrCodes codes)
{
	return codes.len;
}

void TraverseAddrCodes(ThrAddrCodes codes, void(Visit)(ThrAddrCode))
{
	for (int i = 0; i < codes.len; i++)
	{
		cout << "\t" << i << ":\t";
		Visit(codes.base[i]);
	}
}
