#include <pthread.h>
#include "myThreads.h"
#include "ringBuffer.h"
#include "mssgHandling.h"
#include "filesStats.h"

typedef struct myWorkers myWorkers;
typedef struct Worker Worker;
// struct myThreads{
// 	pthread_t* t_Nums;
// 	int count;
// };

struct Worker{
	char* ip;
	unsigned int port;
	int fd;

	Worker* next;
};

struct myWorkers{
	Worker* head;

	int count;
};


void signal_handler(int);
void set_END();
void clean_up();
// //functs

// myThreads* myThreads_Init(int);
// void myThreads_Destroy(myThreads*);
// //////////////////

// void create_threads(myThreads*, int);
// void* thread_Start();

void* serverThread_Start();
void create_threads(myThreads*, int, void*, RB*);
void condDestroy();
void mtxDestroy();
void goService();

void runAll(RB*, int, int, myThreads*);

////////////////////////////

Worker* worker_Init();
void worker_Destroy(Worker*);


myWorkers* workers_Init();
void workers_Destroy(myWorkers*);
int workers_Exist(myWorkers*, unsigned int);
void workers_Add(myWorkers*, char*, unsigned int, int);
void workers_SendAll(myWorkers, char*);
Worker* workers_GetWorker(myWorkers*, int);
char* newQuestion(char*);
void workers_Print(myWorkers*);

char* make_pretty(char*, char**, int);

void get_out();