#ifndef __STRUCTURES_DEFINED

#define __STRUCTURES_DEFINED

/*List Managment*/
typedef struct listnode *Listptr;
struct listnode{
    int data;
    Listptr next;
};

void insert_at_start(Listptr *, int);
void free_list(Listptr *);
int pop_front(Listptr *);
int max_list(Listptr);

/*Pair*/
struct pair{
    int first;
    int second;
};

/*Arrays*/
int *int_array(int);
int *int_array_0(int);

int **two_d_int_array(int, int);
int **two_d_int_array_0(int N, int M);
void two_d_free(int **, int);
void two_d_free_char(char **, int);

/*Graphs*/
void find_adj(int **, int **, int, struct pair, int);

/*Bit array stacks*/
typedef struct bit_stack *Stack_bitptr;

struct bit_stack {
    unsigned long long int **arr;
    int top;
};

void push_bit(Stack_bitptr, unsigned long long int *);
unsigned long long int *pop_bit(Stack_bitptr);

#endif