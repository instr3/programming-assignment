#include "nemu.h"
#include <stdlib.h>

/* Here using a more efficient way to calculate the expression.
 * First init the brackets and then	deal with the expression by partition.
 * Algorithm Complexity: O(n).
 */

//Todo :	delete 'malloc' and 'free'
//			Add more ops.
int *bracketInfo, *bracketStack, bracketStackCount;
//BracketCompletion(e,in)
//Find the corresponding bracket of e which length is in.
bool BracketCompletion(char *e, int in)
{
	bracketStackCount = 0;
	int i;
	for (i = 0; i < in; ++i)
	{
		if (e[i] == '(')
		{
			bracketStack[++bracketStackCount] = i;
		}
		else if (e[i] == ')')
		{
			if (bracketStackCount <= 0)return false;
			bracketInfo[i] = bracketStack[bracketStackCount];
			bracketInfo[bracketStack[bracketStackCount]] = i;
			--bracketStackCount;
		}
	}
	return bracketStackCount == 0;
}

//Expr Error Codes.
enum ExprErrors
{
	NO_ERROR = 0,
	MATH_ERROR = 1,//Divide by 0 etc.
	SYNTEX_ERROR = 2,
	MEMORY_ACCESS_ERROR = 3
}errorCode;
int errorFrom, errorTo;

//TestBinaryOp(e,ib)
//Test if a '-' or '*' is binary op at e[ib].
inline bool TestBinaryOp(char *e, int ib)
{
	if (ib == 0)return false;
	char pre = e[ib - 1];
	if (pre == ' ')
	{
		if (ib == 1)return false;
		pre = e[ib - 2];
	}
	//Test whether the prev "token" is an item or an operator.
	return (pre == ')' || (pre >= 'a' && pre <= 'z') || (pre >= 'A' && pre <= 'Z') || (pre >= '0' && pre <= '9') || pre == '_' || pre == '.');
}
//Recursive Evaluation.
uint32_t SubEvaluate(char *e, int ib, int ie)
{
#define return_error(e,f,t) do{\
	errorCode = e; \
	errorFrom = f; \
	errorTo = t; \
	return 0; \
}while (0)
	if (e[ib] == ' ')++ib;
	if (e[ie] == ' ')--ie;
	if (ib > ie)
	{
		return_error(SYNTEX_ERROR, ie, ib);
	}
	if (e[ib] == '('&&e[ie] == ')'&&bracketInfo[ib] == ie)
	{
		return SubEvaluate(e, ib + 1, ie - 1);
	}
	uint32_t lfv, rtv;
	int i;
	//1.Binary + -
	for (i = ie; i >= ib; --i)
	{
		if (e[i] == ')')
		{
			i = bracketInfo[i] - 1;
		}
		if (e[i] == '+' || e[i] == '-')
		{
			if (!TestBinaryOp(e, i))continue;
			lfv = SubEvaluate(e, ib, i - 1);
			if (errorCode != NO_ERROR)return 0;
			rtv = SubEvaluate(e, i + 1, ie);
			if (errorCode != NO_ERROR)return 0;
			return e[i] == '+' ? lfv + rtv : lfv - rtv;
		}
	}
	//2.Binary * /
	for (i = ie; i >= ib; --i)
	{
		if (e[i] == ')')
		{
			i = bracketInfo[i] - 1;
		}
		if (e[i] == '*' || e[i] == '/')
		{
			if (!TestBinaryOp(e, i))continue;
			lfv = SubEvaluate(e, ib, i - 1);
			if (errorCode != NO_ERROR)return 0;
			rtv = SubEvaluate(e, i + 1, ie);
			if (errorCode != NO_ERROR)return 0;
			if (e[i] == '/'&&rtv == 0)
			{
				return_error(MATH_ERROR, ib, ie);
			}
			return e[i] == '*' ? lfv * rtv : lfv / rtv;
		}
	}
	

	//Single item

	//Assert it's number
	bool isnumber = true;// , isregister = false;
	bool ishex = false;//0xBASE16
	if (e[ib] == '$')//Get Variables
	{

	}
	if (ib + 1 < ie&&e[ib] == '0'&&e[ib + 1] == 'x')
	{
		ishex = true;
		ib += 2;
	}
	for (i = ib; i <= ie; ++i)
	{
		if (ishex)
		{
			if ((e[i]<'0' || e[i]>'9') && (e[i]<'A' || e[i]>'F') && (e[i]<'a' || e[i]>'f'))isnumber = false;
		}
		else
		{
			if (e[i]<'0' || e[i]>'9')isnumber = false;
		}

	}
	if (!isnumber)
	{
		return_error(SYNTEX_ERROR, ib, ie);
	}
	char sc = e[ie + 1];
	e[ie + 1] = 0;//Create end of string
	uint32_t res;
	sscanf(e + ib, ishex ? "%x" : "%u", &res);
	e[ie + 1] = sc;//Recover the expr
	return res;
#undef return_error
}

//InnerTrim(e)
//Reduce consequent space number of e to 1
void InnerTrim(char *e)
{
	bool space = true;
	char *dest = e;
	while (*e)
	{
		if (*e == ' ')
		{
			if (!space)
			{
				space = true;
				*dest = ' ';
				++dest;
			}
		}
		else
		{
			space = false;
			*dest = *e;
			++dest;
		}
		++e;
	}
	*dest = 0;
}

uint32_t expr(char *e, bool *success)
{
	InnerTrim(e);
	int n = strlen(e);
	if (n == 0)
	{
		printf("Empty expression encountered.\n");
		success = false;
		return 0;
	}
	bracketInfo = malloc(sizeof(int)*(n + 1));
	bracketStack = malloc(sizeof(int)*(n + 1));
	if (!bracketInfo || !bracketStack)
	{
		success = false;
		printf("Memory limit exceeded.\n");
		return 0;
	}
	if (!BracketCompletion(e, n))
	{
		success = false;
		printf("Brackets do not match in expr \"%s\".\n", e);
		return 0;
	}
	errorCode = 0;
	uint32_t res = SubEvaluate(e, 0, n - 1);

	free((void*)bracketInfo);
	free((void*)bracketStack);
	if (errorCode)
	{
		success = false;
		if (errorCode == MATH_ERROR)printf("Math error");
		if (errorCode == SYNTEX_ERROR)printf("Syntex error");
		if (errorCode == MEMORY_ACCESS_ERROR)printf("Memory access error");
		printf(" occured in expr \"%s\" near '", e);
		int i;
		for (i = errorFrom; i <= errorTo; ++i)putchar(e[i]);
		printf("'.\n");
		return 0;
	}
	*success = true;
	return res;
}
