#ifndef __COMMON_H__
#define __COMMON_H__

extern int loop, num_alive;
extern int ldboard, ldnbngb;
extern double t1, t2;
extern double temps;
extern int *board;
extern int *nbngb;
extern int BS; // size of the problem
extern int maxloop; // number of iterations
extern int print; // a boolean to print outputs

void init();

#endif