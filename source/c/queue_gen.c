#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/c/queue_gen.h"

static Node*
create_node (const void* pCont, size ContSize)
{
  // allocate memory for node
  Node* pNode = (Node*)malloc (sizeof (Node));

  if (pNode != NULL)
    {
      // allocate memory for data
      void* pData = malloc (ContSize);

      if (pData != NULL)
        {
          memcpy (pData, pCont, ContSize);

          pNode->data = pData;
          pNode->prev = NULL;
        }
      else
        {
          free (pNode);
          pNode = NULL;
        }
    }

  return pNode;
}

QueueGen*
queuegen_init ()
{
  QueueGen* pTemp = (QueueGen*)malloc (sizeof (QueueGen));

  if (pTemp != NULL)
    {
      pTemp->head = NULL;
      pTemp->tail = NULL;
    }

  return pTemp;
}

bool
push_queuegen (QueueGen* pQueueGen, const void* pCont, size ContSize)
{
  Node* pNode = create_node (pCont, ContSize);

  if (pNode != NULL)
    {
      // check if queue is empty
      if (pQueueGen->head == NULL)
        {
          pQueueGen->tail = pNode;
          pQueueGen->head = pNode;
        }
      else
        {
          pQueueGen->tail->prev = pNode;
          pQueueGen->tail = pNode;
        }
    }
  else
    {
      return false;
    }

  return true;
}

bool
pop_queuegen (QueueGen* pQueueGen, void* pCont, size ContSize)
{
  Node* pNode = pQueueGen->head;

  if (pQueueGen->head != NULL)
    {
      // get data
      memcpy (pCont, pNode->data, ContSize);

      // make queue's state actual
      Node* pTemp = pQueueGen->head;
      pQueueGen->head = pQueueGen->head->prev;

      // check if queue is empty
      if (pQueueGen->head == NULL)
        {
          pQueueGen->tail = NULL;
        }

      // free memory of copied and popped node
      free (pTemp->data);
      free (pTemp);
    }
  else
    {
      return false;
    }

  return true;
}

bool
peek_queuegen (QueueGen* pQueueGen, void* pCont, size ContSize)
{
  memcpy (pCont, pQueueGen->head->data, ContSize);

  return true;
}