#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#include <sys/wait.h> /* sockets */
#include <netdb.h> /* gethostbyaddr */
#include <unistd.h> /* fork */
#include <stdlib.h> /* exit */
#include <ctype.h> /* toupper */
#include <signal.h> 


void mySend(char* out, int outSize, int bufSize, int myPipe);
int sendMssg_viaSocket(unsigned int port, char* ip, char* mssg);
int sendMssg_viaSocketExisting(int, char*);
int getMssg(int myPipe, int bufSize, char** myline);
char* getMssg_viaSocket(int);

typedef struct ListNode ListNode;

typedef struct WaitingList WaitingList;

struct ListNode{
	int pipe;
	ListNode* next;
};

struct WaitingList{
	ListNode* head;
	int entries;
	int cur;
};


WaitingList* WaitingList_create();
void WaitingList_destroy(WaitingList* self);
void WaitingList_popPipe(WaitingList* self, int pipe);
int WaitingList_getPipe(WaitingList* self);
void WaitingList_add(WaitingList* self, int pipe);
int WaitingList_Empty(WaitingList* self);

ListNode* ListNode_create(int pipe);
void ListNode_destroy(ListNode* self);