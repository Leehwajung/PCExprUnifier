#ifndef __PCEXPR_H
#define __PCEXPR_H

#include <stdbool.h>    // for bool type, true and false (C99)


/*** 매크로 정의 ***/

#define NODE_STR_SIZE   (50)


/*** 데이터 구조 ***/

/* Type definition for a pointer for a node */
typedef struct anode *nodeptr;

/* Type definition for a node for linked list representiong an expression */
typedef struct anode {
	char str[NODE_STR_SIZE];    // string
	nodeptr down;               // pointer to the linked list for a list element
	nodeptr right;              // pointer to the next node
} node;

/* 치환의 저장을 위한 데이터 구조의 포인터 */
typedef struct substi_node *substi_nodeptr;

/* 치환의 저장을 위한 데이터 구조 */
typedef struct substi_node {
	nodeptr replace_term;
	nodeptr var;
	substi_nodeptr next;
} substi_node;


/*** 함수 선언 ***/

/* Unification을 구현하는 함수
 * 3 개의 입력 인수를 가짐.
 * 처음 2 개: 단일화할 두 PC 문장. (연결 리스트에 대한 포인터로 공급됨.)
 * 3 번째: 단일화 성공에 필요한, 치환을 받아 오기 위한 포인터 변수의 주소.
 *         실행 과정에서 필요한 치환이 발견되었으면 result_substi에 붙여 놓음.
 * 함수 반환값: 성공이면 true (1), 아니면 false (0).
 */
bool unify(nodeptr exp1, nodeptr exp2, substi_nodeptr *substi);

/* 스트링을 분석하여 연결리스트 구조를 만드는 함수
 */
nodeptr get_structure_of_expression(const char *exp_str);

/* Head의 구조 중 VarString 이름의 노드마다 nodeTerm 노드의 내용으로 변경하는 함수
 * Head: 메인함수에서 준비된 expression 구조의 첫 노드에  대한 포인터.
 * VarString: 변수 이름의 스트링.
 * nodeTerm: Head 구조 내에서 VarString 이름의 모든 변수를 nodeTerm으로 변경하여 주어야 한다.
 */
int apply_substitution_element(nodeptr head, const char varString[], nodeptr nodeTerm);


#endif  /* !__PCEXPR_H */
