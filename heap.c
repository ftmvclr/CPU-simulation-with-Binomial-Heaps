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
/*
so imma just check if the processPtr is null and if it is 
i will send it to some function to add the first process 
to the queue and i will just change the time to its arrival time accordingly*/

struct binomialRoot;
typedef struct process{
	struct process *parent;
	struct process *child;
	struct process *sibling;
	int degree; // number of children or something
	char process_id[4]; // P99 3 chars at most
	int e_i; // og execution time
	double t_remains; // remaining exec time
	int t_arr; // arrival time
	int nth_rodeo; // initially 1, but if this aint its first rodeo n = ?
	double total_waited_time; // this gets updated with each insertion and we will divide this by process_amount to find awt of all nodes
	double pri_factor;
	struct binomialRoot *as_root;
} Process;
Process *microP = NULL; // where do we even update this
Process *input[10];

struct binomialRoot{
	struct process *processPtr;
} *RoR; // root of the roots
typedef struct binomialRoot BinomialRoot;

int time; // odd but ok
int quantum; // optimize for average waiting time
int process_amount; // how many were there?
Process *triple_ptrs[2];

double average_wait_time();
void manage_line(char *line, int i);
void manage_input(FILE *f);
BinomialRoot *node_create_returns_root(char *id, int t_arr, int e_i, int i);
void traverse_nodes_in_q(Process *ptr);
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

void debug_print_list(Process *head) {
	Process *current = head;
	Process *seen[20]; // Simple array to track visited nodes
	int count = 0;

	printf("\n[DEBUG LIST]: ");
	while(current != NULL) {
		printf("%s (Deg %d) -> ", current->process_id, current->degree);

		// Check if we have seen this node before
		for(int i = 0; i < count; i++) {
			if(seen[i] == current) {
				printf("CYCLE DETECTED at %s! INFINITE LOOP IMMINENT.\n", current->process_id);
				exit(666);
			}
		}
		seen[count++] = current;
		if(count >= 20) break; // Safety break
		current = current->sibling;
	}
	printf("NULL\n");
}

int main(){
	RoR = (BinomialRoot *)malloc(sizeof(BinomialRoot));
	if(RoR == NULL)
		return;
	RoR->processPtr = NULL;

	FILE *file = fopen("input.txt", "r");
	manage_input(file);

//	int i;
	Process **accessor;
//	for(i = 1; i < 10; i++){
		quantum = 1;
		anything_new(); // for time 0
		while(there_exists_process()){
			// now send the array to a function that will form all those binomial trees or something
			time++; // time flies
			anything_new(); // time has passed so we need to know if new stuff arrived
			// we queued the new stuff now what?
			accessor = findmin(); // ok find the min, come back after that then what?
			microP = accessor[0];
			// now that we found the highest priority node, we need to like execute it.
			microP->t_remains = microP->t_remains - quantum;
			if(microP->t_remains <= 0){ 
				puts("2");
				// remove completely
				heapRemove(RoR, *accessor, *(accessor + 1));
				cleanUp(microP);

				traverse_nodes_in_q(RoR->processPtr);
			}
			else{
				puts("1");
				// remove and then add back into the queue 
				heapRemove(RoR, accessor[0], accessor[1]);
				traverse_nodes_in_q(RoR->processPtr);
				cleanUp(microP);
				debug_print_list(RoR->processPtr);
				RoR->processPtr = heapUnion(accessor[0]); // MEGA ERROR FIX THIS
			}
//			update_processes_in_q(microP);
			// penalty for the already executed process
			microP->pri_factor = microP->t_remains * 1 / (exp(-(pow(2 * microP->t_remains / 3 * microP->e_i, 3))));
		}
	//}
}

Process** findmin(){
	puts("3");
	// use RoR well because the min is one of the roots duh
	if(RoR == NULL)
		exit(5);
	Process *prev_of_min = NULL;
	Process *temp_prev = NULL;
	Process *current = RoR->processPtr;
	Process *found_min = RoR->processPtr;
	while(current != NULL){
		if(current->pri_factor < found_min->pri_factor){
			puts("4");
			found_min = current;
			prev_of_min = temp_prev;
		}
		else if(current->pri_factor == found_min->pri_factor){ // same e_i so there is tie breaker t_arr
			if(current->t_arr < found_min->t_arr){
				found_min = current;
				prev_of_min = temp_prev;
			}
			puts("5");
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
	puts("6");
	int i;
	for(i = 0; i < process_amount; i++){
		if(input[i]->t_arr == time){
			puts("how about this"); // calls once
			// we have newly arrived process here so add it to the queue
			RoR->processPtr = heapUnion(input[i]);
		}
	}
	return 0;
}

//we literally execute half of this function and then quit with some code FIX THIS!!
void traverse_nodes_in_q(Process *ptr){
	static int a = 0;
	if(a == 0)
		puts("rec");
	a++;
	puts("wait-");
	if(ptr == NULL){
		return;
	}
	if(ptr->child != NULL){
		puts("child isnt null");
		traverse_nodes_in_q(ptr->child);
	}
	if(ptr->sibling != NULL){
		puts("sibling isnt null");
		traverse_nodes_in_q(ptr->sibling);
	}
	puts("how many times does it even?"); // 3 for now
	ptr->total_waited_time++;
}

void update_processes_in_q(Process *p){
	puts("shouldnt be called");
	// the used pointer gets sliver bit of penalty for wasting CPU frfr
	p->nth_rodeo++;
	p->pri_factor = p->t_remains * 1 / (exp(-(pow(2 * p->t_remains / 3 * p->e_i, 3))));
	// rest of the nodes need their wait time upped lol
//	traverse_nodes_in_q(header);
}

Process *heapUnion(Process *uni) {
	puts("7");
	Process *new_head;
	Process *prev;
	Process *aux;
	Process *next;

	new_head = heapMerge(RoR, uni); 
	RoR->processPtr = NULL;
//	uni/*->processPtr*/ = NULL;

	if(new_head == NULL) // this is always NULL for some reason
		return NULL;

	prev = NULL;
	aux = new_head;
	next = aux->sibling; // ok we are gone here accessing null's sibling
	puts("inf loop check");
	while(next != NULL) {
	//	puts("code doesn't reach here");
		if(aux->degree != next->degree ||
			(next->sibling != NULL &&
				next->sibling->degree == aux->degree)) {
			prev = aux;
			aux = next;
		}
		else {
			if(aux->pri_factor <= next->pri_factor) {
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
	for(i = 0; i < 10 && input[i] != NULL; i++){ 
		printf("created node --- id: %s, ei: %d, t_arr: %d\n",
			input[i]->process_id, input[i]->e_i, input[i]->t_arr);
	}
	process_amount = i; // does work don't change
}
 
void manage_line(char *line, int i){
	char *id = strtok(line, " \t\n");
	char *eiptr = strtok(NULL, " \t\n");
	char *t_arrptr = strtok(NULL, " \t\n");

	int ei = atoi(eiptr);
	int t_arr = atoi(t_arrptr);
	
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
	node->nth_rodeo = 1;
	node->total_waited_time = 0;
	node->pri_factor = e_i;

	input[i] = node;
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

void panicCall(){

}
// don't call this function in main. union will call it
Process *heapMerge(BinomialRoot *heap1, Process *proc) {
	Process *head;
	Process *tail;
	Process *h1It;
	Process *h2It;
	puts("reaches here");

	if(RoR == NULL){
		panicCall();
	}
	puts("reaches here question mark");
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