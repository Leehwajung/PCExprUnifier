#include <stdio.h>
#include "pcexpr.h"

#define EXPR_STR_SIZE   1000
#define EXPR_FILE       "expressions.txt"


void print_out(substi_nodeptr substi);


/* main
 * �� ���� PC expression�� ���� unification �۾��� �����ϴ� ���α׷��� �ۼ��ϰ� �����Ѵ�.
 * unify()�� �̿��Ͽ� unification �۾��� �����Ѵ�.
 * �Է�: ���Ͽ��� �Է� �޴´�. ������, �� ���� PC ���� ������ �Ͽ�, ���� ���� PC ���� ������ �ִ´�.
 *       �� �� ���̿��� ù ���ڰ� %�� �ٷ� �����Ѵ�.
 *       ���� ( expressions.txt ) ��:
 *       (Arel x (bfun (caption Ateam) u) (mofu x) z)
 *       (Arel Bob (bfun y Chen) (mofu v) y)
 *       %
 *       (Lookat x (goalkeeper (firstteam Korea)) (3fl GS))
 *       (y Crowds (z (firstteam w)) v)
 *       %
 *       (Request (x (transocean KE)) (y Acompany) lunch2)
 *       (Request (x (transocean KE)) (softsoda x) lunch)
 * 
 * ���α׷� ���� ��:
 * Unify result: Bob / x, (caption Ateam) / y, Chen / u, Bob / v, (caption Ateam) / z
 * Unify result: Lookat / y, Crowds / x, goalkeeper / z, Korea / w, (3fl GS) / v
 * Unify result: Unification failed.
 * Program ends!
 */
int main(void)
{
	// main() �Լ��� ����ȭ�� ���ϴ� �־��� �� expressions�� ��Ʈ�� ���·� �޴´�.
	// (ó���� �־����� �̵��� �ݵ�� ����Ʈ���� �Ѵ�.)
	// nodeptr exp;
	// exp = get_structure_of_expression("(Passed(cousin  Bill)  medicalexam)");

	FILE *fp = NULL;
	char expression1[EXPR_STR_SIZE], expression2[EXPR_STR_SIZE];
	nodeptr exp1 = NULL, exp2 = NULL;
	nodeptr head1 = NULL, head2 = NULL;
	substi_nodeptr substitution = NULL;
	bool result = false;

	if (fopen_s(&fp, EXPR_FILE, "r")) {
		fprintf_s(stderr, "File open failed.\n");
		return 1;
	}

	while (!feof(fp)) {
		if (fgets(expression1, EXPR_STR_SIZE, fp) != NULL && expression1[0] != '%'
					&& fgets(expression2, EXPR_STR_SIZE, fp) != NULL && expression2[0] != '%') {
			head1 = get_structure_of_expression(expression1);
			head2 = get_structure_of_expression(expression2);
			substitution = NULL;
				
			// �Ķ���� &substitution�� ���Ͽ� �˾Ƴ� ġȯ�� ��ȯ�ϴ� �� �̿��Ѵ�.
			result = unify(head1, head2, &substitution);
				
			printf_s("Unify result: ");
			if (result == true) {
				print_out(substitution);
			}
			else {
				fprintf_s(stderr, "Unification failed.\n");
			}
		}
	}

	fclose(fp);
	printf_s("Program ends!\n");

	return 0;
}

void print_out(substi_nodeptr substi)
{
	substi_nodeptr curr = substi;
	if (curr != NULL) {
		while (true) {
			printf_s("%s / %s", curr->replace_term->str, curr->var->str);
			curr = curr->next;
			if (curr != NULL) {
				printf_s(", ");
			}
			else {
				break;
			}
		}
	}
	printf_s("\n");
}
