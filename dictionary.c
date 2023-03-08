#include "dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "structures.h"
#include "bitarrays.h"


char ***load_dict(char *dict_name, int *lengths, int *max_len){

    //Learning the size of the dictionary
    FILE *dict = fopen(dict_name, "r");
    if(dict == NULL){
        fprintf(stderr, "Dictionary %s doesn't exist\n", dict_name);
        return NULL;
    }
    int ch;
    int len;
    *max_len = 0;
    while(!feof(dict)){
        len = 0;
        ch = getc(dict);
        while(!isspace(ch) && !feof(dict)){
            ++len;
            ch = getc(dict);
        }
        *max_len = max(len, *max_len);
        ++lengths[len];

    }

    char ***dictionary = malloc(sizeof(char **) * (*max_len + 1));
    if(dictionary == NULL){
        fprintf(stderr, "Unable to allocate enough memory\n");
        exit(1);
    }
    int i, j;
    for(i = 2; i <= *max_len; i++){ // for every langth in range (2, max_len) 
        dictionary[i] = malloc(sizeof(char*) * lengths[i]); // create a string array of with size = lengths[i] 
            for(j = 0; j < lengths[i]; j++){
                dictionary[i][j] = malloc(sizeof(char) * (i+1));
            } 
    }

    rewind(dict); //go to the beggining of the file

    char *word = malloc(sizeof(char) * (*max_len+1));
    int *indexes = int_array_0(*max_len+1);
    while(!feof(dict)){
        fscanf(dict, "%s\n", word);
        len = strlen(word);
        if(len > 1){
            strcpy(dictionary[len][indexes[len]++], word);
        }
    }

    free(indexes);
    free(word);
    fclose(dict);
 
    return dictionary;
}

void free_dict(char ***dictionary, int *lengths, int max_len){
    int i, j;
    for(i = 2; i <= max_len; i++){
        for(j = 0; j < lengths[i]; j++){
            free(dictionary[i][j]);
        }
        free(dictionary[i]);
    }
    free(dictionary);
}

    /* lexicon [length][letter][position][variable]*/
unsigned long long int ****make_lexicon(char ***dictionary, int *lengths, int max_len){
    
    unsigned long long int ****lexicon = malloc(sizeof(unsigned long long int ***) * (max_len + 1));
    // Subdictionary with words of length i
    if(lexicon == NULL){
        fprintf(stderr, "Unable to allocate enough memory\n");
        exit(1);
    }
    int i, j, k;
    for(i = 2; i <= max_len; i++){ // for every length of word 
        lexicon[i] = malloc(sizeof(unsigned long long int **) * 26); // for every letter of the alphabet
        if(lexicon[i] == NULL){
            fprintf(stderr, "Unable to allocate enough memory\n");
            exit(1);
        }
        for(j = 0; j < 26; j++){
            lexicon[i][j] = malloc(sizeof(unsigned long long int *) * (i+1)); // for every position <= i = length of the word
            if(lexicon[i][j] == NULL){
                fprintf(stderr, "Unable to allocate enough memory\n");
                exit(1);
            }
            lexicon[i][j][0] = NULL; 
            for(k = 1; k <= i; k++){
                lexicon[i][j][k] = bit_array(lengths[i]); 
            }
        } 
    }

    for(i = 2; i <= max_len; i++){ // for every length of word
        for(j = 0; j < lengths[i]; j++){ // for every word of length i
            for(k = 0; k < i; k++){ // for every letter of the word
                SET_BIT(lexicon[i][dictionary[i][j][k] - 'a'][k+1], j);
            }
        }
    }
    return lexicon;
}

unsigned long long int *get_lexicon(unsigned long long int ****lexicon, int length, char letter, int position){
    return lexicon[length][letter - 'a'][position];
}

void free_lexicon(unsigned long long int ****lexicon, int max_len){
    int i, j, k;
    for(i = 2; i <= max_len; i++){
        for(j = 0; j < 26; j++){
            for(k = 1; k <= i; k++){
                free(lexicon[i][j][k]);
            }
            free(lexicon[i][j]);
        }
        free(lexicon[i]);
    }
    free(lexicon);
}

int check_word(char *word, char ***dictionary, int *lengths){ //checks if a word exists in a dictionary
    int len = strlen(word);

    int i;
    for(i = 0; i < lengths[len]; ++i){
        if(strcmp(word, dictionary[len][i]) == 0) return i;
    }
    return -1;
}