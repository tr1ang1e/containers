// (StructType*)0 = cast zero value to StructType pointer
#define GET_NODE(pListItem, StructType, ListItemName)   \
    (StructType *)((char *)(pListItem) - (char *) &((StructType *)0)->ListItemName)

// all operations are with struct ListS fields
typedef struct ListS
{
    struct ListS* prev;
    struct ListS* next;
} List;

typedef struct NodeS
{
    void* data1;
    void* data2;
    void* data3;

    List ListItem;    // Nodes list navigation
} Node;

int main()
{
    Node* pNode = (Node*)malloc(sizeof(Node));
    List* pList = &(pNode->ListItem);

    Node* pN = GET_NODE(pList, Node, ListItem);
}