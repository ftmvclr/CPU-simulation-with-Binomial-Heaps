#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#define _CRT_SECURE_NO_WARNINGS 

/*rn i store all the input as a pointer array to process struct
then i will increment time one by one and check 
if there are any new processes that should arrive at that time,
then add that to the queue and find the min again, 
make it the microP variable, 
do that process until q time passes */
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
Process *header;

struct binomialRoot{
	struct process *processPtr;
	struct binomialRoot *nextRoot;
} *RoR; // root of the roots
typedef struct binomialRoot BinomialRoot;

int time; // odd but ok
int quantum; // optimize for average waiting time
int process_amount; // how many were there?
BinomialRoot *triple_ptrs[2];

double average_wait_time();
void manage_line(char *line, int i);
void manage_input(FILE *f);
BinomialRoot *node_create_returns_root(char *id, int t_arr, int e_i, int i);
void traverse_nodes_in_q(Process *ptr);
Process *heapUnion(BinomialRoot *uni);
void update_processes_in_q(Process *);
void heapRemove(BinomialRoot *heap, BinomialRoot *primitive_node, BinomialRoot *primitive_before);
Process *heapMerge(BinomialRoot *heap1, BinomialRoot *heap2);
BinomialRoot *heapCreate(Process *node);
int there_exists_process();
int anything_new();
BinomialRoot **findmin();

int main(){
	FILE *file = fopen("input.txt", "r");
	manage_input(file);

	int i;
	BinomialRoot **accessor;
	for(i = 1; i < 10; i++){
		quantum = i;
		while(0){
			// now send the array to a function that will form all those binomial trees or something
			time++; // time flies
			anything_new(); // time has passed so we need to know if new stuff arrived
			// we queued the new stuff now what?
			accessor = findmin(); // ok find the min, come back after that then what?
			microP = accessor[0]->processPtr;
			// now that we found the highest priority node, we need to like execute it.
			microP->t_remains = microP->t_remains - quantum;
			if(microP->t_remains == 0){ 
				// remove completely
				heapRemove(RoR, *accessor, *(accessor + 1));
				traverse_nodes_in_q(header);
			}
			else{
				heapRemove(RoR, accessor[0], accessor[1]);
				traverse_nodes_in_q(header);
				heapUnion(accessor[0]);
				// remove and then add back into the queue 
			}
//			update_processes_in_q(microP);
			// penalty for the already executed process
			microP->pri_factor = microP->t_remains * 1 / (exp(-(pow(2 * microP->t_remains / 3 * microP->e_i, 3))));
		}
	}
}

BinomialRoot** findmin(){
	// use RoR well because the min is one of the roots duh
	if(RoR == NULL)
		exit(5);
	BinomialRoot *prev_of_min = NULL;
	BinomialRoot *temp_prev = NULL;
	BinomialRoot *current = RoR;
	BinomialRoot *found_min = RoR;
	while(current != NULL){
		if(current->processPtr->pri_factor < found_min->processPtr->pri_factor){
			found_min = current;
			prev_of_min = temp_prev;
		}
		else if(current->processPtr->pri_factor = found_min->processPtr->pri_factor) // same e_i so there is tie breaker t_arr
			if(current->processPtr->t_arr < found_min->processPtr->t_arr)
				found_min = current;
		// move a step
		temp_prev = current;
		current = current->nextRoot;
	}
// pass instead:	heapRemove(RoR, found_min, prev_of_min);
	triple_ptrs[0] = found_min;
	triple_ptrs[1] = prev_of_min;
	return triple_ptrs;
//	return found_min->processPtr;
}

int anything_new(){
	int i;
	for(i = 0; i < 10; i++){
		if(input[i]->t_arr == time)
			// we have newly arrived process here so add it to the queue
			heapUnion(input[i]->as_root); // i hope this does add to the queue indeed!!
			return 1;
	}
	return 0;
}

// i think this is where we do the "preempting" thingy
int check_microP(){
	return 1;
}

void traverse_nodes_in_q(Process *ptr){
	if(ptr->child != NULL){
		traverse_nodes_in_q(ptr->child);
	}
	if(ptr->sibling != NULL){
		traverse_nodes_in_q(ptr->child);
	}
	ptr->total_waited_time++;
}

void update_processes_in_q(Process *p){
	// the used pointer gets sliver bit of penalty for wasting CPU frfr
	p->nth_rodeo++;
	p->pri_factor = p->t_remains * 1 / (exp(-(pow(2 * p->t_remains / 3 * p->e_i, 3))));
	// rest of the nodes need their wait time upped lol
//	traverse_nodes_in_q(header);
}

Process *heapUnion(BinomialRoot *uni) {
	Process *new_head;
	Process *prev;
	Process *aux;
	Process *next;

	new_head = heapMerge(RoR, uni);
	RoR->processPtr = NULL;
	uni->processPtr = NULL;

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

void heapRemove(BinomialRoot *heap, BinomialRoot *primitive_node, BinomialRoot *primitive_before){
	BinomialRoot *temp;
	Process *child;
	Process *new_head;
	Process *next;
	Process *node = primitive_node->processPtr;
	Process *before = primitive_before->processPtr;

	if(node == heap->processPtr)
		heap->processPtr = node->sibling;
	else if(before != NULL)
		before->sibling = node->sibling;

	new_head = NULL;
	child = node->child;

	while(child != NULL) {
		next = child->sibling;
		child->sibling = new_head;
		child->parent = NULL;
		new_head = child;
		child = next;
	}
	// TODO FIX REPLACED HEAPINIT THING HERE I THOUGHT IT WAS UNNECESSARY
	temp = (BinomialRoot *)malloc(sizeof(BinomialRoot));
	if(temp == NULL)
		return;
	temp->processPtr = NULL;

	temp->processPtr = new_head;
	heap->processPtr = heapUnion(temp);
	free(temp);
}

/*BinomialRoot *heapInit(void){
	BinomialRoot *heap;

	heap = (BinomialRoot *)malloc(sizeof(BinomialRoot));
	if(heap == NULL)
		return NULL;

	heap->processPtr = NULL;
	return heap;
}*/

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
	heap->nextRoot = NULL; // addition
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
	if(RoR == NULL)
		return 0;
	else
		return 1;
}

/*void heapFree(BinomialRoot *heap){
	while(heapMin(heap) != NULL);
	free(heap);
}*/

// don't call this function in main. union will call it
Process *heapMerge(BinomialRoot *heap1, BinomialRoot *heap2) {
	Process *head;
	Process *tail;
	Process *h1It;
	Process *h2It;

	if(heap1->processPtr == NULL)
		return heap2->processPtr;
	if(heap2->processPtr == NULL)
		return heap1->processPtr;

	h1It = heap1->processPtr;
	h2It = heap2->processPtr;

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