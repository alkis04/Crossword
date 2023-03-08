#include <stdio.h>
#include <stdlib.h>
#include "bitarrays.h"

unsigned long long int *bit_array(int bits){
    int brackets = (bits + (sizeof(unsigned long long int) * 8 - 1)) / (sizeof(unsigned long long int) * 8); int i;
    unsigned long long int *array = malloc(sizeof(unsigned long long int) * brackets);
    if(array == NULL){
        fprintf(stderr, "Unable to allocate enough memory\n");
        exit(1);
    }
    for(i = 0; i < brackets; ++i)
        array[i] = 0ULL;
    return array;
}

void reset_bit_arr(unsigned long long int *array, int bits){
    int brackets = (bits + (sizeof(unsigned long long int) * 8 - 1)) / (sizeof(unsigned long long int) * 8); int i;
    for(i = 0; i < brackets; ++i)
        array[i] = 0ULL; 
}

int check_empty(unsigned long long int *arr, int bits){
    int brackets = (bits + (sizeof(unsigned long long int) * 8 - 1)) / (sizeof(unsigned long long int) * 8); int i;
    for(i = 0; i < brackets; ++i)
        if(arr[i] != 0ULL) return 0;
    return 1; // all bits are 0
}

int max_bit(unsigned long long int *arr, int bits){
    int brackets = (bits + (sizeof(unsigned long long int) * 8 - 1)) / (sizeof(unsigned long long int) * 8); int i;
    --brackets;
    for(i = brackets; i >= 0; --i) if(arr[i] != 0ULL) break;
    if(i == -1) return -99; // the set is empyty
    // i is the index of the last non zero element
    int j = 63;
    while(!GET_BIT(arr, i * (8 * sizeof(unsigned long long int)) + j)){
        --j; 
    }
    return i * (8 * sizeof(unsigned long long int)) + j;
}

int count_ones(unsigned long long int *arr, int bits){ // counts up to whole bracket 
    int brackets = (bits + (sizeof(unsigned long long int) * 8 - 1)) / (sizeof(unsigned long long int) * 8); int i;
    int ones = 0;
    unsigned long long int s = 1ULL;
    for(i = 0; i < brackets; ++i){
        if(arr[i] == 0)continue;
        s = 1ULL;
        while(s != 0){
            if(arr[i] & s) ones++;
            s <<= 1;
        }
    }
    return ones;
}

void set_bit_arr(unsigned long long int *arr, int bits){
    int brackets = (bits + (sizeof(unsigned long long int) * 8 - 1)) / (sizeof(unsigned long long int) * 8); int i;
    for(i = 0; i < brackets; ++i) arr[i] = ~0;
    arr[brackets - 1] >>= ((8  * sizeof(unsigned long long int) - bits) % (8 * sizeof(unsigned long long int))); //restores the leftover zeros that became one
}

void union_bit(unsigned long long int *array1, unsigned long long int *array2, int bits){
    int brackets = (bits + (sizeof(unsigned long long int) * 8 - 1)) / (sizeof(unsigned long long int) * 8); int i;
    for(i = 0; i < brackets; ++i) array1[i] = array1[i] | array2[i];
}

void diff_bit(unsigned long long int *array1, unsigned long long int *array2, int bits){
    int brackets = (bits + (sizeof(unsigned long long int) * 8 - 1)) / (sizeof(unsigned long long int) * 8); int i;
    for(i = 0; i < brackets; ++i) array1[i] = array1[i] & ~array2[i]; 
}


void intersect_bit(unsigned long long int *array1, unsigned long long int *array2, int bits){
    int brackets = (bits + (sizeof(unsigned long long int) * 8 - 1)) / (sizeof(unsigned long long int) * 8); int i;
    for(i = 0; i < brackets; ++i) array1[i] = array1[i] & array2[i];
}
