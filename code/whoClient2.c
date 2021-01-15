#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include <time.h>
#include <sys/wait.h>
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <sys/select.h>
#include <dirent.h>
#include <poll.h>
#include <errno.h>


#include "whoClient.h"

// ./whoClient –q queryFile -w numThreads –sp servPort –sip servIP

int main(int argc, char* argv[]){
	printf("hello\n");

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~ READ ARGS

	char* qFile = NULL;
	int tNum;
	int sPort;
	char* sIP = NULL;

	int i = 0;

	while(i < argc){
		if(strcmp(argv[i], "-q") == 0){
			qFile = strdup(argv[++i]);
		}
		else if(strcmp(argv[i], "-w") == 0){
			sscanf(argv[++i], "%d", &tNum);
		}
		else if(strcmp(argv[i], "-sp") == 0){
			sscanf(argv[++i], "%d", &sPort);	
		}
		else if(strcmp(argv[i], "-sip") == 0){
			sIP = strdup(argv[++i]);
		}

		i++;
	}

	int linesNum = -1;
	char** fileLines = read_file(qFile, &linesNum);
	printf("-- lines: %d\n", linesNum);



	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~ SET SIGNALS
	signal(2, signal_handler);


	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~ MAKE THREADS
	myThreads* threads = myThreads_Init(tNum); 
	create_threads(threads, tNum, &clientThread_Start);


	runAll(threads, tNum, fileLines, linesNum, sIP, sPort);

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~ CLEAN MEM

	condDestroy();
	destroyMutex();


	while(linesNum > 0){
		free(fileLines[--linesNum]);
	}
	free(fileLines);

	free(qFile);
	if(sIP != NULL)
		free(sIP);
}

void signal_handler(int signum){
	signal(2, signal_handler);
}