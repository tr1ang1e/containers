#ifndef __RBTREE__
#define __RBTREE__

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

  #define RBT_KEY_SIZE (256)

  typedef enum COLOR_E
  {
    BLACK,
    RED,
  } COLOR;

  typedef struct RBTNodeS
  {
    char key[RBT_KEY_SIZE];
    void* data;

    COLOR color;

    struct RBTNodeS* left;
    struct RBTNodeS* right;
    struct RBTNodeS* parent;
  } RBTNode;

  EXPORT bool is_NIL_same (RBTNode* pNIL);
  EXPORT bool rbt_destroy (RBTNode** pRoot);
  EXPORT bool rbt_insert (RBTNode** pRoot, void* pItem, size_t itemSize, const char* key);
  EXPORT bool rbt_get (RBTNode* pRoot, void* pItem, size_t itemSize, const char* key);
  
  // EXPORT bool rbt_init(RBTNode* pRoot);                          // no need without thread-safety approach
  // EXPORT bool rbt_delete (RBTNode** pRoot, const char* key);     // not debugged yet

#ifdef __cplusplus
}
#endif

#endif    // __RBTREE__