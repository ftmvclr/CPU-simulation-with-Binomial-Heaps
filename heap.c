/* FATIMA AVCILAR 150123017*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

struct binomialRoot;
typedef struct process{
	struct process *parent;
	struct process *child;
	struct process *sibling;
	int degree; /* root's # of children*/
	char process_id[4]; /* P99, 3 chars at most*/
	int e_i; /* og execution time*/
	double t_remains; /* exec time (updated)*/
	int og_t_arr; /* og arrival time*/
	int t_arr; /* arrival time (updated)*/
	double total_waited_time;
	double pri_factor;
} Process;
Process *microP = NULL; /* where do we even update this*/
Process *input[100];

struct binomialRoot{
	struct process *processPtr;
} *RoR; /* root of the roots*/
typedef struct binomialRoot BinomialRoot;

int time = 0; /* global time*/
int quantum; /* optimize for average waiting time*/
int process_amount; /* # of nodes in the input file*/
int process_handled; /* AKA # of nodes put in the queue at least once*/
int e_max = 0; /* global max execution time*/
int rec_num = 0;
double times[2];
Process *triple_ptrs[2];
Process **accessor;

void manage_line(char *line, int i);
void manage_input(FILE *f);
void node_create(char *id, int t_arr, int e_i, int i);
void engine(int[], int);
double *average_total_wait_time();
int there_exists_process();
int anything_new();
void waiting_room(Process *ptr);
void queue_printer(Process *ptr, int);
void reset_input();
void cleanUp();
Process **findmin();
/*provided and/or modified*/
Process *heapUnion(Process *uni);
void heapRemove(BinomialRoot *heap, Process *node, Process *before);
Process *heapMerge(BinomialRoot *heap1, Process *proc);

int main(){
	int print_optimum = 0;
	int superior_quantum[10] = {0};
	double temp_awt;
	int i, j, k = 0;
	double min_awt_ever;
	FILE *file;
	min_awt_ever = INT_MAX;
	RoR = (BinomialRoot *)malloc(sizeof(BinomialRoot));
	if(RoR == NULL)
		return;
	RoR->processPtr = NULL;

	file = fopen("input.txt", "r");
	manage_input(file);

	for(i = 1; i < 10; i++){
		quantum = i;
		engine(NULL, quantum);
		temp_awt = average_total_wait_time()[0];
		if(temp_awt < min_awt_ever){ /* new quantum is better than the ones we found before*/
			min_awt_ever = temp_awt;
			j = 0; /* reset*/
			superior_quantum[j++] = quantum;
		}
		else if(temp_awt == min_awt_ever){
			superior_quantum[j++] = quantum; 
		}
		reset_input();
	}
	for(k = 0; k < j; k++){
		engine(superior_quantum, superior_quantum[k]);
		reset_input();
	}
}

void engine(int superior_quantum[], int quantum){
	int i, j;
	if(superior_quantum != NULL){
		/* just the titles*/
		printf("An optimum q = %d\n", quantum);
		printf("Time\t\tProcesses in BH\t\t\t\tPriority Value of Processes in BH\n");
	}
	while(there_exists_process() || (process_amount - process_handled > 0)){

		if(time == 0) anything_new();
		accessor = findmin();
		microP = accessor[0];
		if(superior_quantum != NULL){
			printf("%d\t\t", time);
			queue_printer(RoR->processPtr, 0); /* id names*/
			if(rec_num == 1){
				printf("\t");
			}
			printf("\t\t\t\t");
			rec_num = 0;
			queue_printer(RoR->processPtr, 1); /* factors numerically*/
			puts("");
		}
		heapRemove(RoR, accessor[0], accessor[1]);

		for(j = 0; j < quantum; j++){
			microP->t_remains--;
			time++;
			
			waiting_room(RoR->processPtr);
			anything_new();
			if(microP->t_remains <= 0){
				break;
			}
		}
		if(microP->t_remains <= 0){
			cleanUp();
		}
		else{
			cleanUp();
			microP->t_arr = time;
			microP->pri_factor = microP->t_remains * (1.0 / exp(-pow((2.0 * microP->t_remains) / (3.0 * e_max), 3)));
			RoR->processPtr = heapUnion(microP);
		}
	}
	if(superior_quantum != NULL){
		printf("%d\t\tEMPTY", time); /* we need to print time before stating empty */
		printf("\n\nPID\t\tWaiting Time\n");
		for(i = 0; i < 99 && input[i] != NULL; i++){
			printf("%-4s%18.0f\n", input[i]->process_id, input[i]->total_waited_time);
		}
		printf("AWT: %.0f/%d = %.3f\n\n", average_total_wait_time()[1], process_amount, average_total_wait_time()[0]);
	}
}

Process** findmin(){
	if(RoR == NULL)
		exit(5);
	Process *prev_of_min = NULL;
	Process *temp_prev = NULL;
	Process *current;
	Process *found_min;
	current = RoR->processPtr;
	found_min = RoR->processPtr;
	while(current != NULL){
		if(current->pri_factor < found_min->pri_factor){
			found_min = current;
			prev_of_min = temp_prev;
		}
		/* same e_i so there is tie breaker t_arr*/
		else if(current->pri_factor == found_min->pri_factor){ 
			if(current->t_arr < found_min->t_arr){
				found_min = current;
				prev_of_min = temp_prev;
			}
		}
		/* move a step*/
		temp_prev = current;
		current = current->sibling;
	}
	triple_ptrs[0] = found_min;
	triple_ptrs[1] = prev_of_min;
	return triple_ptrs;
}

int anything_new(){ /* hence the name*/
	int i;
	for(i = 0; i < process_amount; i++){
		if(input[i]->og_t_arr == time){
			RoR->processPtr = heapUnion(input[i]);
			process_handled++;
		}
	}
	return 0;
}

/* for mode 0, only print the ids; else print prifactors*/
void queue_printer(Process *ptr, int mode){
	if(ptr == NULL)
		return;
	if(ptr->child != NULL)
		queue_printer(ptr->child, mode);
	if(ptr->sibling != NULL)
		queue_printer(ptr->sibling, mode);
	if(!mode){
		printf("%-4s", ptr->process_id);
		rec_num++;
	}
	else
		printf("%s: %.3f ",ptr->process_id, ptr->pri_factor);
}
/* increments the waiting time of all processes currently in the heap*/
void waiting_room(Process *ptr){
	if(ptr == NULL)
		return;
	if(ptr->child != NULL)
		waiting_room(ptr->child);
	if(ptr->sibling != NULL)
		waiting_room(ptr->sibling);
	ptr->total_waited_time++;
}

/* called whenever a node is supposed to be inserted*/
Process *heapUnion(Process *uni) {
	Process *new_head;
	Process *prev;
	Process *aux;
	Process *next;

	new_head = heapMerge(RoR, uni);
	RoR->processPtr = NULL;

	if(new_head == NULL)
		return NULL;

	prev = NULL;
	aux = new_head;
	next = aux->sibling;

	while(next != NULL) {
		if(aux->degree != next->degree ||
			(next->sibling != NULL &&
				next->sibling->degree == aux->degree)) {
			prev = aux;
			aux = next;
		}
		else {

			int make_aux_parent = 0;

			if(aux->pri_factor < next->pri_factor) {
				make_aux_parent = 1; 
			}
			else if(aux->pri_factor == next->pri_factor) {
				if(aux->t_arr <= next->t_arr) {
					make_aux_parent = 1;
				}
			}

			if(make_aux_parent) {
				aux->sibling = next->sibling;
				next->parent = aux;
				next->sibling = aux->child;
				aux->child = next;
				aux->degree++;
			}
			else {
				if(prev == NULL)
					new_head = next;
				else
					prev->sibling = next;

				aux->parent = next;
				aux->sibling = next->child;
				next->child = aux;
				next->degree++;
				aux = next;
			}
		}
		next = aux->sibling;
	}
	return new_head;
}

/* called whenever a node is to be removed from the queue
it might be that it was just done executing
or it could be that it is going to be re-inserted with updated values */
void heapRemove(BinomialRoot *heap, Process *node, Process *before) {
	BinomialRoot *temp;
	Process *child;
	Process *new_head;
	Process *next;

	if(node == heap->processPtr) {
		heap->processPtr = node->sibling;
	}
	else if(before != NULL) {
		before->sibling = node->sibling;
	}
	else exit(99);

	new_head = NULL;
	child = node->child;

	while(child != NULL) {
		next = child->sibling;
		child->sibling = new_head;
		child->parent = NULL;
		new_head = child;
		child = next;
	}

	temp = (BinomialRoot *)malloc(sizeof(BinomialRoot));
	if(temp == NULL) return;
	temp->processPtr = new_head;
	heap->processPtr = heapUnion(temp->processPtr);

	free(temp);
}

/*the functions below are for extracting the input from the file*/
void manage_input(FILE *f){
	int i = 0;
	char line[12];
	while(fgets(line, 12, f) != NULL && i < 99){
		line[11] = '\0';
		manage_line(line, i);
		i++;
	}
	for(i = 0; i < 99 && input[i] != NULL; i++);
	process_amount = i; /* does work don't change*/
}
 
void manage_line(char *line, int i){
	char *id, *eiptr, *t_arrptr;
	int ei = 0, t_arr = 0;

	id = strtok(line, " \t\n");
	eiptr = strtok(NULL, " \t\n");
	t_arrptr = strtok(NULL, " \t\n");

	ei = atoi(eiptr);
	t_arr = atoi(t_arrptr);
	if(ei > e_max)
		e_max = ei;
	node_create(id, t_arr, ei, i);
}

void node_create(char *id, int t_arr, int e_i, int i){
	Process *node;
	node = (Process *)malloc(sizeof(Process));
	if(node == NULL) exit(7);

	node->parent = NULL;
	node->child = NULL;
	node->sibling = NULL;
	node->degree = 0;
	strcpy_s(node->process_id, 4, id);
	node->e_i = e_i;
	node->t_remains = e_i;
	node->t_arr = t_arr;
	node->og_t_arr = t_arr;
	node->total_waited_time = 0;
	node->pri_factor = e_i;
	input[i] = node;
}

/*called to return [0] as average, [1] as total*/
double *average_total_wait_time(){
	int i; double t = 0;
	for(i = 0; i < process_amount; i++)
		t += input[i]->total_waited_time;
	times[0] = t / process_amount;
	times[1] = t;
	return times;
}

/* used as bool*/
int there_exists_process(){
	if(RoR == NULL || RoR->processPtr == NULL)
		return 0;
	else
		return 1;
}
/* prevents circuits in the tree, was causing infinite loops without htis function*/
void cleanUp(){
	microP->child = NULL;
	microP->sibling = NULL;
	microP->parent = NULL;
	microP->degree = 0;
}

/* called to reset after each quantum value trial*/
void reset_input(){
	int i;
	if(RoR != NULL) {
		RoR->processPtr = NULL;
	}
	process_handled = 0;
	for(i = 0; i < process_amount; i++){
		input[i]->total_waited_time = 0;
		input[i]->t_remains = input[i]->e_i;
		input[i]->t_arr = input[i]->og_t_arr;
		input[i]->parent = NULL;
		input[i]->sibling = NULL;
		input[i]->child = NULL;
		input[i]->degree = 0;
		input[i]->pri_factor = input[i]->e_i;
	}
	time = 0;
}

/* only union function calls this function and it is not to be manually called*/
Process *heapMerge(BinomialRoot *heap1, Process *proc) {
	Process *head;
	Process *tail;
	Process *h1It;
	Process *h2It;

	if(heap1->processPtr == NULL)
		return proc;
	if(proc == NULL)
		return heap1->processPtr;

	h1It = heap1->processPtr;
	h2It = proc;

	if(h1It->degree <= h2It->degree) {
		head = h1It;
		h1It = h1It->sibling;
	}
	else{
		head = h2It;
		h2It = h2It->sibling;
	}

	tail = head;

	while(h1It != NULL && h2It != NULL) {
		if(h1It->degree <= h2It->degree) {
			tail->sibling = h1It;
			h1It = h1It->sibling;
		}
		else {
			tail->sibling = h2It;
			h2It = h2It->sibling;
		}
		tail = tail->sibling;
	}

	tail->sibling = (h1It != NULL) ? h1It : h2It;
	return head;
}