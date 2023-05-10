#ifndef _RBTREE_H_
#define _RBTREE_H_

#include <stddef.h>

// tree 구현을 위한 구조체와 열거형 정의
typedef enum { RBTREE_RED, RBTREE_BLACK } color_t;

// 각 노드에 저장되는 값의 데이터 형식 정의
typedef int key_t;

// tree의 각 노드를 표현하는 구조체 정의
typedef struct node_t {
  color_t color;
  key_t key;
  struct node_t *parent, *left, *right;
} node_t;

// tree 자체를 나타내는 구조체 정의
typedef struct {
  node_t *root;
  node_t *nil;  // for sentinel
} rbtree;

rbtree *new_rbtree(void);

void delete_rbtree(rbtree *t);
void  delete_node(rbtree *t, node_t *node);

void left_rotate(rbtree *t, node_t *x);
void right_rotate(rbtree *t, node_t *x);

node_t *rbtree_insert(rbtree *t, const key_t key);
void rbtree_insert_fixup(rbtree *t, node_t *node);

void rbtree_transplant(rbtree *t, node_t *u, node_t *v);

int rbtree_erase(rbtree *t, node_t *p);
void rbtree_erase_fixup(rbtree *t, node_t *x);

node_t *rbtree_find(const rbtree *t, const key_t key);
node_t *rbtree_min(const rbtree *t);
node_t *rbtree_max(const rbtree *t);

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n);
void rbtree_inorder(const rbtree *t, node_t *current, key_t *arr, int *arr_idx);

#endif  // _RBTREE_H_
