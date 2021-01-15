#make
CC = gcc
CFLAGS = -Wall -g

all:	master\
		worker\
		whoServer\
		whoClient\
# 		whoClient2\

#####################################################################
#####################################################################

master: main.o main_functs.o mssgHandling.o filesStats.o
	$(CC) $(CFLAGS) -o master main.o main_functs.o mssgHandling.o filesStats.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

main_functs.o: main_functs.c main_functs.h 
	$(CC) $(CFLAGS) -c main_functs.c 

########################################################

worker: worker.o functs.o myHash.o myPatient.o RBT.o myList.o mssgHandling.o filesStats.o heap.o
	$(CC) $(CFLAGS) -o worker worker.o functs.o myHash.o myPatient.o RBT.o myList.o mssgHandling.o filesStats.o heap.o

worker.o: worker.c
	$(CC) $(CFLAGS) -c worker.c

########################################################

whoServer: whoServer.o serverFuncts.o filesStats.o myThreads.o mssgHandling.o ringBuffer.o
	$(CC) $(CFLAGS) -o whoServer whoServer.o filesStats.o serverFuncts.o mssgHandling.o myThreads.o ringBuffer.o -lpthread

whoServer.o: whoServer.c whoServer.h
	$(CC) $(CFLAGS) -c whoServer.c


########################################################

whoClient: whoClient.o clientFuncts.o myThreads.o mssgHandling.o
	$(CC) $(CFLAGS) -o whoClient whoClient.o clientFuncts.o mssgHandling.o myThreads.o -lpthread

whoClient.o: whoClient.c whoClient.h
	$(CC) $(CFLAGS) -c whoClient.c

whoClient2: whoClient2.o clientFuncts2.o myThreads.o mssgHandling.o
	$(CC) $(CFLAGS) -o whoClient2 whoClient2.o clientFuncts2.o mssgHandling.o myThreads.o -lpthread

whoClient2.o: whoClient2.c whoClient.h
	$(CC) $(CFLAGS) -c whoClient2.c


########################################################


myHash.o: myHash.c myHash.h
	$(CC) $(CFLAGS) -c myHash.c

myPatient.o: myPatient.c myPatient.h
	$(CC) $(CFLAGS) -c myPatient.c

RBT.o: RBT.c RBT.h
	$(CC) $(CFLAGS) -c RBT.c

myList.o: myList.c myList.h
	$(CC) $(CFLAGS) -c myList.c

mssgHandling.o: mssgHandling.c mssgHandling.h
	$(CC) $(CFLAGS) -c mssgHandling.c 

filesStats.o: filesStats.c filesStats.h
	$(CC) $(CFLAGS) -c filesStats.c 

functs.o: functs.c functs.h
	$(CC) $(CFLAGS) -c functs.c

heap.o: heap.c heap.h
	$(CC) $(CFLAGS) -c heap.c

ringBuffer.o: ringBuffer.c ringBuffer.h
	$(CC) $(CFLAGS) -c ringBuffer.c

serverFuncts.o: serverFuncts.c whoServer.h
	$(CC) $(CFLAGS) -c serverFuncts.c -lpthread

clientFuncts.o: clientFuncts.c whoClient.h
	$(CC) $(CFLAGS) -c clientFuncts.c -lpthread

clientFuncts2.o: clientFuncts2.c whoClient.h
	$(CC) $(CFLAGS) -c clientFuncts2.c -lpthread

myThreads.o: myThreads.c myThreads.h
	$(CC) $(CFLAGS) -c myThreads.c -lpthread


#####################################################################
#####################################################################

.Phony: clean

clean:
	 rm master worker whoClient whoServer *.o log*
