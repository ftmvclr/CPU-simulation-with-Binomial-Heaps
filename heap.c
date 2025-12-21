#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#define _CRT_SECURE_NO_WARNINGS 

typedef struct process{
	struct process *parent;
	struct process *child;
	struct process *sibling;
	int degree; // number of children or something
	char process_id[4]; // P99 3 chars at most
	int e_i; // execution time
	double t_remains; // remaining exec time
	int t_arr; // arrival time
	int nth_rodeo; // initially 1, but if this aint its first rodeo n = ?
	double total_waited_time; // this gets updated with each insertion and we will divide this by process_amount to find awt of all nodes
	double pri_factor;
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

double average_wait_time();
Process *manage_line(char *line);
void manage_input(FILE *f);
Process *node_Create(char *id, int t_arr, int e_i);
void update_processes_in_q(Process *);
int there_exists_process();
int anything_new();
Process *findmin();
Process *binomial_tree_maker();

int main(){
	FILE *input = fopen("input.txt", "r");
	manage_input(input);

	int i;
	return 0;
	for(i = 1; i < 10; i++){
		quantum = i;
		/*while(0){
			// now send the array to a function that will form all those binomial trees or something
			binomial_tree_maker(); // let's assume this does reorder everything lmao
			time++; // time flies
			if(anything_new){ // time has passed so we need to know if new stuff arrived
				binomial_tree_maker();
			}
			microP = findmin;
			// now that we found the highest priority node, we need to like execute it.
			microP->t_remains = microP->t_remains - quantum;
			update_processes_in_q(microP);
		}*/
	}
}

Process *findmin(){
	// use RoR well because the min is one of the roots duh
	if(RoR == NULL)
		exit(5);
	BinomialRoot *temp = RoR;
	BinomialRoot *next_in_line = RoR;
	while(temp != NULL){
		if(temp->processPtr->pri_factor < next_in_line->processPtr->pri_factor){
			next_in_line = temp;
		}
		else if(temp->processPtr->pri_factor = next_in_line->processPtr->pri_factor){ // same e_i so there is tie breaker t_arr
			next_in_line = (temp->processPtr->t_arr <
				next_in_line->processPtr->t_arr) ? temp : next_in_line;
		}
		temp = temp->nextRoot;
	}
	return next_in_line->processPtr;
}

int anything_new(){
	int i;
	for(i = 0; i < 10; i++){
		if(input[i]->t_arr == time)
			return 1;
	}
	return 0;
}
Process *binomial_tree_maker(){
	int i;
	for(i = 0; i < 3; i++){
		input[0]; //dumm
	}
	return NULL;
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
	traverse_nodes_in_q(header);
}

merge_root_lists(){

}
Process *node_Create(char *id, int t_arr, int e_i){

	Process *node = (Process *)malloc(sizeof(Process));
	if(node == NULL) return NULL;

	node->parent = NULL;
	node->child = NULL;
	node->sibling = NULL;
	node->degree = 0;
	strcpy_s(node->process_id, 4, id);
	node->e_i =e_i;
	node->t_remains = e_i;
	node->t_arr = t_arr;
	node->nth_rodeo = 1;
	node->total_waited_time = 0;
	node->pri_factor = e_i;

	return node;
}

void manage_input(FILE *f){
	int i = 0;
	char line[12];
	while(fgets(line, 12, f) != NULL && i < 10){
		line[11] = '\0';
		input[i++] = manage_line(line);
	}
	for(i = 0; i < 10 && input[i] != NULL; i++){ 
		printf("created node --- id: %s, ei: %d, t_arr: %d\n",
			input[i]->process_id, input[i]->e_i, input[i]->t_arr);
	}
	process_amount = i; // does work don't change
}
 
Process *manage_line(char *line){
	char *id = strtok(line, " \t\n");
	char *eiptr = strtok(NULL, " \t\n");
	char *t_arrptr = strtok(NULL, " \t\n");

	int ei = atoi(eiptr);
	int t_arr = atoi(t_arrptr);
	
	Process *createdNode = node_Create(id, t_arr, ei);
	return createdNode;
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