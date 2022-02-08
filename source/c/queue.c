#include <stdlib.h>
#include <string.h>

#include "../../include/c/queue.h"

/************************************************************************
 *                                QUEUE                                 *
 ************************************************************************/
static Node*
create_node (const void* pItem, size_t itemSize)
{
  Node* pNode = NULL;

  do
    {
      if (pItem == NULL)
        {
          break;
        }

      // allocate memory for node
      pNode = (Node*)malloc (sizeof (Node));

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
      pNode->next = NULL;
    }
  while (0);

  return pNode;
}

bool
queue_init (Queue* pQueue)
{
  if (pQueue != NULL)
    {
      pQueue->head = NULL;
      pQueue->tail = NULL;

      return true;
    }

  return false;
}

bool
queue_push (Queue* pQueue, const void* pItem, size_t itemSize)
{
  bool result = false;

  do
    {
      if (pItem == NULL)
        {
          break;
        }

      Node* pNode = create_node (pItem, itemSize);

      if (pNode == NULL)
        {
          break;
        }

      // check if queue is empty
      if (pQueue->head == NULL)
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
    }
  while (0);

  return result;
}

bool
queue_pop (Queue* pQueue, void* pItem, size_t itemSize)
{
  bool result = false;

  do
    {
      if (pItem == NULL)
        {
          break;
        }

      Node* pNode = pQueue->head;

      if (pNode == NULL)
        {
          break;
        }

      // get data
      memcpy (pItem, pNode->data, itemSize);

      // make queue's state actual
      Node* pTemp = pQueue->head;
      pQueue->head = pQueue->head->next;

      // check if queue is empty
      if (pQueue->head == NULL)
        {
          pQueue->tail = NULL;
        }

      // free memory of copied and popped Node
      free (pTemp->data);
      free (pTemp);

      result = true;
    }
  while (0);

  return result;
}

bool
queue_peek (Queue* pQueue, void* pItem, size_t itemSize)
{
  int result = false;

  do
    {
      if (pQueue->head == NULL && pQueue->tail == NULL)
        {
          break;
        }

      if (pItem == NULL || pQueue->head == NULL)
        {
          break;
        }

      memcpy (pItem, pQueue->head->data, itemSize);

      result = true;
    }
  while (0);

  return result;
}


/************************************************************************
 *                             QUEUE_SAFE                               *
 ************************************************************************/

bool
queue_safe_init (QueueSafe* pQueue)
{
  bool result = false;

  do
    {
      if (pQueue == NULL)
        {
          break;
        }

      if (!mutex_init (&pQueue->mutex))
        {
          break;
        }

      // if (!condition_init (&pQueue->cond))
      //   {
      //     break;
      //   }

      result = queue_init (&pQueue->queue);
    }
  while (0);

  return result;
}

bool
queue_safe_close (QueueSafe* pQueue)
{
  bool result = false;

  if (mutex_lock (&pQueue->mutex))
    {
      // free memory if queue is not empty
      while (pQueue->queue.head != NULL)
        {
          Node* pNode = pQueue->queue.head;
          pQueue->queue.head = pQueue->queue.head->next;

          free (pNode->data);
          free (pNode);
        }

      pQueue->queue.tail = NULL;

      (void)mutex_unlock (&pQueue->mutex);

      result = true;

      (void)mutex_destroy (&pQueue->mutex);

      // (void)condition_destroy (&pQueue->cond);
    }

  return result;
}

bool
queue_safe_push (QueueSafe* pQueue, const void* pItem, size_t itemSize)
{
  bool result = false;

  if (mutex_lock (&pQueue->mutex))
    {
      // push data
      result = queue_push (&pQueue->queue, pItem, itemSize);

      // unlock mutex
      (void)mutex_unlock (&pQueue->mutex);
    }

  return result;
}

bool
queue_safe_pop (QueueSafe* pQueue, void* pItem, size_t itemSize)
{
  bool result = false;

  if (mutex_lock (&pQueue->mutex))
    {
      result = queue_pop (&pQueue->queue, pItem, itemSize);

      (void)mutex_unlock (&pQueue->mutex);
    }

  return result;
}

bool
queue_safe_peek (QueueSafe* pQueue, void* pItem, size_t itemSize)
{
  int result = false;

  if (mutex_lock (&pQueue->mutex) == true)
    {
      result = queue_peek (&pQueue->queue, pItem, itemSize);

      (void)mutex_unlock (&pQueue->mutex);
    }

  return result;
}

bool
queue_safe_push_with_notify (QueueSafe* pQueue, const void* pItem, size_t itemSize)
{
  bool result = false;

  if (mutex_lock (&pQueue->mutex))
    {
      result = queue_push (&pQueue->queue, pItem, itemSize);

      // (void)condition_signal (pQueue->cond);

      (void)mutex_unlock (&pQueue->mutex);
    }

  return result;
}

/* TODO: */ bool
queue_safe_pop_with_wait (QueueSafe* pQueue, void* pItem, size_t itemSize, int timeoutMs)
{
  return false;
}

/* TODO: */ bool
queue_safe_peek_with_wait (QueueSafe* pQueue, void* pItem, size_t itemSize, int timeoutMs)
{
  return false;
}