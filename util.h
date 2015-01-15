#ifndef __UTIL_H__
#define __UTIL_H__
#include <getopt.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int BS;
int maxloop;

#define cell( _i_, _j_ ) board[ ldboard * (_j_) + (_i_) ]
#define local_cell( _i_, _j_ ) local_board[ row_block_size * (_j_) + (_i_) ]
#define ngb( _i_, _j_ )  nbngb[ ldnbngb * ((_j_) - 1) + ((_i_) - 1 ) ]

int get_arg(int argc,char** argv,int* nb_row, int* nb_col);
double mytimer(void);
void output_board(int N, int *board, int ldboard, int loop);
int generate_initial_board(int N, int *board, int ldboard);

#endif