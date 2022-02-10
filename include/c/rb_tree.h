#ifndef __RBTREE__
#define __RBTREE__

#include <stdbool.h>
#include <stddef.h>

#define RBT_KEY_SIZE (256)

typedef enum COLOR_E
{
  BLACK,
  RED,
} COLOR;

typedef enum SUBTREE_E
{
  NONE,
  LEFT,
  RIGHT,
} SUBTREE;

typedef struct RBTNodeS
{
  char key[RBT_KEY_SIZE];
  void* data;    // void* data;

  COLOR color;

  struct RBTNodeS* left;
  struct RBTNodeS* right;
  struct RBTNodeS* parent;
} RBTNode;

typedef struct FoundInfoS
{
  RBTNode* pParent;
  RBTNode* pNode;
  SUBTREE subtree;
} FoundInfo;

RBTNode NIL = { "", NULL, BLACK, NULL, NULL, NULL };

#ifdef __cplusplus
extern "C"
{
#endif

  bool rbt_init (RBTNode* pRoot);
  bool rbt_close (RBTNode* pRoot);
  bool rbt_insert (RBTNode** pRoot, void* pItem, size_t itemSize, const char* key);
  bool rbt_delete (RBTNode** pRoot, const char* key);
  bool rbt_get (RBTNode* pRoot, void* pItem, size_t itemSize, const char* key);

#ifdef __cplusplus
}
#endif

#endif    // !__RBTREE__