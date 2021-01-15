#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "whoClient.h"


pthread_mutex_t prints_mtx = PTHREAD_MUTEX_INITIALIZER;

int GO = 0;
// int HERE = 0;
// pthread_mutex_t here_mtx = PTHREAD_MUTEX_INITIALIZER;

pthread_barrier_t here_bar;

// int END = 0;

char** read_file(char* myfile, int* linesNum){
	
	char* target = malloc(strlen(myfile) + 9);
	memset(target, 0, 9 + strlen(myfile));

	strcat(target, "../data/");
	strcat(target, myfile);

	FILE* fp = NULL;

	fp = fopen(target, "r");
	if(fp == NULL){
		printf("Cant Open File !!\n");
		free(target);
		return NULL;
	}


	char** lines = NULL;
	char buffer[200];
	int totalLines = 0;

	while(fgets(buffer, 200, fp) != NULL){
		char** tempArray = realloc(lines, (totalLines+1)*sizeof(char*));
		lines = tempArray;

		lines[totalLines] = strndup(buffer, strlen(buffer)-1);
		// printf("line: %s\n", lines[totalLines]);
		totalLines++;
	}

	fclose(fp);
	free(target);

	*linesNum = totalLines;
	return lines;
}


void* clientThread_Start(void* info){
	// printf("HELLLO, %lu,\n", pthread_self());

	pInfo* myInfo = (pInfo*) info;
	
	struct sockaddr *serverptr = (struct sockaddr*) &(myInfo->server);

	pthread_barrier_wait(&here_bar);

	int s = socket(AF_INET, SOCK_STREAM, 0);
	while(connect(s, serverptr, sizeof(myInfo->server)) <0){
		// printf("error\n");
		// return -1;
	}

	sendMssg_viaSocketExisting(s, myInfo->line);
	// int s = sendMssg_viaSocket(myInfo->pNum, myInfo->ip, myInfo->line);

	if(s > 0){
		char* anwser = NULL;
		anwser = getMssg_viaSocket(s);


		pthread_mutex_lock(&prints_mtx);

		printf("\n\t~~~~~~~~~~~~~~~\n");
		printf("Question:\n\t%s\n",myInfo->line);
		printf("Anwser:\n\t%s\n", anwser);
		free(anwser);
		

		pthread_mutex_unlock(&prints_mtx);

		close(s);

	}

	free(myInfo->line);
	free(myInfo->ip);
	free(myInfo);

	pthread_exit(NULL);
}

void myThreads_StopAll(){

}


int create_threads(myThreads* threads, int tNum, void* myfunc, char** lines, int lNum, char* IP, int pNum , int cur){
	if(tNum <= lNum - cur)
		pthread_barrier_init(&here_bar, NULL, tNum);
	else
		pthread_barrier_init(&here_bar, NULL, (lNum - cur)%tNum);

	struct sockaddr_in server;
	struct hostent *rem = NULL;
	if((rem = gethostbyname(IP)) == NULL ){
		return -1;
	}

	server.sin_family = AF_INET;
	memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
	server.sin_port = htons(pNum);

	int i = 0;

	while(i < tNum){
		if(cur >= lNum)
			break;

		pInfo* myInfo = malloc(sizeof(pInfo));
		myInfo->line = strdup(lines[cur]);
		myInfo->ip = strdup(IP);
		myInfo->pNum = pNum;
		myInfo->server = server;

		// char* to_pass = strdup(lines[cur]);

		pthread_create(&threads->t_Nums[i], NULL, myfunc, myInfo);
		// pthread_create(&threads->t_Nums[i], NULL, myfunc, to_pass);
		threads->count++;
		cur++;
		// printf("mpla\n");
		i++;
	}

	myThreads_Destroy(threads);
	pthread_barrier_destroy(&here_bar);

	return cur;
}

void condInit(){
	// printf("mphke\n");
	// pthread_cond_init(&c_var, NULL);
}

void condDestroy(){
	// printf("mphke\n");
	// pthread_cond_destroy(&c_var);
}

void destroyMutex(){
	// printf("mphke\n");
	
	pthread_mutex_destroy(&prints_mtx);
}
