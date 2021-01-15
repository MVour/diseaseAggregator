#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "myPatient.h"

////////////// Patient

myPatient* patient_create(char* id, char* fName, char* lName, char* diseaseID, char* country, char* entrDate, char* age){
    // printf("phra: %s %s %s %s %s %s %s\n", id, fName, lName, diseaseID, country, entrDate, exitDate);
    myPatient* patient = malloc(sizeof(myPatient));

    patient->id = strdup(id);
    patient->fName = strdup(fName);
    patient->lName = strdup(lName);
    patient->diseaseID = strdup(diseaseID);
    patient->country = strdup(country);
    sscanf(age, "%d", &patient->age);

    patient->entrDate = strdup(entrDate);

    patient->exitDate = NULL;
    
    // if(cmpDate(entrDate, patient->exitDate) == 0){
    //     // printf("Record: %s Denied ~ Bad Date\n", id);
    //     patient_destroy(patient);
    //     return NULL;
    // }


    return patient;
}

void patient_destroy(myPatient *patient){
    free(patient->id);
    free(patient->fName);
    free(patient->lName);
    free(patient->diseaseID);
    free(patient->country);
    free(patient->entrDate);
    if(patient->exitDate != NULL)
        free(patient->exitDate);

    free(patient);
}


void patient_addExitDate(myPatient* p, char* d2){
    
    if(cmpDate(p->entrDate, d2) == 0){
        // printf("error");
        printf(" Error ~ Invalid Exit_Date || Bad Input\n");
        return;
    }

    p->exitDate = strdup(d2);

    printf("Record updated\n");

}



int cmpDate(char* D1, char* D2){
    char* d1 = strdup(D1);
    // printf("D1: %s, D2: %s\n",D1, D2);
    if(D2 == NULL){
        free(d1);
        return 1;
    }
    char* d2 = strdup(D2);

    int dd1, dd2, mm1, mm2, yy1, yy2;

    sscanf(strtok(d1, "-\n"), "%d", &dd1);
    sscanf(strtok(NULL, "-\n"), "%d", &mm1);
    sscanf(strtok(NULL, "-\n"), "%d", &yy1);

    sscanf(strtok(d2, "-\n"), "%d", &dd2);
    sscanf(strtok(NULL, "-\n"), "%d", &mm2);
    sscanf(strtok(NULL, "-\n"), "%d", &yy2);

    int flag = 1;   // 1 ~ NO SWAP || 0 ~ SWAP ||||||||||| 1 ~ D1 is older

    if(yy1 > yy2){      //  SMALLER IS OLDER
        flag = 0;
    }
    else if(yy1 == yy2){
        if(mm1 > mm2){
            flag = 0;
        }
        else if(mm1 == mm2){
            if(dd1 > dd2){
                flag = 0;
            }
            else if(dd1 == dd2){    // IDIO DATE
                flag = 3;
            }
        }
    }


    free(d1);
    free(d2);

    return flag;
}

char* pat_toString(myPatient* pat){

    char Age[10];
    memset(Age, 0, 10);
    sprintf(Age, "%d", pat->age);

    int len = strlen(pat->id) + strlen(pat->fName) \
                + strlen(pat->lName) + strlen(pat->diseaseID) \
                + strlen(pat->entrDate) \
                + strlen(Age) + 6;

    if(pat->exitDate != NULL){
        len += strlen(pat->exitDate) + 1;
    }

    char* to_give = malloc(len+1);
    memset(to_give, 0, len+1);

    strcat(to_give, pat->id);
    strcat(to_give, " ");

    strcat(to_give, pat->fName);
    strcat(to_give, " ");

    strcat(to_give, pat->lName);
    strcat(to_give, " ");

    strcat(to_give, pat->diseaseID);
    strcat(to_give, " ");

    strcat(to_give, Age);
    strcat(to_give, " ");

    strcat(to_give, pat->entrDate);
    strcat(to_give, " ");

    if(pat->exitDate != NULL){
        strcat(to_give, pat->exitDate);
        strcat(to_give, " ");
    }

    return to_give;

}