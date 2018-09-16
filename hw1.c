/*
  Author: Shitij Vishwakarma
  Email: svishwakarma2015@my.fit.edu
  Course: CSE 2010 Algorithms and Data Structures
  Section: 02
  Description: The C program below is supposed to simulate a chat session. 
  Representatives: Alice, Bob, Carol, David, and Emily are under the class 
  of REPLIST. These are linked lists that take in and temporarily store the 
  information of their current customers. The entire program is time driven. 
  More specifically, the end time of the chats. The approach taken in this
  program is as follows:
  	-Initialize the memory with Representatives "waiting for their first 
  	call session to start".
  	-Each line from the file is read and catogorized.
  	-The ones that start with "ChatRquest" are separated as nodes of linked 
  	list of type "CHAT". Info for each customer is saved in it. 
  	-By calculating the end time for each customer, the values of their 
  	names, chat start time, and chat end time is stored in the REPLIST 
  	struct (with their respective reps). Once the list of representatives 
  	is full, it is sorted by end times, in accending order. 
  	-Doing so provides us with easy calculation of "MaxWaitTime" when asked.
  	-Gives us the the oppertunity to easily maintain the chats, and end them
  	sequentially.
  	-After the sorting, for the rest of the program: when each chat session ends, 
  	the REPLIST node is moved to the end of the list whilst deleting the 
  	customer info WITH the customer node from the "CHAT" list as well. A message 
  	"ChatEnded" with the timestamp from the rep node is displayed on the screen.
  	-The Customer node that was on hold (more specifically the Customer node
  	that was not assigned to a rep, from the list of "CHAT"), gets assigned 
  	to the newly added end-of-the-list rep node.
  	-This algorithm is followed for the entire program until EOF is reached. 
 */
//the following libraries are used in this program:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_BUFFER 40000
#define EMPTY 0
#define FULL 1
#define HOLDING 0
#define TAKEN 1
typedef struct CHAT{
	char request_type[22];	//struct type "CHAT" stores the information of the
	float RequestTime;	//each customers. "request_type[]" stores the "ChatRequest"
	char customer[3];	//"RequestTime" stores the time of request.
	float ChatDuration;	//"customer[]" stores the detail of its customer
	char decision[6];	//ChatDuration stores the duration of the chat spcified 
	float StartTime;	//by the customer. "decision" stores whther they want
	float EndTime;		//to wait or try later.
	float quit_on_hold_time;	//quit_on_hold_time stores if the customer chooses to quit
	int status;				//*link is the pointer to the link that stores the
	struct REPLIST *link;	 //info about the rep the customer is in chat with
	struct CHAT *next;		//*next is a point to the next CHAT node in the list.
}CHAT;
typedef struct QUEUE_C{
	CHAT *head;
	CHAT *tail;
	int size;
}QUEUE_C;

//REPLIST is the struct that stores the info of the reps
//avaible. RepName stores the name of the rep.
//status stores the status whether the rep is available or occupied
//*link_r is a pointer to CHAT struct. It links the assigned customer 
//to the rep.
//*next pointer points to the next node in REPLIST.
typedef struct REPLIST{
	char RepName[6];
	int status;
	struct CHAT *link_r;
	struct REPLIST *next;
}REPLIST;
typedef struct QUEUE_R{			//QUEUE_R is a pointer to REPLIST. it stores the head and the 
	REPLIST *head;				//tail pointer that point to the beggining (head) and the 
	REPLIST *tail;				//end (tail) of the REPLIST list.
}QUEUE_R;

//the following functions will be used in this program:
void PrintRequestQueues(QUEUE_C *CustomerQueue);
void GoOnline(QUEUE_R *RepQueue);
void ChatRoom(QUEUE_C *CustomerQueue, QUEUE_R *RepQueue);
int PushBack(REPLIST *rep_mover, QUEUE_R *RepQueue);
void PrintRepQueues(QUEUE_R *RepQueue);

//input from command line: 
int main(int argc, char *argv[]){
	FILE *file_ptr;
	file_ptr = fopen(argv[1], "r");
	if(file_ptr == NULL){
		perror("File missing. ");
		return(-1);
	}
	QUEUE_C CustomerQueue;					//CustomerQueue is pointer to the SESH linked list. This holds the main head and tail pointers for SESH.
	QUEUE_R RepQueue;						//QUEUE_R is the pointer to the REPLIST linked list. It contains the main head and tail pointer for REPLIST.

	CustomerQueue.head = NULL;				//INITAILIZING the CustomerQueue pointer.
	CustomerQueue.tail = NULL;
	CustomerQueue.size = 0;					//CustomerQueue.size stores the size of the linked list.

	RepQueue.head = NULL;					//Initializing the pointer to REPLIST linked list.
	RepQueue.tail = NULL;
	char data_highlighter[MAX_BUFFER];

	while(fgets(data_highlighter, MAX_BUFFER, file_ptr) != NULL){

		char *token = strtok(data_highlighter, " ");

		if((strcmp(token, "ChatRequest")) == 0){

			char request[12], customer_id[3], decision[6];
			float duration;
            double RequestTime;
            CHAT *NewRequest = (CHAT*)malloc(sizeof(CHAT));				//NewRequest is the new node for the chat command.
																		//char *token is a pointer to a char that temporarily stores the value of tokenized field.
			strcpy(request, token);										//for tokenizing the function "strtok" is used, that tokenizes the first field (and the following
			strcpy(NewRequest->request_type, request);					//feilds) until the specified condition. In our case it a single space " ".
																		//RequestTime is the time when the chat was requested. It is stored as a float in order to make the
			token = strtok(NULL, " ");									//the time conversion from minutes to 24 HR clock easier. "duration" is the duration of the chat requested
			RequestTime = atof(token);									//by the customer. NewRequest node represents each new chat request.
			NewRequest->RequestTime = RequestTime;

			token = strtok(NULL, " ");
			strcpy(customer_id, token);
			strcpy(NewRequest->customer, customer_id);

			token = strtok(NULL, " ");
			duration = atof(token);
			NewRequest->ChatDuration = duration;

			token = strtok(NULL, " ");
			strcpy(decision, token);
			strcpy(NewRequest->decision, decision);

			NewRequest->status = HOLDING;

			if(CustomerQueue.size == 0){								//if statement to create a linked list. It sets the head and tail pointer to the first node ever (when CustomerQueue.size = 0);
				CustomerQueue.tail = NewRequest;
				CustomerQueue.head = NewRequest;
			}
			else{														//and then consecutively increases the size of the linked list and move the tail pointer accordingly, with the addition of
				CustomerQueue.tail->next = NewRequest;					//each node.
				CustomerQueue.tail = NewRequest;
			}
			(CustomerQueue.size)++;
		}


		else if((strcmp(token, "QuitOnHold")) == 0){

			char quit[5], customer_id[3];
			float quit_time;

			CHAT *NewRequest = (CHAT*)malloc(sizeof(CHAT));

			strcpy(quit, token);
			strcpy(NewRequest->request_type, quit);

			token = strtok(NULL, " ");
			NewRequest->quit_on_hold_time = atof(token);

			token = strtok(NULL, " ");
			strcpy(customer_id, token);
			strcpy(NewRequest->customer, customer_id);


			if(CustomerQueue.size == 0){								//if statement to create a linked list. It sets the head and tail pointer to the first node ever (when CustomerQueue.size = 0);
				CustomerQueue.tail = NewRequest;
				CustomerQueue.head = NewRequest;
			}
			else{														//and then consecutively increases the size of the linked list and move the tail pointer accordingly, with the addition of
				CustomerQueue.tail->next = NewRequest;					//each node.
				CustomerQueue.tail = NewRequest;
			}
			(CustomerQueue.size)++;
		}


		else if((strcmp(token, "PrintAvailableRepList")) == 0 || (strcmp(token, "PrintAvailableRepList\r\n")) == 0){
			int length = strlen(token), i;
			char avaiable_rep[22];
			float RequestTime;

			if((strcmp(token, "PrintAvailableRepList\r\n")) == 0){
				for(i = 0; i < length+1; i++){
					printf("%c", data_highlighter[i]);

					if(data_highlighter[i] == '\n'){
						data_highlighter[i] = '\0';
					}
				}
			}

			CHAT *NewRequest = (CHAT*)malloc(sizeof(CHAT));

			strcpy(avaiable_rep, token);
			strcpy(NewRequest->request_type, avaiable_rep);

			token = strtok(NULL, " ");

			RequestTime = atof(token);
			NewRequest->RequestTime = RequestTime;

			if(CustomerQueue.size == 0){								//if statement to create a linked list. It sets the head and tail pointer to the first node ever (when CustomerQueue.size = 0);
				CustomerQueue.tail = NewRequest;
				CustomerQueue.head = NewRequest;
			}
			else{														//and then consecutively increases the size of the linked list and move the tail pointer accordingly, with the addition of
				CustomerQueue.tail->next = NewRequest;					//each node.
				CustomerQueue.tail = NewRequest;
			}
			(CustomerQueue.size)++;
		}
		else if((strcmp(token, "PrintMaxWaitTime")) == 0){

			CHAT *NewRequest = (CHAT*)malloc(sizeof(CHAT));

			strcpy(NewRequest->request_type, token);

			token = strtok(NULL, " ");
			NewRequest->RequestTime = atof(token);

			if(CustomerQueue.size == 0){								//if statement to create a linked list. It sets the head and tail pointer to the first node ever (when CustomerQueue.size = 0);
				CustomerQueue.tail = NewRequest;
				CustomerQueue.head = NewRequest;
			}
			else{														//and then consecutively increases the size of the linked list and move the tail pointer accordingly, with the addition of
				CustomerQueue.tail->next = NewRequest;					//each node.
				CustomerQueue.tail = NewRequest;
			}
			(CustomerQueue.size)++;
		}
		else{
			printf("flag6\n");
			printf("Invalid Command.\n");
		}
	}
	//PrintRequestQueues(&CustomerQueue);
	GoOnline(&RepQueue);
	//PrintRepQueues(&RepQueue);
	ChatRoom(&CustomerQueue, &RepQueue);
	fclose(file_ptr);
}

/*void PrintRequestQueues(QUEUE_C *CustomerQueue){
	CHAT *TempNode;
	TempNode = CustomerQueue->head;
	int i = 0;
	while(TempNode != NULL){
		if((strcmp(TempNode->request_type, "ChatRequest")) == 0){
			i++;
			printf("%s %.0lf %s %.0lf %s\n", TempNode->request_type, TempNode->RequestTime, TempNode->customer, TempNode->ChatDuration, TempNode->decision);
			TempNode = TempNode->next;
		}
		else if((strcmp(TempNode->request_type, "QuitOnHold")) == 0){
			i++;
			printf("%s %.0lf %s\n", TempNode->request_type, TempNode->quit_on_hold_time, TempNode->customer);
			TempNode = TempNode->next;
		}
		else if((strcmp(TempNode->request_type, "PrintAvailableRepList")) == 0){
			i++;
			printf("%s %.0lf\n", TempNode->request_type, TempNode->RequestTime);
			TempNode = TempNode->next;
		}
		else if((strcmp(TempNode->request_type, "PrintMaxWaitTime")) == 0){
			i++;
			printf("%s %.0lf\n", TempNode->request_type, TempNode->RequestTime);
			TempNode = TempNode->next;
		}
	}
}*/


void GoOnline(QUEUE_R *RepQueue){									//forms the rep list. Links every rep in order: Alice, Bob, Carol, Daivid, Emily

	int i;
	for(i = 0; i < 5; i++){
		REPLIST *NewRep = (REPLIST*)malloc(sizeof(REPLIST));		//NewRep represents the new node
		NewRep->next = NULL;
		if(i == 0){
			strcpy(NewRep->RepName, "ALice");
			NewRep->status = EMPTY;
		}
		else if(i == 1){
			strcpy(NewRep->RepName, "Bob");
			NewRep->status = EMPTY;
		}
		else if(i == 2){
			strcpy(NewRep->RepName, "Carol");
			NewRep->status = EMPTY;
		}
		else if(i == 3){
			strcpy(NewRep->RepName, "David");
			NewRep->status = EMPTY;
		}
		else if(i == 4){
			strcpy(NewRep->RepName, "Emily");
			NewRep->status = EMPTY;
		}

		if(i == 0){
			RepQueue->tail = NewRep;
			RepQueue->head = NewRep;
		}
		else if(i > 0 && i < 5){
			RepQueue->tail->next = NewRep;
			RepQueue->tail = NewRep;
		}
	}
}


//the function ChatRoom below is assigns a rep to every new customer if all the reps are not busy. 
//if they are vusy, the function keeps the cutomers on hold until they are done.

void ChatRoom(QUEUE_C *CustomerQueue, QUEUE_R *RepQueue){
	int i;
	REPLIST *temp_r, *rep_mover; //pointers to REPLIST
	CHAT *temp_c; //pointer to CHAT
	temp_r = RepQueue->head;
	temp_c = CustomerQueue->head;

	while(temp_c != NULL){
		if((strcmp(temp_c->request_type, "ChatRequest")) == 0){
			if(temp_c->status == HOLDING){
				if(temp_r->status == EMPTY){
					//printf("temp_r is: %s\n", temp_r->RepName);
					temp_c->link = temp_r;
					temp_c->status = TAKEN;
					if(temp_c->RequestTime < 1000){
						printf("RepAssignment %s %s 0%.0lf\n", temp_c->link->RepName, temp_c->customer, temp_c->RequestTime);
					}
					else if(temp_c->RequestTime > 1000){
						printf("RepAssignment %s %s %lf", temp_c->link->RepName, temp_c->customer, temp_c->RequestTime);
					}
					
					temp_c = temp_c->next;
					rep_mover = temp_r;
					RepQueue->head = RepQueue->head->next;
					temp_r = RepQueue->head;
					PushBack(rep_mover, RepQueue);
				}
				else if(temp_r->status == FULL){
					//printf("%s %d\n", temp_r->RepName, temp_r->status);
					if(temp_c->RequestTime < 1000){
						printf("PutOnHold %s 0%.0lf\n", temp_c->customer, temp_c->RequestTime);
					}
					else if(temp_c->RequestTime > 1000){
						printf("PutOnHold %s %.0lf\n", temp_c->customer, temp_c->RequestTime);
					}
					//printf("the rep list full\n");
					temp_c = temp_c->next;
				}
			}
		}
		else if((strcmp(temp_c->request_type, "PrintAvailableRepList")) == 0){
			printf("AvailableRepList ");
			if(temp_c->RequestTime < 1000){
				printf("0%.0lf ", temp_c->RequestTime);
			}
			else if(temp_c->RequestTime > 1000){
				printf("%.0lf ", temp_c->RequestTime);
			}

			while(temp_r->status != FULL){
				printf("%s ", temp_r->RepName);
				temp_r = temp_r->next;
			}
			printf("\n");
			temp_c = temp_c->next;
			//break;
		}
		else{
			temp_c = temp_c->next;
		}
	}
}


//PushBack function pushes every busy rep to the back of the REPLIST.
int PushBack(REPLIST *rep_mover, QUEUE_R *RepQueue){
	rep_mover->status = FULL;
	RepQueue->tail->next = rep_mover;
	RepQueue->tail = rep_mover;
	//PrintRepQueues(RepQueue);
	return(rep_mover->status);
}
 //prints the repqueue when called.
void PrintRepQueues(QUEUE_R *RepQueue){
	REPLIST *TempNode;
	TempNode = RepQueue->head;
	for(int i = 0; i < 5; i++){
		printf(" %s", TempNode->RepName);
		TempNode = TempNode->next;
	}
	printf("\n");
}