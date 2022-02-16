#include <stdlib.h>
#include <string.h>

#include "../../include/c/queue.h"

/************************************************************************
 *                                QUEUE                                 *
 ************************************************************************/
#define QUEUE_IS_EMPTY(queue) ((queue)->head == NULL)

static QueueItem* create_node (const void* pItem, const size_t itemSize)
{
  QueueItem* pNode = NULL;

  do
  {
    if (pItem == NULL)
    {
      break;
    }

    // allocate memory for node
    pNode = (QueueItem*)malloc (sizeof (QueueItem));
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

    memcpy (pData, pItem, itemSize);

    pNode->data = pData;
    pNode->dataSize = itemSize;
    pNode->next = NULL;

  } while (0);

  return pNode;
}

bool queue_init (Queue* pQueue)
{
  if (pQueue != NULL)
  {
    pQueue->head = NULL;
    pQueue->tail = NULL;

    return true;
  }

  return false;
}

void queue_destroy (Queue* pQueue)
{
  if (pQueue != NULL)
  {
    while (pQueue->head != NULL)
    {
      QueueItem* pNode = pQueue->head;
      pQueue->head = pQueue->head->next;

      free (pNode->data);
      free (pNode);
    }

    pQueue->tail = NULL;
  }
}

bool queue_push (Queue* pQueue, const void* pItem, const size_t itemSize)
{
  bool result = false;

  do
  {
    if (pItem == NULL)
    {
      break;
    }

    QueueItem* pNode = create_node (pItem, itemSize);
    if (pNode == NULL)
    {
      break;
    }

    if (QUEUE_IS_EMPTY (pQueue))
    {
      pQueue->tail = pNode;
      pQueue->head = pNode;
    }
    else
    {
      pQueue->tail->next = pNode;
      pQueue->tail = pNode;
    }

    result = true;

  } while (0);

  return result;
}

bool queue_pop (Queue* pQueue, void* pItem, const size_t itemSize)
{
  bool result = false;

  do
  {
    if (pItem == NULL || QUEUE_IS_EMPTY (pQueue))
    {
      break;
    }

    // validate the requested item size
    if (itemSize != pQueue->head->dataSize)
    {
      break;
    }

    memcpy (pItem, pQueue->head->data, itemSize);

    QueueItem* pTemp = pQueue->head;
    pQueue->head = pQueue->head->next;

    if (QUEUE_IS_EMPTY (pQueue))
    {
      pQueue->tail = NULL;
    }

    // free memory of copied and popped Node
    free (pTemp->data);
    free (pTemp);

    result = true;

  } while (0);

  return result;
}

bool queue_peek (Queue* pQueue, void* pItem, const size_t itemSize)
{
  int result = false;

  do
  {
    if (pItem == NULL || QUEUE_IS_EMPTY (pQueue))
    {
      break;
    }

    // validate the requested item size
    if (itemSize != pQueue->head->dataSize)
    {
      break;
    }

    memcpy (pItem, pQueue->head->data, itemSize);

    result = true;

  } while (0);

  return result;
}

/************************************************************************
 *                             QUEUE_SAFE                               *
 ************************************************************************/

bool concurrent_queue_init (QueueSafe* pQueue)
{
  bool result = false;

  bool internalQueueInited = queue_init (&pQueue->queue);

  if (internalQueueInited)
  {
    if (mutex_init (&pQueue->mutex))
    {
      if (condition_init (&pQueue->cond))
      {
        result = true;
      }
      else
      {
        (void)mutex_destroy (&pQueue->mutex);
      }
    }
  }

  return result;
}

void concurrent_queue_destroy (QueueSafe* pQueue)
{
  if (mutex_lock (&pQueue->mutex))
  {
    queue_destroy (&pQueue->queue);

    (void)mutex_unlock (&pQueue->mutex);

    (void)mutex_destroy (&pQueue->mutex);
    (void)condition_destroy (&pQueue->cond);
  }
}

bool concurrent_queue_push (QueueSafe* pQueue, const void* pItem, const size_t itemSize)
{
  bool result = false;

  if (mutex_lock (&pQueue->mutex))
  {
    result = queue_push (&pQueue->queue, pItem, itemSize);

    if (result)
    {
      (void)condition_broadcast (&pQueue->cond);
    }

    (void)mutex_unlock (&pQueue->mutex);
  }

  return result;
}

bool concurrent_queue_pop (QueueSafe* pQueue, void* pItem, const size_t itemSize,
                           const unsigned int asyncWaitMs)
{
  bool result = false;

  if (mutex_lock (&pQueue->mutex))
  {
    Queue* pPrivateQueue = &pQueue->queue;

    if (QUEUE_IS_EMPTY (pPrivateQueue))
    {
      // miss.. let's wait for items..
      if (condition_wait (&pQueue->cond, &pQueue->mutex, asyncWaitMs))
      {
        /*
          ok.. wake up signal was received and we are in the locked scope,
          but it is possible that another waiter thread has already extracted the data
          so, queue may be empty now - check again
        */

        if (!QUEUE_IS_EMPTY (pPrivateQueue))
        {
          result = queue_pop (pPrivateQueue, pItem, itemSize);
        }
      }
    }
    else
    {
      result = queue_pop (pPrivateQueue, pItem, itemSize);
    }

    (void)mutex_unlock (&pQueue->mutex);
  }

  return result;
}

bool concurrent_queue_peek (QueueSafe* pQueue, void* pItem, const size_t itemSize,
                            const unsigned int asyncWaitMs)
{
  bool result = false;

  if (mutex_lock (&pQueue->mutex))
  {
    Queue* pPrivateQueue = &pQueue->queue;

    if (QUEUE_IS_EMPTY (pPrivateQueue))
    {
      if (condition_wait (&pQueue->cond, &pQueue->mutex, asyncWaitMs))
      {
        if (!QUEUE_IS_EMPTY (pPrivateQueue))
        {
          result = queue_peek (pPrivateQueue, pItem, itemSize);
        }
      }
    }
    else
    {
      result = queue_peek (pPrivateQueue, pItem, itemSize);
    }

    (void)mutex_unlock (&pQueue->mutex);
  }

  return result;
}