// FATIMA AVCILAR 150123017
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#define _CRT_SECURE_NO_WARNINGS 
/*
rn i store all the input as a pointer array to process struct
then i will increment time one by one and check 
if there are any new processes that should arrive at that time,
then add that to the queue and find the min again, 
make it the microP variable, 
do that process until q time passes */

// we need to format the output and that is it
/*
Which process is allocated to the CPU at each time unit
All processes in the heap and their priority values at each time unit
*/

struct binomialRoot;
typedef struct process{
	struct process *parent;
	struct process *child;
	struct process *sibling;
	int degree; // number of children or something
	char process_id[4]; // P99 3 chars at most
	int e_i; // og execution time
	double t_remains; // remaining exec time
	int og_t_arr; // og arrival time
	int t_arr; // arrival time
	int nth_rodeo; // initially 1, but if this aint its first rodeo n = ?
	double total_waited_time; // this gets updated with each insertion and we will divide this by process_amount to find awt of all nodes
	double pri_factor;
	struct binomialRoot *as_root;
} Process;
Process *microP = NULL; // where do we even update this
Process *input[10];
Process *input_backup[10];

struct binomialRoot{
	struct process *processPtr;
} *RoR; // root of the roots
typedef struct binomialRoot BinomialRoot;

int time = 0; // odd but ok
int quantum; // optimize for average waiting time
int process_amount; // how many were there?
int process_handled; // AKA put in the queue at least once
int e_max = 0;
Process *triple_ptrs[2];
Process **accessor;

double average_wait_time();
void manage_line(char *line, int i);
void manage_input(FILE *f);
BinomialRoot *node_create_returns_root(char *id, int t_arr, int e_i, int i);
void waiting_room(Process *ptr);
Process *heapUnion(Process *uni);
void update_processes_in_q(Process *);
void heapRemove(BinomialRoot *heap, Process *node, Process *before);
//void heapRemove(BinomialRoot *heap, BinomialRoot *primitive_node, BinomialRoot *primitive_before);
Process *heapMerge(BinomialRoot *heap1, Process *proc);
BinomialRoot *heapCreate(Process *node);
int there_exists_process();
int anything_new();
void cleanUp(Process *heap);
Process **findmin();
void reset_input();
void engine(int, int);
void queue_printer(Process *ptr, int);

int main(){
	int print_optimum = 0;
	int superior_quantum = 0;
	int temp_time = 99999999;

	RoR = (BinomialRoot *)malloc(sizeof(BinomialRoot));
	if(RoR == NULL)
		return;
	RoR->processPtr = NULL;

	FILE *file = fopen("input.txt", "r");
	manage_input(file);

	int i;
	for(i = 1; i < 10; i++){
		quantum = i;
		engine(quantum, print_optimum);
		if(average_wait_time() < temp_time){
			temp_time = average_wait_time();
			superior_quantum = quantum;
		}
		reset_input();
	}
	engine(superior_quantum, ++print_optimum);
}
void engine(int quantum, int bool_opt){
	int i;
	if(bool_opt){
		printf("An optimum q = %d\n", quantum);
		printf("Time\t\tProcesses in BH\t\tPriority Value of Processes in BH\n0\t\t");
	}
	while(there_exists_process() || (process_amount - process_handled > 0)){

		if(time == 0) anything_new();
		accessor = findmin();
		microP = accessor[0];
		if(bool_opt){
			queue_printer(RoR->processPtr, 0); // id names
			queue_printer(RoR->processPtr, 1); // factors numerically
			puts("");
		}
		heapRemove(RoR, accessor[0], accessor[1]);

		int j;
		for(j = 0; j < quantum; j++){
			microP->t_remains--;
			time++;
			if(bool_opt)
				printf("%d\t\t", time);
			waiting_room(RoR->processPtr);
			anything_new();
			if(microP->t_remains <= 0){
				break;
			}
		}
		if(microP->t_remains <= 0){
			cleanUp(microP);
		}
		else{
			cleanUp(microP);
			microP->t_arr = time;
			microP->pri_factor = microP->t_remains * (1.0 / exp(-pow((2.0 * microP->t_remains) / (3.0 * e_max), 3)));
			RoR->processPtr = heapUnion(microP);
		}
	}
	if(bool_opt){
		printf("\nPID\t\tWaiting Time\n");
		for(i = 0; i < 10 && input[i] != NULL; i++){
			printf("%-4s%20.0f\n", input[i]->process_id, input[i]->total_waited_time);
		}
		printf("AWT: %.3f\n", average_wait_time());

	}
}
Process** findmin(){
	// use RoR well because the min is one of the roots duh
	if(RoR == NULL)
		exit(5);
	Process *prev_of_min = NULL;
	Process *temp_prev = NULL;
	Process *current = RoR->processPtr;
	Process *found_min = RoR->processPtr;
	while(current != NULL){
		if(current->pri_factor < found_min->pri_factor){
			found_min = current;
			prev_of_min = temp_prev;
		}
		else if(current->pri_factor == found_min->pri_factor){ // same e_i so there is tie breaker t_arr
			if(current->t_arr < found_min->t_arr){
				found_min = current;
				prev_of_min = temp_prev;
			}
		}
		// move a step
		temp_prev = current;
		current = current->sibling;
	}
	triple_ptrs[0] = found_min;
	triple_ptrs[1] = prev_of_min;
	return triple_ptrs;
//	return found_min->processPtr;
}

int anything_new(){
	int i;
	for(i = 0; i < process_amount; i++){
		if(input[i]->og_t_arr == time){
			RoR->processPtr = heapUnion(input[i]);
			process_handled++;
		}
	}
	return 0;
}
// for mode 0, only print the ids; else print prifactors
void queue_printer(Process *ptr, int mode){
	if(ptr == NULL){
		return;
	}
	if(ptr->child != NULL){
		queue_printer(ptr->child, mode);
	}
	if(ptr->sibling != NULL){
		queue_printer(ptr->sibling, mode);
	}
	if(!mode)
		printf("%-4s", ptr->process_id);
	else
		printf("%18.3f", ptr->pri_factor);
}

void waiting_room(Process *ptr){
	if(ptr == NULL){
		return;
	}
	if(ptr->child != NULL){
		waiting_room(ptr->child);
	}
	if(ptr->sibling != NULL){
		waiting_room(ptr->sibling);
	}
	ptr->total_waited_time++;
}

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

void manage_input(FILE *f){
	int i = 0;
	char line[12];
	while(fgets(line, 12, f) != NULL && i < 10){
		line[11] = '\0';
		manage_line(line, i);
		i++;
	}
	for(i = 0; i < 10 && input[i] != NULL; i++);/*{
		printf("created node --- id: %s, ei: %d, t_arr: %d\n",
			input[i]->process_id, input[i]->e_i, input[i]->t_arr);
	}*/
	process_amount = i; // does work don't change
}
 
void manage_line(char *line, int i){
	char *id = strtok(line, " \t\n");
	char *eiptr = strtok(NULL, " \t\n");
	char *t_arrptr = strtok(NULL, " \t\n");

	int ei = atoi(eiptr);
	int t_arr = atoi(t_arrptr);
	if(ei > e_max)
		e_max = ei;
	node_create_returns_root(id, t_arr, ei, i);
}

BinomialRoot *node_create_returns_root(char *id, int t_arr, int e_i, int i){

	Process *node = (Process *)malloc(sizeof(Process));
	if(node == NULL) return NULL;

	node->parent = NULL;
	node->child = NULL;
	node->sibling = NULL;
	node->degree = 0;
	strcpy_s(node->process_id, 4, id);
	node->e_i = e_i;
	node->t_remains = e_i;
	node->t_arr = t_arr;
	node->og_t_arr = t_arr;
	node->nth_rodeo = 1;
	node->total_waited_time = 0;
	node->pri_factor = e_i;
	

	input[i] = node;
	input_backup[i] = node; // should only copy the basis
	return heapCreate(node);
}

BinomialRoot *heapCreate(Process *node){
	BinomialRoot *heap;

	heap = (BinomialRoot *)malloc(sizeof(BinomialRoot));
	if(heap == NULL)
		return NULL;

	heap->processPtr = node;
	node->as_root = heap;
	return heap;
}

double average_wait_time(){ // all nodes
	int i; double t = 0;
	for(i = 0; i < process_amount; i++){
//		printf("%s waited this much: %f\n", input[i]->process_id, input[i]->total_waited_time);
		t += input[i]->total_waited_time;
	}
	return t / process_amount;
}

int there_exists_process(){
	if(RoR == NULL || RoR->processPtr == NULL)
		return 0;
	else
		return 1;
}

void cleanUp(Process *heap){
	microP->child = NULL;
	microP->sibling = NULL;
	microP->parent = NULL;
	microP->degree = 0;
}

void panicCall(){ // ?

}
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

// don't call this function in main. union will call it
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