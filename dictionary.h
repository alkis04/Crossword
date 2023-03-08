#define WORD_BUFFER 50

#define max(x,y) (((x) > (y)) ? (x) : (y))

char ***load_dict(char *, int *, int *);
void free_dict(char ***, int*, int);

unsigned long long int ****make_lexicon(char ***, int *, int);
void free_lexicon(unsigned long long int ****, int);
unsigned long long int *get_lexicon(unsigned long long int ****, int, char, int);
int check_word(char *, char ***, int *);