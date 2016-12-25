#include <malloc.h>
#include <string.h>

#include "pcexpr.h"


/*** 매크로 정의 ***/

#ifndef NULL
#define NULL    ((void*) 0)
#endif  // !NULL

#define STACK_SIZE          (100)
#define STACK_INIT_INDEX    (-1)


/*** 열거형 정의 ***/

/* PC Expression의 종류 */
typedef enum expr_kind {
	//NONE        = 0x00, // 타입을 알 수 없음.
	LIT_ATOM    = 0x02, // 상수 atom: 노드가 좌측 괄호를 갖지 않음. 그리고 변수가 아닌 형태의 스트링을 가짐.
	VAR_ATOM    = 0x03, // 변수 atom: 노드가 좌측 괄호를 갖지 않음. 그리고 저장된 스트링이 u~z 사이의 글자 한 개.
	LIST        = 0x04  // list: 노드가 스트링 "("를 가지는 경우임.
} expr_kind;


/*** 함수 선언 (로컬) ***/

/* 토큰을 획득하는 함수 (로컬)
 * 주어진 스트링의 expression을 분해하여 token을 생성한다.
 * 예: expression = "(Bigger(father(teacher  Bill)) Joe1)"
 *     출력된 token 열: (, Bigger, (, father, (, teacher, Bill, ), ), Joe1, )
 * str_dst: 현재 획득한 토큰을 저장할 공간.
 * str_exp: 토크나이징할 PC Expression 스트링, 두 번째 호출부터 NULL을 인수로 전달하면 이전 스트링을 계속해서 토크나이징함.
 * 함수 반환값: 성공하면 true, 실패하면 false.
 */
bool read_token(char *str_dst, const char *str_exp);

/* 새 노드를 할당하는 함수 (로컬)
 * 새 노드를 할당하고 전체 값을 0으로 초기화한다.
 */
nodeptr malloc_node();

/* expr이 어떤 종류인지 판단하는 함수 (로컬)
 */
expr_kind get_kind_of_expr(nodeptr expr);

/* 두 exprs를 비교하는 함수 (로컬)
 */
bool compare_nodes(nodeptr exp1, nodeptr exp2);

/* 새 치환 노드를 할당하는 함수 (로컬)
 * 새 치환 노드를 할당하고 전체 값을 0으로 초기화한다.
 */
substi_nodeptr malloc_substi_node();

/* expr 내에 varString 변수를 가지는 노드를 찾는 함수 (로컬)
 * 함수 반환값: 찾으면 true, 없으면 false
 */
bool has_node(nodeptr expr, const char varString[]);

/* expr이 List일 때 치환할 스트링을 가져오는 함수 (로컬)
 */
void node_strcpy(char *strDestination, size_t numberOfElements, nodeptr expr);

/* expr의 원소의 수 (로컬)
 */
int num_nodes_of_expr(nodeptr expr);


/*** 함수 정의 ***/

nodeptr get_structure_of_expression(const char *exp_str)
{
	// 알고리즘
	// (1)
	nodeptr stack[STACK_SIZE];
	int top = STACK_INIT_INDEX; // 스택 초기화. We use a stack to handle lists embedding sub-lists.
	nodeptr exp = NULL;
	nodeptr curr = NULL;
	nodeptr tp, tp2;
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
		if (!hasToken) {    // no more tokens left
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
}   // end of get_structure_of_expression()

bool unify_sub(nodeptr head1, nodeptr exp1, nodeptr head2, nodeptr exp2, substi_nodeptr *substi)
{
	substi_nodeptr stp = NULL;
	nodeptr p1 = NULL, p2 = NULL, temp1 = NULL, temp2 = NULL;
	substi_nodeptr temp_substi = NULL;
	bool res = false;

	/* (0) */
	*substi = NULL; // 일단 아무 치환도 필요치 않은 것으로 초기화.

	/* (1) exp1 과 exp2 각각이 어떤 종류인지를 판단한다. */
	const expr_kind exp1Kind = get_kind_of_expr(exp1);
	const expr_kind exp2Kind = get_kind_of_expr(exp2);

	/* (2)  exp1의 종류에 따라 다음 중 한가지를 처리한다: */
	switch (exp1Kind)
	{
	// 2-1) exp1 = 상수 atom 인 경우:
	case LIT_ATOM:
		switch (exp2Kind)
		{
		// exp2가 exp1과 동일한 상수이면
		case LIT_ATOM:
			if (compare_nodes(exp1, exp2)) {
				return true;    // 1
			}
			else {
				return false;   // 0
			}
			
		// (exp2 = 변수)인 경우:
		case VAR_ATOM:
			stp = malloc_substi_node(); // malloc a substi_node
			strcpy_s(stp->replace_term, NODE_STR_SIZE, exp1->str);
			strcpy_s(stp->var, NODE_STR_SIZE, exp2->str);
			stp->next = NULL;
			*substi = stp;  // 치환 요소 한 개를 붙인다.
			apply_substitution_element(head2, exp2->str, exp1); // head2의 구조 중 exp2 변수 노드마다 exp1 노드의 내용으로 변경한다.
			return true;    // 치환 요소는 한 개만을 넘긴다.
			
		// (exp2 = list)인 경우:
		case LIST:
			return false;   // 0
		}
		break;

	// 2-2) exp1 = 변수인 경우:
	case VAR_ATOM:
		switch (exp2Kind)
		{
		// exp2가 변수 atom인 경우:
		case VAR_ATOM:  // (의도적으로 break문을 없앰)
			// exp2가 exp1과 동일한 변수
			if (compare_nodes(exp1, exp2)) {
				return true;    // 1
			}
			
		// exp2가 변수 또는 상수 atom인 경우:
		case LIT_ATOM:
			stp = malloc_substi_node(); // malloc a substi_node
			strcpy_s(stp->replace_term, NODE_STR_SIZE, exp2->str);
			strcpy_s(stp->var, NODE_STR_SIZE, exp1->str);
			stp->next = NULL;
			*substi = stp;  // 치환 요소 한 개를 가진 리스트를 넘긴다.
			apply_substitution_element(head1, exp1->str, exp2);
			return true;    // 1
			
		// exp2가 list이면
		case LIST:
			if (has_node(exp2, exp1->str) != false) {   // 만약 exp2 내에 exp1의 변수를 가지는 노드가 존재하면
				return true;    // 1
			}
			else {
				stp = malloc_substi_node();
				node_strcpy(stp->replace_term, NODE_STR_SIZE, exp2);
				strcpy_s(stp->var, NODE_STR_SIZE, exp1->str);
				stp->next = NULL;
				*substi = stp;  // *substi에 (exp2 / exp1) 치환 요소를 매단다.
				apply_substitution_element(head1, exp1->str, exp2);
				return true;    // 치환 요소는 한 개만을 넘긴다.
			}
		}
		break;

	// 2-3)  exp1 = list 인 경우:
	// (주: 이 판단은 exp1->str이 "("임을 확인하여 가능하다.)
	case LIST:
		switch (exp2Kind)
		{
		// exp2가 상수 atom이면
		case LIT_ATOM:
			return false;   // 0
			
		// exp2가 변수이면
		case VAR_ATOM:
			if (has_node(exp1, exp2->str)) {    // exp1 내에 exp2와 동일한 변수를 가지는 노드가 존재하면
				return true;    // 1
			}
			else {
				stp = malloc_substi_node();
				node_strcpy(stp->replace_term, NODE_STR_SIZE, exp1);
				strcpy_s(stp->var, NODE_STR_SIZE, exp2->str);
				stp->next = NULL;
				*substi = stp;  // *substi에 (exp1 / exp2) 치환 요소를 매단다.
				apply_substitution_element(head2, exp2->str, exp1); // head2의 구조 내에 exp2의 변수를 가지는 노드를 모두 exp1을 가지는 노드로 변경한다.
				return true;    // 치환 요소는 한 개만을 넘긴다.
			}
			break;
			
		// exp2가 리스트이면 (exp1, exp2 모두 list이다.)
		case LIST:
			if (num_nodes_of_expr(exp1) != num_nodes_of_expr(exp2)) {   // exp1과 exp2의 원소의 수가 다르다면
				return false;   // 0
			}
			substi_nodeptr curr = NULL;
			// exp1, exp2 모두 '('를 가진 노드를 가리키고 있다.
			p1 = exp1->right;
			p2 = exp2->right;  // 각자 다음 노드로 이동.
			
			do {
				if (p1->str[0] == '\0') {   //p1이 '\0'를 가진 노드를 가리키면
					temp1 = p1->down;  // 리스트 노드이다. "(" 노드를 가리키게 한다.
				}
				else {
					temp1 = p1;
				}
				
				if (p2->str[0] == '\0') {   //p2이 '\0'를 가진 노드를 가리키면
					temp2 = p2->down;  // 리스트 노드이다. "(" 노드를 가리키게 한다.
				}
				else {
					temp2 = p2;
				}
				
				temp_substi = NULL;
				res = unify_sub(head1, temp1, head2, temp2, &temp_substi);    // recursive call
				if (res == false) { // 0
					return false;   // 0
				}
				
				// Attach substitution elements in the list of temp_substi to the list of *substi;
				if (temp_substi != NULL) {
					if (*substi == NULL) {
						*substi = temp_substi;
					}
					else {
						curr->next = temp_substi;
					}
					while (temp_substi != NULL) {
						curr = temp_substi;
						temp_substi = temp_substi->next;
					}
				}
				
				p1 = p1->right;
				p2 = p2->right;
				res = (p2->str[0] == ')');
				if (p1->str[0] == ')') {    // p1 과 p2 가 가리키는 노드 중 하나라도 ')'를 가진다면
					if (res) {  // p1, p2 둘다 ')'를 가진다면
						break;  // exit this loop;
					}
					else {
						return false;   // 0
					}
				}
				else if (res) {
					return false;   // 0
				}
			} while (true);
			
			return true;    // 파라미터 *substi을 통하여 치환이 호출 측으로 전달된다.
		}
		break;
	}
	return false;
}   // end of unify_sub()

bool unify(nodeptr exp1, nodeptr exp2, substi_nodeptr *substi)
{
	return unify_sub(exp1, exp1, exp2, exp2, substi);
}   // end of unify()

bool apply_substitution_element_sub(nodeptr head, const char varString[], nodeptr nodeTerm)
{
	// head: 메인함수에서 준비된 expression 구조의 첫 노드에 대한 포인터.
	// varString: 변수 노드의 스트링. 이 노드는 head 구조의 한 노드이다.
	// nodeTerm: head 구조 내에서 nodeVar의 변수를 모두 nodeTerm 으로 변경하여 주어야 한다.
	//
	// Return value: it returns true (1) if successful; Otherwise false (0) is returned. (질문: false(0)가 반환되는 경우가 있는가?)

	bool res = false;
	do {
		if (get_kind_of_expr(head) == VAR_ATOM) {   // head->str is a variable
			if (strcmp(head->str, varString) == 0) {    // 치환이 필요한 노드를 발견함 (head->str == VarString)
				if (get_kind_of_expr(nodeTerm) & 0x02) {    // nodeTerm->str is an atom
					strcpy_s(head->str, NODE_STR_SIZE, nodeTerm->str);  // 변수를 atom 으로 변경함 (head->str = nodeTerm->str)
				}
				else if (nodeTerm->str[0] == '(') { // (nodeTerm->str = "(")
					head->str[0] = '\0';            // (head->str = "\0")
					head->down = nodeTerm;          // 변수를 리스트로 변경함
				}
			}
		}
		
		else if (head->str[0] == '\0') {    // head->str == "\0"
			res = apply_substitution_element(head->down, varString, nodeTerm);
			if (res == false) {
				return false;   // 0
			}
		}
		
		head = head->right; // 우측 노드로 이동한다.
		if (head->str[0] == ')') {  // 이 리스트의 원소를 모두 처리하였음. (head->str = ")")
			break;
		}
	} while (true);
	
	return true;    // 1
}   // end of apply_substitution_element_sub()

bool apply_substitution_element(nodeptr head, const char varString[], nodeptr nodeTerm)
{
	char varStrRepo[NODE_STR_SIZE];
	strcpy_s(varStrRepo, NODE_STR_SIZE, varString);
	return apply_substitution_element_sub(head, varStrRepo, nodeTerm);
}   // end of apply_substitution_element()

bool read_token(char *str_dst, const char *str_exp)
{
	static char *currAddr = NULL;
	int strIndex = 0;
	bool isFound = false;

	if (currAddr == NULL) {
		currAddr = (char*) str_exp; // 스트링의 시작 주소
	}

	while (*currAddr != '\0') {
		switch (*currAddr)
		{
			// 공백 문자
		case ' ':
		case '\t':
		case '\n':
			currAddr++;         // 다음 문자로 이동
			if (isFound) {      // 스트링 토큰이면 스트링을 완성하고 종료
				str_dst[strIndex] = '\0';   // 스트링 완성
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
			str_dst[strIndex] = '\0';   // 스트링 완성
			return true;    // 토큰 획득 성공

			// 스트링 토큰의 문자 모으기
		default:
			isFound = true; // 토큰 확인
			if (strIndex < NODE_STR_SIZE - 1) { // NODE_STR_SIZE 크기까지만 저장
				str_dst[strIndex++] = *currAddr;
			}
			currAddr++;     // 다음 문자로 이동
			break;
		}
	}

	currAddr = NULL;
	return false;   // 토큰 획득 실패
}

nodeptr malloc_node()
{
	nodeptr exp = (nodeptr) malloc(sizeof(node));   // malloc a node (새 노드를 할당한다)
	return (nodeptr) memset(exp, 0, sizeof(node));  // curr->down = NULL;의 동작을 포함함.
}

expr_kind get_kind_of_expr(nodeptr expr)
{
	// expr이 어떤 종류인지를 판단한다.
	char firChar = expr->str[0];

	// 가능성은 다음과 같다:
	// list: expr 포인터가 가리키는 노드가 스트링 "("를 가지는 경우임.
	if (firChar == '(') {
		return LIST;
	}

	// 변수 atom: expr 포인터가 가리키는 노드가 좌측 괄호를 갖지 않음. 그리고 저장된 스트링이 u~z 사이의 글자 한 개.
	else if (strlen(expr->str) == 1 && (firChar >= 'u' && firChar <= 'z')) {
		return VAR_ATOM;
	}

	// 상수 atom: expr 포인터가 가리키는 노드가 좌측 괄호를 갖지 않음. 그리고 변수가 아닌 형태의 스트링을 가짐.
	else {
		return LIT_ATOM;
	}
}

bool compare_nodes(nodeptr exp1, nodeptr exp2)
{
	return strcmp(exp1->str, exp2->str) == 0 ? true : false;
}

substi_nodeptr malloc_substi_node()
{
	substi_nodeptr exp = (substi_nodeptr) malloc(sizeof(substi_node));  // malloc a substi_node 
	return (substi_nodeptr) memset(exp, 0, sizeof(substi_node));
}

bool has_node(nodeptr expr, const char varString[])
{
	if (strcmp(expr->str, varString) == 0) {
		return true;
	}
	else {
		if (expr->right != NULL) {
			if (has_node(expr->right, varString)) {
				return true;
			}
		}
		if (expr->down != NULL) {
			if (has_node(expr->down, varString)) {
				return true;
			}
		}
	}
	return false;
}

void node_strcpy(char *strDestination, size_t numberOfElements, nodeptr expr)
{
	int bracketNum = 0;

	strcpy_s(strDestination, numberOfElements, expr);
	do {
		if (expr->str[0] == '(') {
			bracketNum++;
		}
		else if (expr->str[0] == ')') {
			bracketNum--;
		}
		expr = expr->right;
		if (bracketNum > 0 && expr != NULL) {
			strcat_s(strDestination, numberOfElements, " ");
			strcat_s(strDestination, numberOfElements, expr->str);
		}
		else {
			break;
		}
	} while (true);
}

int num_nodes_of_expr_sub(nodeptr expr, int num)
{
	num++;
	if (expr->right != NULL) {
		num = num_nodes_of_expr_sub(expr->right, num);
	}
	//if (expr->down != NULL) {
	//	num = num_nodes_of_expr_sub(expr->down, num);
	//}
	return num;
}

int num_nodes_of_expr(nodeptr expr)
{
	return num_nodes_of_expr_sub(expr, 0);
}
