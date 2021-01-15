#include "mssgHandling.h"
#include "filesStats.h"


char** giveThemNames(int i);

WaitingList* makeWaitingList(int** fdArray, int numWorkers);

StatsList* getStats(WaitingList* myList, int bufferSize);

void printMenu();

int listCountries(char*** CNames, int CCount);
int diseaseFreq(char* buf, int** fdArray, char*** CNames, int CCount, int bufSize);
int topk_AgeRanges(char*, int**, char***, int, int);
int searchPatientRecord(char*, int**, int, int);
int numPatientAdm_Dis(char*, int**, char***, int, int);

void make_log(int, int, int, char***, int);
void replaceChild(pid_t, char***, int, char***, int, char*, int**, char* , char*);
void clean_up(StatsList*, WaitingList*, int**, int, char***, int, char***, char*, char*, char*, char*);
void killemALL(char*** CNames, int CCount);
void getPorts(WaitingList*, int, char***);


void handler(int signum);
void exitHandler(int signum);
void deadChild();