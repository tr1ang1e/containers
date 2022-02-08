#include <stdlib.h>
#include <string.h>

#include "../../include/c/rb_tree.h"

/************************************************************************
 *                               PRIVATE                                *
 ************************************************************************/

/* done */ static bool is_key_valid(const char* key)
{
  bool result = true;

  size_t keyLen = strnlen(key, RBT_KEY_SIZE);
  
  if ( keyLen > RBT_KEY_SIZE - 1 )    // for '\0'
  {
    result = false;
  }

  return result;
}

/* done */ static RBTNode* rbt_find(RBTNode* pRoot, const char* key)
{
  RBTNode* pTemp = NULL;

  do
  {
    if ( pRoot == NULL )
    {
      break;
    }
    
    if ( !is_key_valid(key) )
    {
      break;
    }

    pTemp = pRoot;

    while ( pTemp != &NIL )
    {  
      int result = strcmp(pTemp->key, key);

      if ( result > 0 ) // pTemp->key > key , go left
      {
        pTemp = pTemp->left;
      }
      else if ( result < 0 ) // pTemp->key < key , go right
      {
        pTemp = pTemp->right;
      }
      else  // found
      {
        break;
      }
    }

  } while (0);

  return pTemp;
}

/* done */ static RBTNode* rbt_create_node(const void* pItem, size_t itemSize, const char* key)
{
  RBTNode* pNode = NULL;

  do
  {
    if ( pItem == NULL )
    {
      break;
    }

    if ( !is_key_valid(key) ) 
    {
      break;
    }
    
    // allocate memory for node 
    // after key cheking to avoid excess malloc
    pNode = (RBTNode*)malloc(sizeof(RBTNode));
    if ( pNode == NULL )
    {
      break;
    }

    strcpy(pNode->key, key);

    // allocate memory for data
    void* pData = malloc(itemSize);
    if ( pData == NULL )
    {
      free(pNode);
      pNode == NULL;
      break;
    }

    memcpy(pData, pItem, itemSize);

    // default settings
    pNode->color = RED;    
    pNode->left = &NIL;
    pNode->right = &NIL;
    pNode->parent = NULL;

  } while (0);

  return pNode;
}

static IS_BALANCED rbt_is_balanced(RBTNode* pNode)
{
  // recursive traversal (post-condition)
  // if (left - right ... )

  return BALANCED;
}

/* state before left rotation
 *
 *       Node      
 *      /   \                
 *    A      Temp          
 *          /   \
 *         B     C
 */

/* done */ static void rbt_rot_left(RBTNode** pNode)
{
  RBTNode* pTemp = (*pNode)->right;   

  // Node <-> B               
  (*pNode)->right = pTemp->left;                     
  (*pNode)->right->parent = (*pNode);               

  pTemp->parent = (*pNode)->parent;

  // Temp <-> Node                                                  
  pTemp->left = *pNode;
  (*pNode)->parent = pTemp;

  *pNode = pTemp;
}

/* state before right rotation
 *
 *            Node      
 *           /   \                
 *       Temp     A             
 *      /    \
 *     C      B
 */

/* done */ static void rbt_rot_right(RBTNode** pNode)
{
  RBTNode* pTemp = (*pNode)->left; 

  // Node <-> B               
  (*pNode)->left = pTemp->right;                     
  (*pNode)->left->parent = (*pNode);

  pTemp->parent = (*pNode)->parent;

  // Temp <-> Node 
  pTemp->right = *pNode;
  (*pNode)->parent = pTemp;

  *pNode = pTemp;
}

static bool rbt_balance(RBTNode** pRoot, RBTNode** pNode)
{
  // check from inserted to root nodes rbt_is_balanced
  // if not balansed, rotate (left or right)

  return false;
}

/************************************************************************
 *                               PUBLIC                                 *
 ************************************************************************/

/* done */ bool rbt_init(RBTNode* pRoot)
{
  bool result = false;
  
  do
  {
    if ( pRoot == NULL )
    {
      break;
    }
    
    memset(pRoot->key, '\0', RBT_KEY_SIZE);
    pRoot->data = NULL;
    pRoot->color = BLACK;
    pRoot->left = NULL;
    pRoot->right = NULL;
    pRoot->parent = NULL;

    result = true;

  } while (0);
  
  return result;
}

bool rbt_close(RBTNode* pRoot)
{
  // recursive traversal
  // free memory of data field + free memory of nodes
  return false;
}

bool rbt_insert(RBTNode* pRoot, void* pItem, size_t itemSize, const char* key)
{
  return false;
}

bool rbt_delete(RBTNode* pRoot, const char* key)
{
  return false;
}

bool rbt_get(RBTNode* pRoot, void* pItem, size_t itemSize, const char* key)
{
  return false;
}