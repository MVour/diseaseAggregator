#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ringBuffer.h"

RB* ringBuffer_Init(int Len){
	RB* newRB = malloc(sizeof(RB));

	newRB->buffer = malloc(Len*sizeof(info*));
	int i = 0;
	while(i < Len){
		newRB->buffer[i] = info_Init();
		i++;
	}

	newRB->len = Len;
	newRB->indexes = 0;
	newRB->readIndex = -1;
	newRB->writeIndex = -1;
	newRB->totalInputs = 0;

	return newRB;
}

void ringBuffer_Destroy(RB* myRB){
	printf("Destroing buffer ... total inputs: %d\n", myRB->totalInputs);
	int i = 0;
	while(i < myRB->len){
		info_Destroy(myRB->buffer[i++]);
	}

	free(myRB->buffer);
	free(myRB);
}

info* info_Init(){
	info* newInfo = malloc(sizeof(info));

	newInfo->ip = NULL;
	newInfo->port = -1;
	newInfo->target = NULL;
	newInfo->fd = -1;

	return newInfo;
}

void info_Set(info* myInfo, char* IP, unsigned int port, char* Target, int fd){
	if(myInfo->ip != NULL)
		free(myInfo->ip);
	
	
	if(myInfo->target != NULL)
		free(myInfo->target);

	myInfo->ip = strndup(IP, strlen(IP));
	myInfo->port = port;
	myInfo->target = strdup(Target);
	myInfo->fd = fd;
}

void info_Destroy(info* myInfo){
	if(myInfo == NULL)
		return;
	if(myInfo->ip != NULL)
		free(myInfo->ip);

	if(myInfo->target != NULL)
		free(myInfo->target);

	free(myInfo);
}

int isFull(RB* myRB){
	// printf("indexes: %d, len: %d\n", myRB->indexes, myRB->len);
	if(myRB->indexes == myRB->len){
		// printf("EINAI FOULL\n");
		return 1;
	}
	else
		return 0;
}

int isEmpty(RB* myRB){
	if(myRB->indexes == 0)
		return 1;
	else
		return 0;
}

info* getItem(RB* myRB){
	if(myRB->indexes == 0)
		return NULL;

	myRB->indexes--;
	myRB->readIndex = (myRB->readIndex + 1) % myRB->len;

	info* copyInfo = info_Copy(myRB->buffer[myRB->readIndex]);

	return copyInfo;
}

info* info_Copy(info* to_copy){
	if(to_copy == NULL)
		return NULL;

	info* newInfo = info_Init();

	newInfo->fd = to_copy->fd;
	newInfo->ip = strdup(to_copy->ip);
	newInfo->port = to_copy->port;
	newInfo->target = strdup(to_copy->target);

	return newInfo;
}

int placeItem(RB* myRB, info* item){
	// printf("isFull: %d, indexes: %d, len: %d\n", isFull(myRB), myRB->indexes, myRB->len);
	if(isFull(myRB) == 1)
		return -1;

	myRB->writeIndex = (myRB->writeIndex + 1) % myRB->len;
	myRB->indexes++;
	myRB->totalInputs++;
	info_Set(myRB->buffer[myRB->writeIndex], item->ip, item->port, item->target, item->fd);

	return 1;
}

void RB_printSize(RB* myRB){
	printf("RB_Indexes: %d / %d\n", myRB->indexes, myRB->len);
}

void RB_printInfo(RB* myRB){
	RB_printSize(myRB);
	printf("totalIndexes: %d\n", myRB->totalInputs);
	int i = 0;
	while(i < myRB->len){
		printf("myRB->buffer[%d] ~ fd: %d, info: %s\n", i, myRB->buffer[i]->fd, myRB->buffer[i]->target);
		i++;
	}

}