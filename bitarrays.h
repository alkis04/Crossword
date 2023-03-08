#define SET_BIT(array, bit) ((array)[(bit) / (sizeof(unsigned long long int) * 8)] |= (1ULL << ((bit) % (sizeof(unsigned long long int) * 8))))
#define GET_BIT(array, bit) (((array)[(bit) / (sizeof(unsigned long long int) * 8)] >> ((bit) % (sizeof(unsigned long long int) * 8))) & 1ULL)
#define RESET_BIT(array, bit) ((array)[(bit) / (sizeof(unsigned long long int) * 8)] &= ~(1ULL << ((bit) % (sizeof(unsigned long long int) * 8))))

unsigned long long int *bit_array(int);
void reset_bit_arr(unsigned long long int *, int);
int check_empty(unsigned long long int *, int);
int max_bit(unsigned long long int *, int);
void set_bit_arr(unsigned long long int *, int);
void set_bit_arr(unsigned long long int *, int);
void union_bit(unsigned long long int *, unsigned long long int *, int);
void diff_bit(unsigned long long int *, unsigned long long int *, int);
void intersect_bit(unsigned long long int *, unsigned long long int *, int );
int count_ones(unsigned long long int *, int);