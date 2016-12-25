#ifndef __PCEXPR_H
#define __PCEXPR_H

#include <stdbool.h>    // for bool type, true and false (C99)


/*** ��ũ�� ���� ***/

#define NODE_STR_SIZE   (50)


/*** ������ ���� ***/

/* Type definition for a pointer for a node */
typedef struct anode *nodeptr;

/* Type definition for a node for linked list representiong an expression */
typedef struct anode {
	char str[NODE_STR_SIZE];    // string
	nodeptr down;               // pointer to the linked list for a list element
	nodeptr right;              // pointer to the next node
} node;

/* ġȯ�� ������ ���� ������ ������ ������ */
typedef struct substi_node *substi_nodeptr;

/* ġȯ�� ������ ���� ������ ���� */
typedef struct substi_node {
	nodeptr replace_term;
	nodeptr var;
	substi_nodeptr next;
} substi_node;


/*** �Լ� ���� ***/

/* Unification�� �����ϴ� �Լ�
 * 3 ���� �Է� �μ��� ����.
 * ó�� 2 ��: ����ȭ�� �� PC ����. (���� ����Ʈ�� ���� �����ͷ� ���޵�.)
 * 3 ��°: ����ȭ ������ �ʿ���, ġȯ�� �޾� ���� ���� ������ ������ �ּ�.
 *         ���� �������� �ʿ��� ġȯ�� �߰ߵǾ����� result_substi�� �ٿ� ����.
 * �Լ� ��ȯ��: �����̸� true (1), �ƴϸ� false (0).
 */
bool unify(nodeptr exp1, nodeptr exp2, substi_nodeptr *substi);

/* ��Ʈ���� �м��Ͽ� ���Ḯ��Ʈ ������ ����� �Լ�
 */
nodeptr get_structure_of_expression(const char *exp_str);

/* Head�� ���� �� VarString �̸��� ��帶�� nodeTerm ����� �������� �����ϴ� �Լ�
 * Head: �����Լ����� �غ�� expression ������ ù ��忡  ���� ������.
 * VarString: ���� �̸��� ��Ʈ��.
 * nodeTerm: Head ���� ������ VarString �̸��� ��� ������ nodeTerm���� �����Ͽ� �־�� �Ѵ�.
 */
int apply_substitution_element(nodeptr head, const char varString[], nodeptr nodeTerm);


#endif  /* !__PCEXPR_H */
