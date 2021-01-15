#include "myPatient.h"
// #include "myHash.h"

typedef struct list_node list_node;
typedef struct infoNode infoNode;

struct list_node{
    myPatient* patient;
    list_node* next;
};

struct infoNode{
    list_node* head;
    int size;
};

///// Lists Functs

infoNode* list_create();
list_node* list_add(infoNode* , myPatient*);      //pushes to last spot
myPatient* list_pop(infoNode*);       //pops first list element
// unsigned int list_remove(myPatient* patient);
void list_print(infoNode*);
int list_returnsize(infoNode*);
void list_destroy(infoNode*);
void list_stats(infoNode*);
myPatient* list_search(infoNode*, char*);


///// Functs

int record_valid(myPatient*, myPatient*);


