#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "heap.h"

///////////////////////////// HEAP_NODE

heapNode* heapNode_create(int v, char* k, int n){
	heapNode* newNode = (heapNode*)malloc(sizeof(heapNode));
	
	newNode->par = NULL;
	newNode->childr = NULL;
	newNode->childl = NULL;
	newNode->key = strdup(k);
	newNode->value = v;
	newNode->num = n;
	// newNode->cell = i;
	// printf("NEW NODES KEY: %s I GOT: %s\n", newNode->key, k);

	return newNode;
}

void heapNode_add(heapNode* n, int V){
	n->value += V;

}

void heapNode_destroy(heapNode* n){

	// printf("ekane free to key mesa sto heapNode\n");

	free(n->key);
	free(n);
	// printf("vgainei apo heapNode Destroy\n");
}

////////////////////////////// 	MAX_HEAP

maxHeap* maxHeap_create(char* Info, int size){
	maxHeap* newHeap = malloc(sizeof(maxHeap));

	newHeap->root = NULL;
	newHeap->entries = 0;
	newHeap->info = NULL;

	if(Info != NULL){
		newHeap->info = strdup(Info);
	}

	return newHeap;
}

void maxHeap_destroy(maxHeap* myHeap){

	// maxHeap_destroyAllNodes(myHeap, myHeap->root);

	if(myHeap->info != NULL){
		free(myHeap->info);
	}

	if(myHeap->root != NULL)
		maxHeap_deleteALL(myHeap, myHeap->root);

	free(myHeap);
}

int maxHeap_Empty(maxHeap* myHeap){
	if(myHeap->entries == 0)
		return 1;
	return 0;
}

void maxHeap_deleteALL(maxHeap* myHeap, heapNode* n){

	if(n->childr != NULL)
		maxHeap_deleteALL(myHeap, n->childr);
	if(n->childl != NULL)
		maxHeap_deleteALL(myHeap, n->childl);

	heapNode_destroy(n);

}

void maxHeap_add(maxHeap* myHeap, char* K, int V){
	// printf("ADD\n");
		// SEARCH FOR SAME KEY TO ADD VALUE
	heapNode* temp = maxHeap_findAndReturn(myHeap, myHeap->root, K);

	if(temp != NULL){
		// printf("\tHeap Node Already Exists ~ Updating Node ..");
		heapNode_add(temp, V);
		maxHeap_fixUp(myHeap, temp);
	}
	else{
		// printf("\tNew Heap Node ~ Updating Heap ..\n");
		maxHeap_Insert(myHeap, K, V);
		myHeap->entries++;
	}
// 
	// printf("\t, for key: %s\n", K);
	// printf("\t.. vghke\n");
}


heapNode* maxHeap_findAndReturn(maxHeap* myHeap, heapNode* n, char* K){
	// printf("FindAndReturn..\n");
	heapNode* temp = NULL;

	if(myHeap->root == NULL)
		return NULL;

	if(strcmp(n->key, K) == 0)
		return n;
	if(n->childr != NULL){
		temp = maxHeap_findAndReturn(myHeap, n->childr, K);
		if(temp != NULL)
			return temp;
	}
	if(n->childl != NULL){	
		temp = maxHeap_findAndReturn(myHeap, n->childl, K);
		if(temp != NULL)
			return temp;
	}
	return NULL;
}

heapNode* maxHeap_findAndReturnNUM(maxHeap* myHeap, heapNode* n, int K){
	// printf("FindAndReturn..\n");
	heapNode* temp = NULL;

	if(n->num ==  K)
		return n;

	if(n->childr != NULL){
		temp = maxHeap_findAndReturnNUM(myHeap, n->childr, K);
		// printf("mphke r\n");
		if(temp != NULL)
			return temp;
	}

	if(n->childl != NULL){
		temp = maxHeap_findAndReturnNUM(myHeap, n->childl, K);
		// printf("mphke l\n");
		if(temp != NULL)
			return temp;
	}

	return temp;
}


void maxHeap_Insert(maxHeap* myHeap, char* K, int V){
	// printf("Insert key: %s ..\n", K);

	heapNode* newNode = heapNode_create(V, K, myHeap->entries);

	if(myHeap->root == NULL){
		myHeap->root = newNode;
		return;
	}

	int par = maxHeap_findParent(myHeap->entries);
	heapNode* parNode = maxHeap_findAndReturnNUM(myHeap, myHeap->root, par);
	newNode->par = parNode;
	if(parNode->childl == NULL)
		parNode->childl = newNode;
	else
		parNode->childr = newNode;

	// else{
	// 	printf("SKATA STHN INSERT !!\n");
	// }


	// maxHeap_fixUp(myHeap, newNode);
	// printf("\t.. vghke\n");
}

void maxHeap_fixUp(maxHeap* myHeap, heapNode* n){
	// printf("fixUP for i: %d ..\n", i);
	if(n->par == NULL){
		// printf("\t .. vghke\n");
		return;
	}

	// int parent = maxHeap_findParent(i);
	// printf("myV: %d, parV: %d\n", myHeap->heapTable[i]->value, myHeap->heapTable[parent]->value);

	if(n->value <= n->par->value){
		// printf("\t ..... vghke\n");		
		return;
	}
	else{
		// printf("\nBefore swap: i = %d, par = %d\n",i , parent);
		maxHeap_swap(n, n->par);
		// printf("After swap: i = %d, par = %d\n",i , parent);
		maxHeap_fixUp(myHeap, n->par);	// i is now the parent
	}
}

int maxHeap_findParent(int i){

	return (i-1)/2;
}

// int maxHeap_findLeftCh(maxHeap* myHeap, int i){
// 	if(2*i + 1 > myHeap->entries-1)
// 		return INT_MIN;
// 	return (2*i + 1);
// }

// int maxHeap_findRightCh(maxHeap* myHeap, int i){
// 	if(2*i + 2 > myHeap->entries-1){
// 		return INT_MIN;
// 	}
// 	return (2*i + 2);
// }


void maxHeap_swap(heapNode* n1, heapNode* n2){
		// SWAP IS CHANGE VALUE AND KEY !!!!
			// NOT NUM !!!!

	int val0 = n1->value;
	char* key0 = strdup(n1->key);

	n1->value = n2->value;
	free(n1->key);
	n1->key = strdup(n2->key);

	n2->value = val0;
	free(n2->key);
	n2->key = strdup(key0);

	free(key0);

}

heapNode* maxHeap_pop(maxHeap* myHeap){

	if(myHeap->entries == 0)
		return NULL;

	if(myHeap->entries == 1){
		heapNode* temp = Copy(myHeap->root);
		heapNode_destroy(myHeap->root);
		myHeap->root = NULL;
		// char* temp = strdup(myHeap->root->key);
		
		myHeap->entries--;
		return temp;
	}

	heapNode* temp = Copy(myHeap->root);
	heapNode_add(myHeap->root, INT_MIN);

	// printf("Entries: %d, n num: %d\n",myHeap->entries, myHeap->root->num);
	heapNode* lastNode = maxHeap_findAndReturnNUM(myHeap, myHeap->root, myHeap->entries-1);

	maxHeap_swap(myHeap->root, lastNode);
	// printf("Gonna pop: %s, first is: %s\n", myHeap->heapTable[myHeap->entries-1]->key, myHeap->heapTable[0]->key);

	myHeap->entries--;
	// heapNode* temp = lastNode;
	
	// heapNode_destroy(lastNode);

	if(lastNode->par->childr == lastNode)
		lastNode->par->childr = NULL;
	else
		lastNode->par->childl = NULL;

	heapNode_destroy(lastNode);

	if(myHeap->root != NULL)
		maxHeap_heapify(myHeap, myHeap->root);

	return temp;

}

void maxHeap_heapify(maxHeap* myHeap, heapNode* n){

	if(n->childr == NULL && n->childl == NULL)	// KAI '-' GT H SYGKRISH GINETAI KA8E FORA ME TA PAIDIA
		return;

	int childr = -1;
	int childl = -1;

	// printf("childr: %d, dhildl: %d, i: %d\n", childr, childl, i);

	if(n->childr != NULL)
		childr = n->childr->value;

	if(n->childl != NULL)
		childl = n->childl->value;


	if(childl > childr){
		if(childl > n->value){
			maxHeap_swap(n->childl, n);
			maxHeap_heapify(myHeap, n->childl);
		}
	}
	else{
		if(childr > n->value){
			maxHeap_swap(n->childr, n);
			maxHeap_heapify(myHeap, n->childr);
		}
	}
}

heapNode* Copy(heapNode* n){

	if(n == NULL){
		return NULL;
	}

	heapNode* temp = heapNode_create(n->value, n->key, n->num);
	return temp;
}