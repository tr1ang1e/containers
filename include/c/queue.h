
typedef struct NodeS
{
  int value;
  struct NodeS* prev;
} Node;

Node* create_node (int);

typedef struct QueueS
{
  Node* head;    // pop from head
  Node* tail;    // push to tail
} Queue;

Queue* queue_init ();
Queue* push (Queue*, Node*);
Node* pop (Queue*);
Node* peek (Queue*);