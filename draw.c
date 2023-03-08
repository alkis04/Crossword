#include <stdio.h>
#include <stdlib.h>
#include "structures.h"

#define MAX_VARIABLES 2000
extern int v[MAX_VARIABLES];
extern char ***dictionary;
extern int length_of_v[MAX_VARIABLES];

void draw_f(int N, int **wordshor, int **wordsver){
    int i, j;
    char **cross = malloc(N * sizeof(char *));
    if (cross == NULL){
        fprintf(stderr, "Unable to allocate enough memory\n");
        exit(1);
    }
    for(i = 0; i < N; i++){
        cross[i] = malloc(N * sizeof(char));
        if (cross[i] == NULL){
            fprintf(stderr, "Unable to allocate enough memory\n");
            exit(1);
        }
    }
    // Put the words in the crossword
    int character = 0;
    for(i = 0; i < N; ++i){
        character = 0;
        for(j = 0; j < N; ++j){
            if(wordshor[i][j] > 0){
                cross[i][j] = dictionary[length_of_v[wordshor[i][j]]][v[wordshor[i][j]]][character];
                ++character;
            }else{
                character = 0;
                if(wordshor[i][j] == -1) cross[i][j] = '#';
            }
        }
    }
    for(i = 0; i < N; ++i){
        character = 0;
        for(j = 0; j < N; ++j){
            if(wordsver[i][j] > 0){
                cross[j][i] = dictionary[length_of_v[wordsver[i][j]]][v[wordsver[i][j]]][character];
                ++character;
            }else{
                character = 0;
            }
        }
    }
    for(i = 0; i < N; ++i){
        for(j = 0; j < N; ++j){
            if(cross[i][j] == '#') printf("###");
            else printf(" %c ", cross[i][j]);
        }
        printf("\n");
    }
    two_d_free_char(cross, N);
    
}