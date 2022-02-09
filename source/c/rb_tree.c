#include <stdlib.h>
#include <string.h>

#include "../../include/c/rb_tree.h"

/************************************************************************
 *                               PRIVATE                                *
 ************************************************************************/

/* done */ static void
rbt_actualize_root (RBTNode** pRoot)
{
  while ((*pRoot)->parent != NULL)
  {
    *pRoot = &((*pRoot)->parent);
  }

  (*pRoot)->color = BLACK;
}

/* done */ static bool
is_key_valid (const char* key)
{
  bool result = true;

  size_t keyLen = strnlen (key, RBT_KEY_SIZE);

  if (keyLen > RBT_KEY_SIZE - 1)    // for '\0'
  {
    result = false;
  }

  return result;
}

/* done */ static RBTNode*
rbt_find_parent (RBTNode* pRoot, const char* key)
{
  /* peturn pointer to node which is
   * potentially a parent of necessary node
   *
   * variant 1 = necessary node was found
   * Parent.left or Parent.right points to node / call rbt_find_child()
   * result in:
   *    - success for rbt_find()
   *    - error for rbt_insert()
   *
   * variant 2 = necessary node was not found
   * at least one of Parent.left and Parent.right points to NIL / call rbt_find_child()
   * result in:
   *    - error for rbt_find()
   *    - success for rbt_insert()
   */

  RBTNode* pTemp;
  RBTNode* pParent;

  do
  {
    if (pRoot == NULL)
    {
      break;
    }

    if (!is_key_valid (key))
    {
      break;
    }

    pTemp = pRoot;
    pParent = NULL;

    while (pTemp != &NIL)
    {
      int result = strcmp (pTemp->key, key);

      if (result > 0)    // pTemp->key > key , go left
      {
        pParent = pTemp;
        pTemp = pTemp->left;
      }
      else if (result < 0)    // pTemp->key < key , go right
      {
        pParent = pTemp;
        pTemp = pTemp->right;
      }
      else    // found
      {
        break;
      }
    }

  } while (0);

  return pParent;
}

/* done */ static RBTNode**
rbt_find_child (RBTNode* pParent, const char* key)
{
  /* universal function, return:
   *    not pointer to child (because impossible to define is it right or left child)
   *    but pointer to (pointer to child) in parent (resultPtr)
   *
   *          (l) Parent (r)
   *          /            \
   *         A              B
   *
   * variant 1 = B wasn't found (NIL)
   *             return &(Parent.right)
   *             possible ckeck: if (*resultPtr == NIL) <insert>
   *
   * variant 2 = B was found
   *             return &(Parent.right)
   *             possible check: if (*resultPtr != NIL) <memcpy>
   */

  RBTNode** resultPtr = NULL;

  int result = strcmp (pParent->key, key);

  if (result > 0)
  {
    resultPtr = &(pParent->left);
  }
  else if (result < 0)
  {
    resultPtr = &(pParent->right);
  }

  return resultPtr;
}

/* done */ static RBTNode*
rbt_create_node (const void* pItem, size_t itemSize, const char* key)
{
  RBTNode* pNode = NULL;

  do
  {
    if (pItem == NULL)
    {
      break;
    }

    if (!is_key_valid (key))
    {
      break;
    }

    // allocate memory for node
    // after key cheking to avoid excess malloc
    pNode = (RBTNode*)malloc (sizeof (RBTNode));
    if (pNode == NULL)
    {
      break;
    }

    strcpy (pNode->key, key);

    // allocate memory for data
    void* pData = malloc (itemSize);
    if (pData == NULL)
    {
      free (pNode);
      pNode == NULL;
      break;
    }

    memcpy (pData, pItem, itemSize);

    // default settings
    pNode->color = RED;
    pNode->left = &NIL;
    pNode->right = &NIL;
    pNode->parent = NULL;

  } while (0);

  return pNode;
}

/* done */ static void
rbt_free_memory (RBTNode* pNode)
{
  // depth-first = left sub-tree
  if (pNode->left != &NIL)
  {
    rbt_free_memory (pNode->left);
  }

  // depth-first = right sub-tree
  if (pNode->right != &NIL)
  {
    rbt_free_memory (pNode->right);
  }

  // wheh both left and right children are NIL
  free (pNode->data);
  free (pNode);
}

/* state before left rotation
 *
 *      Parent (don't know if Node == Parent.left or Parent.right)
 *        |
 *       Node
 *      /   \
 *    A      Temp
 *          /   \
 *         B     C
 */
/* done */ static void
rbt_rot_left (RBTNode* pNode)
{
  RBTNode* pTemp = pNode->right;

  // Node <-> B
  pNode->right = pTemp->left;
  pNode->right->parent = pNode;

  pTemp->parent = pNode->parent;

  // Temp <-> Node
  pTemp->left = pNode;
  pNode->parent = pTemp;

  // actualize Parent state
  if (pNode->parent != NULL)
  {
    if (pNode->parent->left == pNode)
    {
      pNode->parent->left = pTemp;
    }
    else
    {
      pNode->parent->right = pTemp;
    }
  }
}

/* state before right rotation
 *
 *          Parent (don't know if Node == Parent.left or Parent.right)
 *             |
 *            Node
 *           /   \
 *       Temp     A
 *      /    \
 *     C      B
 */
/* done */ static void
rbt_rot_right (RBTNode* pNode)
{
  RBTNode* pTemp = pNode->left;

  // Node <-> B
  pNode->left = pTemp->right;
  pNode->left->parent = pNode;

  pTemp->parent = pNode->parent;

  // Temp <-> Node
  pTemp->right = pNode;
  pNode->parent = pTemp;

  // actualize Parent state
  if (pNode->parent != NULL)
  {
    if (pNode->parent->left == pNode)
    {
      pNode->parent->left = pTemp;
    }
    else
    {
      pNode->parent->right = pTemp;
    }
  }
}

/* done */ static void
rbt_insert_balance (RBTNode* pNode)
{
  /* possible state after insertion
   *
   *  v1       A           v2        A
   *         /   \                 /   \
   *        B     C               B     C
   *         \                   /
   *          X                 X
   *
   *
   *  v3       A           v4        A
   *         /   \                 /   \
   *        B     C               B     C
   *             /                       \
   *            X                         X
   */

  while (pNode->parent != NULL && pNode->parent->color == RED)
  {
    // v1, v2: B on the LEFT of A
    if (pNode->parent == pNode->parent->parent->left)
    {
      // C is RED
      if (pNode->parent->parent->right->color == RED)
      {
        pNode->parent->color = BLACK;                    // B
        pNode->parent->parent->color == RED;             // A
        pNode->parent->parent->right->color == BLACK;    // C

        pNode = pNode->parent->parent;    // A is next we deal with
      }
      // C is BLACK = impossible to be immidiately after insertion
      else
      {
        // v1: X on the RIGHT of B
        if (pNode == pNode->parent->right)
        {
          rbt_rot_left (pNode->parent);
        }

        pNode->color = BLACK;
        pNode->parent->color = RED;
        rbt_rot_right (pNode->parent);
      }
    }
    // v3, v4: B on the RIGHT of A
    else
    {
      // B is RED
      if (pNode->parent->parent->left->color == RED)
      {
        pNode->parent->color = BLACK;                   // C
        pNode->parent->parent->color == RED;            // A
        pNode->parent->parent->left->color == BLACK;    // B

        pNode = pNode->parent->parent;    // A is next we deal with
      }
      // B is BLACK = impossible to be immidiately after insertion
      else
      {
        // v3: X on the LEFT of C
        if (pNode == pNode->parent->left)
        {
          rbt_rot_right (pNode->parent);
        }

        pNode->color = BLACK;
        pNode->parent = RED;
        rbt_rot_left (pNode->parent);
      }
    }
  }
}

static void
rbt_delete_balance ()
{
  // check from inserted to root nodes rbt_is_balanced
  // if not balansed, rotate (left or right)
}

/************************************************************************
 *                               PUBLIC                                 *
 ************************************************************************/

/* done */ bool
rbt_init (RBTNode* pRoot)
{
  bool result = false;

  do
  {
    if (pRoot == NULL)
    {
      break;
    }

    memset (pRoot->key, '\0', RBT_KEY_SIZE);
    pRoot->data = NULL;
    pRoot->color = BLACK;
    pRoot->left = NULL;
    pRoot->right = NULL;
    pRoot->parent = NULL;

    result = true;

  } while (0);

  return result;
}

/* done */ bool
rbt_close (RBTNode* pRoot)
{
  bool result = false;

  do
  {
    // destoy mutex

    // destroy conditional

    // free memory
    rbt_free_memory (pRoot);

    result = true;

  } while (0);

  return false;
}

/* done */ bool
rbt_insert (RBTNode** pRoot, void* pItem, size_t itemSize, const char* key)
{
  bool result = false;

  do
  {
    RBTNode* pTemp = rbt_create_node (pItem, itemSize, key);

    if (pTemp == NULL)
    {
      break;
    }

    if (*pRoot == NULL)
    {
      pTemp->color = BLACK;
      *pRoot = pTemp;
    }
    else
    {
      RBTNode* pParent = rbt_find_parent (*pRoot, key);
      RBTNode** resultPtr = rbt_find_child (pParent, key);

      if (*resultPtr != &NIL)    // node with such key already exists
      {
        break;
      }

      // insertion
      pTemp->parent = pParent;
      *resultPtr = pTemp;

      // balansing
      rbt_insert_balance (pTemp);
      rbt_actualize_root (pRoot);
    }

    result = true;

  } while (0);

  return result;
}

bool
rbt_delete (RBTNode* pRoot, const char* key)
{
  return false;
}

/* done */ bool
rbt_get (RBTNode* pRoot, void* pItem, size_t itemSize, const char* key)
{
  int result = false;

  do
  {

    if (pRoot == NULL)
    {
      break;
    }

    if (pItem == NULL)
    {
      break;
    }

    RBTNode* pParent = rbt_find_parent (pRoot, key);
    RBTNode** resultPtr = rbt_find_child (pParent, key);

    if (*resultPtr == &NIL)    // node was not found
    {
      break;
    }

    memcpy (pItem, (*resultPtr)->data, itemSize);

    result = true;

  } while (0);

  return result;

  return false;
}