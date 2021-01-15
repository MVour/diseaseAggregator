#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h> 

#include "whoServer.h"

int main(int argc, char* argv[]){

// ./whoServer –q queryPortNum -s statisticsPortNum –w numThreads –b bufferSize	
	// ~~~~~~~~~~~~~~~~~~ SET SINGAL
	signal(2, signal_handler);


	// ~~~~~~~~~~~~~~~~~~ READ ARGS

	int i = 0;

	int qPNum = 0;
	int sPNum = 0;
	int tNum = 0;
	int bSize = 0;

	while(i < argc){
		// printf("arg[%d]: %s\n", i, argv[i]);

		if(strcmp(argv[i], "-w") == 0){
			sscanf(argv[++i], "%d", &tNum);
		}

		else if(strcmp(argv[i], "-q") == 0){
			sscanf(argv[++i], "%d", &qPNum);
		}

		else if(strcmp(argv[i], "-s") == 0){
			sscanf(argv[++i], "%d", &sPNum);
		}

		else if(strcmp(argv[i], "-b") == 0){
			sscanf(argv[++i], "%d", &bSize);
		}

		i++;
	}

	// ~~~~~~~~~~~~~~~~~ MAKE BUFFER
	RB* myRB = ringBuffer_Init(bSize);

	// ~~~~~~~~~~~~~~~~~ MAKE THREADS
	myThreads* threads = myThreads_Init(tNum); 
	create_threads(threads, tNum, &serverThread_Start, myRB);

	runAll(myRB, qPNum, sPNum, threads);


	// ~~~~~~~~~~~~~~~~~ CLEAN MEM
	// myThreads_Destroy(threads);
	ringBuffer_Destroy(myRB);
	clean_up();
}


void signal_handler(int singnum){
	signal(2, signal_handler);
	set_END();
}