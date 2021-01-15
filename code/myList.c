#include <stdio.h>
#include <stdlib.h>    
#include "myList.h"
#include <string.h>

infoNode* list_create(){
    infoNode* new_list = malloc(sizeof(struct infoNode));
    new_list->head = NULL;
    new_list->size = 0;
    return new_list;
}

list_node* list_add(infoNode* mylist, myPatient* Patient){
    list_node* to_insert = malloc(sizeof(list_node));
    to_insert->patient = Patient;

    if(mylist->head == NULL){
        to_insert->next = NULL;
        mylist->head = to_insert;
    }
    else{
        int check = cmpDate(mylist->head->patient->entrDate, Patient->entrDate);
                                            //// ginetai etsi epeidh kanei kai sort parallhla
                                            // alliws mporouse apla me id==id? CHILL

        if(check == 0){     // 0 ~ FIRST ARG IS SMALLER -> SWAP
            to_insert->next = mylist->head;
            mylist->head = to_insert;
        }
        else{
            if(check == 3){
                if(record_valid(mylist->head->patient, Patient) == -1){
                    free(to_insert);
                    return NULL;
                }
            }
            list_node* temp = mylist->head;
            while(temp->next != NULL){
                check = cmpDate(temp->next->patient->entrDate, Patient->entrDate);
                if(check == 0){
                    break;
                }
                if(check == 3){
                    if(record_valid(temp->next->patient, Patient) == -1){
                        free(to_insert);
                        return NULL;
                    }
                }
                temp = temp->next;
            }
            to_insert->next = temp->next;
            temp->next = to_insert;
        }
    }

    mylist->size++;
    return to_insert;
}

myPatient* list_pop(infoNode* mylist){
    if (mylist->size == 0)
        return NULL;
    
    list_node* newHead = mylist->head->next;
    myPatient* patient = (mylist->head)->patient;
    free(mylist->head);
    mylist->head = newHead;
    mylist->size --;

    return patient;
}

void list_print(infoNode* mylist){
    list_node* printer;
    printer = mylist->head;
    printf("Im printing patient_list\n");
    while (printer != NULL){
        printf("%s \n", (printer->patient)->fName);
        printer = printer->next;
    }
}

int list_returnsize(infoNode* mylist){
    if (mylist == NULL){
        return -1;
    }
    return mylist->size;
}

void list_destroy(infoNode* l){
    myPatient* pop = list_pop(l);
    while(pop != NULL){
        patient_destroy(pop);
        pop = list_pop(l);
    }
    free(l);
}

void list_stats(infoNode* l){
    printf(">>>>> List Stats:\n");
    printf(">\tList Size: %d\n", l->size);
    printf(">\tList Head: %s\n", l->head->patient->fName);
    printf(">>>>>\n");
}

myPatient* list_search(infoNode* mylist, char* id){

    if(mylist->head == NULL)
        return NULL;


    list_node* temp = mylist->head; 
    
    while(temp != NULL){

        myPatient* tempPat = temp->patient;
        // printf("tempPat: %s, target: %s\n", tempPat->id, id);
        if(strcmp(tempPat->id, id) == 0){
            return tempPat;
        }
        temp = temp->next;
    }
    return NULL;
}

////////////// Functs


int record_valid(myPatient* p1, myPatient* p2){

    if(strcmp(p1->id, p2->id) == 0){
        return -1;
    }

    return 0;

}

