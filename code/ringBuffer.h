
typedef struct ringBuffer RB;
typedef struct info info;

struct ringBuffer{
	info** buffer;

	int len;
	int indexes;
	int totalInputs;
	int writeIndex;
	int readIndex;
	
};

struct info{
	char* ip;
	unsigned int port;
	char* target; // "queue" - "worker"

	int fd;

};

RB* ringBuffer_Init(int);
void ringBuffer_Destroy(RB*);

info* info_Init();
void info_Set(info*, char*, unsigned int, char*, int);
void info_Destroy(info*);
info* info_Copy(info* to_copy);

info* getItem(RB*);
int placeItem(RB*, info*);
int isFull(RB*);
int isEmpty(RB*);
void RB_printSize(RB*);
void RB_printInfo(RB*);