#include <stdio.h>
#include "pcexpr.h"

#define EXPR_STR_SIZE   1000
#define EXPR_FILE       "expressions.txt"


void print_out(substi_nodeptr substi);


int main(void)
{
	// main() 함수는 단일화를 원하는 주어진 두 expressions를 스트링 형태로 받는다.
	// (처음에 주어지는 이들은 반드시 리스트여야 한다.)
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

	// 파라메터 &substitution를 통하여 알아낸 치환을 반환하는 데 이용한다.
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
