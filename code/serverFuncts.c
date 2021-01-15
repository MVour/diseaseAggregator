#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
// #include <poll.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h> /* sockets */
#include <netdb.h> /* gethostbyaddr */
#include <ctype.h> /* toupper */

#include "whoServer.h"


#define MYIP "127.0.0.1"
#define BSIZE 200

int END_ALL = 0;

StatsList* myStats = NULL;
pthread_mutex_t Stats_mtx = PTHREAD_MUTEX_INITIALIZER;

myWorkers* Slaves = NULL;
pthread_mutex_t Slaves_mtx = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t ConnectingSlaves_mtx = PTHREAD_MUTEX_INITIALIZER;


pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t useBuf = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t prints_mtx = PTHREAD_MUTEX_INITIALIZER;

// pthread_cond_t c_var = PTHREAD_COND_INITIALIZER;
pthread_cond_t bufnotempty = PTHREAD_COND_INITIALIZER;

int EMPTY = 0;

// int GO = 0;

void* serverThread_Start(void* rb){
	RB* myRB = (RB*) rb;

	while(1){
		info* myInfo = NULL;

		pthread_mutex_lock(&mtx);

		pthread_cond_wait(&bufnotempty, &mtx);
		if(END_ALL == 1){
			pthread_mutex_unlock(&mtx);
			break;
		}
		char* mssg = NULL;

		pthread_mutex_lock(&useBuf);
		myInfo = getItem(myRB);
		pthread_mutex_unlock(&useBuf);

		pthread_mutex_unlock(&mtx);

		if(myInfo == NULL){
			continue;
		}

		if(myInfo->fd != -1){
			mssg = getMssg_viaSocket(myInfo->fd);
			if(mssg == NULL)
				continue;
		}

		if(strcmp(myInfo->target, "worker") == 0){
			char* wPort = NULL;
			unsigned int wport = -1;

			wPort = strtok(mssg, "/");
			sscanf(wPort, "%u", &wport);

				// ```` UPDATE STATS
			pthread_mutex_lock(&Stats_mtx);
			StatsList_byString(myStats, strtok(NULL, "\n"));
			pthread_mutex_unlock(&Stats_mtx);

				// ```` UPDATE WORKERS LIST
			pthread_mutex_lock(&Slaves_mtx);
			if(!workers_Exist(Slaves, wport)){
				workers_Add(Slaves, myInfo->ip, wport, myInfo->fd);

				// workers_Print(Slaves);

				pthread_mutex_lock(&prints_mtx);
				printf("Workers Connected: %d\n", Slaves->count);
				pthread_mutex_unlock(&prints_mtx);
			}
			pthread_mutex_unlock(&Slaves_mtx);
		
			close(myInfo->fd);

		}
		else{

			char* qHolder = strdup(mssg);
			char* anwser = newQuestion(mssg);

			pthread_mutex_lock(&prints_mtx);
			printf("\t~~~~~~~~~~~~~~~\n");
			printf("Question:\n%s\n", qHolder);
			
			if(anwser == NULL)
				anwser = strdup("NONE");

			printf("Anwser:\n\t%s\n", anwser);
			
			pthread_mutex_unlock(&prints_mtx);
			
			
			
			// SEND ANWSER TO CLIENT
			sendMssg_viaSocketExisting(myInfo->fd, anwser);
			free(anwser);
			if(qHolder != NULL)
				free(qHolder);

			char* endstr = NULL;
			endstr = getMssg_viaSocket(myInfo->fd);

			free(endstr);

			// printf("////// FUNNY STATS //////////\n\n");
			// RB_printInfo(myRB);
			// printf("\n/////////////////////////////\n");

			close(myInfo->fd); // mono gia twra meta prepei na meinie anoixto gia apanhsh
		}

		info_Destroy(myInfo);
		if(mssg != NULL)
			free(mssg);

	}

	pthread_exit((void*)NULL);
}

void myThreads_StopAll(){

}

void thread_readCommand(){

}

void create_threads(myThreads* threads, int tNum, void* myfunc, RB* myRB){
	int i = 0;

	while(i < tNum){
		pthread_create(&threads->t_Nums[i], NULL, myfunc, myRB);
		threads->count++;
		i++;
	}
}


void condDestroy(){
	// pthread_cond_destroy(&c_var);
	pthread_cond_destroy(&bufnotempty);
}

void mtxDestroy(){
	pthread_mutex_destroy(&Stats_mtx);
	pthread_mutex_destroy(&mtx);
	pthread_mutex_destroy(&prints_mtx);
	pthread_mutex_destroy(&ConnectingSlaves_mtx);
	pthread_mutex_destroy(&Slaves_mtx);
	pthread_mutex_destroy(&useBuf);
}

void goService(){
	// printf("mphkes\n");
	pthread_cond_signal(&bufnotempty);
}

void runAll(RB* myRB, int qPNum, int sPNum, myThreads* threads){

	// ~~~~~~~~~~~~~~~~~ INIT STATS - WORKERS LIST
	myStats = StatsList_init();
	Slaves = workers_Init();

	// ~~~~~~~~~~~~~~~~~ MAKE SOCKETS

										// QUERY SOCKET

	int s1 = socket(AF_INET, SOCK_STREAM , 0);

	struct sockaddr_in server1, server2;
	server1.sin_family = AF_INET;
	server1.sin_addr.s_addr = htonl(INADDR_ANY);
	server1.sin_port = htons(qPNum);

	int reuse = 1;
	if(setsockopt(s1, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(int)) == -1){
		get_out();
		myThreads_Destroy(threads);
		return;
	}
	if(setsockopt(s1, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) == -1){
		get_out();
		myThreads_Destroy(threads);
		return;
	}

										// WORKERS SOCKET

	int s2 = socket(AF_INET, SOCK_STREAM , 0);
	server2.sin_family = AF_INET;
	server2.sin_addr.s_addr = htonl(INADDR_ANY);
	server2.sin_port = htons(sPNum);

	reuse = 1;
	if(setsockopt(s2, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(int)) == -1){
		get_out();
		myThreads_Destroy(threads);
		return;
	}
	if(setsockopt(s2, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) == -1){
		get_out();
		myThreads_Destroy(threads);
		return;
	}

	if(bind(s2, (struct sockaddr *) &server2, sizeof(server2)) == -1){
		get_out();
		myThreads_Destroy(threads);
		return;
	}

	if(bind(s1, (struct sockaddr *) &server1, sizeof(server1)) == -1){
		get_out();
		myThreads_Destroy(threads);
		return;
	}
	if(listen(s2, 1) == -1){
		get_out();
		myThreads_Destroy(threads);
		return;
	}

	if(listen(s1, 1) == -1){
		get_out();
		myThreads_Destroy(threads);
		return;
	}

pthread_mutex_lock(&prints_mtx);
	printf("\n- - - - - - - - - -\n");
	printf("SERVER READY ...\n");
	printf("\tlistening for stats at port: %d\n", sPNum);
	printf("\tlistening for queues at port: %d\n", qPNum);
	printf("- - - - - - - - - -\n\n");
pthread_mutex_unlock(&prints_mtx);				
									//MAKE FD SET FOR SELECT

	fd_set readfds;
	FD_ZERO(&readfds);
    FD_SET(s1, &readfds);
    FD_SET(s2, &readfds);

	int newS = -1;

	int mpla = -1;
	if(s1>s2)
		mpla = s1;
	else
		mpla = s2;


	info* myInfo = info_Init();
	while(1){
		if(newS == -1 && END_ALL == 0){
			if(select(mpla+1, &readfds, 0, 0, 0)){
				// printf("MPHKE SELECT GMT\n");
				struct sockaddr_storage inc_adrs;
				socklen_t inc_len = (sizeof(inc_adrs));

				if(END_ALL == 1){
					// printf("AAAAA\n");
					pthread_cond_broadcast(&bufnotempty);
					break;
				}

				struct sockaddr_in* temp = NULL;

				if(FD_ISSET(s1, &readfds)){
					// printf("AAAAAAAAAAA: %d\n", s1);
					char ipbuf[20];
					memset(ipbuf, 0, 20);

					if(s1 <= 0)
						continue;
					newS = accept(s1, (struct sockaddr*)&inc_adrs, &inc_len);

					temp = (struct sockaddr_in*) &inc_adrs;

					inet_ntop( AF_INET, &(temp->sin_addr), ipbuf, 20);

					info_Set(myInfo, ipbuf, (unsigned int) ntohs(temp->sin_port), "queue", newS);
				}
				if(FD_ISSET(s2, &readfds)){

					char ipbuf[20];
					memset(ipbuf, 0, 20);

					if(s2 <= 0)
						continue;
					newS = accept(s2, (struct sockaddr*)&inc_adrs, &inc_len);

					temp = (struct sockaddr_in*)&inc_adrs;
					inet_ntop(AF_INET, &(temp->sin_addr), ipbuf, 20);

					info_Set(myInfo, ipbuf, (unsigned int) ntohs(temp->sin_port), "worker", newS);
				}
				FD_ZERO(&readfds);
			    FD_SET(s1, &readfds);
			    FD_SET(s2, &readfds);
			}
		}
		else{
			pthread_mutex_lock(&mtx);
			if(placeItem(myRB, myInfo) == 1){
				pthread_mutex_lock(&prints_mtx);
				pthread_mutex_unlock(&prints_mtx);
				newS = -1;
			}

			pthread_mutex_unlock(&mtx);
		}

		pthread_mutex_lock(&mtx);
		
		if(!isEmpty(myRB)){
			pthread_mutex_unlock(&mtx);
			goService();
		}
		else
			pthread_mutex_unlock(&mtx);

		if(END_ALL == 1){
			pthread_cond_broadcast(&bufnotempty);
			break;
		}
		
	}

	close(s1);
	close(s2);
	// printf("MALAKIA\n");
	myThreads_Destroy(threads);
}


////////////////////////////////

Worker* worker_Init(){
	Worker* newW = malloc(sizeof(Worker));

	newW->ip = NULL;
	newW->port = -1;
	newW->fd = -1;
	newW->next = NULL;

	return newW;
}

void worker_Destroy(Worker* self){
	if(self == NULL)
		return;

	free(self->ip);
	free(self);

}

myWorkers* workers_Init(){
	myWorkers* newList = malloc(sizeof(myWorkers));

	newList->head = NULL;
	newList->count = 0;

	return newList;
}

void workers_Destroy(myWorkers* self){
	if(self == NULL)
		return;

	Worker* temp = NULL;
	Worker* cur = self->head;

	while(cur != NULL){
		temp = cur->next;
		worker_Destroy(cur);
		cur = temp;
	}

	free(self);

}

void workers_Add(myWorkers* self, char* IP, unsigned int port, int fd){
	Worker* newW = worker_Init();
	newW->ip = strdup(IP);
	newW->port = port;
	newW->fd = fd;

	Worker* temp = self->head;

	if(temp != NULL){
		while(temp->next != NULL){
			temp = temp->next;
		}
		temp->next = newW;
	}
	else{
		self->head = newW;
	}

	self->count++;
	// printf("SLAVE ADDED !!!!\n");

}

int workers_Exist(myWorkers* self, unsigned int target){
	Worker* temp = self->head;
	int i = self->count;

	while(i > 0){
		if(temp->port == target)
			return 1;
		temp = temp->next;
		i--;
	}

	return 0;

}

Worker* workers_GetWorker(myWorkers* self, int target){
	if(target >= self->count)
		return NULL;

	int i = 0;
	Worker* temp = self->head;
	while(i < target){
		temp = temp->next;
		i++;
	}
	return temp;
}

void workers_Print(myWorkers* self){
	printf("Workers_count: %d\n", self->count);

	Worker* temp = self->head;

	int i = 0;
	while(i<self->count){
		printf("%d\n", i);
		printf("\tip: %s, port: %u, fd: %d\n",temp->ip, temp->port, temp->fd);
		temp = temp->next;
		i++;
	}
}

char* newQuestion(char* quest){
	pthread_mutex_lock(&ConnectingSlaves_mtx);

	char* final_anwser = NULL;
	int totalLen = 0;

	Worker* temp = NULL;

	char** anwsers = NULL;// malloc((Slaves->count)*sizeof(char*));
	int anwsers_count = 0;

	int i = 0;
	while(i < Slaves->count){
		temp = workers_GetWorker(Slaves, i);
		int newS = sendMssg_viaSocket(temp->port, temp->ip, strtok(quest, "~"));

		// printf("I just asked worker: %d\n", i);

		char* anwser = NULL;
		while(anwser == NULL){
//			printf("mpla\n");
			anwser = getMssg_viaSocket(newS);
		}
		sendMssg_viaSocketExisting(newS, "END");

		// printf("anwser: %s\n",anwser);
		char* tok = strtok(anwser, "/~");
		anwsers = realloc(anwsers, (anwsers_count+1)*sizeof(char*));
		anwsers[anwsers_count] = strdup(tok);
		totalLen += strlen(tok);
		anwsers_count++;

//		final_anwser = realloc(final_anwser, strlen(anwser));
//		strcat(final_anwser, anwser);
		free(anwser);
		close(newS);
		i++;
	}

	pthread_mutex_unlock(&ConnectingSlaves_mtx);
// printf("totalLen: %d\n", totalLen);
	final_anwser = make_pretty(strtok(quest, " "), anwsers, anwsers_count);
		
	while(anwsers_count > 0){
		free(anwsers[--anwsers_count]);
	}
	free(anwsers);

	return final_anwser;
}

void set_END(){
	END_ALL = 1;
}

void clean_up(){
	// printf("mphke\n");
	StatsList_destroy(myStats);
	workers_Destroy(Slaves);
	mtxDestroy();
	condDestroy();
}

char* make_pretty(char* tok, char** anwsers, int anwsers_count){
	char* final = NULL;

	if(strcmp(tok, "/diseaseFrequency") == 0 || strcmp(tok, "2") == 0){
		int sum = 0;
		
		char sumBuf[20];
		memset(sumBuf, 0, 20);

		int i = 0;
		while(i < anwsers_count){
			int temp = 0;
			if(strcmp(anwsers[i], "FAIL") != 0 && strcmp(anwsers[i], "NONE") != 0){
				sscanf(anwsers[i], "%d", &temp);
			}
			i++;
			sum += temp;
		}

		sprintf(sumBuf, "%d", sum);

		final = malloc(strlen(sumBuf) + 1);
		memset(final, 0, strlen(sumBuf) + 1);

		strcat(final, sumBuf);

	}

	else if(strcmp(tok, "/topk-AgeRanges") == 0 || strcmp(tok, "3") == 0){
		int i = 0;
		int fullLen = 1;
		while(i < anwsers_count){
			if(strcmp(anwsers[i], "FAIL") != 0 && strcmp(anwsers[i], "NONE") != 0){
				char* clean_anws = NULL;
				clean_anws = strtok(anwsers[i], "/~");
				while(clean_anws != NULL){

					char* hold = NULL;
					int holdsize = 0;
					if(final != NULL){
						hold = malloc(fullLen+1);
						memset(hold, 0, fullLen+1);
						strcat(hold, final);
						holdsize = fullLen;
					}

					fullLen += strlen(clean_anws) + 2;

					final = realloc(final, fullLen);
					memset(final, 0, fullLen);

					if(hold != NULL){
						strncat(final, hold, holdsize);
						free(hold);
					}
					strcat(final, clean_anws);
					strcat(final, "\n\t");

					clean_anws = strtok(NULL, "/~");
				}
			}
			i++;
		}
	}

	else if(strcmp(tok, "/searchPatientRecord") == 0 || strcmp(tok, "4") == 0){
		int i = 0;
		while(i < anwsers_count){
			// printf("PATIENT::::: %s\n", anwsers[i]);
			if(strcmp(anwsers[i], "FAIL") != 0 && strcmp(anwsers[i], "NONE") != 0){
				break;
			}
			i++;
		}

		if(i != anwsers_count){
			if(strcmp(anwsers[i], "FAIL") != 0 && strcmp(anwsers[i], "NONE") != 0){
		
				final = malloc(strlen(anwsers[i]) + 1);
				memset(final, 0, strlen(anwsers[i]) + 1);
				strcat(final, anwsers[i]);
			}
		}
	}

	else if(strcmp(tok, "/numPatientAdmissions") == 0 || strcmp(tok, "5") == 0){
		int i = 0;
		int fullLen = 1;
		while(i < anwsers_count){
			if(strcmp(anwsers[i], "FAIL") != 0 && strcmp(anwsers[i], "NONE") != 0){
				char* clean_anws = NULL;
				clean_anws = strtok(anwsers[i], "/~");
				while(clean_anws != NULL){

					char* hold = NULL;
					int holdsize = 0;
					if(final != NULL){
						hold = malloc(fullLen+1);
						memset(hold, 0, fullLen+1);
						strcat(hold, final);
						holdsize = fullLen;
					}

					fullLen += strlen(clean_anws) + 2;

					final = realloc(final, fullLen);
					memset(final, 0, fullLen);

					if(hold != NULL){
						strncat(final, hold, holdsize);
						free(hold);
					}
					strcat(final, clean_anws);
					strcat(final, "\n\t");

					clean_anws = strtok(NULL, "/~");
				}
			}
			i++;
		}
	}

	else if(strcmp(tok, "/numPatientDischarges") == 0 || strcmp(tok, "6") == 0){
		int i = 0;
		int fullLen = 0;
		while(i < anwsers_count){
			// printf("anwsers[%d]: %s\n", i, anwsers[i]);
			if(strcmp(anwsers[i], "FAIL") != 0 && strcmp(anwsers[i], "NONE") != 0){
				char* clean_anws = NULL;
				clean_anws = strtok(anwsers[i], "/~");
				while(clean_anws != NULL){
					
					char* hold = NULL;
					int holdsize = 0;
					if(final != NULL){
						char* hold = malloc(fullLen+1);
						memset(hold, 0, fullLen+1);
						strcat(hold, final);
						holdsize = fullLen;
					}

					fullLen += strlen(clean_anws) + 2;
					
					final = realloc(final, fullLen);
					memset(final, 0, fullLen);

					if(hold != NULL){
						strncat(final, hold, holdsize);
						free(hold);
					}
					strcat(final, clean_anws);
					strcat(final, "\n\t");
					
					clean_anws = strtok(NULL, "/~");
				}
			}
			i++;
		}
	}


	if(final == NULL){
		final = malloc(5);
		memset(final, 0, 5);
		strcat(final, "NONE");
	}

	return final;
}

void get_out(){
	pthread_cond_broadcast(&bufnotempty);
	END_ALL = 1;
}