#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Windows inclusive screen cleaner
// I am not using clrscr directly because mingw doesn't support clrscr
#ifdef _WIN32
#define clrscr() system("cls");
#else
#define clrscr() printf("\e[1;1H\e[2J");
#endif

enum sanitizertype{
	INTEGER,
	STRING
};

struct studentIDNode{
	int studentID;
	struct studentIDNode *idnext;
};

struct studentNameNode{
	char *studentName;
	struct studentNameNode *namenext;
};

typedef struct studentIDNode IDNode;
typedef struct studentNameNode NameNode;

IDNode *idhead = NULL;
NameNode *namehead = NULL;

int AddStudent(int IDNo, char *Name){
	IDNode *IDcur = idhead;
	NameNode *Namecur= namehead;


	// I just copied the conflift change from UpdateStudent() please refer to there if you want to understand what i am doing
	// Only difference here is that we dont rewind because we dont need to search the whole list to find what we needed to change 
	// we can just add it to the next and be done.
	while (IDcur != NULL && Namecur != NULL) {
		if (IDcur->studentID == IDNo || strcmp(Namecur->studentName, Name) == 0) {
			break;
		}
		IDcur=IDcur->idnext;
		Namecur=Namecur->namenext;
	}

	if (IDcur != NULL || Namecur != NULL) {
		printf("User input conflicts with the student list(Already in use): %s/%d\n", Name, IDNo);
	}
	else {
		IDNode *IDnewItem=(IDNode *)malloc(sizeof(IDNode)*1);
		if (IDnewItem == NULL) return 1; // Memory allocation failure.


		NameNode *NamenewItem=(NameNode *)malloc(sizeof(NameNode)*1);
		if (NamenewItem == NULL){ 
			free(IDnewItem);
			return 1; // Memory allocation failure.
		}
		NamenewItem->studentName = (char *)malloc(strlen(Name) + 1);
		if (NamenewItem->studentName ==NULL){
			free(NamenewItem);
			free(IDnewItem);
			return 1; // Memory allocation failure.
		}
		IDnewItem->studentID=IDNo;
		IDnewItem->idnext=idhead;
		idhead=IDnewItem;	
		strcpy(NamenewItem->studentName, Name);
		NamenewItem->namenext=namehead;
		namehead=NamenewItem;
		printf("Student %s with the number of %d is added to the list. \n",namehead->studentName, idhead->studentID);
	}
	return 0; // No memory allocation fail. 
}

void DeleteStudent(int IDNo, char *Name){
	NameNode *Namecur = namehead;
	NameNode *Nameprev = NULL;
	
	IDNode *IDcur = idhead;
	IDNode *IDprev = NULL;
	while (IDcur != NULL && Namecur != NULL) {
		if (IDcur->studentID == IDNo && strcmp(Namecur->studentName, Name) == 0) {

			// This printf is only here because otherwise we cannot print the studentName and studentID outside of this if condition
			// since we freed them and i want to see what happens instead of outputing the user input directly. 
			// This would only run if the check succeeds so its safe and there's no reson for free to fail.
			printf("Student %s with the number of %d is removed from the list. \n",Namecur->studentName, IDcur->studentID);

			// ID Deletion
			if (IDcur == idhead) {
				idhead = IDcur->idnext;
				free(IDcur);
			} else {
				IDprev->idnext = IDcur->idnext;
				free(IDcur);
			}
			// Name Deletion
			if (Namecur == namehead) {
				namehead = Namecur->namenext;
				free(Namecur->studentName);
				free(Namecur);
			} else {
				Nameprev->namenext = Namecur->namenext;
				free(Namecur->studentName);
				free(Namecur);
			}

			return; // To prevent looking for the rest of the node and prevent the deletion message
				// This exists solely to complete the entire function without executing the deletion message etc.
				// We want this because otherwise we need to move the cur's and look whether or not deletion succeded one by one.
		} 
		IDprev = IDcur;
		IDcur = IDcur->idnext;
		Nameprev = Namecur;
		Namecur = Namecur->namenext;
	}
	printf("Student info deletion failed.\n");
}

int UpdateStudent(int IDNo, char *Name, int newID, char *newName){
	IDNode *IDcur = idhead;
	NameNode *Namecur = namehead;
	
	// Input conflict check (I know there's the same while condition beneath this but if we merge them together 
	// then we can only detect conflict after we make a change to the linked list so i am just keeping this seperate i just dont wanna deal with it)
	while (IDcur != NULL && Namecur != NULL) {
		if ( (IDcur->studentID == newID && newID != IDNo) || (strcmp(Namecur->studentName, newName) == 0 && strcmp(newName, Name) != 0) ) {
			break;
		}
		IDcur=IDcur->idnext;
		Namecur=Namecur->namenext;
	}
	if (IDcur != NULL || Namecur != NULL) {
		printf("User input conflicts with the student list(Already in use): %s/%d\n", newName, newID);
	}
		// We move cursors to the next so we can check whether it will return null or not.
		// If it returns null then it would be correct since that would be the end of the list
		// If it aint then its a duplicate since the loop didn't finish properly due to the break in the while loop.
		// If it is then in order to do the update we must rewind the changes we made to the list and reset the current like we did in the else statement.
		// I am doing this because i dont wanna add a check variable and bunch of if statements this is just easier.
	else {
		IDcur=idhead;
		Namecur=namehead;
		while (IDcur != NULL && Namecur != NULL) {
			if (IDcur->studentID == IDNo && strcmp(Namecur->studentName, Name) == 0 ) { // student will only be updated if the input matches the records
				IDcur->studentID=newID;
				free(Namecur->studentName);
				
				Namecur->studentName = (char *)malloc(strlen(newName) + 1);
				if (Namecur->studentName == NULL) return 1; // Memory allocation failed.

				strcpy(Namecur->studentName, newName);
				printf("Student Name is changed from: %s to %s\nStudent Number is changed from %d to %d\n", Name,Namecur->studentName,IDNo,IDcur->studentID);
				break;
			}
			IDcur=IDcur->idnext;
			Namecur=Namecur->namenext;
		}
		if (IDcur == NULL || Namecur == NULL) {
			printf("Student info change failed.\n");
		}
	}
	return 0; // No memory allocation fail.
}

void displayID(){
	IDNode *IDcur=idhead;

	if (idhead == NULL){
		return;
	}

	for (; IDcur!=NULL; IDcur=IDcur->idnext) {
		printf("Student ID: %d\n",IDcur->studentID);
	} }

void displayName(){
	NameNode *Namecur=namehead;

	if (namehead == NULL){
		return;
	}

	for (; Namecur!=NULL; Namecur=Namecur->namenext) {
		printf("Student Name: %s\n",Namecur->studentName);
	} }

void FindID(int IDNo){
	IDNode *IDcur = idhead;
	NameNode *Namecur = namehead;

	while (IDcur != NULL && Namecur != NULL) {
		if (IDcur->studentID == IDNo) {
			printf("Student Name: %s\n", Namecur->studentName);
			break;
		}
		IDcur=IDcur->idnext;
		Namecur=Namecur->namenext;
	}
	if (IDcur == NULL) {
		printf("Search failed.\n");
	}
}

void FindName(char *Name){
	IDNode *IDcur = idhead;
	NameNode *Namecur = namehead;

	while (IDcur != NULL && Namecur != NULL) {
		if (strcmp(Namecur->studentName, Name) == 0) {
			printf("Student ID: %d\n", IDcur->studentID);
			break;
		}
		IDcur=IDcur->idnext;
		Namecur=Namecur->namenext;
	}
	if (IDcur == NULL || Namecur == NULL) { 
		printf("Search failed.\n");
	}
}

int StringSanitizer(char *buffer, enum sanitizertype type){
	if (buffer == NULL) return 0;

	if (type == INTEGER) {
		if (strchr(buffer, '\n') == NULL) {
			while (getchar() != '\n' && feof(stdin) == 0); // feof exists in order to prevent EOF passing as legitimate when pressing CTRL+D
			return 0;
		}
		buffer[strcspn(buffer, "\n")] = '\0'; // buffer will directly point to the inputno and do the change in that variable as well.
						      // this is not some local variable edit it would edit the main function fgets input.
		if (strspn(buffer, "0123456789") == strlen(buffer)) {
			return atoi(buffer); // string modification success
		}
	}

	if (type == STRING) {	
		if (strchr(buffer, '\n') == NULL) {
			while (getchar() != '\n' && feof(stdin) == 0); // feof exists in order to prevent EOF passing as legitimate when pressing CTRL+D
			return 0;
		}
		buffer[strcspn(buffer, "\n")] = '\0'; // buffer will directly point to the inputname and do the change in that variable as well.
						      // this is not some local variable edit it would edit the main function fgets input.
		if (buffer[0] == '\0' || strspn(buffer, " \t") == strlen(buffer)) return 0;

		if (strpbrk(buffer, "0123456789") != NULL) {
			return 0; // Names can't have integers and strspn is not enough since someone can write a1 etc. and it will not detect since its a string
				  // strpbrk would look up every line in the string and if it finds any integer in this case it would return something besides NULL so check works.
		}
		return 1; // string modification success
	}
	return 0;
}
void FreeNodes(){
	IDNode *IDcur = idhead;
	NameNode *Namecur =namehead;
	while (IDcur != NULL) {
		idhead = IDcur->idnext;
		free(IDcur);
		IDcur=idhead;
	}


	while (Namecur != NULL) {
		namehead = Namecur->namenext;
		free(Namecur->studentName);
		free(Namecur);
		Namecur=namehead;
	}
}

int main(){
	atexit(FreeNodes); // Please do remember that due to this every exit call will clear the memory before exiting.
	signal(SIGINT, exit);
	int inputno;
	int choice=0;
	int newID;
	char inputbuffer[11];
	char newName[25];
	char inputname[25];
	do {
		printf("Select your course of action:\n1- Add a student\n2- Delete a student\n3- Update the student information\n4- Find a student via ID no\n5- Search a student via name\n6- Display all the student ID numbers\n7- Display all the student names\n8- Exit the program\n");
	
		// this stuff is just to replace scanf
		// I dont want scanf since i cant gurantee that someone wouldn't just type letters in the integer input such as inputs 1a 126asd etc.
		fgets(inputbuffer, sizeof(inputbuffer), stdin);
		choice=StringSanitizer(inputbuffer, INTEGER);
		switch (choice) {
			case 1:
				clrscr();
				printf("Input the ID No (MAX 9 DIGITS):\n");
				do { 
					inputno=0; // just in case inputno gets assigned to any other number in one of the switchcases.
					fgets(inputbuffer, sizeof(inputbuffer), stdin);
					inputno=StringSanitizer(inputbuffer, INTEGER);
				}while (inputno <= 0 && printf("Invalid input, try again.\n")); // If the string input is empty ask again.
	
				printf("Input the student name: (MAX 25 CHARACTER)\n");
				do { 
					fgets(inputname, sizeof(inputname), stdin);
				}while (StringSanitizer(inputname, STRING) == 0 && printf("Invalid input, try again.\n"));
			
				if (AddStudent(inputno, inputname) == 1) {
					fprintf(stderr, "Premature exit: Memory Allocation Failed"); // stderr is unbuffered it would return this message immediately.
					exit(EXIT_FAILURE); // Calls FreeNodes() due to atexit
				}
				break;
			case 2:
				clrscr();
				printf("Input the ID No (MAX 9 DIGITS):\n");

				do { 
					inputno=0; // just in case inputno gets assigned to any other number in one of the switchcases.
					fgets(inputbuffer, sizeof(inputbuffer), stdin); 
					inputno=StringSanitizer(inputbuffer, INTEGER);
				}while (inputno <= 0 && printf("Invalid input, try again.\n")); // If the string input is empty ask again.
				
				printf("Input the student name: (MAX 25 CHARACTER)\n");
				do { 
					fgets(inputname, sizeof(inputname), stdin);
				}while (StringSanitizer(inputname, STRING) == 0 && printf("Invalid input, try again.\n")); // IF the string input is empty ask again.
			
				DeleteStudent(inputno,inputname);
				break;
			case 3:
				clrscr();
				printf("Input the ID No (MAX 9 DIGITS):\n");
				do { 
					inputno=0; // just in case inputno gets assigned to any other number in one of the switchcases.
					fgets(inputbuffer, sizeof(inputbuffer), stdin);
					inputno=StringSanitizer(inputbuffer, INTEGER);
				}while (inputno <= 0 && printf("Invalid input, try again.\n")); // If the string input is empty ask again.
	
				printf("Input the student name: (MAX 25 CHARACTER)\n");
				do { 
					fgets(inputname, sizeof(inputname), stdin);
				}while (StringSanitizer(inputname, STRING) == 0 && printf("Invalid input, try again.\n")); // If the string input is empty ask again.
	
				printf("Input the new ID No (MAX 9 DIGITS):\n");
				do { 
					newID=0; // just in case inputno gets assigned to any other number in one of the switchcases.
					fgets(inputbuffer, sizeof(inputbuffer), stdin);
					newID=StringSanitizer(inputbuffer, INTEGER);
				}while (newID <= 0 && printf("Invalid input, try again.\n")); // If the string input is empty ask again.
	
				printf("Input the new student name: (MAX 25 CHARACTER)\n");
				do { 
					fgets(newName, sizeof(newName), stdin);
				}while (StringSanitizer(newName, STRING) == 0 && printf("Invalid input, try again.\n")); // If the string input is empty ask again
		
				if (UpdateStudent(inputno,inputname,newID,newName) == 1) {
					fprintf(stderr, "Premature exit: Memory Allocation Failed"); // stderr is unbuffered it would return this message immediately.
					exit(EXIT_FAILURE); // Calls FreeNodes() due to atexit
				}
				break;
			case 4:
				clrscr();
				printf("Input the ID No (MAX 9 DIGITS):\n");
				do { 
					inputno=0; // just in case inputno gets assigned to any other number in one of the switchcases.
					fgets(inputbuffer, sizeof(inputbuffer), stdin);
					inputno=StringSanitizer(inputbuffer, INTEGER);
				}while (inputno <= 0 && printf("Invalid input, try again.\n")); // If the string input is empty ask again.
				
				FindID(inputno);
				break;
			case 5:
				clrscr();
				printf("Input the student name: (MAX 25 CHARACTER)\n");
				do { 
					fgets(inputname, sizeof(inputname), stdin);
				}while (StringSanitizer(inputname, STRING) == 0 && printf("Invalid input, try again.\n")); // If the string input is empty ask again.
				
				FindName(inputname);
				break;
			case 6:
				clrscr();
				displayID();
				break;
			case 7:
				clrscr();
				displayName();
				break;
			case 8: 
				exit(EXIT_SUCCESS); // Due to atexit(FreeNodes) this will clear the memory and then close the app.
				break;
			default:
				printf("Unknown behaviour.\n");
				break;
		}
	}while (choice>=0 && choice <= 8);


	return 0;
}
