#include "RBT.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

///////////////

RBnode* RBnode_create(list_node* lNode, int flag){
	RBnode* newNode = malloc(sizeof(RBnode));

	newNode->par = NULL;
	newNode->childr = NULL;
	newNode->childl = NULL;
	if(flag==1)
		newNode->colorp = b;
	else
		newNode->colorp = r;

	if(flag==0){
		newNode->lNode = lNode;
		newNode->key = strdup(lNode->patient->entrDate);
	}

	return newNode;	
//	printf( "RB NODE CREATED with key: "<< key );
}

void RBnode_destroy(RBnode* myNode){

	free (myNode->key);
	free (myNode);

//	printf( "RB NODE DELETED !!" );
}

char* RBnode_getKey(RBnode* myNode){
	return myNode->key;
}
char* RBnode_getlName(RBnode* myNode){
	return myNode->lNode->patient->lName;
}

char* RBnode_getfName(RBnode* myNode){
	return myNode->lNode->patient->fName;
}

// char* RBnode_getG(RBnode* myNode){
// 	return g;
// }

void RBnode_recolor(RBnode* myNode){
	if(myNode->colorp == r)
		myNode->colorp = b;
	else
		myNode->colorp = r;
}

enum color RBnode_getColor(RBnode* myNode){
	return myNode->colorp;
}

RBnode* RBnode_getChildl(RBnode* myNode){
	return myNode->childl;
}

RBnode* RBnode_getChildr(RBnode* myNode){
	return myNode->childr;
}

RBnode* RBnode_getParent(RBnode* myNode){
	return myNode->par;
}

RBnode* RBnode_getGParent(RBnode* myNode){
	return RBnode_getParent(myNode->par);
}

RBnode* RBnode_getSibling(RBnode* myNode){
	//printf( "mphke get sibling" );
	///printf( key);
	//printf( par->key );
	if(RBnode_getChildr(myNode->par) == myNode){
		//printf("edw");
		return RBnode_getChildl(RBnode_getParent(myNode));
	}
	else{
		//printf("EDW");
		return RBnode_getChildr(RBnode_getParent(myNode));
	}
}

RBnode* RBnode_getUncle(RBnode* myNode){
	//printf( "mphke get uncle" );
	return RBnode_getSibling(RBnode_getParent(myNode));
}

void RBnode_setChildr(RBnode* myNode, RBnode* n){
	myNode->childr = n;
}

void RBnode_setChildl(RBnode* myNode, RBnode* n){
	myNode->childl = n;
}

void RBnode_setParent(RBnode* myNode, RBnode* n){
	myNode->par = n;
}


void RBnode_remove(RBnode* myNode){
	if(RBnode_getChildr(RBnode_getParent(myNode)) == myNode){
		RBnode_setChildr(RBnode_getParent(myNode),NULL);
	}
	else
		RBnode_setChildl(RBnode_getParent(myNode),NULL);
	RBnode_destroy(myNode);
}

/////////////////////////////////

RBT* RBT_create(){
	RBT* newRBT = malloc(sizeof(RBT));
	newRBT->root = NULL;
	newRBT->height = 0;
	return newRBT;
	// printf("R-B TREE CREATED !!\n");
}

void RBT_destroy(RBT* myRBT){
	myRBT->height = RBT_findHeight(myRBT, myRBT->root);

	// printf("\nTrees height before deletion: %d\n", myRBT->height);
	
	RBT_deleteRBTnodes(myRBT, myRBT->root);
	myRBT->height = RBT_findHeight(myRBT, myRBT->root);
	
	if(myRBT->root == NULL){
		// printf("ROOT DELETED !!\n");
	}
	if(myRBT->height == 0 && myRBT->root == NULL){
		// printf( "RBT DELETED !!" );
	}
	else{
		// printf( "ERROR AT DELETION OF RBT!!" );
	}
//	printf(height);

	free(myRBT);
}

void RBT_deleteRBTnodes(RBT* myRBT, RBnode* n){
	if(RBnode_getChildr(n) != NULL)
		RBT_deleteRBTnodes(myRBT, RBnode_getChildr(n));
	if(RBnode_getChildl(n) != NULL)
		RBT_deleteRBTnodes(myRBT, RBnode_getChildl(n));
	if(RBnode_getParent(n)==NULL)
		myRBT->root = NULL;
	RBnode_destroy(n);
}

char RBT_getClr(RBT* myRBT, RBnode* n){
	if(n == NULL || RBnode_getColor(n) == 0)
		return 'b';
	else
		return 'r';
}

void RBT_setRoot(RBT* myRBT, RBnode* n){
	myRBT->root = n;
	if(RBnode_getColor(myRBT->root) == 1)
			RBnode_recolor(myRBT->root);
//	printf( "EGINE ROOT O: "<<root->getKey()<< " ME XRWMA: " << root->getColor() );
}

RBnode* RBT_getRoot(RBT* myRBT){
	return myRBT->root;
}

void RBT_rotateLeft(RBT* myRBT, RBnode* n){
	RBnode* temp = NULL;

	if( RBnode_getChildr(n) == NULL)
		return;
		// printf( "CONNOT ROTATE LEFT !!" );
	else{
		if( n == myRBT->root){
			RBT_setRoot(myRBT, RBnode_getChildr(n));
		}
		else if( RBnode_getParent(n) != NULL){
			if(RBnode_getChildl(RBnode_getParent(n)) == n)
				RBnode_setChildl(RBnode_getParent(n), RBnode_getChildr(n));
			else
				RBnode_setChildr(RBnode_getParent(n), RBnode_getChildr(n));
		}

		RBnode_setParent(RBnode_getChildr(n), RBnode_getParent(n));

		if(RBnode_getChildl(RBnode_getChildr(n)) != NULL)
			temp = RBnode_getChildl(RBnode_getChildr(n));
		RBnode_setChildl(RBnode_getChildr(n), n);
		RBnode_setParent(n, RBnode_getChildr(n));
		if(temp != NULL){
			RBnode_setChildr(n, temp);
			RBnode_setParent(RBnode_getChildr(n), n);
		}
		else
			RBnode_setChildr(n, NULL);
	}
	//updateSelfs(root);
}

void RBT_rotateRight(RBT* myRBT, RBnode* n){
	RBnode* temp = NULL;

	if( RBnode_getChildl(n) == NULL)
		return;
		// printf( "CONNOT ROTATE LEFT !!" );
	else{
		if( n == myRBT->root){
			RBT_setRoot(myRBT, RBnode_getChildl(n));
		}
		else if( RBnode_getParent(n) != NULL){
			if(RBnode_getChildr(RBnode_getParent(n)) == n)
				RBnode_setChildr(RBnode_getParent(n), RBnode_getChildl(n));
			else
				RBnode_setChildl(RBnode_getParent(n), RBnode_getChildl(n));
		}

		RBnode_setParent(RBnode_getChildl(n), RBnode_getParent(n));

		if(RBnode_getChildr(RBnode_getChildl(n)) != NULL)
			temp = RBnode_getChildr(RBnode_getChildl(n));
		RBnode_setChildr(RBnode_getChildl(n), n);
		RBnode_setParent(n, RBnode_getChildl(n));
		if(temp != NULL){
			RBnode_setChildl(n, temp);
			RBnode_setParent(RBnode_getChildl(n), n);
		}
		else
			RBnode_setChildl(n, NULL);
	}
	//updateSelfs(root);
}


void RBT_Correct(RBT* myRBT, RBnode* n){
//	printf( "----> mphke Correct" );
	char UncClr = RBT_getClr(myRBT, RBnode_getUncle(n));
	
	if( myRBT->root != NULL){
		if(UncClr == 1){		//check if uncle is red
//			printf("MPHKE: " << n->getKey());
			RBnode_recolor(RBnode_getParent(n));
			RBnode_recolor(RBnode_getUncle(n));
			RBnode_recolor(RBnode_getGParent(n));
			RBT_validCheck(myRBT, RBnode_getGParent(n));	/////////////////////////////MEXRI EDW
		}		
		else if(UncClr == 'b'){	//check if uncle is black
			if( RBnode_getChildl(RBnode_getParent(n)) == n){
				if(RBnode_getChildr(RBnode_getGParent(n)) == RBnode_getParent(n) ){	//triangle
					RBT_rotateRight(myRBT, RBnode_getParent(n));
					RBT_validCheck(myRBT, RBnode_getChildr(n));
				}
				else{													//line
					RBnode_recolor(RBnode_getGParent(n));
					RBnode_recolor(RBnode_getParent(n));
					RBT_rotateRight(myRBT, RBnode_getGParent(n));
				}
			}
			else if(RBnode_getChildr(RBnode_getParent(n)) ==n){
				if(RBnode_getChildl(RBnode_getGParent(n)) == RBnode_getParent(n)){		//triangle
					RBT_rotateLeft(myRBT, RBnode_getParent(n));
					RBT_validCheck(myRBT, RBnode_getChildl(n));
				}
				else{												//line
					RBnode_recolor(RBnode_getGParent(n));
					RBnode_recolor(RBnode_getParent(n));
					RBT_rotateLeft(myRBT, RBnode_getGParent(n));
				}
			}			
		}
	}
}

int RBT_add(RBT* myRBT, list_node* lNode){
	RBnode* n = RBnode_create(lNode, 0);
	int flag =0;
//	height=findHeight(root);
//	printf("height: "<<height);
//	printf("SKATA");
	RBT_Insert(myRBT, n);
	return flag;
	//printSelfs(root);
}

int RBT_Insert(RBT* myRBT, RBnode *n){
//	printf( "----> MPHKE STHN INSERT" );
	RBnode* temp1 = NULL;
	RBnode* temp2 = NULL;
	temp1 = myRBT->root;
	
	if(myRBT->root == NULL){

		RBT_setRoot(myRBT, n);
	}
	else{
		while(temp1!=NULL){
			temp2 = temp1;
			if(cmpDate(RBnode_getKey(n), RBnode_getKey(temp1)) != 1)
				temp1 = RBnode_getChildr(temp1);
			// else if(cmpDate(RBnode_getKey(n) , RBnode_getKey(temp1)) == 3){	// CASE ME IDIO KEY/DATE

			// 	// !!!!
			// 	// 8ELW FUNCT P NA TSEKAREI AN YPARXEI HDH H EGGRAFH (ISWS AUTO NA TO KANW STO HASH KALYTERA)
			// 	// !!!!

			// 	//printf( "KEY: "<< n->getKey()<<" ALREADY EXIST !!" );
			// 	RBnode_destroy(n);
			// 	n = NULL;
			// 	return 1;
			// }
			else
				temp1 = RBnode_getChildl(temp1);
		}
		if(n != NULL){
			RBnode_setParent(n, temp2);
		
//		printf( "EKANE SET PARENT o: "<<n->getKey()<< " me par: "<<n->getParent()->getKey());
			if (cmpDate(RBnode_getKey(n) , RBnode_getKey(temp2)) != 1)
				RBnode_setChildr(temp2, n);
			else
				RBnode_setChildl(temp2, n);
		}
//		printf("skata");
	}
	if(n != NULL)
		RBT_validCheck(myRBT, n);
	return 0;
}

void RBT_validCheck(RBT* myRBT, RBnode* n){	
//	printf( "----> Mphke valid" );
//	updateSelfs(root);
	if(n == myRBT->root && RBnode_getColor(n) == 1)
		RBnode_recolor(n);
	else if(RBT_getClr(myRBT, n) == RBT_getClr(myRBT, RBnode_getParent(n)) && n != myRBT->root)
		RBT_Correct(myRBT, n);

}

int RBT_findHeight(RBT* myRBT, RBnode* n){
	RBnode* temp = n;
	int hl = 0;
	int hr = 0;

	while(temp != NULL){
		if(RBnode_getChildr(temp) != NULL){
			hr ++;
			hr += RBT_findHeight(myRBT, RBnode_getChildr(temp));
			break;
		}
		if(RBnode_getChildl(temp) != NULL){
			hl ++;
			hl += RBT_findHeight(myRBT, RBnode_getChildl(temp));
			break;
		}
		if(RBnode_getChildr(temp) == NULL && RBnode_getChildl(temp) == NULL)
			break;
	}
	if(hr >= hl)
		return hr;
	else
		return hl;
}


RBnode* RBT_search(RBT* myRBT, char* K, RBnode *n){
	RBnode* temp = n;
	RBnode* temp2 = NULL;
//	printf( "tmp key: " << temp->getKey()<< " / K: " << K );

	if(cmpDate(RBnode_getKey(temp), K) == 3){
		return temp;
	}
	else{
		if(RBnode_getChildl(temp) != NULL){
			temp2 = RBT_search(myRBT, K, RBnode_getChildl(temp));
			if(temp2!=NULL)
				return temp2;
		}
	
	
		if(RBnode_getChildr(temp) != NULL){
			temp2 = RBT_search(myRBT, K, RBnode_getChildr(temp));
			if(temp2 != NULL)
				return temp2;
			}
		}
	return temp2;
}


RBnode* RBT_findFirstOlderOrEven(RBT* myRBT, char* K, RBnode* n){
	RBnode* temp = n;
	RBnode* temp2 = NULL;

	// RBT_printNode(myRBT, n);
	// RBT_printNode(myRBT, n->childr);
//	printf( "tmp key: " << temp->getKey()<< " / K: " << K );
	int comp = cmpDate(RBnode_getKey(temp), K);

	if(comp == 0 || comp == 3){
		return temp;
	}
	else{
		if(RBnode_getChildl(temp) != NULL){
			temp2 = RBT_search(myRBT, K, RBnode_getChildl(temp));
			if(temp2!=NULL)
				return temp2;
		}
	
	
		if(RBnode_getChildr(temp) != NULL){
			temp2 = RBT_search(myRBT, K, RBnode_getChildr(temp));
			if(temp2 != NULL)
				return temp2;
			}
		}
	return temp2;
}


int RBT_countBigKeys(RBT* myRBT, char* K, RBnode* start){

	int count = 0;
    // temp2Node = temp1Node;

    if(start == NULL){
    	return 0;
    }


	RBT_printNode(myRBT, start);

    int comp = cmpDate(K, start->key);
    // printf("\t\tK: %s, nodeK: %s, comp: %d\n", K, start->key, comp);
    if(comp != 1){
    	count++;
        count += RBT_countBigKeys(myRBT, K, start->childr);
        count += RBT_countBigKeys(myRBT, K, start->childl);
    }
    else{
    	if(cmpDate(K, start->childr->lNode->patient->entrDate) != 1)
        	count += RBT_countBigKeys(myRBT, K, start->childr);
    }    

	return count;

}

int RBT_countMiddleKeys(RBT* myRBT, char* K1, char* K2, RBnode* start, char* extraK){

	int count = 0;
    // temp2Node = temp1Node;

    if(start == NULL){
    	return 0;
    }

    // printf("extraK: %s\n", extraK);

	// RBT_printNode(myRBT, start);

    int comp1 = cmpDate(K1, start->key);
    int comp2 = cmpDate(K2, start->key);
    // printf("\t\tK: %s, nodeK: %s, comp1: %d, comp2: %d\n", K1, start->key, comp1, comp2);
    if(comp1 != 0 && comp2 != 1){
    	if(extraK == NULL){
    		count++;
    		// printf("id: %s\n", start->lNode->patient->id);
    	}
    	else{
    		if(strcmp(extraK, start->lNode->patient->country) == 0){
    			// printf("patient country: %s, comp1: %d, comp2: %d\n", start->lNode->patient->country, comp1 , comp2);
    			count++;
    		}
    	}
        count += RBT_countMiddleKeys(myRBT, K1, K2, start->childr, extraK);
        count += RBT_countMiddleKeys(myRBT, K1, K2, start->childl, extraK);
    }
    else{
    	if(comp2 == 1 && start->childl != NULL){
    		// if(cmpDate(K2, start->childl->key) != 1)
    			count += RBT_countMiddleKeys(myRBT, K1, K2, start->childl, extraK);
    	}
    	if(comp1 == 0 && start->childr != NULL){
    		// if(cmpDate(K1, start->childr->key) != 0)
    			count += RBT_countMiddleKeys(myRBT, K1, K2, start->childr, extraK);
    	}
    }    

	return count;

}


int RBT_find(RBT* myRBT, char* K){
//	printf( "search gia: " << K << "  : " << search(K,this->root) );
	if(RBT_search(myRBT, K, myRBT->root) != NULL)
		return 1;
	else
		return 0;
}

void RBT_printNode(RBT* myRBT, RBnode* n){

	printf("%s %s", RBnode_getKey(n), RBnode_getlName(n));
	printf("%s\n", RBnode_getfName(n)) ;

}

void RBT_remove(RBT* myRBT, RBnode* temp){
	RBnode* tcl = RBnode_getChildl(temp);
	RBnode* tcr = RBnode_getChildr(temp);
	RBnode* replacement = NULL;
	RBnode* x = NULL;
	int flag = 0;

// STEP 1:
	if(tcl == NULL && tcr == NULL){
		x = RBnode_create(NULL, 1);
		RBnode_setParent(x, temp);
		flag = 1;
	}
	else if(tcl != NULL && tcr != NULL){
		replacement = RBT_findRep(myRBT, temp);
		x = RBnode_getChildr(replacement);
	}
	else if(tcl != NULL){
		replacement = tcl;
		x = replacement;
	}
	else{
		replacement = tcr;
		x = replacement;
	}

//STEP 2:
	//if(temp->getColor() == 1 && replacement == NULL){
	//}
	if(RBnode_getColor(temp) == 0 && replacement == NULL){
		RBT_replace(myRBT, temp, replacement, 1);
		RBT_correctDeletion(myRBT, x);
	}
	else if(RBnode_getColor(temp) == 1 && RBnode_getColor(replacement) == 1){
		RBT_replace(myRBT, temp, replacement, 0);
	}
	else if(RBnode_getColor(temp) == 1 && RBnode_getColor(replacement) == 0){
		RBnode_recolor(replacement);
		RBT_correctDeletion(myRBT, x);
	}
	else if(RBnode_getColor(temp) == 0 && RBnode_getColor(replacement) == 1){
		RBnode_recolor(replacement);
	}
	else if(RBnode_getColor(temp) == 0 && RBnode_getColor(replacement) == 0){
		RBT_correctDeletion(myRBT, x);
	}
	if(flag==1){
		if(RBnode_getChildl(RBnode_getParent(x)) == x)
			RBnode_setChildl(RBnode_getParent(x), NULL);
		else
			RBnode_setChildr(RBnode_getParent(x), NULL);
		RBnode_destroy(x);
	}
	// printf("VGAINEI APO REMOVE!!" );
}

void RBT_correctDeletion(RBT* myRBT, RBnode* n){

//STEP 3:
	RBnode* x = n;
	RBnode* s = RBnode_getSibling(x);
			//Case0:
	if(RBnode_getColor(x) == 1){
		RBnode_recolor(x);
	}
			//Case1:
	else if(RBnode_getColor(x) == 0 && RBT_getClr(myRBT, s) ==1){
		// printf( "mphkes se case1");
		RBnode_recolor(s);
		RBnode_recolor(RBnode_getParent(x));
		if(RBnode_getChildl(RBnode_getParent(x)) == x)
			RBT_rotateLeft(myRBT, RBnode_getParent(x));
		else
			RBT_rotateRight(myRBT, RBnode_getParent(x));
		RBT_correctDeletion(myRBT, x);
	}
	else if(RBnode_getColor(x)==0 && RBnode_getColor(s) == 0){
		// printf("MPHKE KAPOU" );
				//Case2:
		if(RBT_getClr(myRBT, RBnode_getChildr(s)) == 0 && RBT_getClr(myRBT, RBnode_getChildl(s)) ==0){
			// printf( "mphkes se case2");
			RBnode_recolor(s);
			x = RBnode_getParent(x);
			if(RBnode_getColor(x) == 1)
				RBnode_recolor(x);
			else
				RBT_correctDeletion(myRBT, x);
		}//Case3.1-4.1: x=LeftChild !!
		else if(RBnode_getChildl(RBnode_getParent(x)) == x){
			// printf( "mphkes se case3-4");
			//Case3.1: / s->leftchild=RED
			if(RBT_getClr(myRBT, RBnode_getChildl(s)) == 1){
				RBnode_recolor(RBnode_getChildl(s));
				RBnode_recolor(s);
				RBT_rotateRight(myRBT, s);
				RBT_correctDeletion(myRBT, x);
			}
			//Case4.1: /s->rightChild=RED
			else{
				if(RBnode_getColor(RBnode_getParent(x)) != RBnode_getColor(s))
					RBnode_recolor(s);
				if(RBnode_getColor(RBnode_getParent(x)) != 0)
					RBnode_recolor(RBnode_getParent(x));
				RBnode_recolor(RBnode_getChildr(s));
				RBT_rotateLeft(myRBT, RBnode_getParent(x));
			}
		}//Case3.2-4.2: x=RightChild
		else{
			// printf( "mphkes se case3-4");
			//Case3.2: / s->rightChild=RED
			if(RBT_getClr(myRBT, RBnode_getChildr(s)) ==1){
				RBnode_recolor(RBnode_getChildr(s));
				RBnode_recolor(s);
				RBT_rotateLeft(myRBT, s);
				RBT_correctDeletion(myRBT, x);
			}
			//Case4.2: /s->leftChild=RED
			else{
				if(RBnode_getColor(RBnode_getParent(x)) != RBnode_getColor(s))
					RBnode_recolor(s);
				if(RBnode_getColor(RBnode_getParent(x)) != 0)
					RBnode_recolor(RBnode_getParent(x));
				RBnode_recolor(RBnode_getChildl(s));
				RBT_rotateRight(myRBT, RBnode_getParent(x));
			}

		}
	}

}

RBnode* RBT_findRep(RBT* myRBT, RBnode* n){
	if(RBnode_getChildl(RBnode_getChildr(n)) != NULL){
		return RBnode_getChildl(RBnode_getChildr(n));
	}
	else{
		return RBnode_getChildr(n);
	}
}

void RBT_replace(RBT* myRBT, RBnode* n,RBnode* rplc, int flag){
	if(n == myRBT->root){
		RBT_setRoot(myRBT, rplc);
	}
	RBnode_setParent(rplc, RBnode_getParent(n));
	if(flag == 0){
		RBnode_setChildl(rplc, RBnode_getChildl(n));
		RBnode_setChildr(rplc, RBnode_getChildr(n));
	}
	if(n != myRBT->root){
		if(RBnode_getChildl(RBnode_getParent(n)) == n){
			RBnode_setChildl(RBnode_getParent(n), rplc);
		}
		else
			RBnode_setChildr(RBnode_getParent(n), rplc);
	}
	RBnode_destroy(n);

}
