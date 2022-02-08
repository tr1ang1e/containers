#ifndef CONTAINERS_H
#define CONTAINERS_H

#include "thread_utils.h"

typedef struct NodeS
{
  void* data;
  struct NodeS* next;
} Node;

typedef struct QueueS
{
  Node* head;    // pop from head
  Node* tail;    // push to tail
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

  struct ListItemS* next;
  struct ListItemS* prev;
} ListItem;

#ifdef __cplusplus
extern "C"
{
#endif

  /************************************************************************
   *                                QUEUE                                 *
   ************************************************************************/
  bool queue_init (Queue* pQueue);
  bool queue_push (Queue* pQueue, const void* pItem, size_t itemSize);
  bool queue_pop (Queue* pQueue, void* pItem, size_t itemSize);
  bool queue_peek (Queue* pQueue, void* pItem, size_t itemSize);

  /************************************************************************
   *                             QUEUE SAFE                               *
   ************************************************************************/
  bool queue_safe_init (QueueSafe* pQueue);
  bool queue_safe_close (QueueSafe* pQueue);
  bool queue_safe_push (QueueSafe* pQueue, const void* pItem, size_t itemSize);
  bool queue_safe_push_with_notify (QueueSafe* pQueue, const void* pItem,
                                    size_t itemSize);
  bool queue_safe_pop (QueueSafe* pQueue, void* pItem, size_t itemSize);
  bool queue_safe_peek (QueueSafe* pQueue, void* pItem, size_t itemSize);

  // not implemented
  bool queue_safe_pop_with_wait (QueueSafe* pQueue, void* pItem, size_t itemSize,
                                 int timeoutMs);
  bool queue_safe_peek_with_wait (QueueSafe* pQueue, void* pItem, size_t itemSize,
                                  int timeoutMs);

  /************************************************************************
   *                                LIST                                  *
   ************************************************************************/
  ListItem* list_test_create_item (void* info);
  void list_test_insert_item (ListItem** head, ListItem* pItem);
  void list_test_remove_item (ListItem** head, ListItem* pItem);
  ListItem* list_test_find_item (ListItem* head, void* info);

  bool list_init (ListItem* pHead);
  bool list_close (ListItem** pHead);
  bool list_insert_item (ListItem** pHead, const void* pItem, size_t itemSize,
                         const char* key);
  bool list_get_item (ListItem* pHead, void* pItem, size_t itemSize, const char* key);
  bool list_remove_item (ListItem** pHead, const char* key);

#ifdef __cplusplus
}
#endif

#endif    // !CONTAINERS_H