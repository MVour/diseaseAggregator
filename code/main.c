#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <sys/select.h>
#include <dirent.h>
#include <poll.h>
#include <errno.h>

#include "main_functs.h"


#define WRITE_END 1
#define READ_END 0

int UPDATE = 0;
int EXIT = 0;
int CHILD_DEAD = 0;


int main(int argc, char** argv){

	// ~~~~~~~~~~~~~~~~~~~ READ ARGS
	int i = 0;

	int numWorkers =0;
	int bufferSize = 0;
	char* input_dir = NULL;
	char* serverIP = NULL;
	char* serverPort = NULL;

	while(i < argc){
	
		if(strcmp(argv[i], "-w") == 0){
			sscanf(argv[++i], "%d", &numWorkers);
		}
		
		else if(strcmp(argv[i], "-b") == 0){
			sscanf(argv[++i], "%d", &bufferSize);
		}
		
		else if(strcmp(argv[i], "-i") == 0){
			input_dir = strdup(argv[++i]);

		}

		else if(strcmp(argv[i], "-s") == 0){
			serverIP = strdup(argv[++i]);

		}

		else if(strcmp(argv[i], "-p") == 0){
			serverPort = strdup(argv[++i]);

		}


		i++;
	}

	
	// ~~~~~~~~~~~~~~~~~~~~ CALC ASSIGNS - GET COUNTRIES NAMES

	DIR *d = NULL;
    struct dirent *dir = NULL;

    char* target = malloc(4+strlen(input_dir));
    memset(target, 0, 4+strlen(input_dir));
    strcat(target, "../");

    strcat(target, input_dir);

    if( (d = opendir(target)) == NULL){
    	printf("ERROR: Cant Open Dir\n");
    	free(input_dir);
    	return -1;
    }

    int countCountries = 0;
    while ((dir = readdir(d)) != NULL){
    	if(strcmp(dir->d_name, "..") == 0 || strcmp(dir->d_name, ".") == 0)
    		continue;
        countCountries++;
    }

    char*** countriesNames = (char***) malloc(countCountries*sizeof(char**));
    rewinddir(d);
    countCountries = 0;
    while ((dir = readdir(d)) != NULL){
    	if(strcmp(dir->d_name, "..") == 0 || strcmp(dir->d_name, ".") == 0)
    		continue;
        countriesNames[countCountries] = malloc(3*sizeof(char*));
        countriesNames[countCountries++][0] = strndup(dir->d_name, strlen(dir->d_name));
    }
    closedir(d);


	// ~~~~~~~~~~~~~~~~~~~~ MAKE FD_ARRAY / FORK WORKERS / SEND COUNTRIES -> WORKERS

    char*** namedPipesArray = malloc(numWorkers*sizeof(char**));
    int** fdArray = malloc(numWorkers*sizeof(int*));

    i = 0;

    pid_t p;
    char passBuffSize[10];
	sprintf(passBuffSize, "%d", bufferSize);

    int passed = 0;

    while(i < numWorkers){
    	
    	namedPipesArray[i] = giveThemNames(i);

    	mkfifo(namedPipesArray[i][READ_END], 0666);
    	mkfifo(namedPipesArray[i][WRITE_END], 0666);

		fdArray[i] = malloc(2*sizeof(int));

    	fdArray[i][READ_END] = open(namedPipesArray[i][READ_END], O_RDONLY|O_NONBLOCK|O_TRUNC);

    	p = fork();

		if(p == 0){
			execl("./worker", "./worker","-w", namedPipesArray[i][READ_END], "-r", namedPipesArray[i][WRITE_END], "-b", passBuffSize, "-i", input_dir, NULL);
		}
		else{
			char cpid[15];
			memset(cpid, 0, 15);
			sprintf(cpid, "%d", p);

			int to_pass = 0;
			int left = countCountries % numWorkers;
			int extraWork = 0;
			
			if(left != 0 && i >= numWorkers - left)
				extraWork = 1;

			char* to_send;
			int size_toSend = 0;
			
			while( to_pass < (countCountries / numWorkers) || extraWork == 1){
				if(to_pass == countCountries / numWorkers && extraWork == 1){
					size_toSend += strlen(countriesNames[to_pass + passed][0]);
					extraWork = 0;
				}
				else{
					size_toSend += strlen(countriesNames[to_pass + passed][0]);			
				}

				to_pass++;

			}
			
				// ~~~~~~~~~~~~~ PREPEARE DATA FOR SENDING TO WORKER !!!!!

			int digits = 1;
			int temp = to_pass;
			while(temp/10 !=0){
				digits++;
				temp = temp/10;
			}
			// printf("digits: %d\n", digits);
			char* keepDigits = malloc(digits+1);
			memset(keepDigits, 0, digits+1);
			sprintf(keepDigits, "%d", to_pass);

			size_toSend += 2 + 2 + to_pass + digits + 1 + strlen(serverPort) + strlen(serverIP) + 2;	// NAMES + "C:" + DIGITS + "/"
			to_send = malloc(size_toSend+1);
			memset(to_send, 0, size_toSend+1);
			
			// printf("to_sendSize: %d, to_send: %s, size_toSend: %d\n", size_toSend, to_send, size_toSend);
			strcat(to_send, "0");
			strcat(to_send, "/");


			strcat(to_send, serverIP);
			strcat(to_send, "/");
			
			strcat(to_send, serverPort);
			strcat(to_send, "/");

			int count = 0;
			strcat(to_send, "C:");

			strcat(to_send, keepDigits);
			strcat(to_send, "/");
			
			while(count < to_pass){
				strcat(to_send, countriesNames[count+passed][0]);
				countriesNames[count+passed][1] = strndup(cpid, strlen(cpid));
				countriesNames[count+passed][2] = "0";
				strcat(to_send, "/");
				count++;
			}

			passed += to_pass;
			fdArray[i][WRITE_END] = open(namedPipesArray[i][WRITE_END], O_WRONLY|O_TRUNC);
			
			mySend(to_send, size_toSend, bufferSize, fdArray[i][WRITE_END]);

			free(to_send);
			free(keepDigits);
		}
		i++;
    }

// ~~~~~~~~~~~~~~~~~~~ GET PORTS FROM WORKES
    WaitingList* mple = makeWaitingList(fdArray, numWorkers);

	getPorts(mple, bufferSize, countriesNames);

	WaitingList_destroy(mple);


// ~~~~~~~~~~~~~~~~~~~ GET STATS FROM WOKERS
    // ~~ CREATE PIPES WATING LIST

    WaitingList* ALLWaiting = makeWaitingList(fdArray, numWorkers);

    // WaitingList* myWaitingList = makeWaitingList(fdArray, numWorkers);

	// StatsList* myStats = getStats(myWaitingList, bufferSize);
    StatsList* myStats = NULL;

	// WaitingList_destroy(myWaitingList);


/// ~~~~~~~~~~~~~~~~~~ SET SIGNALS

	// signal(10, handler);
	signal(2, exitHandler);
	signal(3, exitHandler);
	signal(17, deadChild);


/// ~~~~~~~~~~~~~~~~~~ PRINT MENU - GET ORDERS

    int totalAsked = 0;
    int anwsered = 0;
    int failed = 0;



    while(1){

		if(EXIT == 1){			

			killemALL(countriesNames, countCountries);				

			make_log(totalAsked, anwsered, failed, countriesNames, countCountries);
			clean_up(myStats, ALLWaiting, fdArray, numWorkers, countriesNames, countCountries, namedPipesArray, input_dir, target, serverIP, serverPort);
			return 1 ;
		}

		if(CHILD_DEAD == 1){
			pid_t Cpid;
			int status;
		    while((Cpid = waitpid(-1, &status, WNOHANG)) > 0){
		    	// printf("MPHKES, PID: %d\n", Cpid);
		        replaceChild(Cpid, countriesNames, countCountries, namedPipesArray, bufferSize, input_dir, fdArray, serverPort, serverIP);
			}
			CHILD_DEAD = 0;
		}

    }
	return 0;
}

void exitHandler(int signum){
	signal(signum, exitHandler);
	EXIT = 1;
}

void deadChild(){
	signal(17, deadChild);
	CHILD_DEAD = 1;
}