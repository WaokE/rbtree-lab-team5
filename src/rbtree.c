#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>
key_t inorder_count;

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  p->nil = (node_t *)calloc(1, sizeof(node_t));
  p->nil->color = RBTREE_BLACK;
  // TODO: initialize struct if needed
  p->root = p->nil;
  
  return p;
}

void rotate_left(rbtree *t, node_t *x){
  node_t *y = x->right;
  x->right = y->left;
  if (y->left != t->nil){
    y->left->parent = x;
  }
  y->parent = x->parent;
  if (t->root == x){
    t->root = y;
  }
  // x가 부모의 왼쪽이면
  else if (x == x->parent->left){
    x->parent->left = y;
  }
  // x가 부모의 오른쪽이면
  else{
    x->parent->right = y;
  }
  y->left = x;
  x->parent = y;
}

void rotate_right(rbtree *t, node_t *y){
  node_t *x = y->left;
  y->left = x->right;
  if (x->right != t->nil){
    x->right->parent = y;
  }
  x->parent = y->parent;
  if (t->root == y){
    t->root = x;
  }
  // y가 부모의 왼쪽이면
  else if (y == y->parent->left){
    y->parent->left = x;
  }
  // y가 부모의 오른쪽이면
  else{
    y->parent->right = x;
  }
  x->right = y;
  y->parent = x;
}

int postorder_traversal(rbtree *t, node_t *node){
  if (node->left != t->nil){
    postorder_traversal(t, node->left);
  }
  if (node->right != t->nil){
    postorder_traversal(t, node->right);
  }
  // 노드 free
  free(node);
  return 0;
}

// 후위순회로 트리 탐색하며 각 노드들 free, 마지막으로 트리 free
void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  if (t->root != t->nil){
    postorder_traversal(t, t->root);
  }
  free(t);
}

void insert_fixup(rbtree *t, node_t *node_inserted){
  node_t *grandparent = node_inserted->parent->parent;
  node_t *parent = node_inserted->parent;
  // 삽입 노드 부모 색이 BLACK -> 문제 X
  if (parent->color == RBTREE_BLACK){
    return;
  }
  // 삽입 노드 부모 색이 RED -> 문제 발생
  else{
    // 삼촌 노드 찾기
    node_t *node_uncle;
    if (grandparent->left == parent){
      node_uncle = grandparent->right;
    }
    else{
      node_uncle = grandparent->left;
    }
    // Case 1
    if (node_uncle->color == RBTREE_RED){
      parent->color = RBTREE_BLACK;
      node_uncle->color = RBTREE_BLACK;
      grandparent->color = RBTREE_RED;
      if (t->root == grandparent){
        t->root->color = RBTREE_BLACK;
      }
      insert_fixup(t, grandparent);
    }
    // Case 2, 3
    else {
      // Case 2-1 (오른쪽으로 꺾임)
      if (parent->left == node_inserted && grandparent->right == parent){
        // TODO: 부모 기준으로 오른쪽으로 회전 - Case 3로 만들어줌
        rotate_right(t, parent);
        parent = node_inserted;
      }
      // Case 2-2 (왼쪽으로 꺾임)
      else if (parent->right == node_inserted && grandparent->left == parent){
        // TODO: 부모 기준으로 왼쪽으로 회전 - Case 3 로 만들어줌
        rotate_left(t, parent);
        parent = node_inserted;
      }
      // Case 3
      color_t temp_color = parent->color;
      parent->color = grandparent->color;
      grandparent->color = temp_color;
      if (grandparent->left == parent){
        rotate_right(t, grandparent);
      }
      else{
        rotate_left(t, grandparent);
      }
    }
  }
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  // 삽입할 노드 선언
  node_t *node_to_insert = (node_t *)calloc(1, sizeof(node_t));
  node_to_insert->color = RBTREE_RED;
  node_to_insert->key = key;
  
  // 노드를 삽입할 부모 탐색
  node_t *current = t->root;
  node_t *parent_location = t->nil;
  while (current != t->nil){
    parent_location = current;
    if (current->key == key){
      current = current->right;
    }
    else if (current->key > key){
      current = current->left; 
    }
    else if (current->key < key){
      current = current->right; 
    }
  }

  
  // 부모를 찾지 못했다면(루트노드 삽입시)
  if (parent_location == t->nil){
    t->root = node_to_insert;
    node_to_insert->left = t->nil;
    node_to_insert->right = t->nil;
    node_to_insert->parent = parent_location;
    // 2번 속성 만족을 위해 루트 BLACK으로
    node_to_insert->color = RBTREE_BLACK;
  }

  // 찾았다면
  else{
    if (parent_location->key == node_to_insert->key){
      parent_location->right = node_to_insert;
    }
    else if (parent_location->key > node_to_insert->key){
      parent_location->left = node_to_insert;
    }
    else if (parent_location->key < node_to_insert->key){
      parent_location->right = node_to_insert;
    }
      node_to_insert->parent = parent_location;
      node_to_insert->right = t->nil;
      node_to_insert->left = t->nil;
      insert_fixup(t, node_to_insert);
  }
  return node_to_insert;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t *current = t->root;
  while (current != t->nil){
    if (current->key == key){
      return current;
    }
    else if (current->key < key){
      current = current->right;
    }
    else if (current->key > key){
      current = current->left;
    }
  }
  return NULL;
}

node_t *erase_min(node_t *n, node_t *nil){
  node_t *min = n;
  while (n != nil)               
  {
    min = n;
    n = n->left; 
  }
  return min;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  node_t *min = erase_min(t->root, t->nil);
  return min;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement fin
  node_t *current = t->root;
  node_t *max = t->root;
  while (current != t->nil){
    max = current;
    current = current->right;
  }
  return max;
}

void rb_transplant(rbtree *T,node_t *u, node_t *v){
  if (u->parent == T->nil){
    T->root = v;
  }
  else if (u == u->parent->left){
    u->parent->left = v;
  }
  else{
    u->parent->right = v;
  }
  v->parent = u->parent;
}

void delete_fixup(rbtree *t, node_t *x){
  while (x != t->root && x->color == RBTREE_BLACK){
    if (x == x->parent->left){
      node_t *w = x->parent->right;
      // Case 1
      if (w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        rotate_left(t, x->parent);
        w = x->parent->right;
      }
      // Case 2
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }
      // Case 3
      else{ 
        if (w->right->color == RBTREE_BLACK) {
        w->left->color = RBTREE_BLACK;
        w->color = RBTREE_RED;
        rotate_right(t, w);
        w = x->parent->right;
      }
      // Case 4
      w->color = x->parent->color;
      x->parent->color = RBTREE_BLACK;
      w->right->color = RBTREE_BLACK;
      rotate_left(t, x->parent);
      x = t->root;
    }
    }
    else{
      node_t *w = x->parent->left;
      // Case 1
      if (w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        rotate_right(t, x->parent);
        w = x->parent->left;
      }
      // Case 2
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }
      // Case 3
      else{ 
      if (w->left->color == RBTREE_BLACK) {
        w->right->color = RBTREE_BLACK;
        w->color = RBTREE_RED;
        rotate_left(t, w);
        w = x->parent->left;
      }
      // Case 4
      w->color = x->parent->color;
      x->parent->color = RBTREE_BLACK;
      w->left->color = RBTREE_BLACK;
      rotate_right(t, x->parent);
      x = t->root;
    }
  }
  }
  x->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  node_t *x;
  node_t *y = p;
  color_t original_color = y->color;
  if (p->left == t->nil){
    x = p->right;
    rb_transplant(t, p, p->right);
  }
  else if (p->right == t->nil){
    x = p->left;
    rb_transplant(t, p, p->left);
  }
  else{
    y = erase_min(p->right, t->nil);
    original_color = y->color;
    x = y->right;
    if (y->parent == p){
      x->parent = y;
    }
    else{
      rb_transplant(t, y, y->right);
      y->right = p->right;
      y->right->parent = y;
    }
    rb_transplant(t, p, y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }

  if (original_color == RBTREE_BLACK){
    delete_fixup(t, x);
  }
  return 0;
}

void inorder_traversal(const rbtree *t, node_t *node, key_t *arr, key_t n){
  if (inorder_count >= n){
    return;
  }
  if (node->left){
    inorder_traversal(t, node->left, arr, n);
  }
  if (node != t->nil){
    arr[inorder_count] = node->key;
    inorder_count++;
    }
  if (node->right){
    inorder_traversal(t, node->right, arr, n);
  }
} 

key_t rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  inorder_count = 0;
  inorder_traversal(t, t->root, arr, n);
  return 0;
}