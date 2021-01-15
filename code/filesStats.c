#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filesStats.h"



StatsList* StatsList_init(){
	StatsList* newNode = malloc(sizeof(StatsList));

	newNode->head = NULL;
	newNode->datesCount = 0;

	return newNode;
}

void StatsList_destroy(StatsList* self){
	if(self == NULL)
		return;
	dateStats_destroy(self->head);
	free(self);
}

void StatsList_addDateRec(StatsList* self, dateStats* to_add){
	dateStats* temp = self->head;
	if(self->head == NULL){
		self->head = to_add;
	}
	else{
		while(temp->next != NULL){
			temp = temp->next;
		}
		temp->next = to_add;
	}

	self->datesCount++;
}


char* StatsList_toString(StatsList* self){

	char** StatsArray = malloc(self->datesCount*sizeof(char*));

	dateStats* temp = self->head;
	int len = 0;

	int i = 0;
	while(temp != NULL){
		StatsArray[i] = dateStats_toString(temp);
		temp = temp->next;
		len += strlen(StatsArray[i]);
		i++;
	}

	char* together = malloc(len+1+i);
	memset(together, 0, len+1+1);
	i = 0;
	while(i < self->datesCount){
		strcat(together, StatsArray[i]);
		free(StatsArray[i]);
		strcat(together, "$");
		i++;
	}
	free(StatsArray);

	return together;
}

void StatsList_print(StatsList* self){
	dateStats* temp = self->head;
	while(temp != NULL){
		dateStats_print(temp);
		temp = temp->next;
	}
}

void StatsList_byString(StatsList* self, char* buf){
	char* tempbuf = buf;
	char* line = NULL;
// printf("tempbuf: %s\n", tempbuf);
	line = strtok_r(tempbuf, "$", &tempbuf);

	// dateStats* myDate = self->head;
	dateStats* tempDate = self->head;
	if(self->head != NULL){
		while(tempDate->next != NULL){
			tempDate = tempDate->next;
		}
	}
	
	while(strcmp(line, "~") != 0){

		char* date = strtok_r(line, "/", &line);
		char* country = strtok_r(line, "/", &line);

		// printf("BY STRING date: %s country: %s\n", date, country);

		if(self->head == NULL){
			self->head = dateStats_init(date, country);
			tempDate = self->head;
		}
		else{
			tempDate->next = dateStats_init(date, country);
			tempDate = tempDate->next;
		}

		diseaseNode* tempD = tempDate->dList;
		int totalD = 0;

		char* tok = strtok_r(line, "/", &line);

		while(tok != NULL){
			// printf("BY STRING tok: %s\n", tok);

			// char* disease = strdup(tok);
			diseaseNode* myDisease = diseaseNode_init(tok);
			// free(disease);

			ageNode* myAge = ageNode_init();

			int totalEntries = 0;
			int count = 0;

			while(count < 4){
				char* group = strtok_r(line, ":", &line);
				char* Entries = strtok_r(line, "/", &line);
				int entries = 0;
				sscanf(Entries , "%d", &entries);

				if(strcmp(group, "G1") == 0){
					myAge->group1 = entries;
				}
				else if(strcmp(group, "G2") == 0){
					myAge->group2 = entries;
				}
				else if(strcmp(group, "G3") == 0){
					myAge->group3 = entries;
				}
				else{
					myAge->group4 = entries;
				}
				totalEntries += entries;
				count++;
			}
			myDisease->ageStats = myAge;
			myDisease->pat_num = totalEntries;
			totalD++;

			if(tempDate->dList == NULL){
				tempDate->dList = myDisease;
				tempD = tempDate->dList;
			}
			else{
				tempD->next = myDisease;
				tempD = tempD->next;
			}
			tok = strtok_r(line, "/", &line);
		}
		tempDate->dcount = totalD;
		line = strtok_r(tempbuf, "$", &tempbuf);
	}

}

// void StatsList_addRec(StatsList* self, char* date, char* country){
// 	dateStats* temp = self->head;
// 	if(temp == NULL){
// 		self->head = dateStats_init(date, country);
// 		return;
// 	}

// 	while(temp->next != NULL){
// 		if(strcmp(temp->date, date) == 0 && strcmp(temp->country, country) == 0){
// 			dateStats_addRec(temp, )
// 		}
// 	}
// }

//////////////////////////////////////////////////////

dateStats* dateStats_init(char* date, char* country){

	dateStats* newNode = malloc(sizeof(dateStats));

	newNode->country = strdup(country);
	newNode->date = strdup(date);
	newNode->dcount = 0;
	newNode->dList = NULL;
	newNode->next = NULL;

	return newNode;
}

void dateStats_destroy(dateStats* self){
	if(self==NULL)
		return;

	if(self->next != NULL)
		dateStats_destroy(self->next);

	free(self->date);
	free(self->country);
	if(self->dList != NULL)
		diseaseNode_destroy(self->dList);
	free(self);

}

void dateStats_addRec(dateStats* self, char* disease, char* country, int age){
	if(self->dList == NULL){
		self->dList = diseaseNode_init(disease);
		self->dcount++;
		diseaseNode_addRec(self->dList, age);
		return;
	}

	diseaseNode* tempNode = self->dList;

	while(tempNode != NULL){
		if(strcmp(tempNode->disease, disease) == 0){
			diseaseNode_addRec(tempNode, age);
			break;
		}
		if(tempNode->next == NULL){
			tempNode->next = diseaseNode_init(disease);
			self->dcount++;
			diseaseNode_addRec(tempNode->next, age);
			return;
		}
		tempNode = tempNode->next;
	}
}

void dateStats_print(dateStats* self){
	printf("%s (toatl disease %d)\n", self->date, self->dcount);
	printf("%s\n", self->country);
	diseaseNode* temp = self->dList;
	while(temp != NULL){
		diseaseNode_print(temp);
		printf("\n");
		temp = temp->next;
	}
}

char* dateStats_toString(dateStats* self){

	char* buf = NULL;
	char** stringArray = malloc(self->dcount*sizeof(char*));

	int len = 0;
	int i = 0;
	diseaseNode* temp = self->dList;

	while(i < self->dcount){
		stringArray[i] = diseaseNode_toString(temp);
		temp = temp->next;
		len += strlen(stringArray[i]);
		i++;
	}

	len += 3 + strlen(self->country) + strlen(self->date);
	buf = malloc(len);
	memset(buf, 0, len);

	strcat(buf, self->date);
	strcat(buf, "/");
	strcat(buf, self->country);
	strcat(buf, "/");

	while(i > 0){
		strcat(buf, stringArray[--i]);
		// strcat(buf, "/");
		free(stringArray[i]);
	}

	free(stringArray);

	return buf;

}


/////////////////////////////////////////////////

diseaseNode* diseaseNode_init(char* disease){
	
	diseaseNode* newNode = malloc(sizeof(diseaseNode));

	newNode->disease = strdup(disease);
	newNode->pat_num = 0;
	newNode->next = NULL;
	newNode->ageStats = NULL;
	return newNode;
}

void diseaseNode_destroy(diseaseNode* self){
	if(self->next != NULL){
		diseaseNode_destroy(self->next);
	}
	ageNode_destroy(self->ageStats);

	free(self->disease);
	free(self);
}

void diseaseNode_addRec(diseaseNode* self, int age){

	if(self->ageStats == NULL){
		self->ageStats = ageNode_init();
	}

	ageNode_addRec(self->ageStats, age);
	self->pat_num++;
}

void diseaseNode_print(diseaseNode* self){
	printf("%s (total cases: %d)\n", self->disease, self->pat_num);
	ageNode_print(self->ageStats);
}

char* diseaseNode_toString(diseaseNode* self){

	char* buf = NULL;

	char* ageString = ageNode_toString(self->ageStats);

	int len = strlen(ageString) + strlen(self->disease) + 2;
	buf = malloc(len);
	memset(buf, 0, len);

	strcat(buf, self->disease);
	strcat(buf, "/");
	strcat(buf, ageString);

	free(ageString);

	return buf;

}


/////////////////////////////////////////////////

ageNode* ageNode_init(){

	ageNode* newNode = malloc(sizeof(ageNode));

	newNode->group1 = 0;
	newNode->group2 = 0;
	newNode->group3 = 0;
	newNode->group4 = 0;

	return newNode;
}

void ageNode_addRec(ageNode* self, int age){

	if(age <= 20)
		self->group1++;
	else if(age <= 40)
		self->group2++;
	else if(age <= 60)
		self->group3++;
	else if(age <= 120)
		self->group4++;


}

void ageNode_destroy(ageNode* self){
	free(self);
}

void ageNode_print(ageNode* self){
	printf("Age range: 0-20 years: %d cases\n", self->group1);
	printf("Age range: 21-40 years: %d cases\n", self->group2);
	printf("Age range: 41-60 years: %d cases\n", self->group3);
	printf("Age range: 60+ years: %d cases\n", self->group4);
}

char* ageNode_toString(ageNode* self){

	char* buf = malloc(50);
	memset(buf, 0, 50);

	char num1[10];
	char num2[10];
	char num3[10];
	char num4[10];

	sprintf(num1, "%d", self->group1);
	strcat(buf, "G1:");
	strcat(buf, num1);


	sprintf(num2, "%d", self->group2);
	strcat(buf, "/G2:");
	strcat(buf, num2);

	sprintf(num3, "%d", self->group3);
	strcat(buf, "/G3:");
	strcat(buf, num3);

	sprintf(num4, "%d", self->group4);
	strcat(buf, "/G4:");
	strcat(buf, num4);

	strcat(buf, "/");
	// printf("ageString: %s\n", buf);

	return buf;

}