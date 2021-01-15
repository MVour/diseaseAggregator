//RBT 
// #include "myHash.h"
// #include "myPatient.h"
#include "myList.h"

enum color{b = 0, r = 1};

typedef struct RBnode RBnode;
typedef struct RBT RBT;

struct RBnode{
		enum color colorp;

		char* key;

		list_node* lNode;


		RBnode* par;
		RBnode* childr;
		RBnode* childl;
};


RBnode* RBnode_create(list_node* lNode, int flag);			//done
void RBnode_destroy(RBnode*);					//done

enum color RBnode_getColor(RBnode*);		//done
char* RBnode_getKey(RBnode*);
char* RBnode_getfName(RBnode*);
char* RBnode_getlName(RBnode*);
int RBnode_getAge(RBnode*);
char* RBnode_getPC(RBnode*);
char* RBnode_getG(RBnode*);

RBnode* RBnode_getParent(RBnode*);		//done
RBnode* RBnode_getGParent(RBnode*);		//done
RBnode* RBnode_getSibling(RBnode*);
RBnode* RBnode_getUncle(RBnode*);			//done
RBnode* RBnode_getChildr(RBnode*);		//done
RBnode* RBnode_getChildl(RBnode*);		//done

void RBnode_setParent(RBnode*, RBnode* n);	//done
void RBnode_setChildl(RBnode*, RBnode* n);	//done
void RBnode_setChildr(RBnode*, RBnode* n);	//done

void RBnode_recolor(RBnode*);				//done
int RBnode_setVote(RBnode*);
enum vote RBnode_getVote(RBnode*);

void RBnode_remove(RBnode*);


struct RBT{
		RBnode *root;
		int height;
	};


RBT* RBT_create();								//done
void RBT_destroy(RBT*);							//done

void RBT_deleteRBTnodes(RBT*, RBnode* n);


void RBT_setRoot(RBT*, RBnode* n);				//done
RBnode* RBT_getRoot(RBT*);
void RBT_rotateRight(RBT*, RBnode* n);			//done
void rotateLeft(RBT*, RBnode* n);				//done

int RBT_add(RBT*, list_node* lNode);			//done
int RBT_Insert(RBT*, RBnode *n);				//done
void RBT_Correct(RBT*, RBnode* n);			 	//done
void RBT_correctDeletion(RBT*, RBnode*x );
void RBT_validCheck(RBT*, RBnode* n);			//done

char RBT_getClr(RBT*, RBnode* n);

int RBT_findHeight(RBT*, RBnode* n);			//calculate heigth from a given node


RBnode* RBT_search(RBT*, char* K, RBnode* n);
RBnode* RBT_findFirstOlderOrEven(RBT*, char* K, RBnode* n);
int RBT_countBigKeys(RBT*, char* K, RBnode*);
int RBT_countMiddleKeys(RBT*, char* K1, char* K2, RBnode*, char*);
int RBT_find(RBT*, char* K);					//CHECK IF KEY EXISTS
void RBT_printNode(RBT*, RBnode* n);			//PRINT NODE


void RBT_remove(RBT*, RBnode* temp);
RBnode* RBT_findRep(RBT*, RBnode* n);
RBnode* RBT_findX(RBT*, RBnode* n);
void RBT_replace(RBT*, RBnode* n, RBnode* rplc, int flag);
