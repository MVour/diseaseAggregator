// #include "myHash.h"
// #include "filesStats.h"
#include "functs.h"

typedef struct checkNode checkNode;

struct checkNode{
	char* fname;
	checkNode* next;
};


char** getCountries(char*, int* );
void scanFiles(hashTable* cHash, hashTable* dHash, infoNode* patientsList, StatsList*, char** CNames, int CCount, char*);
void gather(hashTable*, hashTable*, infoNode*, StatsList*, char*, char*);
dateStats* fillStructs(hashTable*, hashTable*, infoNode*, char*, char*, char*);
int checkExist(char*);

void clean_up(char** CNames, int CCount, hashTable*, hashTable*, infoNode*, StatsList*, char*, char*, char*, int, int, char*, char*);
void make_log(int, int, int, char**, int);
void notifyParent(int, int);

void addFiles(int signum);
void to_log(int signum);