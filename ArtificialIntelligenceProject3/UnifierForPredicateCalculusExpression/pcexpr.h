/* Expression �� ǥ��
 *
 * Predicate calculus(PC) expression �� ���α׷����� ���� ����Ʈ�� ǥ���Ѵ�.
 * ���� �Լ� unify()�� ���޵Ǵ� exp(1 and 2)�� ���� ����Ʈ�� ù ��忡 ���� �������̴�.
 * unify()�� �Ķ���ͷ� ���޵Ǵ� expression �� ���� ����Ʈ�� �̿��Ͽ� ǥ���Ѵ�.
 * ���� ����Ʈ�� �� ���� expression�� �����ϴ� ���ڿ��� �����ϰ� �ִ� ( ��ȣ ���� ).
 * �� PC expression�� atom �Ǵ� list �� ���� �� �� �����̴�.
 * 
 * (1) PC expression�� ��ȣ�� ���� ���ڿ� �ϳ��� ��� (atom �̶� �θ���): ��) Julie, Kx23, father
 * ��, function constant, variable, object constant, ��ȣ �� ���ڿ� �� ���� ���Ѵ�.
 * atom�� ���ڿ� �� ���� ������ ���� ǥ���ȴ�.
 * 
 * (2) PC expression�� ���� ��ȣ���� �����Ͽ� ���� ��ȣ�� ������ ��Ʈ���� ��� (�̸� list��� �θ���):
 * list�� ���� ���� ��带 ���� ���� ����Ʈ(L �̶� ����)�� ǥ���Ѵ�.
 * ���� �� ����Ʈ�� �м��Ͽ� ��� ���ҵ��� �˾Ƴ���: ������ ������ atom, list �� �� �����̴�.
 * atom ������ ���, �̸� ���� ��� �ϳ��� �غ��Ͽ� ���� ����Ʈ L�� �� ���Ұ� �ǵ��� �� �ڿ� ���δ�.
 * list ������ ���, ��� �ϳ��� �غ��Ͽ� ������ p �� �����ϰ�, ���� ����Ʈ L�� �� �ڿ� ���δ�.
 * ����Ǵ� ������ p �� list ������ ���� ����Ʈ�� ����Ű���� �Ѵ�.
 */

#ifndef __PCEXPR_H
#define __PCEXPR_H

#include <stdbool.h>    // for bool type, true and false (C99)


/*** ��ũ�� ���� ***/

#define NODE_STR_SIZE   (50)    // ����� ��Ʈ�� ũ��


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
	char replace_term[NODE_STR_SIZE];
	char var[NODE_STR_SIZE];
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

/* ��Ʈ���� �м��Ͽ� ���� ����Ʈ ������ ����� �Լ�
 */
nodeptr get_structure_of_expression(const char *exp_str);

/* ���� ����Ʈ �������� Ư�� �̸��� ������ �̸��� Ư�� ����� �������� �����ϴ� �Լ�
 * head: �����Լ����� �غ�� expression ������ ù ��忡 ���� ������.
 * varString: ���� �̸��� ��Ʈ��.
 * nodeTerm: head ���� ������ varString �̸��� ��� ������ nodeTerm���� �����Ͽ� �־�� �Ѵ�.
 * �Լ� ��ȯ��: �����̸� true (1), �ƴϸ� false (0).
 */
bool apply_substitution_element(nodeptr head, const char varString[], nodeptr nodeTerm);


#endif  /* !__PCEXPR_H */
