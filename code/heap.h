typedef struct heapNode heapNode;
typedef struct maxHeap maxHeap;

////////////////////////////////////// HEAP_NODE

struct heapNode{
	
	int value;
	char* key;
	int num;
	// int cell;

	heapNode* par;
	heapNode* childr;
	heapNode* childl;

};

heapNode* heapNode_create(int v, char* k, int n);
void heapNode_destroy(heapNode* n);

void heapNode_add(heapNode* n, int V);


/////////////////////////////////////// MAX_HEAP

struct maxHeap{
	heapNode* root;
	int entries;
	// heapNode** heapTable;
	// int maxHeapSize;
	char* info;
};

maxHeap* maxHeap_create(char*, int );
void maxHeap_destroy(maxHeap* );
void maxHeap_deleteALL(maxHeap*, heapNode* );

void maxHeap_add(maxHeap*, char*, int);
heapNode* maxHeap_findAndReturn(maxHeap*, heapNode*, char* );
heapNode* maxHeap_findAndReturnNUM(maxHeap* myHeap, heapNode* n, int K );

void maxHeap_Insert(maxHeap*, char*, int );

int maxHeap_findParent(int i );
// int maxHeap_findLeftCh(maxHeap* myHeap, int i ); 
// int maxHeap_findRightCh(maxHeap* myHeap, int i );

void maxHeap_fixUp(maxHeap*, heapNode* );
void maxHeap_swap(heapNode*, heapNode* );
void maxHeap_heapify(maxHeap*, heapNode* );
heapNode* maxHeap_pop(maxHeap* );
heapNode* Copy(heapNode* );
int maxHeap_Empty(maxHeap*);