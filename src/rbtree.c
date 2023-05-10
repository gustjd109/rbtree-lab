#include "rbtree.h"
#include <stdlib.h>

/* 새 트리 생성 함수 */
rbtree *new_rbtree(void) {
  // 트리 메모리 할당
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));

  // nil 노드 생성 및 초기화
  node_t *nil_node = (node_t *)calloc(1, sizeof(node_t));
  nil_node->color = RBTREE_BLACK;
  p->nil = p->root = nil_node;

  return p;
}

/* 트리를 순회하면서 각 노드의 메모리 반환 함수 */
void delete_rbtree(rbtree *t) {
  // 트리가 비어있지 않으면, 트리를 순회하면서 각 노드의 메모리 반환
  if(t->root != t->nil) {
    delete_node(t, t->root);
  }
  // nil 노드와 트리 구조체 메모리 반환
  free(t->nil);
  free(t);
}

/* 각 노드와 그 자녀 노드들의 메모리 반환 함수 */
void  delete_node(rbtree *t, node_t *node) {
  // 트리를 후위 순회하면서 각 노드와 그 자녀 노드들의 메모리 반환 
  if(node->left != t->nil) {
    delete_node(t, node->left);
  }
  if(node->right != t->nil) {
    delete_node(t, node->right);
  }
  free(node);
}

/* 노드 삽입 함수 */
node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *y = t->nil;
  node_t *x = t->root;

  // 삽입 노드 생성 및 메모리와 key 할당
  node_t *z = (node_t *)calloc(1, sizeof(node_t));
  z->key = key;

  // 삽입 노드의 부모 위치 탐색
  while(x != t->nil) {
    y = x;
    if(z->key < x->key) {
      x = x->left;
    }
    else {
      x = x->right;
    }
  }

  z->parent = y; // 삽입 노드에 부모 노드 연결
  
  // 삽입 노드가 첫 노드인 경우
  if(y == t->nil) { 
    t->root = z; // 삽입 노드를 루트 노드로 설정
  }
  // 삽입 노드가 찾은 부모 노드보다 작은 경우
  else if(z->key < y->key) {
    y->left = z; // 삽입 노드를 부모 노드 왼쪽 자녀 노드로 설정
  }
  // 삽입 노드가 찾은 부모 노드보다 큰 경우
  else {
    y->right = z; // 삽입 노드를 부모 노드 오른쪽 자녀 노드로 설정
  }

  z->left = z->right = t->nil; // 삽입 노드의 왼, 오른쪽 자녀 노드를 nil 노드로 설정
  z->color = RBTREE_RED; // 삽입 노드의 색은 항상 레드로 설정

  // 노드 삽입 후, 레드 블랙 트리 속성 위반 검사
  rbtree_insert_fixup(t, z);

  return z;
}

/* 노드 삽입 후 불균형 복구 함수 */
void rbtree_insert_fixup(rbtree *t, node_t *z) {
  // 삽입 노드의 삼촌 노드 생성
  node_t *y;

  // 삽입 노드의 부모 노드 색이 레드인 경우 : 속성 4 위반
  while(z->parent->color == RBTREE_RED) {
    // 삽입 노드의 부모 노드가 할아버지 노드의 왼쪽 자녀 노드인 경우
    if(z->parent == z->parent->parent->left) {
      // 삼촌 노드를 할아버지 노드 오른쪽 자녀 노드로 설정
      y = z->parent->parent->right;
      // 삼촌 노드 색이 레드인 경우 : CASE 1
      if(y->color == RBTREE_RED) {
        z->parent->color = y->color = RBTREE_BLACK; // 부모 노드와 삼촌 노드 색 블랙으로 변경
        z->parent->parent->color = RBTREE_RED; // 할아버지 노드 색 레드로 변경
        z = z->parent->parent; // 할아버지 노드가 루트 노드가 아니고, 할아버지 노드 위 노드 색이 레드라면 속성 4를 위반하므로 노드의 위치를 할아버지 노드로 설정하여 속성 4 위반 검사
      }
      else { // 삼촌 노드 색이 블랙인 경우
        if(z == z->parent->right) { // 부모 노드가 할아버지 노드의 왼쪽 자녀 & 삽입 노드가 부모 노드의 오른쪽 자녀인 경우 : CASE 2(오른쪽으로 꺽인 형태)
          // 부모 노드를 기준으로 왼쪽 회전하여 꺽인 부분을 펴서 CASE 3 형태로 만듦
          z = z->parent;
          left_rotate(t, z);
        }
        // 부모 노드가 할아버지 노드의 왼쪽 자녀 & 삽입 노드가 부모 노드의 왼쪽 자녀인 경우 : CASE 3(뻗은 형태)
        // 부모 노드와 할아버지 노드 색을 변경해주고, 할아버지 노드를 기준으로 오른쪽 횐전
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      }
    }
    // 왼쪽과 대칭이므로 위와 동일한 방식 적용
    else {
      y = z->parent->parent->left;
      if(y->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else {
        if(z == z->parent->left) {
          z = z->parent;
          right_rotate(t, z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t, z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK; // 삽입 노드가 루트 노드라면, 색 블랙으로 변경
}

/* 왼쪽 회전 함수 */
void left_rotate(rbtree *t, node_t *x) {
  node_t *y = x->right;
  x->right = y->left;

  if(y->left != t->nil) {
    y->left->parent = x;
  }

  y->parent = x->parent;

  if(x->parent == t->nil) {
    t->root = y;
  }
  else if(x == x->parent->left) {
    x->parent->left = y;
  }
  else {
    x->parent->right = y;
  }

  y->left = x;
  x->parent = y;
}

/* 오른쪽 회전 함수 */
void right_rotate(rbtree *t, node_t *x) {
  node_t *y = x->left;
  x->left = y->right;

  if(y->right != t->nil) {
    y->right->parent = x;
  }

  y->parent = x->parent;

  if(x->parent == t->nil) {
    t->root = y;
  }
  else if(x == x->parent->left) {
    x->parent->left = y;
  }
  else {
    x->parent->right = y;
  }

  y->right = x;
  x->parent = y;
}

/* 키에 해당하는 노드 반환 함수 */
node_t *rbtree_find(const rbtree *t, const key_t key) {
  // 루트 노드부터 시작하여 루트 노드의 왼, 오른쪽 값과 비교하면서 찾고자하는 키 값의 노드 탐색
  node_t *current = t->root;

  while(current != t->nil) {
    if(key == current->key) {
      return current;
    }
    else {
      current = (key < current->key) ? current->left : current->right;
    }
  }
  return NULL;
}

/* 트리 최소값 탐색 함수 */
node_t *rbtree_min(const rbtree *t) {
  // 트리의 왼쪽 서브 트리의 마지막 값 탐색
  node_t *current = t->root;

  while(current->left != t->nil) {
    current = current->left; 
  }
  return current;
}

/* 트리 최대값 탐색 함수 */ 
node_t *rbtree_max(const rbtree *t) {
  // 트리의 오른쪽 서브 트리의 마지막 값 탐색
  node_t *current = t->root;
  
  while(current->right != t->nil) {
    current = current->right;
  }
  return current;
}

/* 노드 삭제 함수 */
int rbtree_erase(rbtree *t, node_t *p) {
  node_t *y = p; // 삭제 노드 설정
  node_t *x; // 삭제 노드 위치를 대체할 노드
  color_t yOriginalColor = y->color; // 삭제 노드 색 저장 설정

  // 삭제 노드 자녀가 하나 또는 없는 경우 : 삭제 노드 색 판단 경우 1
  if(p->left == t->nil) {
    x = p->right; // 삭제 노드 위치를 삭제 노드의 오른쪽 자녀로 변경
    rbtree_transplant(t, p, p->right); // 삭제 노드 오른쪽 자녀 노드를 부모 노드에 연결
  }
  else if(p->right == t->nil) {
    x = p->left; // 삭제 노드 위치를 삭제 노드의 왼쪽 자녀로 변경
    rbtree_transplant(t, p, p->left); // 삭제 노드 왼쪽 자녀 노드를 부모 노드에 연결
  }
  // 삭제 노드 왼, 오른쪽 자녀가 모두 있는 경우 : 삭제 노드 색 판단 경우 2
  // 이 경우, 삭제되는 색 = 삭제되는 노드의 successor(삭제되는 노드의 오른쪽 서브트리 중에서 가장 작은 값)의 색
  else {
    // successor 탐색
    y = p->right;
    while(y->left != t->nil) { 
      y = y->left;
    }
    yOriginalColor = y->color; // 삭제 노드의 위치를 대체할 노드 색을 successor 노드 색으로 변경
    x = y->right;

    rbtree_transplant(t, y, y->right);
    y->right = p->right;
    y->right->parent = y;
    
    rbtree_transplant(t, p, y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }
  
  // 삭제 노드가 적색이면, 삭제해도 black height depth에 변화가 없으므로 속성 5를 위반하지 않음
  // 삭제 노드 왼, 오른쪽 자녀가 모두 있는 경우 삭제되는 색이 계속 변경되므로 삭제 노드 색이 블랙인 경우에만 삭제 후 속성 위반 검사
  if(yOriginalColor == RBTREE_BLACK) {
    rbtree_erase_fixup(t, x);
  }

  free(p); // 삭제 노드 메모리 할당 해제

  return 0;
}

/* 노드 삭제 후 불균형 복구 함수 */
// 여기서 케이스 번호는 알고리즘 책의 케이스 기준으로 설정
void rbtree_erase_fixup(rbtree *t, node_t *x) {
  node_t *w; // 삭제 노드를 대체한 노드의 형제 노드
  
  while(x != t->root && x->color == RBTREE_BLACK) { // 삭제 노드를 대체한 노드가 루트 노드이거나, 색이 블랙이 아닐 때까지 반복문 수행
    // 삭제 노드를 대체한 노드가 부모 노드의 왼쪽 자녀 노드인 경우
    if(x == x->parent->left) {
      w = x->parent->right; // 형제 노드를 대체 노드의 부모 노드 오른쪽 자녀 노드로 설정
      if(w->color == RBTREE_RED) { // 형제 노드 색이 레드인 경우 : CASE 1
        // 형제와 부모 노드 색 변경 후 부모 기준으로 왼쪽 회전
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }
      if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) { // 형제 노드 색이 블랙이고, 형제 오른쪽 자녀 색이 블랙인 경우 : CASE 2
        // 삭제 노드를 대체할 노드의 엑스트라 블랙과 형제 노드 색인 블랙을 부모 노드에 전달 후, 부모 노드에서 속성 위반 검사
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else {
        if(w->right->color == RBTREE_BLACK) { // 형제 노드 색이 블랙이고, 형제 오른쪽 자녀 색이 블랙인 경우 : CASE 3
        // 형제와 형제 왼쪽 자녀 노드 색 변경 후 왼쪽 회전
        w->left->color = RBTREE_BLACK;
        w->color = RBTREE_RED;
        right_rotate(t, w);
        w = x->parent->right;
        }
        // 형제 노드 색이 블랙이고, 형제 오른쪽 자녀 색이 레드인 경우 : CASE 4
        // 형제 노드 색을 형제 부모 노드 색, 부모 노드와 형제 노드 색 변경 후 부모 기준으로 왼쪽 회전
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      }
    }
    // 왼쪽과 대칭이므로 위와 동일한 방식 적용
    else {
      w = x->parent->left;
      if(w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      if(w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else {
        if(w->left->color == RBTREE_BLACK) {
        w->right->color = RBTREE_BLACK;
        w->color = RBTREE_RED;
        left_rotate(t, w);
        w = x->parent->left;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }

  x->color = RBTREE_BLACK; // 삭제 노드를 대체한 노드의 색 블랙으로 변경
}

/* 삭제할 노드의 자녀를 부모 노드에 연결하는 함수 */
void rbtree_transplant(rbtree *t, node_t *u, node_t *v) {
  // 삭제할 노드의 부모가 nil인 경우 = 삭제할 노드가 루트 노드인 경우
  if(u->parent == t->nil) { 
    t->root = v; // 루트 노드를 대체할 노드로 설정
  }
  // 삭제할 노드가 부모 왼쪽 자녀인 경우
  else if(u == u->parent->left) {
    u->parent->left = v; // 삭제할 노드의 부모 왼쪽 자녀를 대체할 노드로 설정
  }
  // 삭제할 노드가 부모 오른쪽 자녀인 경우
  else {
    u->parent->right = v; // 삭제할 노드의 부모 오른쪽 자녀를 대체할 노드로 설정
  }
  v->parent = u->parent; // 대체할 노드의 부모를 삭제할 노드의 부모로 설정
}

/* 트리를 중위 순회로 n번 순회한 결과를 배열에 담는 함수 */
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  int arr_idx = 0;
  rbtree_inorder(t, t->root, arr, &arr_idx);
  return 0;
}

/* 중위 순회 함수 */
void rbtree_inorder(const rbtree *t, node_t *current, key_t *arr, int *arr_idx) {
  if(current->left != t->nil) {
    rbtree_inorder(t, current->left, arr, arr_idx);
  }

  arr[*arr_idx] = current->key;
  *arr_idx += 1;

  if(current->right != t->nil) {
    rbtree_inorder(t, current->right, arr, arr_idx);
  }
}
