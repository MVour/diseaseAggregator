
typedef struct myPatient myPatient;

struct myPatient{
	char* id;
	char* fName;
	char* lName;
	char* diseaseID;
	char* country;
	char* entrDate;
	char* exitDate;
	int age;
};


////// Patient Functs

myPatient* patient_create(char*, char*, char*, char*, char*, char*, char*);
void patient_destroy(myPatient*);


int cmpDate(char*, char*);
void patient_addExitDate(myPatient*, char*);
char* pat_toString(myPatient*);