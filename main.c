#include <stdio.h>
#include <stdlib.h>
#include "structures.h"
#include "dictionary.h"
#include <string.h>
#include "solve.h"
#include "bitarrays.h"
#include "draw.h"

#define MAX_VARIABLES 2000
unsigned long long int ****lexicon; //bit array that stores for every word, the letter in every position
char ***dictionary; //stores every word in the dictionary depending on its size
int length_of_v[MAX_VARIABLES]; //stores the length of every variable in the crossword
int v[MAX_VARIABLES]; //stores the word of every variable in the crossord
unsigned long long int **domain; //bit array that stores the domain of words for every variable int the crossword
int **C; //C[i][j] stores the position of variable i, that crosses with variable j, or 0 if it doesn't crosses
unsigned long long int **conf_set; //for every variable i, stores the set of variables that had a conflict with variable i
int words_total; //the number of variables in the crossword
int lengths_of_variables_set[WORD_BUFFER]; //set of lengths of words in dictionary
int **adj; //for every variable i, stores the variables that crosses with i
int min_dom; //variable with min domain
int min_dom_len; //length of min_dom


typedef struct pair *Coords; //datatype that stores the coordinates of the variables in the crossword

int main(int argc, char *argv[]){
    if(argc < 1){
        fprintf(stderr, "First Parameter should be the Crossword file\n");
        return 2;
    }
    int i, j;
    int dict_arg = 0, draw = 0, checking = 0;
    for(i = 2; i < argc; ++i){ //argv[1] = CrosswordX.txt
        if(!strcmp(argv[i], "-dict")){
            dict_arg = ++i;
            if(i == argc){
                fprintf(stderr, "Invalid Parameters!\n");
                fprintf(stderr, "<dict> Dictionary.txt\n");
                return 2;
            }
        }
        else if(!strcmp(argv[i], "-draw")) draw = 1;
        else if(!strcmp(argv[i], "-check")) checking = 1;
        else{
            fprintf(stderr, "Invalid Parameters!\n");
            return 2;
        }
    }

        /* Input */
    FILE *inpt = fopen(argv[1], "r");
    if(inpt == NULL){
        fprintf(stderr, "File %s doesn't exist:(\n", argv[1]);
        return 2;
    }
   
    int N; //the size of the dictionary
    fscanf(inpt, "%d\n", &N);

    int **wordshor = two_d_int_array_0(N, N); //stores all horizontal variables in the crossword
    int **wordsver = two_d_int_array_0(N, N); //stores all vertical variables in the crossword
    int x, y;

    while(!feof(inpt)){ 
        fscanf(inpt, "%d %d\n", &x, &y);
        wordsver[y - 1][x - 1] = -1;
        wordshor[x - 1][y - 1] = -1;
    }
    fclose(inpt);

        /* Finding Word Positions */ 
    struct pair *pos = malloc(2 * N * ((N - 2) / 3 + 1) * sizeof(struct pair)); //stores the position of every variable in the crossword

    int wordscounthor = 0; //number of horizontal words
    int flag = 1, temp;
    for (i = 0; i < N; ++i){
        flag = 1;
        for (j = 0; j < N; ++j){
            if (wordshor[i][j] == -1){
                flag = 1;
            } else {
                if (flag) {
                    if (j + 1 < N && wordshor[i][j + 1] != -1){
                        wordscounthor++;
                        pos[wordscounthor - 1].first = i;
                        pos[wordscounthor - 1].second = j;
                        length_of_v[wordscounthor] = 0;
                        temp = wordscounthor;
                        // printf("%d %d\n", i, j);
                    } else {
                        temp = 0;
                    }
                }
                if (temp) {
                    length_of_v[wordscounthor]++;
                    lengths_of_variables_set[length_of_v[wordscounthor]] = 1;
                }
                wordshor[i][j] = temp;
                flag = 0;
            }
        }
    }

    int wordscountver = 0; //number of vertical words
    flag = 1;
    for (i = 0; i < N; ++i){
        flag = 1;
        for (j = 0; j < N; ++j){
            if (wordsver[i][j] == -1){
                flag = 1;
            } else {
                if (flag) {
                    if (j + 1 < N && wordsver[i][j + 1] != -1){
                        wordscountver++;
                        pos[wordscounthor + wordscountver - 1].first = i;
                        pos[wordscounthor + wordscountver - 1].second = j;
                        length_of_v[wordscounthor + wordscountver] = 0;
                        temp = wordscounthor + wordscountver;
                    } else {
                        temp = 0;
                    }
                }
                if (temp){
                    length_of_v[wordscounthor + wordscountver]++;
                    lengths_of_variables_set[length_of_v[wordscounthor + wordscountver]] = 1;
                }
                wordsver[i][j] = temp;
                flag = 0;
            }
        }
    }
    words_total = wordscounthor + wordscountver; 

        /*Creating the constraints matrix and the adjacency array*/
    adj = two_d_int_array(words_total + 1, words_total + 1);
    C = two_d_int_array_0(words_total + 1, words_total + 1);

    for (i = 1; i <= wordscounthor; ++i){
        make_constraints(wordshor, wordsver, N, pos[i - 1], C);
        find_adj(wordshor, wordsver, N, pos[i - 1], i);
    }
    for (; i <= words_total; ++i){
        make_constraints(wordsver, wordshor, N, pos[i - 1], C);
        find_adj(wordsver, wordshor, N, pos[i - 1], i);
    }

        /*Loading the Dictionary*/
    int *lengths = int_array_0(WORD_BUFFER); //in the position i, it stores the number of variables with i characters from the dictionary
    int max_len; //the max number of character that a word in the dictionary has
    if(!dict_arg){
        dictionary = load_dict("Words.txt", lengths, &max_len);
    }else{
        dictionary= load_dict(argv[dict_arg], lengths, &max_len);
    }
    if(dictionary == NULL){
        free(lengths);
        two_d_free(C, words_total + 1);
        two_d_free(wordshor, N);
        two_d_free(wordsver, N);
        free(pos);
        return 2;
    }

    //check
    if(checking){
        char word[WORD_BUFFER]; 
        // read words until EOF
        int status = 1;
        i = 1;
        while (scanf("%s", word) != EOF) {
            if(i > words_total){
                ;    
            } else if (lengths[(int)strlen(word)] == 0) {
                printf("Word %s is not in the dictionary\n", word);
                status = 0;
                break; 
            }else if ((int)strlen(word) != length_of_v[i]){
                printf("Word \"%s\" cannot be placed.\n", word);
                status = 0;
                break;
            }else{
                int words_pos = check_word(word, dictionary, lengths);
                if(words_pos == -1){
                    printf("Word \"%s\" is not in the dictionary\n", word);
                    status = 0;
                    break;
                }
                else{
                    v[i] = words_pos;
                    for(j = 1; j <= i; j++){
                        if(check(i, j) == 0){
                            printf("Word \"%s\" cannot be placed.\n", word);
                            status = 0;
                            break;
                        }
                    }
                }

            }
            if(status == 0) break;
            ++i; 
        }
        --i;
        if(status){
            if(i < words_total){
                printf("There are not enough words\n");
            }else if(i > words_total ){
                printf("There are too many words\n");
            }else if (draw) draw_f(N, wordshor, wordsver);
            
        }
    
    }else{
        int res = 1;

            /*Initialize domain*/
        domain = malloc((words_total + 1) * sizeof(unsigned long long int *));
        if (domain == NULL){
            fprintf(stderr, "Unable to allocate enough memory\n");
            exit(1);
        }
        for (i = 0; i <= words_total; i++){
            if(!lengths[length_of_v[i]]){
                if(lengths_of_variables_set[length_of_v[i]]){
                    res = 0;
                }
                continue;
            }
            domain[i] = bit_array(lengths[length_of_v[i]]);
            if (domain[i] == NULL){
                fprintf(stderr, "Unable to allocate enough memory\n");
                exit(1);
            }
            set_bit_arr(domain[i], lengths[length_of_v[i]]);
        }
        

            /*Mapping Dictionary*/
        lexicon = make_lexicon(dictionary, lengths, max_len);
            /*Initializing the solution*/

        conf_set = malloc((words_total + 1) * sizeof(unsigned long long int *));
        if (conf_set == NULL){
            fprintf(stderr, "Unable to allocate enough memory\n");
            exit(1);
        }
        for (i = 0; i <= words_total; i++){
            conf_set[i] = bit_array(words_total + 1);
            if (conf_set[i] == NULL){
                fprintf(stderr, "Unable to allocate enough memory\n");
                exit(1);
            }
        }
        min_dom_len = lengths[length_of_v[1]];
        min_dom = 1;
        for(i = 1; i <= words_total; ++i){
            if(lengths[length_of_v[i] < min_dom_len]){
                min_dom_len = lengths[length_of_v[i]];
                min_dom = i;
            }
        }
        
        if(res) res = solve(words_total, lengths);
        if(res == 0){
            printf("No solution found\n");
        }else{
            if(!draw){
                for(i = 1; i <= words_total; i++){
                    printf("%s\n", dictionary[length_of_v[i]][v[i]]);
                }
            }else{ // draw
                draw_f(N, wordshor, wordsver);  
            }   
        }

            /* Freeing Memory */ 
        for(i = 0; i <= words_total; i++){
            free(conf_set[i]);
        }
        free(conf_set);
        free_lexicon(lexicon, max_len);
        for(i = 0; i <= words_total; i++){
            if(lengths[length_of_v[i]] == 0)continue;
            free(domain[i]);
        }
        free(domain);
    }
    two_d_free(adj, words_total + 1);
    free_dict(dictionary, lengths, max_len);
    free(lengths);
    two_d_free(C, words_total + 1);
    free(pos);
    two_d_free(wordsver, N);
    two_d_free(wordshor, N);
    return 0;
}