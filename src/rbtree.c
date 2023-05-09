#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void);
void delete_rbtree(rbtree *t);
node_t *rbtree_insert(rbtree *t, const key_t key);
void rbtree_insert_fixup(rbtree *t, node_t *node);
void left_rotate(rbtree *t, node_t *node);
void right_rotate(rbtree *t, node_t *node);
node_t *rbtree_find(const rbtree *t, const key_t key);
node_t *rbtree_min(const rbtree *t);
node_t *rbtree_max(const rbtree *t);
int rbtree_erase(rbtree *t, node_t *p);
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n);

// 새 트리 생성 함수
rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree)); // tree의 구조체 동적 할당
  node_t *nil_node = (node_t *)calloc(1, sizeof(node_t)); // nil 노드 생성 및 초기화

  nil_node->color = RBTREE_BLACK; // nil 노드의 색 정의
  p->nil = p->root = nil_node; // tree의 nil과 root에 nil 노드로 설정

  return p;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  free(t);
}

// 노드 삽입 함수
node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *new_node = (node_t *)calloc(1, sizeof(node_t)); // 새 노드 생성 및 동적 할당

  new_node->key = key; // 새 노드의 값을 입력 값으로 설정
  new_node->color = RBTREE_RED; // 노드 삽입 규칙에 의해 새 노드의 색을 레드로 설정
  new_node->left = new_node->right = t->nil; // 새 노드의 왼쪽, 오른쪽 자식을 nil 노드로 설정

  // 노드를 삽입할 위치 탐색
  node_t *current = t->root; // 노드를 삽입할 위치를 찾기 위해 현재 위치 노드를 루트 노드로 설정한 포인트 변수 생성
 
  if(current == t->nil) { // 트리가 비어 있는 경우
    t->root = new_node; // 삽입할 노드를 루트 노드로 설정
    new_node->parent = t->nil;
  }
  else {
    while(current != t->nil) { // 트리가 비어있지 않은 경우
      if(key < current->key) { // 삽입할 노드의 값이 현재 노드의 값보다 작으면, 삽입할 노드를 현재 노드의 왼쪽 서브 트리에 삽입하는 경우
        if(current->left == t->nil) { // 현재 위치 노드의 왼쪽 자녀가 nil 노드인 경우
          current->left = new_node; // 현재 위치 노드의 왼쪽 자녀로 노드 삽입
          break; // 노드를 삽입할 위치를 찾았으므로 if문 탈출
        }
        current = current->left; // 현재 위치 노드의 왼쪽 자녀가 nil 노드가 아닌 경우, 현재 위치 노드를 현재 위치 노드의 왼쪽 자녀로 설정
      }
      else { // 삽입할 노드의 값이 현재 노드의 값보다 크면, 삽입할 노드를 현재 노드의 오른쪽 서브 트리에 삽입하는 경우
        if(current->right == t->nil) { // 현재 위치 노드의 오른쪽 자녀가 nil 노드인 경우
          current->right = new_node; // 현재 위치 노드의 오른쪽 자녀로 노드 삽입
          break; // 노드를 삽입할 위치를 찾았으므로 if문 탈출
        }
        current = current->right; // 현재 위치 노드의 오른쪽 자녀가 nil 노드가 아닌 경우, 현재 위치 노드를 현재 위치 노드의 오른쪽 자녀로 설정
      }
    }
    new_node->parent = current; // 삽입한 노드의 부모 설정
  }
  rbtree_insert_fixup(t, new_node); // 노드 삽입 후 불균형 복구 수행

  return new_node;
}

// 노드 삽입 후 불균형 복구 함수
void rbtree_insert_fixup(rbtree *t, node_t *node) {
  node_t *uncle; // 삽입한 노드의 부모의 형제 노드(삼촌) 설정

  while(node->parent->color == RBTREE_RED) { // 삽입한 노드의 부모 노드의 색이 레드인 경우
    node_t *parent = node->parent; // 삽입한 노드의 부모 노드 설정
    node_t *grand_parent = parent->parent; // 삽입한 노드의 부모의 부모 노드(할아버지) 설정
    // 삽입한 노드의 부모 노드가 할아버지 노드의 오른쪽 자녀이면, 삼촌 노드를 할아버지 노드의 왼쪽 자녀로 설정
    // 삽입한 노드의 부모 노드가 할아버지 노드의 왼쪽 자녀이면, 삼촌 노드를 할아버지 노드의 오른쪽 자녀로 설정
    uncle = ((parent == grand_parent->right) ? grand_parent->left : grand_parent->right);
    if(uncle == grand_parent->right) { // 삼촌 노드가 할아버지 노드의 오른쪽 자녀인 경우
      // CASE 1 : 부모 노드와 삼촌 노드의 색이 레드인 경우
      if(uncle->color == RBTREE_RED) {
        parent->color = uncle->color = RBTREE_BLACK; // 부모와 삼촌 노드의 색을 블랙으로 변경
        grand_parent->color = RBTREE_RED; // 할아버지 노드의 색을 레드로 변경
        node = grand_parent; // 할아버지 노드가 루트 노드가 아니고, 할아버지 노드 위에 색이 레드인 노드가 있다면 연속적인 레드가 오기 때문에 속성 4를 위반하므로 노드의 위치를 할아버지 노드로 설정
      }
      else { // 삼촌 노드의 색이 블랙인 경우
        // CASE 2 : 삼촌 노드의 색이 블랙 & 부모 노드가 할아버지 노드의 왼쪽 자녀 & 삽입한 노드가 부모 노드의 오른쪽 자녀인 경우
        if(node == parent->right) {
          left_rotate(t, node); // 왼쪽 회전 수행
        }
        // CASE 3 : 삼촌 노드의 색이 블랙 & 부모 노드가 할아버지 노드의 왼쪽 자녀 & 삽입한 노드가 부모 노드의 왼쪽 자녀인 경우
        else {
          parent->color = RBTREE_BLACK; // 부모 노드의 색을 블랙으로 변경
          grand_parent->color = RBTREE_RED; // 할아버지 노드의 색을 레드로 변경
          right_rotate(t, parent); // 오른쪽 회전 수행
        }
      }
    }
    else { // 삼촌 노드가 할아버지 노드의 왼쪽 자녀인 경우
      // CASE 1 : 부모 노드와 삼촌 노드의 색이 레드인 경우
      if(uncle->color == RBTREE_RED) {
        parent->color = uncle->color = RBTREE_BLACK; // 부모와 삼촌 노드의 색을 블랙으로 변경
        grand_parent->color = RBTREE_RED; // 할아버지 노드의 색을 레드로 변경
        node = grand_parent; // 할아버지 노드가 루트 노드가 아니고, 할아버지 노드 위에 색이 레드인 노드가 있다면 연속적인 레드가 오기 때문에 속성 4를 위반하므로 노드의 위치를 할아버지 노드로 설정
      }
      else { // 삼촌 노드의 색이 블랙인 경우
        // CASE 2 : 삼촌 노드의 색이 블랙 & 부모 노드가 할아버지 노드의 오른쪽 자녀 & 삽입한 노드가 부모 노드의 왼쪽 자녀인 경우
        if(node == parent->left) { // 삽입한 노드가 부모 노드의 왼쪽 자녀인 경우
          right_rotate(t, node); // 오른쪽 회전 수행
        }
        // CASE 3 : 삼촌 노드의 색이 블랙 & 부모 노드가 할아버지 노드의 오른쪽 자녀 & 삽입한 노드가 부모 노드의 오른쪽 자녀인 경우
        else {
          parent->color = RBTREE_BLACK; // 부모 노드의 색을 블랙으로 변경
          grand_parent->color = RBTREE_RED; // 할아버지 노드의 색을 레드로 변경
          left_rotate(t, parent); // 왼쪽 회전 수행
        }
      }
    }
  }
  t->root->color = RBTREE_BLACK; // 루트 노드의 색을 블랙으로 변경
}

// 왼쪽 회전 함수
void left_rotate(rbtree *t, node_t *node) {
  node_t *parent = node->parent;
  node_t *grand_parent = parent->parent;
  node_t *node_left = node->left;

  if(parent == t->root) { // 노드의 부모 노드가 경우
    t->root = node; // 현재 노드를 루트 노드로 설정
  }
  else { //
    if(grand_parent->left == parent) { // 할아버지 노드의 왼쪽 자녀가 부모인 경우
      grand_parent->left = node; // 할아버지 노드의 왼쪽 자녀를 노드로 변경(1)
    }
    else { // 할아버지 노드의 오른쪽 자녀가 부모인 경우
      grand_parent->right = node; // 할아버지 노드의 오른쪽 자녀를 노드로 변경(1)
    }
  }
  node->parent = grand_parent; // 노드의 부모 노드를 할아버지 노드로 변경(양방향 연결 1)
  parent->parent = node; // 부모 노드의 부모를 노드로 변경(2)
  node->left =parent; // 노드의 왼쪽 자녀를 부모 노드로 변경(양방향 연결 2)
  parent->right = node_left; // 부모 노드의 오른쪽 자녀를 노드의 왼쪽 자녀로 변경(3)
  node_left->parent = parent; // 노드의 왼쪽 자녀의 부모 노드를 부모 노드로 변경(양방향 연결 3)
}

// 오른쪽 회전 함수
void right_rotate(rbtree *t, node_t *node) {
  node_t *parent = node->parent;
  node_t *grand_parent = parent->parent;
  node_t *node_right = node->right;

  if(parent == t->root) { // 노드의 부모 노드가 경우
    t->root = node; // 현재 노드를 루트 노드로 설정
  }
  else { //
    if(grand_parent->left == parent) { // 할아버지 노드의 왼쪽 자녀가 부모인 경우
      grand_parent->left = node; // 할아버지 노드의 왼쪽 자녀를 노드로 변경(1)
    }
    else { // 할아버지 노드의 오른쪽 자녀가 부모인 경우
      grand_parent->right = node; // 할아버지 노드의 오른쪽 자녀를 노드로 변경(1)
    }
  }
  node->parent = grand_parent; // 노드의 부모 노드를 할아버지 노드로 변경(양방향 연결 1)
  parent->parent = node; // 부모 노드의 부모를 노드로 변경(2)
  node->right =parent; // 노드의 오른쪽 자녀를 부모 노드로 변경(양방향 연결 2)
  parent->left = node_right; // 부모 노드의 왼쪽 자녀를 노드의 오른쪽 자녀로 변경(3)
  node_right->parent = parent; // 노드의 오른쪽 자녀의 부모 노드를 부모 노드로 변경(양방향 연결 3)
}

// 키 탐색 함수
node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *current = t->root; // 키를 탐색하기 위해 현재 값을 루트 노드로 하는 포인터 변수 생성 및 설정

  while(current != t->nil) { // 현재 노드의 값이 nil노드가 될때까지 반복문 수행
    if(key == current->key) { // 현재 노드의 값이 찾고자하는 키 값과 같은 경우
      return current; // 현재 노드의 값 반환
    }
    else { // 현재 노드의 값이 찾고자하는 키 값과 다른 경우
      // 찾고자하는 키 값이 현재 노드의 값보다 작으면 현재 노드의 왼쪽 서브트리를 탐색하고, 아니면 오른쪽 서브 트리를 탐색
      current = (key < current->key) ? current->left : current->right;
    }
  }
  // 찾고자하는 키 값이 없는 경우
  return NULL; // NULL 값 반환
}

// 최소값 탐색 함수
node_t *rbtree_min(const rbtree *t) {
  node_t *current = t->root; // 최소값을 탐색하기 위해 현재 값을 루트 노드로 하는 포인터 변수 생성 및 설정
  
  while(current->left != t->nil) { // 현재 노드의 왼쪽 자식의 노드 값이 nil노드가 될때까지 반복문 수행
    current = current->left; // 현재 노드의 값을 현재 노드의 왼쪽 자녀 노드의 값으로 갱신
  }
  return current; // 트리의 왼쪽 서브 트리의 마지막 값 반환
}

// 최대값 탐색 함수
node_t *rbtree_max(const rbtree *t) {
  node_t *current = t->root; // 최대값을 탐색하기 위해 현재 값을 루트 노드로 하는 포인터 변수 생성 및 설정
  
  while(current->right != t->nil) { // 현재 노드의 오른쪽 자식의 노드 값이 nil노드가 될때까지 반복문 수행
    current = current->right; // 현재 노드의 값을 현재 노드의 오른쪽 자녀 노드의 값으로 갱신
  }
  return current; // 트리의 오른쪽 서브 트리의 마지막 값 반환
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}