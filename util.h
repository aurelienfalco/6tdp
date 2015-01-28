#ifndef __UTIL_H__
#define __UTIL_H__
#include <getopt.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unistd.h"

extern int BS; // size of the problem
extern int maxloop; // number of iterations
extern int print; // a boolean to print outputs

#define cell( _i_, _j_ ) board[ ldboard * (_j_) + (_i_) ]
#define cell_ld( _board_, _i_, _j_, _ld_ ) _board_[ _ld_ * (_j_) + (_i_) ]
#define ngb_ld( _board_, _i_, _j_, _ld_ ) _board_[ _ld_ * ((_j_) - 1) + ((_i_) - 1) ]
#define ngb( _i_, _j_ )  nbngb[ ldnbngb * ((_j_) - 1) + ((_i_) - 1 ) ]
#define pprintf(...) do { if (print) printf(__VA_ARGS__); }while(0)

int get_arg(int argc,char** argv,int* nb_row, int* nb_col);
double mytimer(void);
void output_board(int N, int *board, int ldboard, int loop);
void output_block(int M, int N, int *board, int ldboard, int loop);
int generate_initial_board(int N, int *board, int ldboard);

#endif