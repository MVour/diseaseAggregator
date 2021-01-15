// #include "myList.h"
#include "myHash.h"
#include "heap.h"
#include "filesStats.h"


////// FUNCTS
void printMenu();
char* act(int, hashTable*, hashTable*, infoNode*, StatsList*, char* buf);
// void act_1(hashTable*, char*, char*);
char* act_2(hashTable*, char*, char*, char*, char*);
char* act_3(StatsList* myStats, int, char*, char*, char*, char*);
char* act_4(infoNode*, char*);
char* act_5(StatsList*, char*, char*, char*, char*);
char* act_6(hashTable* , char*, char*, char*, char*);
// void act_3_4(hashTable*, char*, char*, char*, int, int);
// void act_5(hashTable*, hashTable*, infoNode*, myPatient*);
// void act_6(infoNode*, char*, char*);
// void act_7(hashTable*, char*);
// void act_8(hashTable* cHash, hashTable* dHash, infoNode* pList);
int isDate(char*);
void treeToHeap(RBT*, maxHeap*, RBnode*, char*, char*, char*);
int statsToHeap(StatsList*, maxHeap*, char* country, char* disease, char*, char*);
// int count_keys(RBnode* n, char* date1, char* date2);