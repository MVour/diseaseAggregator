#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <dirent.h>

#include <signal.h>
#include <poll.h>

#include "worker.h"
#include "mssgHandling.h"

#define C_HASH 3
#define D_HASH 3
#define B_SIZE 500

#define MYIP "128.0.0.2"


int SIG1FLAG = 0;
int MAKE_LOG = 0;

char** scanedFiles = NULL;
int sFiles = 0;

// int buffSize = 0;
// int myPipeWrite = -1;

// char* input_dir = NULL;


int main(int argc, char** argv){
	// printf("MY PID: %d\n", getpid());
	int myPipeWrite = 0;
	int myPipeRead = 0;

	unsigned int myPort = 0;
	// char* myIP = NULL;

	char* PWrite = NULL;
	char* PRead = NULL;
	char* input_dir = NULL;
	int buffSize = 0;

	char test3[50];
	memset(test3, 0, 50);

	// ~~~~~~~~~~~~~~~~~~~~~~~ READING ARGS / PIPE'S ENDS
	int i = 0;
	while(i < argc){
		// printf("argv[%d]: %s\n", i, argv[i]);

		if(strcmp(argv[i], "-w") == 0)
			PWrite = strdup(argv[i+1]);

		else if(strcmp(argv[i], "-r") == 0)
			PRead = strdup(argv[i+1]);

		else if(strcmp(argv[i], "-b") == 0)
			sscanf(argv[i+1], "%d", &buffSize);

		else if(strcmp(argv[i], "-i") == 0)
			input_dir = strdup(argv[i+1]);

		i++;
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~ OPEN PIPES

	myPipeWrite = open(PWrite, O_WRONLY|O_TRUNC);
	myPipeRead = open(PRead, O_RDONLY);


	////////////////////////// C DISEASE / COUTNRY HASH TABLE & PAT. LIST->

	hashTable* cHash = hash_create(C_HASH, B_SIZE);
	hashTable* dHash = hash_create(D_HASH, B_SIZE);

	infoNode* patientsList = list_create();

	StatsList* DStats = StatsList_init();

	////////////////////////// <- C DISEASE HASH TABLE



	// ~~~~~~~~~~~~~~~~~~~~~~~ READ 1ST MSSG - COUNTRIES - PORTS IPS
	char* myline = NULL;
	getMssg(myPipeRead, buffSize, &myline);
	char* tempLine = strdup(myline);
	// printf("w got: %s\n", tempLine);
// 	strcpy(tempLine, myline);

	char* port = strdup(strtok(tempLine, "/"));
	sscanf(port, "%d", &myPort);
	// printf("i got port: %s, Port: %d\n", port, myPort);

	char* serverIP = strdup(strtok(NULL, "/"));
	char* serverPort = strdup(strtok(NULL, "/"));

	int sPort = 0;
	sscanf(serverPort, "%d", &sPort);

	free(tempLine);

	// printf("IP: %s, Port: %s\n", serverIP, serverPort);

	int CCount = 0;
	char** CNames;

	CNames = getCountries(myline, &CCount);
	free(myline);

	// ~~~~~~~~~~~~~~~~~~~~~~~ MAKE SOCKET
						/// SET MY_SERVER
	int sock = socket(AF_INET, SOCK_STREAM , 0);;

	struct sockaddr_in myself;
	unsigned int myself_len = sizeof(myself) +1;

	myself.sin_family = AF_INET;
//	myself.sin_addr.s_addr = inet_addr(INADDR_ANY);
	myself.sin_addr.s_addr = htonl(INADDR_ANY);
	// myself.sin_addr.s_addr = inet_addr(MYIP);

	if(strcmp(port, "0") == 0){
		myself.sin_port = 0;
	}
	else
		myself.sin_port = htons(myPort);

	free(port);

	// // getsockname(sock, (struct sockaddr *)&sock, sizeof(sock));

	int reuse = 1;
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(int)) == -1){
		printf("skata\n");
	}
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) == -1){
		printf("skata\n");
	}
	if(bind(sock, (struct sockaddr *) &myself, sizeof (myself)) == -1){
		printf("skata\n");
	}

	if(getsockname(sock, (struct sockaddr *) &myself, &myself_len) == -1){
		printf("skata\n");
	}


	// printf("myPort: %u, myIP: %u\n", ntohs(myself.sin_port), myself.sin_addr.s_addr);

	myPort = (unsigned int) ntohs(myself.sin_port);
	
	char newport[20];
	memset(newport, 0, 20);
	sprintf(newport, "%u", myPort);
// printf("myport %u, %s\n", myPort, port );

			// send my port to parent
	char* myInfo = malloc(strlen(newport) + 1);
	memset(myInfo, 0, strlen(newport) + 1);
	strcat(myInfo, newport);

//printf("mysend: %s\n", myInfo);

	mySend(myInfo, strlen(myInfo), buffSize, myPipeWrite);
	free(myInfo);
	listen(sock, 1);

	// ~~~~~~~~~~~~~~~~~~~~~~~ CALL GATHER FOR EACH COUNTRY-DIR

	scanFiles(cHash, dHash, patientsList, DStats, CNames, CCount, input_dir);

	// ~~~~~~~~~~~~~~~~~~~~~~~  SEND STATS TO SERVER

	char* testing = StatsList_toString(DStats);
	char* to_send = malloc(strlen(testing)+strlen(newport)+2);
	memset(to_send, 0, strlen(testing)+strlen(newport)+2);
	strcat(to_send, newport);
	strcat(to_send, "/");
	strcat(to_send, testing);

	int newS = sendMssg_viaSocket(sPort, serverIP, to_send);
	close(newS);

	// ~~~~~~~~~~~~~~~~~~~~~~~
	free(testing);
	free(to_send);


	// ~~~~~~~~~~~~~~~~~~~~~~~ SET SIGNALS

		signal(10, addFiles);
		signal(3, to_log);
		signal(2, to_log);



		int totalAsked = 0;
		int anwsered = 0;
		int failed = 0;

		struct pollfd myfd;
		myfd.fd = sock;
		myfd.events = POLLIN;

 	// ~~~~~~~~~~~~~~~~~~~~~~ CLIENT
		struct sockaddr_in client;
		struct sockaddr *clientptr=(struct sockaddr *)&client;
		socklen_t clientlen = 0;


	// ~~~~~~~~~~~~~~~~~~~~~~ GET ORDERS
	printf("WORKER LISTENING AT: %d\n", myPort);
	while(1){


		char* line = NULL;
		while(poll(&myfd, 1, 0) <= 0){

			if(SIG1FLAG == 1){
				mySend("UPDATE", 6, buffSize, myPipeWrite);
				scanFiles(cHash, dHash, patientsList, DStats, CNames, CCount, input_dir);

				pid_t par = getppid();
				kill(par, 10);

				SIG1FLAG = 0;
			}

			if(MAKE_LOG == 1){
				close(sock);
				make_log(totalAsked, anwsered, failed, CNames, CCount);
				clean_up(CNames, CCount, cHash, dHash, patientsList, DStats, input_dir, PWrite, PRead, myPipeWrite, myPipeRead, serverIP, serverPort);


				return 1;
			}

		}

//		struct sockaddr_storage inc_addr;
//		socklen_t inc_len = sizeof(inc_addr);
//		struct sockaddr_in* temp = NULL;
//		unsigned int tempPort = -1;

		int targetS = accept(sock, clientptr, &clientlen);

		line = getMssg_viaSocket(targetS);

		int to_act = 0;
		char* to_send = NULL;
		char* tok = strtok_r(line, " ~\n", &line);
	 // printf("WORKER: line: %s, tok: %s\n", line, tok);
		if(tok == NULL){
			totalAsked++;
			failed++;
			continue;
		}
		else if(strcmp(tok, "/exit") == 0 || strcmp(tok, "0") == 0){
			free(tok);
			make_log(totalAsked, anwsered, failed, CNames, CCount);
			clean_up(CNames, CCount, cHash, dHash, patientsList, DStats, input_dir, PWrite, PRead, myPipeWrite, myPipeRead, serverIP, serverPort);
			break;
		}

		else if(strcmp(tok, "/diseaseFrequency") == 0 || strcmp(tok, "2") == 0){
			to_act = 2;

		}

		else if(strcmp(tok, "/topk-AgeRanges") == 0 || strcmp(tok, "3") == 0){
			to_act = 3;
		}

		else if(strcmp(tok, "/searchPatientRecord") == 0 || strcmp(tok, "4") == 0){
			to_act = 4;
		}

		else if(strcmp(tok, "/numPatientAdmissions") == 0 || strcmp(tok, "5") == 0){
			to_act = 5;
		}

		else if(strcmp(tok, "/numPatientDischarges") == 0 || strcmp(tok, "6") == 0){
			to_act = 6;
		}

		totalAsked++;

		to_send = act(to_act, cHash, dHash, patientsList, DStats, line);

		// printf("WORKER: %s\n", to_send);

		if(to_send != NULL){
			if(strlen(to_send) == 0){
				sendMssg_viaSocketExisting(targetS, "FAIL");
				failed++;
				free(to_send);
			}
			else{	
				sendMssg_viaSocketExisting(targetS, to_send);
				free(to_send);
				anwsered++;
			}
		}
		else{
			// if(to_act != 4){
				sendMssg_viaSocketExisting(targetS, "FAIL");
			// }
			failed++;
		}
		char* endstr = NULL;
		endstr = getMssg_viaSocket(targetS);
		free(endstr);
		close(targetS);

		free(tok);
	}

	// close(sock);


	return 0;
}

/////////////////////////////////////////////////////////////////////////////////


void clean_up(char** CNames, int CCount, hashTable* cHash, hashTable* dHash, infoNode* patientsList, StatsList* DStats, char* input_dir, char* PWrite, char* PRead, int myPipeWrite, int myPipeRead, char* serverIP, char* serverPort){
		// ~~~~~~~~~~~~~~~~~~~~~~~ CLEAN MEM
	while(CCount > 0){
		free(CNames[--CCount]);
	}
	free(CNames);

	while(sFiles > 0){
		free(scanedFiles[--sFiles]);
	}
	free(scanedFiles);

	hash_destroy(cHash);
	hash_destroy(dHash);
	list_destroy(patientsList);
	StatsList_destroy(DStats);

	free(input_dir);

	free(PWrite);
	free(PRead);

	close(myPipeWrite);
	close(myPipeRead);

	free(serverIP);
	free(serverPort);
}

void make_log(int totalAsked, int anwsered, int failed, char** CNames, int CCount){
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
		fprintf(fpout, "%s\n", CNames[--CCount]);
	}

	fprintf(fpout, "\nTOTAL %d\n", totalAsked);
	fprintf(fpout, "SUCCESS %d\n", anwsered);
	fprintf(fpout, "FAILED %d\n", failed);

	free(name);
	free(Pid);

	fclose(fpout);

}
/////////////////////////////////////////////////////////////////////////////////

char** getCountries(char* myline, int* CCount){			// FILL CNAME ARRAY / COUNTRIES COUNT
	char* tok = NULL;

	int isNum = 1;
	char** CNames = NULL;

	int i = 0;

	tok = strtok(myline, ":/");
	while(strcmp(tok, "C") != 0){
		// printf("tok: %s\n", tok);
		tok = strtok(NULL, ":/");
	}
	tok = strtok(NULL, "/");
	while(strcmp(tok, "~") != 0){
		if(isNum == 1){
			sscanf(tok, "%d", CCount);
			
			CNames = (char**)malloc(*CCount*sizeof(char*));
			isNum = 0;
		}
		else{
	
			CNames[i] = strdup(tok);
			// printf("C> %s\n", CNames[i]);
			i++;
	
		}

		tok = strtok(NULL, ":/");
	}
	return CNames;
}

///////////////////////////////////////////////////////////////////////


void scanFiles(hashTable* cHash, hashTable* dHash, infoNode* patientsList, StatsList* DStats, char** CNames, int CCount, char* input_dir){

	int i = 0;
	while(i < CCount){
	    char* target = malloc(5+strlen(CNames[i])+strlen(input_dir));
	    memset(target, 0, 5+strlen(CNames[i])+strlen(input_dir));
	    strcat(target, "../");
	    strncat(target, input_dir, strlen(input_dir));
	    strcat(target, "/");
	    strcat(target, CNames[i]);
	
		gather(cHash, dHash, patientsList, DStats, target, CNames[i]);
// printf("scan: sFiles: %d\n", *sFiles);
		// printf("c> %s\n", CNames[i]);
		// sFiles++;

		i++;
	    free(target);
	    // free(tempArray);
	}
		// StatsList_print(DStats);

}

void gather(hashTable* cHash, hashTable* dHash, infoNode* patientsList, StatsList* DStats, char* myDir, char* country){
	// FILE* fout = NULL;
	// fout = open(workerLog.txt, "r+");

	DIR *d = NULL;
    struct dirent *dir = NULL;
    if( (d = opendir(myDir)) == NULL){
        printf("ERROR: Cant Open Dir\n");
    	// free(input_dir);
    	return;
    }


    while ((dir = readdir(d)) != NULL){
    	if(strcmp(dir->d_name, "..") == 0 || strcmp(dir->d_name, ".") == 0){
    		continue;
    	}


		int check = 0;
		int found = -1;
		while(check < sFiles){
			// printf("check: %d, sFiles: %d, CCount: %d, s1: %s, s2: %s\n", check, sFiles, CCount, scanedFiles[check], dir->d_name);
			if(strncmp(scanedFiles[check], dir->d_name, strlen(dir->d_name)) == 0){
				found = 1;
				break;
			}
			check++;
		}
		if(found == 1){
			continue;
		}

		// printf("GATHERING\n");
    	char* fname = NULL;
    	fname = malloc(strlen(dir->d_name)+strlen(myDir)+2);
    	memset(fname, 0, strlen(dir->d_name)+strlen(myDir)+2);

    	strcat(fname, myDir);
    	strcat(fname,"/");
    	strncat(fname, dir->d_name, strlen(dir->d_name));


		char* date = strndup(dir->d_name, strlen(dir->d_name)-4);

		dateStats* myStats = NULL;
		myStats = fillStructs(cHash, dHash, patientsList, country, date, fname);

		free(date);
		free(fname);

		StatsList_addDateRec(DStats, myStats);

		char** tempArray = NULL;
		tempArray = realloc(scanedFiles, (sFiles+1)*sizeof(char*));
		if(tempArray == NULL){
			printf("error at realloc\n");
			return;
		}

		scanedFiles = tempArray;
		scanedFiles[sFiles] = strndup(dir->d_name, strlen(dir->d_name));
		sFiles ++;
		// printf("cell: %s\n", *scanedFiles[*sFiles]);
		// printf("sFilesA: %d\n", *sFiles);

    }

    closedir(d);
    return;
}


dateStats* fillStructs(hashTable* cHash, hashTable* dHash, infoNode* patientsList, char* country, char* date, char* fname){

	dateStats* myStats = dateStats_init(date, country);

   	FILE* fpin = NULL;
   	fpin = fopen(fname, "r+");

	if(fpin == NULL){
		printf("Can't Open File !\n");
		return NULL;
	}

	char line[100];

	while(fgets(line, 100, fpin) != NULL){
		char* id = strtok(line, " ");
		char* status = strtok(NULL, " ");
		char* name = strtok(NULL, " ");
		char* lname = strtok(NULL, " ");
		char* disease = strtok(NULL, " ");
		char* age = strtok(NULL, " \n");

// printf("id: %s, stat: %s\n", id, status);
		if(strcmp(status, "ENTER") == 0){
			
			myPatient* patient = patient_create(id, name, lname, disease, country, date, age);

			list_node* lNode = list_add(patientsList, patient); 	//CHECK FOR DUPLICATE
			if(lNode == NULL){
				patient_destroy(patient);
				continue;
			}

			hash_add(cHash, country, lNode, hash1(country));
			hash_add(dHash, disease, lNode, hash1(disease)); 

			int ageInt = -1;
			sscanf(age, "%d", &ageInt);
			dateStats_addRec(myStats, disease, country, ageInt);

		}
		else{
			myPatient* pat = list_search(patientsList, id);
			if(pat == NULL){
				printf("ERROR\n");
			}
			else{
				patient_addExitDate(pat, date);
			}
		}

	}
	fclose(fpin);
	return myStats;


}

void notifyParent(int myPipe, int buffSize){
	pid_t mypid = getpid();

 	int copy = mypid;

 	int digits = 1;

 	while(copy/10 != 0){
 		digits++;
 		copy /= 10;
 	}

 	char* Pid = malloc(digits+1);
 	sprintf(Pid, "%d", mypid);


 	pid_t par = getppid();

 	kill(par, 12);
 	mySend(Pid, strlen(Pid), buffSize, myPipe);

 	free(Pid);
}

void addFiles(int signum){
	signal(10, addFiles);
	SIG1FLAG = 1;
	// char* testing = StatsList_toString(DStats);
}

void to_log(int signum){
	signal(signum, to_log);
	MAKE_LOG = 1;
}

// void exit(int signum){

// }
