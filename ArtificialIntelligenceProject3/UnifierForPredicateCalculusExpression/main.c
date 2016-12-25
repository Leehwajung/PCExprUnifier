#include <stdio.h>
#include "pcexpr.h"

#define EXPR_STR_SIZE   1000
#define EXPR_FILE       "expressions.txt"


void print_out(substi_nodeptr substi);


/* main
 * 두 개의 PC expression에 대한 unification 작업을 수행하는 프로그램을 작성하고 실험한다.
 * unify()를 이용하여 unification 작업을 수행한다.
 * 입력: 파일에서 입력 받는다. 파일은, 두 개의 PC 문을 쌍으로 하여, 여러 쌍의 PC 문을 가지고 있는다.
 *       두 쌍 사이에는 첫 글자가 %인 줄로 구별한다.
 *       파일 ( expressions.txt ) 예:
 *       (Arel x (bfun (caption Ateam) u) (mofu x) z)
 *       (Arel Bob (bfun y Chen) (mofu v) y)
 *       %
 *       (Lookat x (goalkeeper (firstteam Korea)) (3fl GS))
 *       (y Crowds (z (firstteam w)) v)
 *       %
 *       (Request (x (transocean KE)) (y Acompany) lunch2)
 *       (Request (x (transocean KE)) (softsoda x) lunch)
 * 
 * 프로그램 실행 예:
 * Unify result: Bob / x, (caption Ateam) / y, Chen / u, Bob / v, (caption Ateam) / z
 * Unify result: Lookat / y, Crowds / x, goalkeeper / z, Korea / w, (3fl GS) / v
 * Unify result: Unification failed.
 * Program ends!
 */
int main(void)
{
	// main() 함수는 단일화를 원하는 주어진 두 expressions를 스트링 형태로 받는다.
	// (처음에 주어지는 이들은 반드시 리스트여야 한다.)
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
				
			// 파라메터 &substitution를 통하여 알아낸 치환을 반환하는 데 이용한다.
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
