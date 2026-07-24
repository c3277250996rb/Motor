#include "my_number.h"


int avg_array[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int aa_length = sizeof(avg_array) / sizeof(avg_array[0]);
int aa_index = 0;
int aa_avarage = 0;
int moving_avarage(int input_update){
    aa_avarage += (input_update - avg_array[aa_index]) / aa_length;
    avg_array[aa_index] = input_update;
    aa_index++;
    aa_index %= aa_length;
    return aa_avarage;
}
