#include <stddef.h>
#include <stdlib.h>

#include "../../include/c/queue.h"

Node*
create_node (int value)
{
  Node* pTemp = (Node*)malloc (sizeof (Node));

  if (pTemp != NULL)
    {
      pTemp->value = value;
      pTemp->prev = NULL;
    }

  return pTemp;
}

Queue*
queue_init ()
{
  Queue* pTemp = (Queue*)malloc (sizeof (Queue));

  if (pTemp != NULL)
    {
      pTemp->head = NULL;
      pTemp->tail = NULL;
    }

  return pTemp;
}

Queue*
push (Queue* pQueue, Node* pNode)
{
  // check if queue is empty
  if (pQueue->head == NULL)
    {
      pQueue->tail = pNode;
      pQueue->head = pNode;
    }
  else
    {
      pQueue->tail->prev = pNode;
      pQueue->tail = pNode;
    }

  return pQueue;
}

Node*
pop (Queue* pQueue)
{
  Node* pNode = pQueue->head;

  if (pQueue->head != NULL)
    {
      Node* pTemp = pQueue->head;
      pQueue->head = pQueue->head->prev;

      // check if queue is empty
      if (pQueue->head == NULL)
        {
          pQueue->tail = NULL;
        }
    }

  return pNode;
}

Node*
peek (Queue* pQueue)
{
  return pQueue->head;
}