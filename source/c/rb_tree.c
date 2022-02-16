#include <stdlib.h>
#include <string.h>

#include "../../include/c/rb_tree.h"

/************************************************************************
 *                               PRIVATE                                *
 ************************************************************************/

static void rbt_actualize_root (RBTNode** pRoot)
{
  while ((*pRoot)->parent != NULL)
  {
    *pRoot = (*pRoot)->parent;
  }

  (*pRoot)->color = BLACK;
}

static bool is_key_valid (const char* key)
{
  bool result = true;

  size_t keyLen = strnlen (key, RBT_KEY_SIZE);

  if (keyLen > RBT_KEY_SIZE - 1)    // for '\0'
  {
    result = false;
  }

  return result;
}

static FoundInfo rbt_find_node (RBTNode* pRoot, const char* key)
{
  /*           variant                 description
   *
   * >>  { NULL, NULL, NONE }    >>   key is not valid
   * >>  { NULL, ROOT, NONE }    >>   found : node is root
   * >>  { ....  ....  .... }    >>   found : node is not root
   * >>  { ...., &NIL, .... }    >>   not found
   *
   */

  FoundInfo info = { NULL, NULL, NONE };

  do
  {
    if (!is_key_valid (key))
    {
      break;
    }

    info.pNode = pRoot;

    while (info.pNode != &NIL)
    {
      int result = strcmp (info.pNode->key, key);

      if (result > 0)    // go left
      {
        info.pParent = info.pNode;
        info.pNode = info.pNode->left;
        info.subtree = LEFT;
      }
      else if (result < 0)    // go right
      {
        info.pParent = info.pNode;
        info.pNode = info.pNode->right;
        info.subtree = RIGHT;
      }
      else    // found , info is n actual state
      {
        break;
      }
    }

  } while (0);

  return info;
}

// old rbt_find implementation
//
// static RBTNode*
// rbt_find_parent (RBTNode* pRoot, const char* key)
// {
//   /* peturn pointer to node which is
//    * potentially a parent of necessary node
//    *
//    * variant 1 = necessary node was found
//    * Parent.left or Parent.right points to node / call rbt_find_child()
//    * result in:
//    *    - success for rbt_find()
//    *    - error for rbt_insert()
//    *
//    * variant 2 = necessary node was not found
//    * at least one of Parent.left and Parent.right points to NIL / call rbt_find_child()
//    * result in:
//    *    - error for rbt_find()
//    *    - success for rbt_insert()
//    */
//
//   RBTNode* pTemp;
//   RBTNode* pParent;
//
//   do
//   {
//     if (pRoot == NULL)
//     {
//       break;
//     }
//
//     if (!is_key_valid (key))
//     {
//       break;
//     }
//
//     pTemp = pRoot;
//     pParent = NULL;
//
//     while (pTemp != &NIL)
//     {
//       int result = strcmp (pTemp->key, key);
//
//       if (result > 0)    // pTemp->key > key , go left
//       {
//         pParent = pTemp;
//         pTemp = pTemp->left;
//       }
//       else if (result < 0)    // pTemp->key < key , go right
//       {
//         pParent = pTemp;
//         pTemp = pTemp->right;
//       }
//       else    // found
//       {
//         break;
//       }
//     }
//
//   } while (0);
//
//   return pParent;
// }
//
// static RBTNode**
// rbt_find_child (RBTNode* pParent, const char* key)
// {
//   /* universal function, return:
//    *    not pointer to child (because impossible to define is it right or left child)
//    *    but pointer to (pointer to child) in parent (resultPtr)
//    *
//    *          (l) Parent (r)
//    *          /            \
//    *         A              B
//    *
//    * variant 1 = B wasn't found (NIL)
//    *             return &(Parent.right)
//    *             possible ckeck: if (*resultPtr == NIL) <insert>
//    *
//    * variant 2 = B was found
//    *             return &(Parent.right)
//    *             possible check: if (*resultPtr != NIL) <memcpy>
//    *
//    * resultPtr values possible meaning:
//    *             >  (resultPtr == NULL) , found node is root
//    *             >  (resultPtr != NULL) , variants:  1. (*resultPtr == &NIL) ,  node
//    was not found
//    *                                                 2. (*resultPtr != &NIL) ,  node
//    was found
//    */
//
//   RBTNode** resultPtr = NULL;
//
//   if (pParent != NULL)
//   {
//     int result = strcmp (pParent->key, key);
//
//     if (result > 0)
//     {
//       resultPtr = &(pParent->left);
//     }
//     else if (result < 0)
//     {
//       resultPtr = &(pParent->right);
//     }
//   }
//
//   return resultPtr;
// }

static RBTNode* rbt_create_node (const void* pItem, size_t itemSize, const char* key)
{
  RBTNode* pNode = NULL;

  do
  {
    if (pItem == NULL)
    {
      break;
    }

    // no need to check if key is valid
    // it was already checked in rbt_find()
    // called from rbt_insert() before rbt_create_node()

    // allocate memory for node
    pNode = (RBTNode*)malloc (sizeof (RBTNode));
    if (pNode == NULL)
    {
      break;
    }

    // allocate memory for data
    void* pData = malloc (itemSize);
    if (pData == NULL)
    {
      free (pNode);
      pNode = NULL;
      break;
    }

    // key and data
    strcpy (pNode->key, key);
    memcpy (pData, pItem, itemSize);

    // default settings
    pNode->color = RED;
    pNode->left = &NIL;
    pNode->right = &NIL;
    pNode->parent = NULL;

  } while (0);

  return pNode;
}

static void rbt_free_memory (RBTNode* pNode)
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

static void rbt_rot_left (RBTNode* pNode)
{
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

static void rbt_rot_right (RBTNode* pNode)
{

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

static void rbt_insert_balance (RBTNode* pNode)
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
        pNode->parent->color = BLACK;                   // B
        pNode->parent->parent->color = RED;             // A
        pNode->parent->parent->right->color = BLACK;    // C

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
        pNode->parent->color = BLACK;                  // C
        pNode->parent->parent->color = RED;            // A
        pNode->parent->parent->left->color = BLACK;    // B

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
        pNode->parent->color = RED;
        rbt_rot_left (pNode->parent);
      }
    }
  }
}

static void rbt_delete_black_node (FoundInfo info)    // right case
{
  /* delete black node without children
   * balancing by using mirror subtree red nodes
   * information = habr.com/ru/company/otus/blog/521034/
   *
   *           Node (info.pParent)
   *         /      \
   *    [subtree]    x (deleted black node)
   *
   * starting from this moment:
   *    > all off actoins will be described relative to the 'pNode = info.pParent'
   *    > 'subtree' will describe mirror subtree side
   *
   * scheme for both 'subtree' value variants
   *
   *             Node             Node
   *           /      \         /      \
   *          A        x       x        A
   *       /     \                   /     \
   *      B       C                 C       B
   *             /  \             /  \
   *            D    E           E    D
   *
   *
   * attention! cases order matters, they are placed
   *     > in decreasing order of red nodes number
   *     > in increasing order of balancing complexity
   */

  RBTNode* pNode = info.pParent;
  SUBTREE subtree = (info.subtree == LEFT) ? RIGHT : LEFT;

  // prepare all nodes pointers depending on the 'subtree' value
  RBTNode* A = (subtree == LEFT) ? pNode->left : pNode->right;
  RBTNode* B = (subtree == LEFT) ? A->left : A->right;
  RBTNode* C = (subtree == LEFT) ? A->right : A->left;
  RBTNode* D;
  RBTNode* E;

  if (C != &NIL)    // D, E exist only if B is not a leaf
  {
    D = (subtree == LEFT) ? C->left : C->right;
    E = (subtree == LEFT) ? C->right : C->left;
  }

  do
  {
    // case 1 ('subtree' independent)
    if (pNode->color == RED && A->color == BLACK && B->color == BLACK && C->color == BLACK)
    {
      pNode->color = BLACK;
      A->color = RED;

      break;
    }

    // case 2
    if (pNode->color == BLACK && A->color == RED && B->color == BLACK)
    {
      pNode->color = RED;
      A->color = BLACK;
      B->color = RED;

      (subtree == LEFT) ? rbt_rot_right (pNode) : rbt_rot_left (pNode);

      break;
    }

    // case 3
    if (pNode->color == BLACK && A->color == RED && D->color == BLACK && E->color == BLACK)
    {
      A->color = BLACK;
      C->color = RED;

      (subtree == LEFT) ? rbt_rot_right (pNode) : rbt_rot_left (pNode);

      break;
    }

    // case 4
    if (pNode->color == BLACK && A->color == RED && D->color == RED)
    {
      D->color = BLACK;

      (subtree == LEFT) ? rbt_rot_left (A) : rbt_rot_right (A);
      (subtree == LEFT) ? rbt_rot_right (pNode) : rbt_rot_left (pNode);

      break;
    }

    // case 5
    if (pNode->color == BLACK && A->color == BLACK && C->color == RED)
    {
      C->color = BLACK;

      (subtree == LEFT) ? rbt_rot_left (A) : rbt_rot_right (A);
      (subtree == LEFT) ? rbt_rot_right (pNode) : rbt_rot_left (pNode);

      break;
    }

    // case 6 (correct current context to the detriment of entire tree balance)
    if (pNode->color == BLACK && A->color == BLACK && B->color == BLACK && C->color == BLACK)
    {
      A->color = RED;

      // recursive call (only if pNode is not root)
      if (pNode->parent != NULL)
      {
        info.pParent = pNode->parent;
        info.subtree = (pNode->parent->left == pNode) ? LEFT : RIGHT;
        rbt_delete_black_node (info);
      }

      break;
    }

  } while (0);
}

static void rbt_delete_balance (FoundInfo info)
{
  /* description
   *
   * 1. already know that node for delete is
   *    - or root with two children
   *    - or any of not root node
   *
   * 2. if node has two children task would be reduced
   *    to delete node with zero or one child and then
   *    rbt_delete_balance() would be recursevily called
   *
   */

  // preparing
  RBTNode* pCurr = NULL;
  RBTNode* pNode = info.pNode;
  RBTNode* pParent = info.pParent;
  SUBTREE subtree = info.subtree;

  // case 1: node has two children, node color doesn't matter
  if (pNode->left != &NIL && pNode->right != &NIL)
  {
    // find nearest node from right
    pCurr = pNode->right;

    while (pCurr->left != &NIL)
    {
      pCurr = pCurr->left;
    }

    // make exchange: key
    strncpy (pNode->key, pCurr->key, RBT_KEY_SIZE);

    // make exchange: data
    void* pData = pNode->data;
    pNode->data = pCurr->data;
    pCurr->data = pData;

    // call rbt_delete_balance() for pCurr node
    info.pNode = pCurr;
    info.pParent = pCurr->parent;
    info.subtree = (pCurr->parent == pNode) ? RIGHT : LEFT;
    rbt_delete_balance (info);
  }

  // case 2: node hasn't children, node is red
  else if (pNode->color == RED && pNode->left == &NIL && pNode->right == &NIL)
  {
    if (subtree = LEFT)
    {
      pParent->left = &NIL;
    }
    else
    {
      pParent->right = &NIL;
    }
  }

  // case 3: node has one child, node is black (child might be only red)
  else if (pNode->color == BLACK && (pNode->left != &NIL || pNode->right != &NIL))
  {
    pCurr = pNode;    // child would be deleted so we keep pNode name for free()

    pNode = (pNode->left != &NIL) ? pNode->left : pNode->right;

    // make exchange: key
    strncpy (pCurr->key, pNode->key, RBT_KEY_SIZE);

    // make exchange: data
    void* pData = pCurr->data;
    pCurr->data = pNode->data;
    pNode->data = pData;

    // remove pNode from tree
    pCurr->left = &NIL;
    pCurr->right = &NIL;
  }

  // case 4: node hasn't children, node is black
  if (pNode->color == BLACK)
  {
    if (subtree == LEFT)
    {
      pParent->left = &NIL;
    }
    else
    {
      pParent->right = &NIL;
    }

    info.pNode = &NIL;
    rbt_delete_black_node (info);    // recursive function
  }

  free (pNode->data);
  free (pNode);
}

/************************************************************************
 *                               PUBLIC                                 *
 ************************************************************************/

bool rbt_init (RBTNode* pRoot)
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

bool rbt_close (RBTNode* pRoot)
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

bool rbt_insert (RBTNode** pRoot, void* pItem, size_t itemSize, const char* key)
{
  /* insertion place:
   *   always to the NIL
   *   exception = red-black tree is empty
   */

  bool result = false;

  do
  {
    if (pItem == NULL)
    {
      break;
    }

    // search place to insert to
    FoundInfo info = rbt_find_node (*pRoot, key);

    if (info.pNode == NULL)    // key is not valid
    {
      break;
    }

    if (info.pNode != &NIL)    // node with such key already exists
    {
      break;    // different logic might be implemented
    }

    RBTNode* pNode = rbt_create_node (pItem, itemSize, key);

    if (pNode == NULL)    // error while creating node to insert
    {
      break;
    }

    if (*pRoot == NULL)    // insertion to root
    {
      pNode->color = BLACK;
      *pRoot = pNode;
    }
    else
    {
      // insertion
      pNode->parent = info.pParent;
      if (info.subtree == LEFT)
      {
        info.pParent->left = pNode;
      }
      else
      {
        info.pParent->right = pNode;
      }

      // balansing
      rbt_insert_balance (pNode);
      rbt_actualize_root (pRoot);
    }

    result = true;

  } while (0);

  return result;
}

bool rbt_delete (RBTNode** pRoot, const char* key)
{
  bool result = false;

  do
  {
    if (*pRoot == NULL)
    {
      break;
    }

    FoundInfo info = rbt_find_node (*pRoot, key);

    if (info.pNode == NULL)    // key is not valid
    {
      break;
    }

    if (info.pNode == &NIL)    // node was not found
    {
      break;
    }

    // node is root and has zero or one child
    // specific case because root hasn't parent
    // if node is root and has two children parent doesn't matter
    if (info.pNode == *pRoot)
    {
      if (info.pNode->left == &NIL && info.pNode->right == &NIL)    // zero children
      {
        *pRoot = NULL;
      }
      else if (info.pNode->left != &NIL && info.pNode->right != &NIL)
      {
        rbt_delete_balance (info);
      }
      else
      {
        if (info.pNode->left != &NIL)
        {
          *pRoot = info.pNode->left;
        }
        else
        {
          *pRoot = info.pNode->right;
        }

        (*pRoot)->color = BLACK;
        (*pRoot)->parent = NULL;
      }

      free (info.pNode->data);
      free (info.pNode);
    }
    else    // node is not root
    {
      rbt_delete_balance (info);
    }

    rbt_actualize_root (pRoot);

    result = true;

  } while (0);

  return result;
}

bool rbt_get (RBTNode* pRoot, void* pItem, size_t itemSize, const char* key)
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

    FoundInfo info = rbt_find_node (pRoot, key);

    if (info.pNode == NULL)    // key is invalid
    {
      break;
    }

    if (info.pNode == &NIL)    // node was not found
    {
      break;
    }

    memcpy (pItem, info.pNode->data, itemSize);

    result = true;

  } while (0);

  return result;
}