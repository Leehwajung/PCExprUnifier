#include <malloc.h>
#include <string.h>

#include "pcexpr.h"


/*** ��ũ�� ���� ***/

#ifndef NULL
#define NULL    ((void*) 0)
#endif  // !NULL

#define STACK_SIZE          (100)
#define STACK_INIT_INDEX    (-1)


/*** ������ ���� ***/

/* PC Expression�� ���� */
typedef enum expr_kind {
	//NONE        = 0x00, // Ÿ���� �� �� ����.
	LIT_ATOM    = 0x02, // ��� atom: ��尡 ���� ��ȣ�� ���� ����. �׸��� ������ �ƴ� ������ ��Ʈ���� ����.
	VAR_ATOM    = 0x03, // ���� atom: ��尡 ���� ��ȣ�� ���� ����. �׸��� ����� ��Ʈ���� u~z ������ ���� �� ��.
	LIST        = 0x04  // list: ��尡 ��Ʈ�� "("�� ������ �����.
} expr_kind;


/*** �Լ� ���� (����) ***/

/* ��ū�� ȹ���ϴ� �Լ� (����)
 * �־��� ��Ʈ���� expression�� �����Ͽ� token�� �����Ѵ�.
 * ��: expression = "(Bigger(father(teacher  Bill)) Joe1)"
 *     ��µ� token ��: (, Bigger, (, father, (, teacher, Bill, ), ), Joe1, )
 * str_dst: ���� ȹ���� ��ū�� ������ ����.
 * str_exp: ��ũ����¡�� PC Expression ��Ʈ��, �� ��° ȣ����� NULL�� �μ��� �����ϸ� ���� ��Ʈ���� ����ؼ� ��ũ����¡��.
 * �Լ� ��ȯ��: �����ϸ� true, �����ϸ� false.
 */
bool read_token(char *str_dst, const char *str_exp);

/* �� ��带 �Ҵ��ϴ� �Լ� (����)
 * �� ��带 �Ҵ��ϰ� ��ü ���� 0���� �ʱ�ȭ�Ѵ�.
 */
nodeptr malloc_node();

/* expr�� � �������� �Ǵ��ϴ� �Լ� (����)
 */
expr_kind get_kind_of_expr(nodeptr expr);

/* �� exprs�� ���ϴ� �Լ� (����)
 */
bool compare_nodes(nodeptr exp1, nodeptr exp2);

/* �� ġȯ ��带 �Ҵ��ϴ� �Լ� (����)
 * �� ġȯ ��带 �Ҵ��ϰ� ��ü ���� 0���� �ʱ�ȭ�Ѵ�.
 */
substi_nodeptr malloc_substi_node();

/* expr ���� varString ������ ������ ��带 ã�� �Լ� (����)
 * �Լ� ��ȯ��: ã���� true, ������ false
 */
bool has_node(nodeptr expr, const char varString[]);

/* expr�� List�� �� ġȯ�� ��Ʈ���� �������� �Լ� (����)
 */
void node_strcpy(char *strDestination, size_t numberOfElements, nodeptr expr);

/* expr�� ������ �� (����)
 */
int num_nodes_of_expr(nodeptr expr);


/*** �Լ� ���� ***/

nodeptr get_structure_of_expression(const char *exp_str)
{
	// �˰���
	// (1)
	nodeptr stack[STACK_SIZE];
	int top = STACK_INIT_INDEX; // ���� �ʱ�ȭ. We use a stack to handle lists embedding sub-lists.
	nodeptr exp = NULL;
	nodeptr curr = NULL;
	nodeptr tp, tp2;
	char tok[NODE_STR_SIZE];
	
	bool hasToken = read_token(tok, exp_str);   // read token into tok
	if (!hasToken || tok[0] != '(') {   // ù ��ū�� �ݵ�� ���� ��ȣ���� �Ѵ�.
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
		
		tp = malloc_node(); // �� ��带 �Ҵ�
		
		if (tok[0] == '(') {    // tok is left parenthesis
			tp->str[0] = '\0';
			curr->right = tp;
			top++;
			stack[top] = tp;    // ���ƿ� ���� ����
			tp2 = malloc_node();
			strcpy_s(tp2->str, NODE_STR_SIZE, "(");
			tp2->down = NULL;
			tp->down = tp2;
			curr = tp2;         // ���� ������ ������
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
	*substi = NULL; // �ϴ� �ƹ� ġȯ�� �ʿ�ġ ���� ������ �ʱ�ȭ.

	/* (1) exp1 �� exp2 ������ � ���������� �Ǵ��Ѵ�. */
	const expr_kind exp1Kind = get_kind_of_expr(exp1);
	const expr_kind exp2Kind = get_kind_of_expr(exp2);

	/* (2)  exp1�� ������ ���� ���� �� �Ѱ����� ó���Ѵ�: */
	switch (exp1Kind)
	{
	// 2-1) exp1 = ��� atom �� ���:
	case LIT_ATOM:
		switch (exp2Kind)
		{
		// exp2�� exp1�� ������ ����̸�
		case LIT_ATOM:
			if (compare_nodes(exp1, exp2)) {
				return true;    // 1
			}
			else {
				return false;   // 0
			}
			
		// (exp2 = ����)�� ���:
		case VAR_ATOM:
			stp = malloc_substi_node(); // malloc a substi_node
			strcpy_s(stp->replace_term, NODE_STR_SIZE, exp1->str);
			strcpy_s(stp->var, NODE_STR_SIZE, exp2->str);
			stp->next = NULL;
			*substi = stp;  // ġȯ ��� �� ���� ���δ�.
			apply_substitution_element(head2, exp2->str, exp1); // head2�� ���� �� exp2 ���� ��帶�� exp1 ����� �������� �����Ѵ�.
			return true;    // ġȯ ��Ҵ� �� ������ �ѱ��.
			
		// (exp2 = list)�� ���:
		case LIST:
			return false;   // 0
		}
		break;

	// 2-2) exp1 = ������ ���:
	case VAR_ATOM:
		switch (exp2Kind)
		{
		// exp2�� ���� atom�� ���:
		case VAR_ATOM:  // (�ǵ������� break���� ����)
			// exp2�� exp1�� ������ ����
			if (compare_nodes(exp1, exp2)) {
				return true;    // 1
			}
			
		// exp2�� ���� �Ǵ� ��� atom�� ���:
		case LIT_ATOM:
			stp = malloc_substi_node(); // malloc a substi_node
			strcpy_s(stp->replace_term, NODE_STR_SIZE, exp2->str);
			strcpy_s(stp->var, NODE_STR_SIZE, exp1->str);
			stp->next = NULL;
			*substi = stp;  // ġȯ ��� �� ���� ���� ����Ʈ�� �ѱ��.
			apply_substitution_element(head1, exp1->str, exp2);
			return true;    // 1
			
		// exp2�� list�̸�
		case LIST:
			if (has_node(exp2, exp1->str) != false) {   // ���� exp2 ���� exp1�� ������ ������ ��尡 �����ϸ�
				return true;    // 1
			}
			else {
				stp = malloc_substi_node();
				node_strcpy(stp->replace_term, NODE_STR_SIZE, exp2);
				strcpy_s(stp->var, NODE_STR_SIZE, exp1->str);
				stp->next = NULL;
				*substi = stp;  // *substi�� (exp2 / exp1) ġȯ ��Ҹ� �Ŵܴ�.
				apply_substitution_element(head1, exp1->str, exp2);
				return true;    // ġȯ ��Ҵ� �� ������ �ѱ��.
			}
		}
		break;

	// 2-3)  exp1 = list �� ���:
	// (��: �� �Ǵ��� exp1->str�� "("���� Ȯ���Ͽ� �����ϴ�.)
	case LIST:
		switch (exp2Kind)
		{
		// exp2�� ��� atom�̸�
		case LIT_ATOM:
			return false;   // 0
			
		// exp2�� �����̸�
		case VAR_ATOM:
			if (has_node(exp1, exp2->str)) {    // exp1 ���� exp2�� ������ ������ ������ ��尡 �����ϸ�
				return true;    // 1
			}
			else {
				stp = malloc_substi_node();
				node_strcpy(stp->replace_term, NODE_STR_SIZE, exp1);
				strcpy_s(stp->var, NODE_STR_SIZE, exp2->str);
				stp->next = NULL;
				*substi = stp;  // *substi�� (exp1 / exp2) ġȯ ��Ҹ� �Ŵܴ�.
				apply_substitution_element(head2, exp2->str, exp1); // head2�� ���� ���� exp2�� ������ ������ ��带 ��� exp1�� ������ ���� �����Ѵ�.
				return true;    // ġȯ ��Ҵ� �� ������ �ѱ��.
			}
			break;
			
		// exp2�� ����Ʈ�̸� (exp1, exp2 ��� list�̴�.)
		case LIST:
			if (num_nodes_of_expr(exp1) != num_nodes_of_expr(exp2)) {   // exp1�� exp2�� ������ ���� �ٸ��ٸ�
				return false;   // 0
			}
			substi_nodeptr curr = NULL;
			// exp1, exp2 ��� '('�� ���� ��带 ����Ű�� �ִ�.
			p1 = exp1->right;
			p2 = exp2->right;  // ���� ���� ���� �̵�.
			
			do {
				if (p1->str[0] == '\0') {   //p1�� '\0'�� ���� ��带 ����Ű��
					temp1 = p1->down;  // ����Ʈ ����̴�. "(" ��带 ����Ű�� �Ѵ�.
				}
				else {
					temp1 = p1;
				}
				
				if (p2->str[0] == '\0') {   //p2�� '\0'�� ���� ��带 ����Ű��
					temp2 = p2->down;  // ����Ʈ ����̴�. "(" ��带 ����Ű�� �Ѵ�.
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
				if (p1->str[0] == ')') {    // p1 �� p2 �� ����Ű�� ��� �� �ϳ��� ')'�� �����ٸ�
					if (res) {  // p1, p2 �Ѵ� ')'�� �����ٸ�
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
			
			return true;    // �Ķ���� *substi�� ���Ͽ� ġȯ�� ȣ�� ������ ���޵ȴ�.
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
	// head: �����Լ����� �غ�� expression ������ ù ��忡 ���� ������.
	// varString: ���� ����� ��Ʈ��. �� ���� head ������ �� ����̴�.
	// nodeTerm: head ���� ������ nodeVar�� ������ ��� nodeTerm ���� �����Ͽ� �־�� �Ѵ�.
	//
	// Return value: it returns true (1) if successful; Otherwise false (0) is returned. (����: false(0)�� ��ȯ�Ǵ� ��찡 �ִ°�?)

	bool res = false;
	do {
		if (get_kind_of_expr(head) == VAR_ATOM) {   // head->str is a variable
			if (strcmp(head->str, varString) == 0) {    // ġȯ�� �ʿ��� ��带 �߰��� (head->str == VarString)
				if (get_kind_of_expr(nodeTerm) & 0x02) {    // nodeTerm->str is an atom
					strcpy_s(head->str, NODE_STR_SIZE, nodeTerm->str);  // ������ atom ���� ������ (head->str = nodeTerm->str)
				}
				else if (nodeTerm->str[0] == '(') { // (nodeTerm->str = "(")
					head->str[0] = '\0';            // (head->str = "\0")
					head->down = nodeTerm;          // ������ ����Ʈ�� ������
				}
			}
		}
		
		else if (head->str[0] == '\0') {    // head->str == "\0"
			res = apply_substitution_element(head->down, varString, nodeTerm);
			if (res == false) {
				return false;   // 0
			}
		}
		
		head = head->right; // ���� ���� �̵��Ѵ�.
		if (head->str[0] == ')') {  // �� ����Ʈ�� ���Ҹ� ��� ó���Ͽ���. (head->str = ")")
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
		currAddr = (char*) str_exp; // ��Ʈ���� ���� �ּ�
	}

	while (*currAddr != '\0') {
		switch (*currAddr)
		{
			// ���� ����
		case ' ':
		case '\t':
		case '\n':
			currAddr++;         // ���� ���ڷ� �̵�
			if (isFound) {      // ��Ʈ�� ��ū�̸� ��Ʈ���� �ϼ��ϰ� ����
				str_dst[strIndex] = '\0';   // ��Ʈ�� �ϼ�
				return true;    // ��ū ȹ�� ����
			}
			else {  // ���� ���ڴ� �ǳ� ��
				break;
			}

			// ��ȣ
		case '(':
		case ')':
			if (!isFound) { // ��ȣ ��ū
				str_dst[strIndex++] = *currAddr;
				currAddr++; // ���� ���ڷ� �̵� (isFound == true�� ���� ���� ��ū�� ������ �����̹Ƿ�, isFound == false�� ���� �̵�)
			}
			str_dst[strIndex] = '\0';   // ��Ʈ�� �ϼ�
			return true;    // ��ū ȹ�� ����

			// ��Ʈ�� ��ū�� ���� ������
		default:
			isFound = true; // ��ū Ȯ��
			if (strIndex < NODE_STR_SIZE - 1) { // NODE_STR_SIZE ũ������� ����
				str_dst[strIndex++] = *currAddr;
			}
			currAddr++;     // ���� ���ڷ� �̵�
			break;
		}
	}

	currAddr = NULL;
	return false;   // ��ū ȹ�� ����
}

nodeptr malloc_node()
{
	nodeptr exp = (nodeptr) malloc(sizeof(node));   // malloc a node (�� ��带 �Ҵ��Ѵ�)
	return (nodeptr) memset(exp, 0, sizeof(node));  // curr->down = NULL;�� ������ ������.
}

expr_kind get_kind_of_expr(nodeptr expr)
{
	// expr�� � ���������� �Ǵ��Ѵ�.
	char firChar = expr->str[0];

	// ���ɼ��� ������ ����:
	// list: expr �����Ͱ� ����Ű�� ��尡 ��Ʈ�� "("�� ������ �����.
	if (firChar == '(') {
		return LIST;
	}

	// ���� atom: expr �����Ͱ� ����Ű�� ��尡 ���� ��ȣ�� ���� ����. �׸��� ����� ��Ʈ���� u~z ������ ���� �� ��.
	else if (strlen(expr->str) == 1 && (firChar >= 'u' && firChar <= 'z')) {
		return VAR_ATOM;
	}

	// ��� atom: expr �����Ͱ� ����Ű�� ��尡 ���� ��ȣ�� ���� ����. �׸��� ������ �ƴ� ������ ��Ʈ���� ����.
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
