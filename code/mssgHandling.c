#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <poll.h>


#include "mssgHandling.h"


#define WRITE_END 1
#define READ_END 0
#define BSIZE 200


void mySend(char* out, int outSize, int bufSize, int myPipe){
	// printf("mySend: received: %s, out_size: %d, buf_Size: %d\n", out, outSize, bufSize);

	char mssgLen[bufSize];
	memset(mssgLen, 0 , bufSize);
	sprintf(mssgLen, "%d", outSize+1);
	// printf("mySend: sending: %s\n", mssgLen);
	write(myPipe, mssgLen, bufSize);

	// printf("Pipe_write: %d\n", myPipe[WRITE_END]);
	if(bufSize >= outSize + 1){
		char outbuf[bufSize];
		memset(outbuf, 0 ,bufSize);

		strcat(outbuf, out);
		strcat(outbuf, "~");
		// printf("tosend: %s \n", outbuf);
		write(myPipe, outbuf, outSize + 1);
	}
	else{
		int div = (outSize+1) /  (bufSize);
		int mod = (outSize+1) % (bufSize);
		int pieces = div;

		if(mod != 0)
			pieces++;

		int passed_pieces = 0;
		// printf("Total pieces to send: %d\n", pieces);
		int passedSize = 0;
		while(passed_pieces < pieces){
			char* outbuf = malloc(bufSize+1);
			memset(outbuf, 0 ,bufSize+1);

			if(passedSize < outSize){
				if((outSize - passedSize) < bufSize ){
					strncpy(outbuf, out+passedSize, outSize-passedSize);
					passedSize += outSize - passedSize;
				}
				else{
					strncpy(outbuf, out+passedSize, bufSize);
					passedSize += bufSize;
					// printf("peak: %s\n", outbuf);
				}
			}
			if(passed_pieces == pieces -1){
					memcpy(outbuf+(outSize%(bufSize)), "~", 1);
			}

			write(myPipe, outbuf, bufSize);

			passed_pieces++;

			free(outbuf);
		}
		// wait(0);
	}


}

int sendMssg_viaSocket(unsigned int port, char* ip, char* mssg){
	int sock;
// printf("MPHKE_0\n");
	struct sockaddr_in server;
	struct sockaddr *serverptr = (struct sockaddr*)&server;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	// printf("creating sok\n");
	struct hostent *rem = NULL;
	if((rem = gethostbyname(ip)) == NULL ){
		return -1;
		// printf("skata\n");
	}

	server.sin_family = AF_INET;
	memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
	server.sin_port = htons(port);

// printf("trying to connect\n");
	while(connect(sock, serverptr, sizeof(server)) <0){
		// printf("error\n");
		// return -1;
	}
// printf("ti skata\n");
	
	return sendMssg_viaSocketExisting(sock, mssg);

}

int sendMssg_viaSocketExisting(int sock, char* mssg){
	// PREP MSS TO SEND
	mySend(mssg, strlen(mssg), BSIZE, sock);

	// LEAVE SOCK OPENED AND RETURN IT
	return sock;
}

int getMssg(int myPipe, int bufSize, char** myline){
	struct pollfd myfd;
	myfd.fd = myPipe;
	myfd.events = POLLIN;

	while(poll(&myfd, 1, 0) <= 0){

	}


	char* inbuf = malloc(bufSize+1);
	memset(inbuf, 0, bufSize+1);

	int mssgSize = 0;
	read(myPipe, inbuf, bufSize);

	if(strlen(inbuf) == 0){
		free(inbuf);
		return -1;
	}

	// printf("i read: %s\n", inbuf);

	sscanf(inbuf, "%d", &mssgSize);

	*myline = malloc(mssgSize+1);
	memset(*myline, 0, mssgSize+1);
	// sleep(1);

	struct pollfd myfd2;
	myfd2.fd = myPipe;
	myfd2.events = POLLIN;

	while(poll(&myfd2, 1, 0) <= 0){

	}


	read(myPipe, inbuf, bufSize);
// printf("i read: %s\n", inbuf);

	while(inbuf[strlen(inbuf)-1] != '~'){
		// printf("tok: %s\n", tok);
		strcat(*myline, inbuf);
		memset(inbuf, 0, bufSize);
		read(myPipe, inbuf, bufSize);

	}

	strcat(*myline, inbuf);
	free(inbuf);
	return 1;
}

char* getMssg_viaSocket(int sock){

	// while(poll(&myfd, 1, 0) <= 0){

	// }
	char* mssg = NULL;
	getMssg(sock, BSIZE, &mssg);

	// close(sock);
	return mssg;
}

////////////////////////////////////////////

WaitingList* WaitingList_create(){
	WaitingList* newNode = malloc(sizeof(WaitingList));
	newNode->head = NULL;
	newNode->entries = 0;
	newNode->cur = 0;

	return newNode;
}

void WaitingList_destroy(WaitingList* self){
	ListNode* temp = self->head;
	while(temp != NULL){
		self->head = temp->next;
		free(temp);
		temp = self->head;
	}
	free(self);
}

void WaitingList_add(WaitingList* self, int pipe){
	ListNode* newNode = ListNode_create(pipe);

	if(self->head == NULL){
		self->head = newNode;
	}
	else{
		ListNode* temp = self->head;
		while(temp->next != NULL){
			temp = temp->next;
		}
		temp->next = newNode;
	}

	self->entries++;
}

void WaitingList_popPipe(WaitingList* self, int pipe){
	ListNode* temp = self->head;
	if(temp->pipe == pipe){
		self->head = temp->next;
		ListNode_destroy(temp);
		self->entries--;
		return;
	}
	while(temp != NULL){
		ListNode* temp2 = temp->next;
		if(temp2->pipe == pipe){
			temp->next = temp2->next;
			ListNode_destroy(temp2);
			self->entries--;
			return;
		}
		temp = temp2;
	}
	return;
}

int WaitingList_getPipe(WaitingList* self){
	if(self->entries == 0)
		return -1;

	if(self->cur >= self->entries)
		self->cur = 0;

	int count = 0;
	ListNode* temp = self->head;
	while(count < self->cur){
		temp = temp->next;
		count++;
	}
	self->cur++;
	return temp->pipe;
}

int WaitingList_Empty(WaitingList* self){
	if(self->entries == 0)
		return 1;
	return 0;
}

/////////////////////////////

ListNode* ListNode_create(int pipe){
	ListNode* newNode = malloc(sizeof(ListNode));
	newNode->pipe = pipe;
	newNode->next = NULL;

	return newNode;
}

void ListNode_destroy(ListNode* self){
	free(self);
}
