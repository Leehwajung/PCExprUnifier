#include <malloc.h>
#include <string.h>

#include "pcexpr.h"


/*** ��ũ�� ���� ***/

#ifndef NULL
#define NULL    ((void*) 0)
#endif  // !NULL

#define STACK_SIZE          (100)
#define STACK_INIT_INDEX    (-1)


/*** �Լ� ���� (����) ***/

/* ��ū�� ȹ���ϴ� �Լ� (����)
 * �־��� ���ڿ��� expression�� �����Ͽ� token�� �����Ѵ�.
 * ��: expression = "(Bigger(father(teacher  Bill)) Joe1)"
 *     ��µ� token ��: (, Bigger, (, father, (, teacher, Bill, ), ), Joe1, )
 * str_dst: ���� ȹ���� ��ū�� ������ ����.
 * str_exp: ��ũ����¡�� PC Expression ���ڿ�, �� ��° ȣ����� NULL�� �μ��� �����ϸ� ���� ���ڿ��� ����ؼ� ��ũ����¡��.
 */
bool read_token(char *str_dst, const char *str_exp);

/* �� ��带 �Ҵ��ϴ� �Լ� (����)
 * �� ��带 �Ҵ��ϰ� ���� 0���� �ʱ�ȭ�Ѵ�.
 */
nodeptr malloc_node();


/*** �Լ� ���� ***/

nodeptr get_structure_of_expression(const char *exp_str)
{
	// �˰���
	// (1)
	nodeptr stack[STACK_SIZE];
	int top = STACK_INIT_INDEX; // ���� �ʱ�ȭ. We use a stack to handle lists embedding sub-lists.
	nodeptr exp = NULL;
	node *curr = NULL;
	node *tp, *tp2;
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
		if (hasToken) {     // no more tokens left
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
}

bool read_token(char *str_dst, const char *str_exp)
{
	static char *currAddr = NULL;
	bool isFound = false;
	int strIndex = 0;

	if (currAddr == NULL) {
		currAddr = str_exp; // ���ڿ��� ���� �ּ�
	}

	while (*currAddr != '\0') {
		switch (*currAddr)
		{
			// ���� ����
		case ' ':
		case '\t':
			currAddr++;         // ���� ���ڷ� �̵�
			if (isFound) {      // ���ڿ� ��ū�̸� ���ڿ��� �ϼ��ϰ� ����
				str_dst[strIndex] = '\0';   // ���ڿ� �ϼ�
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
			str_dst[strIndex] = '\0';   // ���ڿ� �ϼ�
			return true;    // ��ū ȹ�� ����

			// ���ڿ� ��ū�� ���� ������
		default:
			isFound = true; // ��ū Ȯ��
			if (strIndex < NODE_STR_SIZE - 1) { // NODE_STR_SIZE ũ������� ����
				str_dst[strIndex++] = *currAddr;
			}
			currAddr++;     // ���� ���ڷ� �̵�
			break;
		}
	}
	
	return false;   // ��ū ȹ�� ����
}

nodeptr malloc_node()
{
	nodeptr exp = (nodeptr) malloc(sizeof(node));   // malloc a node (�� ��带 �Ҵ��Ѵ�)
	return memset(exp, NULL, sizeof(node));         // curr->down = NULL;�� ������ ������.
}
