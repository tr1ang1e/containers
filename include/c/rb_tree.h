#ifndef __RBTREE__
#define __RBTREE__

#include <stddef.h>
#include <stdbool.h>

#define RBT_KEY_SIZE (256)

typedef enum IS_BALANCED_E
{
  BALANCED,
  ROT_LEFT,
  ROT_RIGHT,
} IS_BALANCED;

typedef enum COLOR_E
{
  BLACK,
  RED,
} COLOR;

typedef struct RBTNodeS
{
  char key[RBT_KEY_SIZE];
  void* data; // void* data;

  COLOR color;

  struct RBTNodeS* left;
  struct RBTNodeS* right;
  struct RBTNodeS* parent;
} RBTNode;

RBTNode NIL = { "", NULL, BLACK, NULL, NULL, NULL };

bool rbt_init(RBTNode* pRoot);
bool rbt_close(RBTNode* pRoot);
bool rbt_insert(RBTNode* pRoot, void* pItem, size_t itemSize, const char* key);
bool rbt_delete(RBTNode* pRoot, const char* key);
bool rbt_get(RBTNode* pRoot, void* pItem, size_t itemSize, const char* key);

#endif // !__RBTREE__