#include <stdio.h>
#include "pcexpr.h"

#define EXPR_STR_SIZE   1000
#define EXPR_FILE       "expressions.txt"


void print_out(substi_nodeptr substi);


int main(void)
{
	// main() �Լ��� ����ȭ�� ���ϴ� �־��� �� expressions�� ��Ʈ�� ���·� �޴´�.
	// (ó���� �־����� �̵��� �ݵ�� ����Ʈ���� �Ѵ�.)
	// nodeptr exp;
	// exp = get_structure_of_expression("(Passed(cousin  Bill)  medicalexam)");

	FILE *fp = NULL;
	char expression1[EXPR_STR_SIZE], expression2[EXPR_STR_SIZE];
	nodeptr exp1 = NULL, exp2 = NULL;
	fscanf_s(fp, "%s", expression1, EXPR_STR_SIZE);
	fscanf_s(fp, "%s", expression2, EXPR_STR_SIZE);

	nodeptr head1 = get_structure_of_expression(expression1);
	nodeptr head2 = get_structure_of_expression(expression2);
	
	substi_nodeptr substitution = NULL;

	// �Ķ���� &substitution�� ���Ͽ� �˾Ƴ� ġȯ�� ��ȯ�ϴ� �� �̿��Ѵ�.
	int result = unify(head1, head2, &substitution);

	if (result == 1) {
		print_out(substitution);
	}
	else {
		fprintf_s(stderr, "unification failed\n");
		return 1;
	}

	return 0;
}

void print_out(substi_nodeptr substi)
{
}
