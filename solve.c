#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include "solve.h"
#include "globals.h"
#include "dictionary.h"
#include "bitarrays.h"

int words_total1; // word_total + 1

Stack_bitptr reductions; //stores for every variable a stack of reductions on its domain
Listptr *future_fc; //for every variable i, it stores the unlabeled variables that cross with i
Listptr *past_fc; //for every variable j, it stores the labeled variables that cross with j
unsigned long long int **bit_past_fc; //past_fc in bit_array form
Listptr labeled; //stack stores the labeled variables
int set_labeled[MAX_VARIABLES]; //labeled in bit_array form
extern int min_dom; //starting point of the order

void make_constraints(int **main_arr, int **nei_arr, int N, struct pair pos, int **C){
    int i = pos.first;
    int j = pos.second;

    while(j < N && main_arr[i][j] == main_arr[i][pos.second]){
        if (nei_arr[j][i] != 0){
            C[main_arr[i][j]][nei_arr[j][i]] = j - pos.second + 1;
        }
        j++;
    }
}

int check(int i, int j){ 
    int c1 = C[i][j];
    int c2 = C[j][i];


    int length1 = length_of_v[i];
    int length2 = length_of_v[j];

    if (!c1) return 1;
    --c1; --c2;

    if (dictionary[length1][v[i]][c1] == dictionary[length2][v[j]][c2]) return 1;
    return 0;
}

int check_forward(int i, int j, int *lengths){
    int length = length_of_v[j];
    int values = lengths[length];
    
    unsigned long long int *reduction = bit_array(values);
    
    int c1 = C[i][j];
    int c2 = C[j][i];
    if(c2){
        //We find the words we should eliminate using the bit array representation of dictionary (aka lexicon)
        char ch = dictionary[length_of_v[i]][v[i]][c1 - 1];
        set_bit_arr(reduction, values);
        diff_bit(reduction, get_lexicon(lexicon, length, ch, c2), values);
        intersect_bit(reduction, domain[j], values);
    }

    if (!check_empty(reduction, values)){
        diff_bit(domain[j], reduction, values);
        push_bit(&reductions[j], reduction);
        insert_at_start(&future_fc[i], j);
        insert_at_start(&past_fc[j], i);
        SET_BIT(bit_past_fc[j], i);
    }else{
        free(reduction);
    }

    return !check_empty(domain[j], values);
}

void undo_reductions(int i, int *lengths){

    Listptr current = future_fc[i];
    int j;
    unsigned long long int *reduction;
    while (current != NULL){
        j = current->data;
        reduction = pop_bit(&reductions[j]);
        union_bit(domain[j], reduction, lengths[length_of_v[j]]);
        RESET_BIT(bit_past_fc[j], past_fc[j]->data);
        pop_front(&past_fc[j]);
        current = current->next;
        free(reduction);
    }
    free_list(&future_fc[i]);
}

void updated_current_domain(int i, int *lengths){
    set_bit_arr(domain[i], lengths[length_of_v[i]]);
    int j;
    unsigned long long int *reduction;
    for (j = 0; j <= reductions[i].top; ++j){
        reduction = reductions[i].arr[j];
        diff_bit(domain[i], reduction, lengths[length_of_v[i]]);
    }
}

int label(int i, int *consistent, int *lengths){
    int j, h, k, l = adj[i][0];
    int length = length_of_v[i];
    int values = lengths[length];
    
    *consistent = 0;
    for(j = 0; j < values; ++j){ //for every word in the domain while not consistent
        if(!GET_BIT(domain[i], j)){
            continue;
        }
        v[i] = j; 
        *consistent = 1;
        
        for(k = 1; k <= l; ++k){ //while consistent
            h = adj[i][k];
            if(set_labeled[h]) continue;
            *consistent = check_forward(i, h, lengths);

            if(!(*consistent)) break;
        } 
        
        if(!(*consistent)){ //we eliminate from the domain i, all the words that have the same letter at the same position with v[i] that creates comflict with h 
            int c1 = C[i][h];
            char ch = dictionary[length][v[i]][c1-1];
            diff_bit(domain[i], get_lexicon(lexicon, length, ch, c1), values);
            undo_reductions(i, lengths);
            union_bit(conf_set[i], bit_past_fc[h], words_total1);
        }else{
            break;
        }

    }

    if(!(*consistent)){
        return i;
    }
    //find the next variable using MRV heuristic
    int min_j = words_total1;
    int min_domain, temp;
    for (j = 1; j <= words_total; ++j){
        if (set_labeled[j]){
            continue;
        }

        if (min_j == words_total1){
            min_domain = count_ones(domain[j], lengths[length_of_v[j]]);
            min_j = j;
        } else {
            temp = count_ones(domain[j], lengths[length_of_v[j]]);
            if (temp < min_domain){
                min_domain = temp;
                min_j = j;
            }
        }
    }
    return min_j;
}

int unlabel(int i, int *consistent, int *lengths){
    int j;
    int h = 0;

    //find the first variable from the stack labeled that belongs to conf_set or past_fc
    Listptr current = labeled; 
    while (current != NULL){
        if (GET_BIT(conf_set[i], current->data) || GET_BIT(bit_past_fc[i], current->data)){
            h = current->data;
            break;
        }
        current = current->next;
    }

    union_bit(conf_set[h], bit_past_fc[i], words_total1);
    union_bit(conf_set[h], conf_set[i], words_total1);
    RESET_BIT(conf_set[i], h);

    int length = length_of_v[h];
    
    while(labeled != NULL){ //when we jump back to h, we remove all the words from labeled until we reach h
        j = pop_front(&labeled);
        set_labeled[j] = 0;
        if (j == h){
            break;
        }
        reset_bit_arr(conf_set[j], words_total1);
        undo_reductions(j, lengths);
        updated_current_domain(j, lengths);

    }

    undo_reductions(h, lengths);
    RESET_BIT(domain[h], v[h]);
    *consistent = !(check_empty(domain[h], lengths[length]));

    return h;
}

int solve(int words_total, int *lengths){

    words_total1 = words_total + 1;
    labeled = NULL;

    reductions = malloc((words_total1) * sizeof(struct bit_stack));
    if (reductions == NULL){
        fprintf(stderr, "Unable to allocate enough memory\n");
        exit(1);
    }
    future_fc = malloc((words_total1) * sizeof(Listptr));
    if (future_fc == NULL){
        fprintf(stderr, "Unable to allocate enough memory\n");
        exit(1);
    }
    past_fc = malloc((words_total1) * sizeof(Listptr));
    if (past_fc == NULL){
        fprintf(stderr, "Unable to allocate enough memory\n");
        exit(1);
    }
    bit_past_fc = malloc((words_total1) * sizeof(unsigned long long int *));
    if (bit_past_fc == NULL){
        fprintf(stderr, "Unable to allocate enough memory\n");
        exit(1);
    }

    int j;
    future_fc[0] = NULL;
    past_fc[0] = NULL;
    for (j = 1; j <= words_total; j++){
        bit_past_fc[j] = bit_array(words_total1);
        future_fc[j] = NULL;
        past_fc[j] = NULL;
        reductions[j].top = -1;
        reductions[j].arr = malloc((words_total1) * sizeof(unsigned long long int*));
        if (reductions[j].arr == NULL){
            fprintf(stderr, "Unable to allocate enough memory\n");
            exit(1);
        }
    }

    int consistent = 1;
    int i = min_dom;
    while(1){
        if (consistent){
            if(consistent){
                insert_at_start(&labeled, i);
                set_labeled[i] = 1;
            }

            i = label(i, &consistent, lengths);

        } else {
            if(set_labeled[i] == 0){
                insert_at_start(&labeled, i);
                set_labeled[i] = 1;
            }
            i = unlabel(i, &consistent, lengths);

        }

        if (i > words_total || i <= 0){
            break;
        }
    }
    int k; 
    for (j = 1; j <= words_total; ++j){
        free_list(future_fc + j);
        free_list(past_fc + j);
    }
    free(future_fc);
    free(past_fc);
    for (j = 1; j <= words_total; ++j){
        free(bit_past_fc[j]);
        for (k = 0; k <= reductions[j].top; ++k){
            free(reductions[j].arr[k]);
        }
        free(reductions[j].arr);
    }
    free(reductions);
    free(bit_past_fc);
    free_list(&labeled);
    return (i > words_total);
}