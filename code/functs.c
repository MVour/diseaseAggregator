 #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functs.h"


/////////////// FUNCTS


void printMenu(){
    printf("\n~~~~~~~~~~~~~ CONTROLS MENU ~~~~~~~~~~~~~\n");
    printf("\t 1. > globalDiseaseStats [date1 date2]\n");
    printf("\t 2. > diseaseFrequency virusName [country] date1 date2\n");
    printf("\t 3. > topk-Diseases k country [date1 date2]\n");
    printf("\t 4. > topk-Countries k disease [date1 date2]\n");
    printf("\t 5. > insertPatientRecord recordID patientFirstName patientLastName diseaseID country entryDate [exitDate]\n");
    printf("\t 6. > recordPatientExit recordID\n");
    printf("\t 7. > numCurrentPatients [disease]\n");
    printf("\t 0. > exit\n");
    printf("\t~~~~~~ SOME EXTRAS:\n");
    printf("\t 8. > Test Prints for Tables and Lists !!\n");
    printf("\n");
}

char* act(int i, hashTable* cHash, hashTable* dHash, infoNode* pList, StatsList* myStats, char* buf){
    char* tok = buf;

    char* anwser = NULL;

    if(i == 1){
        
        // char* date1 = NULL;
        // char* date2 = NULL;
        
        // if(tok != NULL){

        //     date1 = strtok(tok, " \n");
        //     date2 = strtok(NULL, " \n");
        //     if(date2 == NULL){
        //         // printf("\tBad Input - Non Valid Date2 !!\n");
        //         printf("error\n");
        //         return NULL;
        //     }
        // }

        // act_1(dHash, date1, date2);
    }
    else if(i == 2){

        char* date1 = NULL;
        char* date2 = NULL;
        char* virus = NULL;
        char* country = NULL;
        
        if(tok != NULL){

            // virus = strdup(strtok(tok, " "));
            // date1 = strdup(strtok(NULL, " "));
            // date2 = strdup(strtok(NULL, " \n"));
            
            virus = strtok(tok, " ");
            date1 = strtok(NULL, " ");
            date2 = strtok(NULL, " ");

            if(virus == NULL || date1 == NULL || date2 == NULL)
                return NULL;

            // char* mpe = NULL;
            country = strtok(NULL, " ~\n");
            // if(mpe != NULL)
            //     country = strdup(mpe);

        }
        else{
            printf("error\n");
            // printf("\tBad Input - Try Again !!\n");
            return NULL;
        }

        anwser = act_2(dHash, date1, date2, virus, country);

        // free(virus);
        // if(country != NULL)
        //     free(country);
        // free(date1);
        // free(date2);
    }

    else  if(i == 3){
        // printf("tok: %s\n", tok);

        int k;
        // char* K;
        // memset(K, 0, 10);

        char* country = NULL;
        char* disease = NULL;
        char* date1 = NULL;
        char* date2 = NULL;

        if(tok == NULL){
            return NULL;
        }

        sscanf(strtok(tok, " \n"), "%d", &k);
        country = strtok(NULL, " \n");
        disease = strtok(NULL, " \n");
        date1 = strtok(NULL, " \n");
        date2 = strtok(NULL, "~ \n");

        if(country == NULL || disease == NULL || date1 == NULL || date2 == NULL )
            return NULL;

        anwser = act_3(myStats, k, country, disease, date1, date2);

    }

    else if(i == 4){

        char* id = strtok(tok, "~ ");
        if(id == NULL)
            return NULL;

        anwser = act_4(pList, id);

    }

    else if(i == 5 || i == 6){

        char* date1 = NULL;
        char* date2 = NULL;
        char* virus = NULL;
        char* country = NULL;
        
        if(tok != NULL){

            virus = strtok(tok, " ");
            date1 = strtok(NULL, " ");
            date2 = strtok(NULL, " \n");
            country = strtok(NULL, " ~\n");

        }
        else{
            // printf("error\n");
            // printf("\tBad Input - Try Again !!\n");
            return NULL;
        }

        if(virus == NULL || date1 == NULL || date2 == NULL ){
            // printf("mphkes dew\n");
            return NULL;
        }

        if(i == 5)
            anwser = act_5(myStats, virus, date1, date2, country);
        else
            anwser = act_6(dHash, virus, date1, date2, country);


    }

    else{
        // printf("\tBad Input - Try Again !!\n");
        printf("error\n");
    }

    return anwser;
    // printf("\n");
}




char* act_2(hashTable* dHash, char* date1, char* date2, char* virus, char* country){

    // printf("virus: %s, d1: %s, d2: %s, country: %s\n", virus, date1, date2, country);

    int targetCell = hash1(virus) % dHash->tableSize;
    int found = 0;

    record* tempRec = NULL;

    bucket* tempBuc = dHash->myTable[targetCell];        // FIND VIRUS AT D HASH
    while(tempBuc != NULL){
        tempRec = tempBuc->rec;
        while(tempRec != NULL && found == 0){
            if(strcmp(tempRec->key, virus) == 0){
                found = 1;
                break;
            }
            tempRec = tempRec->next;
        }
        if(found == 1)
            break;
        tempBuc = tempBuc->next;

    }

    int num = RBT_countMiddleKeys(tempRec->myTree, date1, date2, tempRec->myTree->root, country);
    int digits = 1;
    int temp = num;

    while(temp /10 != 0){
        digits++;
        temp = temp/10;
        // printf("ACT2 TEMP: %d\n", temp);
    }
    char* anwser = malloc(digits+1);
    memset(anwser, 0, digits+1);
    sprintf(anwser, "%d", num);

    return anwser;

}

char* act_3(StatsList*  myStats, int k, char* country, char* disease, char* date1, char* date2){

    maxHeap* myHeap = maxHeap_create("age", 0);
    int totalEntries = statsToHeap(myStats, myHeap, country, disease, date1, date2);

    // printf("totalEntries: %d\n", totalEntries);
    if(totalEntries == 0){
        maxHeap_destroy(myHeap);
        return NULL;
    }
// maxHeap_destroy(myHeap);
// return NULL;
    char** statsArray = malloc(k*sizeof(char*));
    int len = 0;

    int count = 0;
    while(count < k){
        if(count == totalEntries){
            break;
        }

        heapNode* temp = maxHeap_pop(myHeap);
        if(temp != NULL){
            len += strlen(temp->key)+10;
            statsArray[count] = malloc(len);
            memset(statsArray[count], 0, len);

            strcat(statsArray[count], temp->key);
            double percent = ((double)temp->value/(double)totalEntries) *100;

            strcat(statsArray[count], ": ");

            char Percent[8];
            memset(Percent, 0, 8);
            sprintf(Percent, "%.2f", percent);

            strncat(statsArray[count], Percent, strlen(Percent));

            strcat(statsArray[count], "%");
        //     // printf("statsArray[%d]: %s, key: %s, value: %d, percent: %f, Per: %s, custom:%f ~ %f\n", count, statsArray[count], temp->key, temp->value, percent, Percent, temp->value/totalEntries, temp->value/totalEntries*100);


            heapNode_destroy(temp);

            count++;
        }
        else{
            break;
        }
    }

    maxHeap_destroy(myHeap);

    char* bigMssg = malloc(len+(2*count));
    memset(bigMssg, 0, len+(2*count));

    int cell = 0;
    while(cell < count){
        strncat(bigMssg, statsArray[cell], strlen(statsArray[cell]));
        strcat(bigMssg, "/");
        free(statsArray[cell]);
        cell++;
    }

    free(statsArray);

    return bigMssg;

}

char* act_4(infoNode* pList, char* id){

    list_node* tempNode = pList->head;
    int count = 0;
    while(count < pList->size){
        myPatient* tempPat = tempNode->patient;

        // printf("cmp: %s, %s, count: %d\n", tempPat->id, id, count);
        if(strcmp(tempPat->id, id) == 0){
            return pat_toString(tempPat);
        }
        tempNode = tempNode->next;
        count ++;
    }
    char* re = malloc(5);
    memset(re, 0, 5);
    strcat(re, "NONE");
    return re;

}

char* act_5(StatsList* myStats, char* virus, char* date1, char* date2, char* country){


    char*** DArray = NULL;
    char*** tempDArray = NULL;
    int countEntries = 0;
    int len = 0;

    dateStats* tempDate = myStats->head;
    while(tempDate != NULL){
        diseaseNode* tempDisease = tempDate->dList;

        int comp1 = cmpDate(date1, tempDate->date);
        int comp2 = cmpDate(date2, tempDate->date);
        if( comp1 == 0 || comp2 == 1){
            tempDate = tempDate->next;
            continue;
        }


        if(country != NULL){
            if(strcmp(tempDate->country, country) != 0){
                tempDate = tempDate->next;
                continue;
            }
        }

        while(tempDisease != NULL){
            if(strcmp(tempDisease->disease, virus) == 0){
                break;
            }
            tempDisease = tempDisease->next;            
        }

        if(tempDisease == NULL){
            tempDate = tempDate->next;
            continue;
        }


        char Num[10];
        memset(Num, 0, 10);
        sprintf(Num, "%d", tempDisease->pat_num);

        int i = 0;
        int flag = 0;
        while(i < countEntries){
            if(strcmp(DArray[i][0], tempDate->country) == 0){
                flag = 1;
                break;
            }
            i++;
        }

        if(flag == 1){
            int myNum = 0;
            sscanf(DArray[i][1], "%d", &myNum);

            int tempNum = 0;
            sscanf(Num, "%d", &tempNum);
            
            tempNum += myNum;

            char newNum[10];
            memset(newNum, 0, 10);
            sprintf(newNum, "%d", tempNum);

            free(DArray[i][1]);
            DArray[i][1] = strndup(newNum, strlen(newNum));
        }



        else{
            tempDArray = realloc(DArray, (countEntries+1)*sizeof(char**));
            if(tempDArray == NULL){
                printf("ERROR at reallocing !!\n");
                return NULL;
            }

            DArray = tempDArray;
            DArray[countEntries] = malloc(2*sizeof(char*));

            DArray[countEntries][0] = strdup(tempDate->country);
            DArray[countEntries][1] = strndup(Num, strlen(Num));

            len += strlen(DArray[countEntries][0]);
            len += strlen(DArray[countEntries][1]);
            len += 3;
            countEntries++;
        }

        
        tempDate = tempDate->next;
    }

    char* bigMssg = malloc(len+1);
    memset(bigMssg, 0, len+1);
    int count = 0;
    while(count < countEntries){
        strcat(bigMssg, DArray[count][0]);
        strcat(bigMssg, " ");
        strcat(bigMssg, DArray[count][1]);
        strcat(bigMssg, "/");

        free(DArray[count][0]);
        free(DArray[count][1]);
        free(DArray[count]);

        count++;
    }
    free(DArray);

    return bigMssg;
}

char* act_6(hashTable* dHash, char* virus, char* date1, char* date2, char* country){
// printf("act6\n");
    int targetCell = hash1(virus) % dHash->tableSize;

    bucket* tempBuc = dHash->myTable[targetCell];
    record* tempRec = NULL;

    int found = -1;
    while(tempBuc != NULL){
        tempRec = tempBuc->rec;

        while(tempRec != NULL){
            if(strcmp(tempRec->key, virus) == 0){
                found = 1;
                break;
            }

            tempRec = tempRec->next;
        }
        if(found == 1)
            break;
        tempBuc = tempBuc->next;
    }

    maxHeap* myHeap = maxHeap_create("country", tempRec->entries);

    if(tempRec->myTree->root == NULL){
        // printf("\terror !!\n");
        return NULL;
    }

    treeToHeap(tempRec->myTree, myHeap, tempRec->myTree->root, date1, date2, country);

    if(maxHeap_Empty(myHeap)){
        // printf("edw\n");
        maxHeap_destroy(myHeap);
        char* mssg = malloc(2);
        memset(mssg, 0, 2);
        strcat(mssg, "0");
        return mssg;
    }
    int countEntries = 0;
    int len = 0;
    char*** CArray = malloc(myHeap->entries*sizeof(char**));
    while(!maxHeap_Empty(myHeap)){
        heapNode* temp = maxHeap_pop(myHeap);
        // printf("\t\t> ");
            if(temp != NULL){
                CArray[countEntries] = malloc(2*sizeof(char*));

                CArray[countEntries][0] = strdup(temp->key);

                char Num[10];
                memset(Num, 0, 10);

                sprintf(Num, "%d", temp->value);

                CArray[countEntries][1] = strndup(Num, strlen(Num));

                // printf("C: %s, %s\n", CArray[countEntries][0], CArray[countEntries][1]);

                len += strlen(CArray[countEntries][0]);
                len += strlen(CArray[countEntries][1]);
                len += 2;   // " " + "/"

                countEntries++;
                heapNode_destroy(temp);
            }
            else{
            //     maxHeap_destroy(myHeap);
                break;
            //     // printf(" ~\n");
            }
    }
    maxHeap_destroy(myHeap);

    char* bigMssg = malloc(len+1);
    memset(bigMssg, 0 ,len+1);
    while(countEntries > 0){
        strcat(bigMssg, CArray[--countEntries][0]);
        strcat(bigMssg, " ");
        strcat(bigMssg, CArray[countEntries][1]);
        strcat(bigMssg, "/");

        free(CArray[countEntries][1]);
        free(CArray[countEntries][0]);
        free(CArray[countEntries]);
    }
    free(CArray);

// printf("mssg: %s\n", bigMssg);
    return bigMssg;

}


int isDate(char* s){
    int dd;
    if(sscanf(strtok(s, " -\n"), "%d", &dd) == 0){
        // printf("gonna return -1\n");
        return -1;
    }
    // printf("gonna return 1\n");
    return 1;
}

void treeToHeap(RBT* tree, maxHeap* heap, RBnode* n, char* date1, char* date2, char* extraK){
    // printf("act: %d\n", act);
    if(n == NULL)
        return;

    if(n->lNode->patient->exitDate != NULL){
        int comp1 = cmpDate(date1, n->lNode->patient->exitDate);
        int comp2 = cmpDate(date2, n->lNode->patient->exitDate);

        if( comp1 != 0 && comp2 != 1){
            if(extraK != NULL){
                if(strcmp(n->lNode->patient->country, extraK) == 0)
                    maxHeap_add(heap, n->lNode->patient->country, 1);
            }
            else
                maxHeap_add(heap, n->lNode->patient->country, 1);
        }
        if(n->childr != NULL){
            treeToHeap(tree, heap, n->childr, date1, date2, extraK);
        }
        if(n->childl != NULL){
            treeToHeap(tree, heap, n->childl, date1, date2, extraK);
        }
    }
    else{
        // maxHeap_add(heap, n->lNode->patient->country, 1);

        if(n->childl != NULL)
            treeToHeap(tree, heap, n->childl, date1, date2, extraK);
        if(n->childr != NULL)
            treeToHeap(tree, heap, n->childr, date1, date2, extraK);
    }
}

int statsToHeap(StatsList* myStats, maxHeap* myHeap, char* country, char* disease, char* date1, char* date2){
    int totalEntries = 0;

    dateStats* tempDate = myStats->head;
    while(tempDate != NULL){

        if(strcmp(tempDate->country, country) != 0){
            tempDate = tempDate->next;
            continue;
        }

// printf("date1: %s, date2: %s\n", date1, date2);
        int comp1 = cmpDate(date1, tempDate->date);
        int comp2 = cmpDate(date2, tempDate->date);
        if( comp1 == 0 || comp2 == 1){
            tempDate = tempDate->next;
            continue;
        }
        
        diseaseNode* tempDisease = tempDate->dList;
        while(tempDisease != NULL){
            if(strcmp(tempDisease->disease, disease) == 0){
                ageNode* target = tempDisease->ageStats;
                

                maxHeap_add(myHeap, "0-20", target->group1);
                maxHeap_add(myHeap, "21-40", target->group2);
                maxHeap_add(myHeap, "41-60", target->group3);
                maxHeap_add(myHeap, "60+", target->group4);

                totalEntries += tempDisease->pat_num;
            }
            tempDisease = tempDisease->next;
        }
        tempDate = tempDate->next;
    }
    return totalEntries;

}
