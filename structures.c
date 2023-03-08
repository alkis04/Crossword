#include "structures.h"
#include <stdlib.h>
#include <stdio.h>

extern int **adj;

#define max(x,y) (((x) > (y)) ? (x) : (y));

void insert_at_start(Listptr *list, int d){
    Listptr new_node = malloc(sizeof(struct listnode)); /*Create a new node*/
    new_node->next = (*list); /*Make the next element of the new node, the previous first item of the list*/
    new_node->data = d; /*Store value d*/
    (*list) = new_node; /*Make the *list pointer to point at the new node (the new start of the list)*/
}

void free_list(Listptr *list){
    Listptr current = *list;
    Listptr next;
    while(current != NULL) /*Visit elements up to end*/
    {
        next = current->next; /*Update the next pointer*/
        free(current); /*Free the current node*/
        current = next; /*Go to the next item*/
    }

    *list = NULL; /*Change the list pointer to NULL (by reference)*/
}

int pop_front(Listptr *list){
    if(*list == NULL){
        return 0;
    }
    int ret = (*list)->data;
    Listptr to_free = *list; /*Save the pointer of the first node*/
    *list = (*list)->next; //NOT *(list) /*Move the pointer to the next element (by reference)/*
    free(to_free); /*Free the first node*/
    return ret;
}

int max_list(Listptr list){
    int mx = 0;
    while (list != NULL){
        mx = max(mx, list->data);
        list = list->next;
    }
    return mx;
}

/*Arrays*/
int *int_array(int N){
    int *arr_ptr = malloc(N * sizeof(int));
    if(arr_ptr == NULL){
        fprintf(stderr, "Unable to allocate enough memory\n");
        exit(1);
    }
    return arr_ptr;
}

int *int_array_0(int N){
    int *arr_ptr = calloc(N, sizeof(int));
    if(arr_ptr == NULL){
        fprintf(stderr, "Unable to allocate enough memory\n");
        exit(1);
    }
    return arr_ptr;
}

int **two_d_int_array(int N, int M){
    int **arr_ptr = malloc(N * sizeof(int *));
    if(arr_ptr == NULL){
        fprintf(stderr, "Unable to allocate enough memory\n");
        exit(1);
    }
    int i;
    for(i = 0; i < N; i++){
        arr_ptr[i] = malloc(M * sizeof(int));
        if(arr_ptr[i] == NULL){
            fprintf(stderr, "Unable to allocate enough memory\n");
            exit(1);
        }
    }
    return arr_ptr;
}

int **two_d_int_array_0(int N, int M){
    int **arr_ptr = malloc(N * sizeof(int *));
    if(arr_ptr == NULL){
        fprintf(stderr, "Unable to allocate enough memory\n");
        exit(1);
    }
    int i;
    for(i = 0; i < N; i++){
        arr_ptr[i] = calloc(M, sizeof(int));
        if(arr_ptr[i] == NULL){
            fprintf(stderr, "Unable to allocate enough memory\n");
            exit(1);
        }
    }
    return arr_ptr;
}

void two_d_free(int **arr, int N){
    int i;
    for(i = 0; i < N; i++){
        free(arr[i]);
    }
    free(arr);
}

void two_d_free_char(char **arr, int N){
    int i;
    for(i = 0; i < N; i++){
        free(arr[i]);
    }
    free(arr);
}

/*Graphs*/

void find_adj(int **main_arr, int **nei_arr, int N, struct pair pos, int x){
    int i = pos.first;
    int j = pos.second;

    int len = 0;

    while(j < N && main_arr[i][j] == main_arr[i][pos.second]){
        if (nei_arr[j][i] != 0){
            ++len;
            adj[x][len] = nei_arr[j][i];
        }

        j++;
    }

    adj[x][0] = len;
}


//Bit array stacks

void push_bit(Stack_bitptr stack, unsigned long long int *in_arr){
    stack->top++;
    stack->arr[stack->top] = in_arr;
}

unsigned long long int *pop_bit(Stack_bitptr stack){
    unsigned long long int *res = stack->arr[stack->top];
    stack->top--;
    return res;
}