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
	bool isnumber = true;
	for (i = ib; i <= ie; ++i)
	{
		if (e[i]<'0' || e[i]>'9')isnumber = false;
	}
	if (!isnumber)
	{
		return_error(SYNTEX_ERROR, ib, ie);
	}
	char sc = e[ie + 1];
	e[ie + 1] = 0;//Create end of string
	uint32_t res;
	sscanf(e + ib, "%u", &res);
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
	if (n == 0)return 0;
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

/* We use the POSIX regex functions to process regular expressions.
* Type 'man regex' for more information about POSIX regex functions.
*/
//#include <sys/types.h>
//#include <regex.h>
//
//enum {
//	NOTYPE = 256, EQ
//
//	/* TODO: Add more token types */
//
//};
//
//static struct rule {
//	char *regex;
//	int token_type;
//} rules[] = {
//
//	/* TODO: Add more rules.
//	 * Pay attention to the precedence level of different rules.
//	 */
//
//	{" +",	NOTYPE},				// spaces
//	{"\\+", '+'},					// plus
//	{"==", EQ}						// equal
//};
//
//#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )
//
//static regex_t re[NR_REGEX];
//
///* Rules are used for many times.
// * Therefore we compile them only once before any usage.
// */
//void init_regex() {
//	int i;
//	char error_msg[128];
//	int ret;
//
//	for(i = 0; i < NR_REGEX; i ++) {
//		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
//		if(ret != 0) {
//			regerror(ret, &re[i], error_msg, 128);
//			Assert(ret != 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
//		}
//	}
//}
//
//typedef struct token {
//	int type;
//	char str[32];
//} Token;
//
//Token tokens[32];
//int nr_token;
//
//static bool make_token(char *e) {
//	int position = 0;
//	int i;
//	regmatch_t pmatch;
//	
//	nr_token = 0;
//
//	while(e[position] != '\0') {
//		/* Try all rules one by one. */
//		for(i = 0; i < NR_REGEX; i ++) {
//			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
//				char *substr_start = e + position;
//				int substr_len = pmatch.rm_eo;
//
//				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
//				position += substr_len;
//
//				/* TODO: Now a new token is recognized with rules[i]. Add codes
//				 * to record the token in the array ``tokens''. For certain 
//				 * types of tokens, some extra actions should be performed.
//				 */
//
//				switch(rules[i].token_type) {
//					default: panic("please implement me");
//				}
//
//				break;
//			}
//		}
//
//		if(i == NR_REGEX) {
//			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
//			return false;
//		}
//	}
//
//	return true; 
//}
//
//uint32_t expr(char *e, bool *success) {
//	if(!make_token(e)) {
//		*success = false;
//		return 0;
//	}
//
//	/* TODO: Insert codes to evaluate the expression. */
//	panic("please implement me");
//	return 0;
//}