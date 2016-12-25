/* Expression 의 표현
 *
 * Predicate calculus(PC) expression 은 프로그램에서 연결 리스트로 표현한다.
 * 따라서 함수 unify()에 공급되는 exp(1 and 2)는 연결 리스트의 첫 노드에 대한 포인터이다.
 * unify()의 파라메터로 공급되는 expression 은 연결 리스트를 이용하여 표현한다.
 * 연결 리스트의 각 노드는 expression을 구성하는 문자열을 저장하고 있다 ( 괄호 포함 ).
 * 각 PC expression은 atom 또는 list 두 가지 중 한 형태이다.
 * 
 * (1) PC expression이 괄호가 없는 문자열 하나인 경우 (atom 이라 부른다): 예) Julie, Kx23, father
 * 즉, function constant, variable, object constant, 괄호 등 문자열 한 개를 말한다.
 * atom은 문자열 한 개를 가지는 노드로 표현된다.
 * 
 * (2) PC expression이 좌측 괄호에서 시작하여 우측 괄호로 끝나는 스트링인 경우 (이를 list라고 부른다):
 * list는 여러 개의 노드를 가진 연결 리스트(L 이라 하자)로 표현한다.
 * 먼저 이 리스트를 분석하여 모든 원소들을 알아낸다: 원소의 종류는 atom, list 의 두 가지이다.
 * atom 원소의 경우, 이를 넣은 노드 하나를 준비하여 연결 리스트 L의 한 원소가 되도록 맨 뒤에 붙인다.
 * list 원소의 경우, 노드 하나를 준비하여 포인터 p 를 저장하고, 연결 리스트 L의 맨 뒤에 붙인다.
 * 저장되는 포인터 p 는 list 원소의 연결 리스트를 가리키도록 한다.
 */

#ifndef __PCEXPR_H
#define __PCEXPR_H

#include <stdbool.h>    // for bool type, true and false (C99)


/*** 매크로 정의 ***/

#define NODE_STR_SIZE   (50)    // 노드의 스트링 크기


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
	char replace_term[NODE_STR_SIZE];
	char var[NODE_STR_SIZE];
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

/* 스트링을 분석하여 연결 리스트 구조를 만드는 함수
 */
nodeptr get_structure_of_expression(const char *exp_str);

/* 연결 리스트 구조에서 특정 이름의 노드들의 이름을 특정 노드의 내용으로 변경하는 함수
 * head: 메인함수에서 준비된 expression 구조의 첫 노드에 대한 포인터.
 * varString: 변수 이름의 스트링.
 * nodeTerm: head 구조 내에서 varString 이름의 모든 변수를 nodeTerm으로 변경하여 주어야 한다.
 * 함수 반환값: 성공이면 true (1), 아니면 false (0).
 */
bool apply_substitution_element(nodeptr head, const char varString[], nodeptr nodeTerm);


#endif  /* !__PCEXPR_H */
