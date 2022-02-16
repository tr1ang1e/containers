#ifndef QUEUE_H
#define QUEUE_H

#include "thread_utils.h"

typedef struct QueueItemS
{
  void* data;
  size_t dataSize;
  struct QueueItemS* next;
} QueueItem;

typedef struct QueueS
{
  QueueItem* head;    // pop from head
  QueueItem* tail;    // push to tail
} Queue;

// tread-safety queue
typedef struct QueueSafeS
{
  MUTEX_TYPE mutex;
  CONDITION_TYPE cond;

  Queue queue;
} QueueSafe;

#define LIST_ITEM_KEY_SIZE (256)

typedef struct ListItemS
{
  char key[LIST_ITEM_KEY_SIZE];
  void* data;
  size_t dataSize;

  struct ListItemS* next;
  struct ListItemS* prev;
} ListItem;

/************************************************************************
 *                                QUEUES                                *
 ************************************************************************/
bool queue_init (Queue* pQueue);
void queue_destroy (Queue* pQueue);
bool queue_push (Queue* pQueue, const void* pItem, const size_t itemSize);
bool queue_pop (Queue* pQueue, void* pItem, const size_t itemSize);
bool queue_peek (Queue* pQueue, void* pItem, const size_t itemSize);

bool concurrent_queue_init (QueueSafe* pQueue);
void concurrent_queue_destroy (QueueSafe* pQueue);
bool concurrent_queue_push (QueueSafe* pQueue, const void* pItem, const size_t itemSize);
bool concurrent_queue_pop (QueueSafe* pQueue, void* pItem, const size_t itemSize,
                           const unsigned int asyncWaitMs);
bool concurrent_queue_peek (QueueSafe* pQueue, void* pItem, const size_t itemSize,
                            const unsigned int asyncWaitMs);

#endif    // QUEUE_H