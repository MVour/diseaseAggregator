#include <pthread.h>


typedef struct myThreads myThreads;
typedef struct t_Info t_Info;

struct myThreads{
	pthread_t* t_Nums;
	int count;
};

struct t_Info{
	int id;
	char* info;
};

//functs

myThreads* myThreads_Init(int);
void myThreads_Destroy(myThreads*);
void myThreads_MASSACRE(myThreads*);
//////////////////

// void create_threads(myThreads*, int, void*);
// void myThreads_StopAll(myThreads*);
// void myThreads_StartAll(myThreads*);

//////////////////////////////////////
t_Info* make_info(int, char*);
void destroy_Info(t_Info*);
//////////////////////////////////////
void* thread_Start();