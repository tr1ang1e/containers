#include <stdbool.h>

typedef unsigned int size;

typedef struct NodeS
{
  void* data;
  struct NodeS* prev;
} Node;

static Node* create_node (const void* pCont, size ContSize);

typedef struct QueueGenS
{
  Node* head;    // pop from head
  Node* tail;    // push to tail
} QueueGen;

QueueGen* queuegen_init ();
bool push_queuegen (QueueGen* pQueueGen, const void* pCont, size ContSize);
bool pop_queuegen (QueueGen* pQueueGen, void* pCont, size ContSize);
bool peek_queuegen (QueueGen* pQueueGen, void* pCont, size ContSize);