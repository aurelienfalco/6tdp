#include "util.h"

int loop = 0, num_alive = 0;
int ldboard, ldnbngb;
double t1, t2;
double temps;
int *board;
int *nbngb;
int BS; // size of the problem
int maxloop; // number of iterations
int print; // a boolean to print outputs


void init(){
	num_alive = 0;

    /* Leading dimension of the board array */
	ldboard = BS + 2;
    /* Leading dimension of the neigbour counters array */
	ldnbngb = BS;

	board = malloc( ldboard * ldboard * sizeof(int) );
	nbngb = malloc( ldnbngb * ldnbngb * sizeof(int) );

	num_alive = generate_initial_board( BS, &(cell(1, 1)), ldboard );

	printf("Starting number of living cells = %d\n", num_alive);
	if (print)
		output_board( BS, &(cell(1, 1)), ldboard, 0 );

	t1 = mytimer();
}