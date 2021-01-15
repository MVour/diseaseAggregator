typedef struct dateStats dateStats;
typedef struct diseaseNode diseaseNode;
typedef struct StatsList StatsList;
typedef struct ageNode ageNode;

struct StatsList{
	dateStats* head;
	int datesCount;
};

struct dateStats{
	char* date;
	char* country;
	int dcount;
	diseaseNode* dList;
	dateStats* next;
};

struct diseaseNode{
	char* disease;
	int pat_num;
	diseaseNode* next;
	ageNode* ageStats;
};

struct  ageNode{
	int group1; // 0-20
	int group2; // 21-40
	int group3; // 41-60
	int group4; // 60+
};


StatsList* StatsList_init();
// void StatsList_addRec(StatsList* self, char* date, char* country);
void StatsList_destroy(StatsList* self);
void StatsList_byString(StatsList* self, char* buf);
char* StatsList_toString(StatsList* self);
void StatsList_print(StatsList* self);
void StatsList_addDateRec(StatsList* self, dateStats*);

/////////////////////////////////////////////////

dateStats* dateStats_init(char* date, char* country);
void dateStats_destroy(dateStats* self);

// diseaseNode* dateStats_getD(char* disease);
void dateStats_addRec(dateStats* self, char* disease, char* country, int age);
void dateStats_print(dateStats* self);
char* dateStats_toString(dateStats* self);
// void dateStats_addDNode(diseaseNode* dNode);

////////////////////////////////////////////////

diseaseNode* diseaseNode_init(char* disease);
void diseaseNode_destroy(diseaseNode* self);

// void diseaseNode_addANode(ageNode* aNode);
void diseaseNode_addRec(diseaseNode* self, int age);
void diseaseNode_print(diseaseNode* self);
char* diseaseNode_toString(diseaseNode* self);

////////////////////////////////////////////////

ageNode* ageNode_init();
void ageNode_destroy(ageNode* self);

void ageNode_addRec(ageNode* self, int age);
void ageNode_print(ageNode* self);
char* ageNode_toString(ageNode* self);