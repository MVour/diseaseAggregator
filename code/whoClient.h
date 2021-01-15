#include "myThreads.h"
#include "mssgHandling.h"

typedef struct passedInfo pInfo;

struct passedInfo{
	char* line;
	char* ip;
	int pNum;
	struct sockaddr_in server;
};

// pInfo* pInfo_Init();
// void pInfo_Set(pInfo*, char*, char*, int);
// pInfo* pInfo_Copy(pInfo*);
// void pInfo_Destroy(pInfo*);

char** read_file(char* myfile, int*);
int passCommands(char**, int, int, int);
void setDone(int);

// void call_END(myThreads*);

void* clientThread_Start();
int create_threads(myThreads*, int, void*, char**, int, char*, int, int);

void myThreads_StartAll(int);
void thread_readCommand();
void destroyMutex();
void condInit();
void condDestroy();
// void runAll(myThreads*, int, char**, int, char*, int);

void signal_handler(int);