#include <malloc.h>
#include <string.h>

#include "pcexpr.h"


/*** 매크로 정의 ***/

#ifndef NULL
#define NULL    ((void*) 0)
#endif  // !NULL

#define STACK_SIZE          (100)
#define STACK_INIT_INDEX    (-1)


/*** 함수 선언 (로컬) ***/

/* 토큰을 획득하는 함수 (로컬)
 * 주어진 문자열의 expression을 분해하여 token을 생성한다.
 * 예: expression = "(Bigger(father(teacher  Bill)) Joe1)"
 *     출력된 token 열: (, Bigger, (, father, (, teacher, Bill, ), ), Joe1, )
 * str_dst: 현재 획득한 토큰을 저장할 공간.
 * str_exp: 토크나이징할 PC Expression 문자열, 두 번째 호출부터 NULL을 인수로 전달하면 이전 문자열을 계속해서 토크나이징함.
 */
bool read_token(char *str_dst, const char *str_exp);

/* 새 노드를 할당하는 함수 (로컬)
 * 새 노드를 할당하고 값을 0으로 초기화한다.
 */
nodeptr malloc_node();


/*** 함수 정의 ***/

nodeptr get_structure_of_expression(const char *exp_str)
{
	// 알고리즘
	// (1)
	nodeptr stack[STACK_SIZE];
	int top = STACK_INIT_INDEX; // 스택 초기화. We use a stack to handle lists embedding sub-lists.
	nodeptr exp = NULL;
	node *curr = NULL;
	node *tp, *tp2;
	char tok[NODE_STR_SIZE];
	
	bool hasToken = read_token(tok, exp_str);   // read token into tok
	if (!hasToken || tok[0] != '(') {   // 첫 토큰은 반드시 좌측 괄호여야 한다.
		return NULL;
	}
	
	exp = malloc_node();
	curr = exp;
	strcpy_s(curr->str, NODE_STR_SIZE, "(");
	curr->down = NULL;
	top = 0;
	stack[0] = NULL;    // return position is NULL for left parenthesis corresponding to the first left parenthesis.

	// (2)
	do {
		hasToken = read_token(tok, NULL);   // read the next token into tok
		if (hasToken) {     // no more tokens left
			return NULL;    // Some token is required to come. But no more. So error has occurred.
		}
		
		tp = malloc_node(); // 새 노드를 할당
		
		if (tok[0] == '(') {    // tok is left parenthesis
			tp->str[0] = '\0';
			curr->right = tp;
			top++;
			stack[top] = tp;    // 돌아올 곳을 저장
			tp2 = malloc_node();
			strcpy_s(tp2->str, NODE_STR_SIZE, "(");
			tp2->down = NULL;
			tp->down = tp2;
			curr = tp2;         // 하위 레벨로 내려감
		}
		else if (tok[0] == ')') {   // tok is right parenthesis
			strcpy_s(tp->str, NODE_STR_SIZE, ")");
			tp->down = tp->right = NULL;
			curr->right = tp;
			curr = stack[top];
			top--;
			if (curr == NULL) {
				break;  // exit the loop;
			}   // the final right ) corresponding to the first ( was inputed. So nothing more to do.
		}
		else {	// tok is neither left nor right parenthesis
			strcpy_s(tp->str, NODE_STR_SIZE, tok);
			tp->down = NULL;
			curr->right = tp;
			curr = tp;
		}
	} while (true);

	// (3)
	if (!read_token(tok, NULL)) {    // no more tokens left
		return  exp;
	}   // Success since final right ) is read and no tokens are left.
	else {
		return  NULL;
	}   // Some to kens are remaining after the final right ).
}

bool read_token(char *str_dst, const char *str_exp)
{
	static char *currAddr = NULL;
	bool isFound = false;
	int strIndex = 0;

	if (currAddr == NULL) {
		currAddr = str_exp; // 문자열의 시작 주소
	}

	while (*currAddr != '\0') {
		switch (*currAddr)
		{
			// 공백 문자
		case ' ':
		case '\t':
			currAddr++;         // 다음 문자로 이동
			if (isFound) {      // 문자열 토큰이면 문자열을 완성하고 종료
				str_dst[strIndex] = '\0';   // 문자열 완성
				return true;    // 토큰 획득 성공
			}
			else {  // 공백 문자는 건너 뜀
				break;
			}

			// 괄호
		case '(':
		case ')':
			if (!isFound) { // 괄호 토큰
				str_dst[strIndex++] = *currAddr;
				currAddr++; // 다음 문자로 이동 (isFound == true일 때는 다음 토큰이 가져갈 문자이므로, isFound == false일 때만 이동)
			}
			str_dst[strIndex] = '\0';   // 문자열 완성
			return true;    // 토큰 획득 성공

			// 문자열 토큰의 문자 모으기
		default:
			isFound = true; // 토큰 확인
			if (strIndex < NODE_STR_SIZE - 1) { // NODE_STR_SIZE 크기까지만 저장
				str_dst[strIndex++] = *currAddr;
			}
			currAddr++;     // 다음 문자로 이동
			break;
		}
	}
	
	return false;   // 토큰 획득 실패
}

nodeptr malloc_node()
{
	nodeptr exp = (nodeptr) malloc(sizeof(node));   // malloc a node (새 노드를 할당한다)
	return memset(exp, NULL, sizeof(node));         // curr->down = NULL;의 동작을 포함함.
}
