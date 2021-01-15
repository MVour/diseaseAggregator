#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "main_functs.h"


#define WRITE_END 1
#define READ_END 0


char** giveThemNames(int i){
	int count = 0;

	char** fdRow = malloc(2*sizeof(char*));

	while(count < 2){	
	
		char base[15];
		memset(base, 0, 15);
		
		char addWNum[5];
		memset(addWNum, 0, 15);

		char addSide[5];
		memset(addSide, 0, 15);

		sprintf(addWNum, "%d", i);
		sprintf(addSide, "%d", count);

		strcat(base, "worker");
		strcat(base, addWNum);
		strcat(base, "_");
		strcat(base, addSide);


		fdRow[count] = strdup(base);

		count++;
	}

	return fdRow;

}

WaitingList* makeWaitingList(int** fdArray, int numWorkers){
	int i = 0;
    WaitingList* myWaitingList = WaitingList_create();
    while(i<numWorkers){
		WaitingList_add(myWaitingList, fdArray[i][READ_END]);
		i++;
	}
	return myWaitingList;
}

StatsList* getStats(WaitingList* myWaitingList, int bufferSize){

	StatsList* myStats = StatsList_init();
	while(!WaitingList_Empty(myWaitingList)){
		int tempPipe = WaitingList_getPipe(myWaitingList);
		if(tempPipe == -1){
			printf("STATS ERROR !! ~ exiting..\n");
			return NULL;
		}

		char* line = NULL;
		getMssg(tempPipe, bufferSize, &line);
		if (line != NULL){
			StatsList_byString(myStats, line);
			free(line);
			WaitingList_popPipe(myWaitingList, tempPipe);
		}
	}
	return myStats;
}


void printMenu(){

	printf("\n\t-- Menu\n");

	printf("1 ~ /listCountries\n");
	printf("2 ~ /diseaseFrequency virusName date1 date2 [country]\n");
	printf("3 ~ /topk-AgeRanges k country disease date1 date2\n");
	printf("4 ~ /searchPatientRecord recordID\n");
	printf("5 ~ /numPatientAdmissions disease date1 date2 [country]\n");
	printf("6 ~ /numPatientDischarges disease date1 date2 [country]\n");
	printf("0 ~ /exit\n");

	printf("\t\t~ Extra commands !!\n");
	printf("\t7 ~ /printStats\n");
	
	printf("\t-- Type Your Order\n\n");
}


int listCountries(char*** CNames, int CCount){
	int i = 0;
	while(i < CCount){
		printf("%s %s\n", CNames[i][0], CNames[i][1]);
		i++;
	}

	return 1;

}

int diseaseFreq(char* buf, int** fdArray, char*** CNames, int CCount,int bufSize){
	
	char* command = malloc(strlen(buf));
	memset(command, 0, strlen(buf));
	memcpy(command, buf, strlen(buf)-1);

	// strncpy(command, buf, strlen(buf)-1);

	char* tok = strtok(buf, " ");
	tok = strtok(NULL, " ");
	tok = strtok(NULL, " ");
	tok = strtok(NULL, " ");
	tok = strtok(NULL, " ");

	if(tok != NULL){
		// printf("MAIN: tok: %s\n", tok);
	}

	int i = 0;
	int workerCount = -1;
	int keep = -1;

	WaitingList* myWaitingList = WaitingList_create();

	while(i < CCount){
		int pid = -1;
		int send = 0;
		sscanf(CNames[i][1], "%d", &pid);
		if(pid != keep){
			workerCount++;
			keep = pid;
			send = 1;
		}

		if(tok != NULL){
			// printf("CNAME: %s, tok: %s, cmp: %d\n", CNames[i][0], tok, strcmp(CNames[i][0], tok));
			if(strncmp(CNames[i][0], tok, strlen(CNames[i][0])) == 0){
				// printf("MAIN: SENDING\n");
				mySend(command, strlen(command), bufSize, fdArray[workerCount][WRITE_END]);
				WaitingList_add(myWaitingList, fdArray[workerCount][READ_END]);
				break;
			}
		}
		else{
			if(send == 1){
				mySend(command, strlen(command), bufSize, fdArray[workerCount][WRITE_END]);
				WaitingList_add(myWaitingList, fdArray[workerCount][READ_END]);
				send = 0;
			}
		}

		i++;
	}

	/// GET RESULTS
	int res = 0;
	int failed = 0;

	while(!WaitingList_Empty(myWaitingList)){
		int tempPipe = WaitingList_getPipe(myWaitingList);
		if(tempPipe == -1){
			printf("ERROR !! ~ exiting..\n");
			return -1;
		}

		char* line = NULL;
		getMssg(tempPipe, bufSize, &line);
		// printf("iread: %s\n", line);
		if (line != NULL){
			// printf("MPHKE\n");
			if(strcmp(line, "FAIL~") == 0){
				if(failed == 0)
					printf("Invalid input !!\n\n");
				failed = 1;
				// free(command);
				free(line);
				WaitingList_popPipe(myWaitingList, tempPipe);
				continue;
			}
			int num = 0;
			sscanf(line, "%d", &num);
			res += num;
			free(line);
			WaitingList_popPipe(myWaitingList, tempPipe);
		}
	}

	free(command);

	WaitingList_destroy(myWaitingList);

	if(failed == 0){
		printf("%d\n", res);
		return 1;
	}
	else
		return -1;

}

int topk_AgeRanges(char* buf, int** fdArray, char*** CNames, int CCount, int bufSize){

	char* command = malloc(strlen(buf));
	memset(command, 0, strlen(buf));
	memcpy(command, buf, strlen(buf)-1);

	char* tok = strtok(buf, " ");
	tok = strtok(NULL, " ");
	tok = strtok(NULL, " ");

	int i = 0;
	int workerCount = -1;
	int keep = -1;

	WaitingList* myWaitingList = WaitingList_create();

	while(i < CCount){
		int pid = -1;
		sscanf(CNames[i][1], "%d", &pid);
		if(pid != keep){
			workerCount++;
			keep = pid;
		}
			// printf("CNAME: %s, tok: %s, cmp: %d\n", CNames[i][0], tok, strcmp(CNames[i][0], tok));
		if(strncmp(CNames[i][0], tok, strlen(CNames[i][0])) == 0){
			// printf("MAIN: SENDING\n");
			mySend(command, strlen(command), bufSize, fdArray[workerCount][WRITE_END]);
			WaitingList_add(myWaitingList, fdArray[workerCount][READ_END]);
			break;
		}

		i++;
	}

	/// GET RESULTS
	int failed = 0;

	while(!WaitingList_Empty(myWaitingList)){
		int tempPipe = WaitingList_getPipe(myWaitingList);
		if(tempPipe == -1){
			printf("ERROR !! ~ exiting..\n");
			return -1;
		}

		char* line = NULL;
		getMssg(tempPipe, bufSize, &line);
		if (line != NULL){
			if(strcmp(line, "FAIL~") == 0){
				if(failed == 0)
					printf("Invalid input !!\n\n");
				failed = 1;
				// free(command);
				free(line);
				WaitingList_popPipe(myWaitingList, tempPipe);
				continue;
				// WaitingList_destroy(myWaitingList);
				// return;
			}
			char* tok2 = strtok(line, "/~");
			while(tok2 != NULL){
				printf("%s\n", tok2);
				tok2 = strtok(NULL, "/~");
			}
			free(tok2);

			free(line);
			WaitingList_popPipe(myWaitingList, tempPipe);
		}
	}

	free(command);

	WaitingList_destroy(myWaitingList);

	if(failed == 1)
		return -1;
	else
		return 1;
}


int searchPatientRecord(char* buf, int** fdArray, int fdSize, int bufSize){

	char* command = malloc(strlen(buf));
	memset(command, 0, strlen(buf));
	memcpy(command, buf, strlen(buf)-1);

	// char* tok = strtok(buf, " ");

	int i = 0;
	WaitingList* myWaitingList = WaitingList_create();

	while(i < fdSize){
			// printf("CNAME: %s, tok: %s, cmp: %d\n", CNames[i][0], tok, strcmp(CNames[i][0], tok));

		mySend(command, strlen(command), bufSize, fdArray[i][WRITE_END]);
		WaitingList_add(myWaitingList, fdArray[i][READ_END]);

		i++;
	}

	/// GET RESULTS
	int entries = myWaitingList->entries;
	int none = 0;
	int failed = 0;

	while(!WaitingList_Empty(myWaitingList)){
		int tempPipe = WaitingList_getPipe(myWaitingList);
		if(tempPipe == -1){
			printf("ERROR !! ~ exiting..\n");
			return -1;
		}

		char* line = NULL;
		getMssg(tempPipe, bufSize, &line);
		if (line != NULL){
			if(strcmp(line, "FAIL~") == 0){
				if(failed == 0)
					printf("Invalid input !!\n");
				// free(command);
				failed = 1;
				free(line);
				WaitingList_popPipe(myWaitingList, tempPipe);
				continue;
			}
			else if(strcmp(line, "NONE~") == 0){
				free(line);
				WaitingList_popPipe(myWaitingList, tempPipe);
				none++;
				continue;
			}
			char* tok2 = strtok(line, "~");
			printf("%s\n", tok2);
			free(line);
			// free(tok2);
			WaitingList_popPipe(myWaitingList, tempPipe);
			// break;
		}
	}

	free(command);

	WaitingList_destroy(myWaitingList);

	if(none == entries){
		printf("None found\n");
	}

	if(failed == 1)
		return -1;
	else
		return 1;

}

int numPatientAdm_Dis(char* buf, int** fdArray, char*** CNames, int CCount, int bufSize){

	char* command = malloc(strlen(buf));
	memset(command, 0, strlen(buf));
	memcpy(command, buf, strlen(buf)-1);

	char* tok = strtok(buf, " ");
	tok = strtok(NULL, " ");
	tok = strtok(NULL, " ");
	tok = strtok(NULL, " ");
	tok = strtok(NULL, " ");

	if(tok != NULL){
		// printf("MAIN: tok: %s\n", tok);
	}

	int i = 0;
	int workerCount = -1;
	int keep = -1;

	WaitingList* myWaitingList = WaitingList_create();

	while(i < CCount){
		int pid = -1;
		int send = 0;
		sscanf(CNames[i][1], "%d", &pid);
		if(pid != keep){
			workerCount++;
			keep = pid;
			send = 1;
		}

		if(tok != NULL){
			// printf("CNAME: %s, tok: %s, cmp: %d\n", CNames[i][0], tok, strcmp(CNames[i][0], tok));
			if(strncmp(CNames[i][0], tok, strlen(CNames[i][0])) == 0){
				// printf("MAIN: SENDING\n");
				mySend(command, strlen(command), bufSize, fdArray[workerCount][WRITE_END]);
				WaitingList_add(myWaitingList, fdArray[workerCount][READ_END]);
				break;
			}
		}
		else{
			if(send == 1){
				mySend(command, strlen(command), bufSize, fdArray[workerCount][WRITE_END]);
				WaitingList_add(myWaitingList, fdArray[workerCount][READ_END]);
				send = 0;
			}
		}

		i++;
	}

	/// GET RESULTS
	int failed = 0;
	int none = 0;

	while(!WaitingList_Empty(myWaitingList)){
		int tempPipe = WaitingList_getPipe(myWaitingList);
		if(tempPipe == -1){
			printf("ERROR !! ~ exiting..\n");
			return -1;
		}

		char* line = NULL;
		getMssg(tempPipe, bufSize, &line);
		if (line != NULL){
			if(strcmp(line, "FAIL~") == 0){
				if(failed == 0)
					printf("Invalid input !!\n");
				failed = 1;
				// free(command);
				free(line);
				WaitingList_popPipe(myWaitingList, tempPipe);
				continue;
			}
			char* tok2 = strtok(line, "/~");
			while(tok2 != NULL){
				if(strcmp(tok2, "0") == 0){
					none = 1;
					tok2 = strtok(NULL, "/~");
					continue;
				}
				printf("%s\n", tok2);
				tok2 = strtok(NULL, "/~");
			}
			
			free(line);
			WaitingList_popPipe(myWaitingList, tempPipe);
		}
	}

	free(command);

	WaitingList_destroy(myWaitingList);

	if(none == 1)
		printf("none found\n");

	if(failed == 1)
		return -1;
	else
		return 1;

}


void make_log(int totalAsked, int anwsered, int failed, char*** CNames, int CCount){
 	pid_t mypid = getpid();

 	int copy = mypid;

 	int digits = 1;

 	while(copy/10 != 0){
 		digits++;
 		copy /= 10;
 	}

 	char* Pid = malloc(digits+1);
 	sprintf(Pid, "%d", mypid);

	char* name = malloc(9+digits+1);
	memset(name, 0, 9+digits+1);
	strcat(name, "log_file.");
	strcat(name, Pid);

	FILE* fpout;
	fpout = fopen(name, "w+");

	while(CCount > 0){
		fprintf(fpout, "%s\n", CNames[--CCount][0]);
	}

	fprintf(fpout, "\nTOTAL %d\n", totalAsked);
	fprintf(fpout, "SUCCESS %d\n", anwsered);
	fprintf(fpout, "FAILED %d\n", failed);

	free(name);
	free(Pid);

	fclose(fpout);

}

void replaceChild(pid_t Cpid, char*** CNames, int CCount, char*** NamedPipes, int bufSize, char* input_dir, int** fdArray, char* serverPort, char* serverIP){
	
	int targetCell = -1;

	int count = 0;
	int keep = 0;

	while(count < CCount){
		pid_t checkPid = -1;
		sscanf(CNames[count][1], "%d", &checkPid);


		if(checkPid != keep){
			targetCell++;
			keep = checkPid;


			if(Cpid == checkPid){
				break;
			}
		}
		count++;
	}

// printf("closing: %d\n", fdArray[targetCell][WRITE_END]);
	close(fdArray[targetCell][WRITE_END]);


	pid_t new = -1;
	
	new = fork();

	if(new == 0){
		char passBuffSize[10];
		sprintf(passBuffSize, "%d", bufSize);
	
		execl("./worker", "./worker","-w", NamedPipes[targetCell][READ_END], "-r", NamedPipes[targetCell][WRITE_END], "-b", passBuffSize, "-i", input_dir, NULL);
	}

	else{
		pid_t checkPid = -1;
		
		int len = 0;
		int save = count;
		int to_pass = 0;

		char Npid[15];
		memset(Npid, 0, 15);
		sprintf(Npid, "%d", new);

		sscanf(CNames[count][1], "%d", &checkPid);
		while(checkPid == Cpid){
			to_pass++;
			len += strlen(CNames[count][0]);

			free(CNames[count][1]);

			CNames[count][1] = strndup(Npid, strlen(Npid));
			// printf("C> %s\n", CNames[count][0]);

			if(count+1 == CCount)
				break;
			sscanf(CNames[++count][1], "%d", &checkPid);
		}

		int temp = to_pass;
		int digits = 1;

		while(temp/10 != 0){
			digits++;
			temp /= 10;
		}

		char Sum[10];
		memset(Sum, 0, 10);
		sprintf(Sum, "%d", to_pass);

		char* Mssg = malloc(len+1+to_pass+2+digits+1+strlen(serverPort)+strlen(serverPort)+2+1+strlen(CNames[save][2]));
		memset(Mssg, 0, len+1+to_pass+2+digits+1);

		strcat(Mssg, CNames[save][2]);
		strcat(Mssg, "/");

		strcat(Mssg, serverIP);
		strcat(Mssg, "/");

		strcat(Mssg, serverPort);
		strcat(Mssg, "/");


		strcat(Mssg, "C:");
		strncat(Mssg, Sum, digits);
		strcat(Mssg, "/");

		sscanf(CNames[save][1], "%d", &checkPid);
		while(checkPid == new){
			// printf("to add: %s\n", CNames[save][0]);
			strncat(Mssg, CNames[save][0], strlen(CNames[save][0]));
			strcat(Mssg, "/");

			if(save+1 == CCount)
				break;
			sscanf(CNames[++save][1], "%d", &checkPid);
		}

		

		fdArray[targetCell][WRITE_END] = open(NamedPipes[targetCell][WRITE_END], O_WRONLY|O_TRUNC);

		// printf("openinf: fdArray[%d][WRITE_END]: %d\n", targetCell, fdArray[targetCell][WRITE_END]);
		// printf("Mssg: %s, len: %lu\n", Mssg, strlen(Mssg));
		mySend(Mssg, strlen(Mssg), bufSize, fdArray[targetCell][WRITE_END]);
	
		free(Mssg);
	}

	//// FREE PIPE FROM STATS
	
	char* line = NULL;
	while(line == NULL){
		getMssg(fdArray[targetCell][READ_END], bufSize, &line);
	}
	
	free(line);

}

void clean_up(StatsList* myStats, WaitingList* ALLWaiting, int** fdArray, int numWorkers, char*** countriesNames, int countCountries, char*** namedPipesArray, char* input_dir, char* target, char* serverIP, char* serverPort){

	/////////////////////////////////////////
	//    		CLEAN ALL MEM 			  //
  	///////////////////////////////////////


	StatsList_destroy(myStats);

	WaitingList_destroy(ALLWaiting);

    // ~~~~~~~~~~~~~~~~~~~~ CLOSE ALL PIPES

    int i = 0;
    while(i < numWorkers){
    	close(fdArray[i][0]);
    	close(fdArray[i][1]);
    	i++;
    }


    // ~~~~~~~~~~~~~~~~~~~~ CLEAN MEM

    // printf("CLEANING MEM\n");

    free(input_dir);
    i = 0;
    while(i < countCountries){
    	// printf("> %s, %s\n",  countriesNames[i][0], countriesNames[i][1]);
    	free(countriesNames[i][0]);
    	free(countriesNames[i][1]);
    	// free(countriesNames[i][2]);
    	free(countriesNames[i++]);
    }
    free(countriesNames);
    
    i = 0;
    while(i < numWorkers){
    	free(fdArray[i]);


    	unlink(namedPipesArray[i][1]);
    	unlink(namedPipesArray[i][0]);

    	free(namedPipesArray[i][0]);
    	free(namedPipesArray[i][1]);
    	free(namedPipesArray[i]);

    	i++;
    }
    free(namedPipesArray);
    free(fdArray);
    // closedir(d);
    free(target);

    free(serverIP);
    free(serverPort);
}


void killemALL(char*** CNames, int CCount){
	int count = 0;
	int keep = -1;

	while(count < CCount){
		int pid = -1;
		sscanf(CNames[count][1], "%d", &pid);
		if(pid != keep){
			keep = pid;

			kill(pid, 2);
		}
		count++;

	}
}

void getPorts(WaitingList* wl, int bs, char*** CNames){
	int i = 0;
	while(!WaitingList_Empty(wl)){
		char* line = NULL;
		int tempPipe = WaitingList_getPipe(wl);
		
		while(line == NULL){	
			if(tempPipe == -1){
				printf("ERROR !! ~ exiting..\n");
				return;
			}

			getMssg(tempPipe, bs, &line);
		}

		printf("%s\n", line);
		WaitingList_popPipe(wl, tempPipe);

		char* port = strtok(line, "~");
		
		CNames[i][2] = strndup(port, strlen(port));

		free(line);
		line = NULL;

	}
}