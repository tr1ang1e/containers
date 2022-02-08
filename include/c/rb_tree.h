#ifndef __RBTREE__
#define __RBTREE__

#include <stddef.h>
#include <stdbool.h>

#define RBT_KEY_SIZE (256)

enum COLOR
{
  BLACK,
  RED,
} ;

typedef struct RBTNodeS
{
  const char key[RBT_KEY_SIZE] ;
  int data; // void* data;

  enum COLOR color;

  struct RBTNodeS* left;
  struct RBTNodeS* right;
  struct RBTNodeS* parent;
} RBTNode;

const struct RBTNodeS NIL = { "", 0, BLACK, NULL, NULL, NULL };

bool rbt_init(RBTNode* pRoot);
bool rbt_close(RBTNode* pRoot);
bool rbt_insert(RBTNode* pRoot, int* pData, const char* key);
bool rbt_delete(RBTNode* pRoot, const char* key);
bool rbt_get(RBTNode* pRoot, int* pData, const char* key);

#endif // !__RBTREE__